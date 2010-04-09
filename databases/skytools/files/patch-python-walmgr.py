the latest walmgr from CVS HEAD
--- python/walmgr.py.orig	2009-08-15 11:33:11.000000000 +0900
+++ python/walmgr.py	2009-07-21 19:50:48.000000000 +0900
@@ -41,13 +41,9 @@
 Additional features:
  * Simplified install. Master "setup" command should setup slave directories.
  * Add support for multiple targets on master.
- * Add an optional time based WAL retention parameter, this could be useful if base backups
- are taken from the standby (8.2 only)
  * WAL purge does not correctly purge old WAL-s if timelines are involved. The first
  useful WAL name is obtained from backup_label, WAL-s in the same timeline that are older 
  than first useful WAL are removed. 
- * xrestore should not attempt to copy the file on disk full condition - this
- will result in recovery failure. Pre 8.2 this means starting from zero.
  * Always copy the directory on "restore" add a special "--move" option.
 """
 
@@ -103,6 +99,7 @@
     return True
 
 class WalChunk:
+    """Represents a chunk of WAL used in record based shipping"""
     def __init__(self,filename,pos=0,bytes=0):
         self.filename = filename
         self.pos = pos
@@ -114,33 +111,135 @@
     def __str__(self):
         return "%s @ %d +%d" % (self.filename, self.pos, self.bytes)
 
+class PgControlData:
+    """Contents of pg_controldata"""
+
+    def __init__(self, slave_bin, slave_data, findRestartPoint):
+        """Collect last checkpoint information from pg_controldata output"""
+        self.xlogid = None
+        self.xrecoff = None
+        self.timeline = None
+        self.wal_size = None
+        self.wal_name = None
+        self.is_valid = False
+        self.pg_version = 0
+        matches = 0
+        pg_controldata = "%s %s" % (os.path.join(slave_bin, "pg_controldata"), slave_data)
+
+        for line in os.popen(pg_controldata, "r"):
+            if findRestartPoint:
+                m = re.match("^Latest checkpoint's REDO location:\s+([0-9A-F]+)/([0-9A-F]+)", line)
+            else:
+                m = re.match("^Latest checkpoint location:\s+([0-9A-F]+)/([0-9A-F]+)", line)
+            if m:
+                matches += 1
+                self.xlogid = int(m.group(1), 16)
+                self.xrecoff = int(m.group(2), 16)
+            m = re.match("^Latest checkpoint's TimeLineID:\s+(\d+)", line)
+            if m:
+                matches += 1
+                self.timeline = int(m.group(1))
+            m = re.match("^Bytes per WAL segment:\s+(\d+)", line)
+            if m:
+                matches += 1
+                self.wal_size = int(m.group(1))
+            m = re.match("^pg_control version number:\s+(\d+)", line)
+            if m:
+                matches += 1
+                self.pg_version = int(m.group(1))
+
+        if matches == 4:
+            self.wal_name = "%08X%08X%08X" % \
+                (self.timeline, self.xlogid, self.xrecoff / self.wal_size)
+            self.is_valid = True
+
 class BackupLabel:
-    def __init__(self):
+    """Backup label contents"""
+
+    def __init__(self, backupdir):
+        """Initialize a new BackupLabel from existing file"""
+        filename = os.path.join(backupdir, "backup_label")
         self.first_wal = None
         self.start_time = None
         self.label_string = None
-        self.fromslave = None
-
-def get_backup_label(dirname):
-    label = BackupLabel()
-    filename = os.path.join(dirname, "backup_label")
-    if not os.path.exists(filename):
-        # perhaps this is a backup taken from slave, try .old suffix
-        filename += ".old"
         if not os.path.exists(filename):
-            return None
-        label.fromslave = True
-    for line in open(filename):
-        m = re.match('^START WAL LOCATION: [^\s]+ \(file ([0-9A-Z]+)\)$', line)
-        if m:
-            label.first_wal = m.group(1)
-        m = re.match('^START TIME:\s(.*)$', line)
+            return
+        for line in open(filename):
+            m = re.match('^START WAL LOCATION: [^\s]+ \(file ([0-9A-Z]+)\)$', line)
+            if m:
+                self.first_wal = m.group(1)
+            m = re.match('^START TIME:\s(.*)$', line)
+            if m:
+                self.start_time = m.group(1)
+            m = re.match('^LABEL: (.*)$', line)
+            if m:
+                self.label_string = m.group(1)
+
+class PostgresConfiguration:
+    """Postgres configuration manipulation"""
+
+    def __init__(self, walmgr):
+        """load the configuration from master_config"""
+        self.walmgr = walmgr
+        self.log = walmgr.log
+        self.cf_file = walmgr.cf.get("master_config")
+        self.cf_buf = open(self.cf_file, "r").read()
+
+    def archive_mode(self):
+        """Return value for specified parameter"""
+        # see if explicitly set
+        m = re.search("^\s*archive_mode\s*=\s*'?([a-zA-Z01]+)'?\s*#?.*$", self.cf_buf, re.M | re.I)
         if m:
-            label.start_time = m.group(1)
-        m = re.match('^LABEL: (.*)$', line)
+            return m.group(1)
+        # also, it could be commented out as initdb leaves it
+        # it'd probably be best to check from the database ...
+        m = re.search("^#archive_mode\s*=.*$", self.cf_buf, re.M | re.I)
         if m:
-            label.label_string = m.group(1)
-    return label
+            return "off"
+        return None
+
+    def modify(self, cf_params):
+        """Change the configuration parameters supplied in cf_params"""
+
+        for (param, value) in cf_params.iteritems():
+            r_active = re.compile("^[ ]*%s[ ]*=[ ]*'(.*)'.*$" % param, re.M)
+            r_disabled = re.compile("^.*%s.*$" % param, re.M)
+
+            cf_full = "%s = '%s'" % (param, value)
+
+            m = r_active.search(self.cf_buf)
+            if m:
+                old_val = m.group(1)
+                if old_val == value:
+                    self.log.debug("parameter %s already set to '%s'" % (param, value))
+                else:
+                    self.log.debug("found parameter %s with value '%s'" % (param, old_val))
+                    self.cf_buf = "%s%s%s" % (self.cf_buf[:m.start()], cf_full, self.cf_buf[m.end():])
+            else:
+                m = r_disabled.search(self.cf_buf)
+                if m:
+                    self.log.debug("found disabled parameter %s" % param)
+                    self.cf_buf = "%s\n%s%s" % (self.cf_buf[:m.end()], cf_full, self.cf_buf[m.end():])
+                else:
+                    self.log.debug("found no value")
+                    self.cf_buf = "%s\n%s\n\n" % (self.cf_buf, cf_full)
+
+    def write(self):
+        """Write the configuration back to file"""
+        cf_old = self.cf_file + ".old"
+        cf_new = self.cf_file + ".new"
+
+        if self.walmgr.not_really:
+            cf_new = "/tmp/postgresql.conf.new"
+            open(cf_new, "w").write(self.cf_buf)
+            self.log.info("Showing diff")
+            os.system("diff -u %s %s" % (self.cf_file, cf_new))
+            self.log.info("Done diff")
+            os.remove(cf_new)
+            return
+
+        # polite method does not work, as usually not enough perms for it
+        open(self.cf_file, "w").write(self.cf_buf)
 
 class WalMgr(skytools.DBScript):
 
@@ -153,13 +252,19 @@
 
     def __init__(self, args):
 
-        if len(args) > 0:
-            # hack to determine the role of the node
-            cf = ConfigParser.ConfigParser()
-            cf.read(args[0])
-            for (self.wtype, self.service_name) in [ (MASTER, "wal-master"), (SLAVE, "wal-slave") ]:
-                if cf.has_section(self.service_name):
-                    break
+        if len(args) < 2:
+            # need at least config file and command
+            usage(1)
+
+        # determine the role of the node from provided configuration
+        cf = ConfigParser.ConfigParser()
+        cf.read(args[0])
+        for (self.wtype, self.service_name) in [ (MASTER, "wal-master"), (SLAVE, "wal-slave") ]:
+            if cf.has_section(self.service_name):
+                break
+        else:
+            print >> sys.stderr, "Invalid config file: %s" % args[0]
+            sys.exit(1)
 
         skytools.DBScript.__init__(self, self.service_name, args)
         self.set_single_loop(1)
@@ -170,6 +275,7 @@
 
         if len(self.args) < 2:
             usage(1)
+        self.cfgfile = self.args[0]
         self.cmd = self.args[1]
         self.args = self.args[2:]
         self.script = os.path.abspath(sys.argv[0])
@@ -206,7 +312,7 @@
 
     def assert_valid_role(self,role):
         if self.wtype != role:
-            self.log.warning("Action not available on current node.");
+            self.log.warning("Action not available on current node.")
             sys.exit(1)
 
     def pg_start_backup(self, code):
@@ -256,13 +362,13 @@
     def exec_rsync(self,args,die_on_error=False):
         cmdline = [ "rsync", "-a", "--quiet" ]
         if self.cf.getint("compression", 0) > 0:
-            cmdline.append("-z");
+            cmdline.append("-z")
         cmdline += args
 
         cmd = "' '".join(cmdline)
         self.log.debug("Execute rsync cmd: '%s'" % (cmd))
         if self.not_really:
-            return
+            return 0
         res = os.spawnvp(os.P_WAIT, cmdline[0], cmdline)
         if res == 24:
             self.log.info("Some files vanished, but thats OK")
@@ -289,6 +395,12 @@
             self.log.fatal("exec failed, res=%d (%s)" % (res, repr(cmdline)))
             sys.exit(1)
 
+    def exec_system(self, cmdline):
+        self.log.debug("Execute cmd: '%s'" % (cmdline))
+        if self.not_really:
+            return 0
+        return os.WEXITSTATUS(os.system(cmdline))
+
     def chdir(self, loc):
         self.log.debug("chdir: '%s'" % (loc))
         if self.not_really:
@@ -326,70 +438,75 @@
         except:
             self.log.fatal("Cannot write to %s" % fn)
 
+    def authfile_name(self):
+        return os.path.join(self.cf.get("master_data"), os.path.join("global", "pg_auth"))
+
     def master_stop(self):
+        """Deconfigure archiving, attempt to stop syncdaemon"""
+        data_dir = self.cf.get("master_data")
+        restart_cmd = self.cf.get("master_restart_cmd", "")
+
         self.assert_valid_role(MASTER)
         self.log.info("Disabling WAL archiving")
 
-        self.master_configure_archiving('')
+        self.master_configure_archiving(False, restart_cmd)
 
-    def master_configure_archiving(self, cf_val):
-        cf_file = self.cf.get("master_config")
-        data_dir = self.cf.get("master_data")
-        r_active = re.compile("^[ ]*archive_command[ ]*=[ ]*'(.*)'.*$", re.M)
-        r_disabled = re.compile("^.*archive_command.*$", re.M)
+        # if we have a restart command, then use it, otherwise signal
+        if restart_cmd:
+            self.log.info("Restarting postmaster")
+            self.exec_system(restart_cmd)
+        else:
+            self.log.info("Sending SIGHUP to postmaster")
+            self.signal_postmaster(data_dir, signal.SIGHUP)
+
+        # stop any running syncdaemons
+        pidfile = self.cf.get("pidfile", "")
+        if os.path.exists(pidfile):
+            self.log.info('Pidfile %s exists, attempting to stop syncdaemon.' % pidfile)
+            self.exec_cmd([self.script, self.cfgfile, "syncdaemon", "-s"])
+        self.log.info("Done")
 
-        cf_full = "archive_command = '%s'" % cf_val
+    def master_configure_archiving(self, enable_archiving, can_restart):
+        """Turn the archiving on or off"""
 
-        if not os.path.isfile(cf_file):
-            self.log.fatal("Config file not found: %s" % cf_file)
-        self.log.info("Using config file: %s", cf_file)
+        cf = PostgresConfiguration(self)
+        archive_mode = cf.archive_mode()
 
-        buf = open(cf_file, "r").read()
-        m = r_active.search(buf)
-        if m:
-            old_val = m.group(1)
-            if old_val == cf_val:
-                self.log.debug("postmaster already configured")
+        if enable_archiving:
+            # enable archiving
+            cf_file = os.path.abspath(self.cf.filename)
+            xarchive = "%s %s %s" % (self.script, cf_file, "xarchive %p %f")
+            cf_params = { "archive_command": xarchive }
+
+            if archive_mode:
+                # archive mode specified in config, turn it on
+                self.log.debug("found 'archive_mode' in config -- enabling it")
+                cf_params["archive_mode"] = "on"
+
+                if archive_mode.lower() not in ('1', 'on', 'true') and not can_restart:
+                    self.log.warning("database must be restarted to enable archiving")
             else:
-                self.log.debug("found active but different conf")
-                newbuf = "%s%s%s" % (buf[:m.start()], cf_full, buf[m.end():])
-                self.change_config(cf_file, newbuf)
+                self.log.debug("seems that archive_mode is not set, ignoring it.")
+
         else:
-            m = r_disabled.search(buf)
-            if m:
-                self.log.debug("found disabled value")
-                newbuf = "%s\n%s%s" % (buf[:m.end()], cf_full, buf[m.end():])
-                self.change_config(cf_file, newbuf)
+            # disable archiving
+            if not archive_mode:
+                # archive_mode not set, just reset archive_command and its done
+                self.log.debug("archive_mode not set in config, leaving as is")
+                cf_params = { "archive_command": "" }
+            elif can_restart:
+                # restart possible, turn off archiving, reset archive_command
+                cf_params = { "archive_mode": "off", "archive_command": "" }
             else:
-                self.log.debug("found no value")
-                newbuf = "%s\n%s\n\n" % (buf, cf_full)
-                self.change_config(cf_file, newbuf)
+                # not possible to change archive_mode (requires restart), so we
+                # just set the archive_command to /bin/true to avoid WAL pileup.
+                self.log.debug("master_restart_cmd not specified, leaving archive_mode as is")
+                cf_params = { "archive_command": "/bin/true" }
 
-        self.log.info("Sending SIGHUP to postmaster")
-        self.signal_postmaster(data_dir, signal.SIGHUP)
-        self.log.info("Done")
-
-    def change_config(self, cf_file, buf):
-        cf_old = cf_file + ".old"
-        cf_new = cf_file + ".new"
-
-        if self.not_really:
-            cf_new = "/tmp/postgresql.conf.new"
-            open(cf_new, "w").write(buf)
-            self.log.info("Showing diff")
-            os.system("diff -u %s %s" % (cf_file, cf_new))
-            self.log.info("Done diff")
-            os.remove(cf_new)
-            return
+        self.log.debug("modifying configuration: %s" % cf_params)
 
-        # polite method does not work, as usually not enough perms for it
-        if 0:
-            open(cf_new, "w").write(buf)
-            bak = open(cf_file, "r").read()
-            open(cf_old, "w").write(bak)
-            os.rename(cf_new, cf_file)
-        else:
-            open(cf_file, "w").write(buf)
+        cf.modify(cf_params)
+        cf.write()
 
     def remote_mkdir(self, remdir):
         tmp = remdir.split(":", 1)
@@ -404,25 +521,22 @@
 
     def remote_walmgr(self, command, stdin_disabled = True):
         """Pass a command to slave WalManager"""
-        slave = self.cf.get("slave")
-        slave_config = self.cf.get("slave_config", "")
-        tmp = slave.split(":", 1)
 
         sshopt = "-T"
         if stdin_disabled:
             sshopt += "n"
 
-        cmdline = None
+        slave_config = self.cf.get("slave_config")
+        if not slave_config:
+            raise Exception("slave_config not specified in %s" % self.cfgfile)
 
-        if len(tmp) < 2:
-            raise Exception("cannot find slave hostname")
-        else:
-            host, path = tmp
-            cmdline = [ "ssh", sshopt, host, self.script  ]
+        try:
+            slave = self.cf.get("slave")
+            host, path = slave.split(":", 1)
+        except:
+            raise Exception("invalid value for 'slave' in %s" % self.cfgfile)
 
-        if slave_config:
-            cmdline += [ slave_config ]
-        cmdline += [ command ]
+        cmdline = [ "ssh", sshopt, host, self.script, slave_config, command ]
 
         if self.not_really:
             self.log.info("remote_walmgr: %s" % command)
@@ -433,12 +547,22 @@
         if self.wtype == MASTER:
             self.log.info("Configuring WAL archiving")
 
-            cf_file = os.path.abspath(self.cf.filename)
-            cf_val = "%s %s %s" % (self.script, cf_file, "xarchive %p %f")
+            data_dir = self.cf.get("master_data")
+            restart_cmd = self.cf.get("master_restart_cmd", "")
+
+            self.master_configure_archiving(True, restart_cmd)
+
+            # if we have a restart command, then use it, otherwise signal
+            if restart_cmd:
+                self.log.info("Restarting postmaster")
+                self.exec_system(restart_cmd)
+            else:
+                self.log.info("Sending SIGHUP to postmaster")
+                self.signal_postmaster(data_dir, signal.SIGHUP)
 
-            self.master_configure_archiving(cf_val)
             # ask slave to init
             self.remote_walmgr("setup")
+            self.log.info("Done")
         else:
             # create slave directory structure
             def mkdir(dir):
@@ -474,7 +598,7 @@
                 self.log.info("Running periodic command: %s" % periodic_command)
                 if not elapsed or elapsed > command_interval:
                     if not self.not_really:
-                        rc = os.WEXITSTATUS(os.system(periodic_command))
+                        rc = os.WEXITSTATUS(self.exec_system(periodic_command))
                         if rc != 0:
                             self.log.error("Periodic command exited with status %d" % rc)
                             # dont update timestamp - try again next time
@@ -561,7 +685,8 @@
             self.exec_big_rsync(cmdline)
 
             self.remote_walmgr("xpurgewals")
-        except:
+        except Exception, e:
+            self.log.error(e)
             errors = True
 
         try:
@@ -588,9 +713,10 @@
         3. Wait for WAL apply to complete (look at PROGRESS file)
         4. Rotate old backups
         5. Copy data directory to data.master
-        6. Purge unneeded WAL-s
-        7. Resume WAL apply
-        8. Release backup lock
+        6. Create backup label and history file.
+        7. Purge unneeded WAL-s
+        8. Resume WAL apply
+        9. Release backup lock
         """
         self.assert_valid_role(SLAVE)
         if self.slave_lock_backups() != 0:
@@ -604,10 +730,71 @@
                 self.slave_rotate_backups()
                 src = self.cf.get("slave_data")
                 dst = self.cf.get("full_backup")
+
+                start_time = time.localtime()
                 cmdline = ["cp", "-a", src, dst ]
                 self.log.info("Executing %s" % " ".join(cmdline))
                 if not self.not_really:
                     self.exec_cmd(cmdline)
+                stop_time = time.localtime()
+
+                # Obtain the last restart point information
+                ctl = PgControlData(self.cf.get("slave_bin", ""), dst, False)
+
+                # TODO: The newly created backup directory probably still contains
+                # backup_label.old and recovery.conf files. Remove these.
+
+                if not ctl.is_valid:
+                    self.log.warning("Unable to determine last restart point, backup_label not created.")
+                else:
+                    # Write backup label and history file
+                    
+                    backup_label = \
+"""START WAL LOCATION: %(xlogid)X/%(xrecoff)X (file %(wal_name)s)
+CHECKPOINT LOCATION: %(xlogid)X/%(xrecoff)X
+START TIME: %(start_time)s
+LABEL: SlaveBackup"
+"""
+                    backup_history = \
+"""START WAL LOCATION: %(xlogid)X/%(xrecoff)X (file %(wal_name)s)
+STOP WAL LOCATION: %(xlogid)X/%(xrecoff)X (file %(wal_name)s)
+CHECKPOINT LOCATION: %(xlogid)X/%(xrecoff)X
+START TIME: %(start_time)s
+LABEL: SlaveBackup"
+STOP TIME: %(stop_time)s
+"""
+
+                    label_params = {
+                        "xlogid":       ctl.xlogid,
+                        "xrecoff":      ctl.xrecoff,
+                        "wal_name":     ctl.wal_name,
+                        "start_time":   time.strftime("%Y-%m-%d %H:%M:%S %Z", start_time),
+                        "stop_time":    time.strftime("%Y-%m-%d %H:%M:%S %Z", stop_time),
+                    }
+
+                    # Write the label
+                    filename = os.path.join(dst, "backup_label")
+                    if self.not_really:
+                        self.log.info("Writing backup label to %s" % filename)
+                    else:
+                        lf = open(filename, "w")
+                        lf.write(backup_label % label_params)
+                        lf.close()
+
+                    # Now the history
+                    histfile = "%s.%08X.backup" % (ctl.wal_name, ctl.xrecoff % ctl.wal_size)
+                    completed_wals = self.cf.get("completed_wals")
+                    filename = os.path.join(completed_wals, histfile)
+                    if os.path.exists(filename):
+                        self.log.warning("%s: already exists, refusing to overwrite." % filename)
+                    else:
+                        if self.not_really:
+                            self.log.info("Writing backup history to %s" % filename)
+                        else:
+                            lf = open(filename, "w")
+                            lf.write(backup_history % label_params)
+                            lf.close()
+
                 self.slave_purge_wals()
             finally:
                 self.slave_continue()
@@ -641,7 +828,7 @@
             dst_loc += "/"
 
         # copy data
-        self.exec_rsync([ srcpath, dst_loc ], True)
+        self.exec_rsync([ srcpath, self.authfile_name(), dst_loc ], True)
 
         self.log.debug("%s: done", srcname)
         end_time = time.time()
@@ -720,7 +907,7 @@
             os._exit(0)
         chunk.sync_time += (time.time() - syncstart)
 
-        status = os.waitpid(childpid, 0);
+        status = os.waitpid(childpid, 0)
         rc = os.WEXITSTATUS(status[1]) 
         if rc == 0:
             log = daemon_mode and self.log.debug or self.log.info
@@ -754,9 +941,16 @@
         use_xlog_functions = self.cf.getint("use_xlog_functions", False)
         data_dir = self.cf.get("master_data")
         xlog_dir = os.path.join(data_dir, "pg_xlog")
-        dst_loc = self.cf.get("partial_wals")
-        if dst_loc[-1] != "/":
-            dst_loc += "/"
+
+        auth_loc = os.path.join(self.cf.get("completed_wals"), "")
+        dst_loc = os.path.join(self.cf.get("partial_wals"), "")
+
+        # sync the auth file
+        if not daemon_mode:
+            # avoid the extra rsync in daemon mode - the file is fairly static, so we 
+            # don't need to sync it every N seconds.
+            if self.exec_rsync([self.authfile_name(), auth_loc]) != 0:
+                self.log.warning('Cannot sync auth file')
 
         db = None
         if use_xlog_functions:
@@ -827,13 +1021,16 @@
 
     def xrestore(self):
         if len(self.args) < 2:
-            die(1, "usage: xrestore srcname dstpath")
+            die(1, "usage: xrestore srcname dstpath [last restartpoint wal]")
         srcname = self.args[0]
         dstpath = self.args[1]
+        lstname = None
+        if len(self.args) > 2:
+            lstname = self.args[2]
         if self.wtype == MASTER:
             self.master_xrestore(srcname, dstpath)
         else:
-            self.slave_xrestore_unsafe(srcname, dstpath, os.getppid())
+            self.slave_xrestore_unsafe(srcname, dstpath, os.getppid(), lstname)
 
     def slave_xrestore(self, srcname, dstpath):
         loop = 1
@@ -872,7 +1069,7 @@
             return False
         return True
 
-    def slave_xrestore_unsafe(self, srcname, dstpath, parent_pid):
+    def slave_xrestore_unsafe(self, srcname, dstpath, parent_pid, lstname = None):
         srcdir = self.cf.get("completed_wals")
         partdir = self.cf.get("partial_wals")
         pausefile = os.path.join(srcdir, "PAUSE")
@@ -927,10 +1124,17 @@
         self.exec_cmd(cmdline)
 
         if self.cf.getint("keep_backups", 0) == 0:
-            # cleanup only if we don't keep backup history.
-            # historic WAL files are removed during backup rotation
+            # cleanup only if we don't keep backup history, keep the files needed
+            # to roll forward from last restart point. If the restart point is not
+            # handed to us (i.e 8.3 or later), then calculate it ourselves.
+            # Note that historic WAL files are removed during backup rotation
+            if lstname == None:
+                lstname = self.last_restart_point(srcname)
+                self.log.debug("calculated restart point: %s" % lstname)
+            else:
+                self.log.debug("using supplied restart point: %s" % lstname)
             self.log.debug("%s: copy done, cleanup" % srcname)
-            self.slave_cleanup(srcname)
+            self.slave_cleanup(lstname)
 
         if os.path.isfile(partfile) and not srcfile == partfile:
             # Remove any partial files after restore. Only leave the partial if
@@ -978,9 +1182,8 @@
         # stop postmaster if ordered
         if stop_cmd and os.path.isfile(pidfile):
             self.log.info("Stopping postmaster: " + stop_cmd)
-            if not self.not_really:
-                os.system(stop_cmd)
-                time.sleep(3)
+            self.exec_system(stop_cmd)
+            time.sleep(3)
 
         # is it dead?
         if pidfile and os.path.isfile(pidfile):
@@ -1032,6 +1235,10 @@
                     # restore original xlog files to data_dir/pg_xlog   
                     # symlinked directories are dereferences
                     self.exec_cmd(["cp", "-rL", "%s/pg_xlog" % bak, data_dir])
+                else:
+                    # create an archive_status directory
+                    xlog_dir = os.path.join(data_dir, "pg_xlog")
+                    os.mkdir(os.path.join(xlog_dir, "archive_status"), 0700)
         else:
             data_dir = full_dir
 
@@ -1072,15 +1279,31 @@
                         os.remove(link_loc)
                     os.symlink(link_dst, link_loc)
 
+
         # write recovery.conf
         rconf = os.path.join(data_dir, "recovery.conf")
         cf_file = os.path.abspath(self.cf.filename)
 
-        conf = "\nrestore_command = '%s %s %s'\n" % (self.script, cf_file, 'xrestore %f "%p"')
-        conf += "#recovery_target_time=''\n" + \
-                "#recovery_target_xid=''\n" + \
-                "#recovery_target_inclusive=true\n" + \
-                "#recovery_target_timeline=''\n"
+        # determine if we can use %r in restore_command
+        ctl = PgControlData(self.cf.get("slave_bin", ""), data_dir, True)
+        if ctl.pg_version > 830:
+            self.log.debug('pg_version is %s, adding %%r to restore command' % ctl.pg_version)
+            restore_command = 'xrestore %f "%p" %r'
+        else:
+            if not ctl.is_valid:
+                self.log.warning('unable to run pg_controldata, assuming pre 8.3 environment')
+            else:
+                self.log.debug('using pg_controldata to determine restart points')
+            restore_command = 'xrestore %f "%p"'
+
+        conf = """
+restore_command = '%s %s %s'
+#recovery_target_time=
+#recovery_target_xid=
+#recovery_target_inclusive=true
+#recovery_target_timeline=
+""" % (self.script, cf_file, restore_command)
+
         self.log.info("Write %s" % rconf)
         if self.not_really:
             print conf
@@ -1100,8 +1323,7 @@
 
             # run database in recovery mode
             self.log.info("Starting postmaster: " + start_cmd)
-            if not self.not_really:
-                os.system(start_cmd)
+            self.exec_system(start_cmd)
         else:
             self.log.info("Data files restored, recovery.conf created.")
             self.log.info("postgresql.conf and additional WAL files may need to be restored manually.")
@@ -1110,13 +1332,25 @@
         self.assert_valid_role(SLAVE)
 
         srcdir = self.cf.get("completed_wals")
+        datadir = self.cf.get("slave_data")
         stopfile = os.path.join(srcdir, "STOP")
+        src_authfile = os.path.join(srcdir, "pg_auth")
+        dst_authfile = os.path.join(datadir, os.path.join("global", "pg_auth"))
+
         if self.not_really:
             self.log.info("Writing STOP file: %s" % stopfile)
         else:
             open(stopfile, "w").write("1")
         self.log.info("Stopping recovery mode")
 
+        if os.path.isfile(src_authfile):
+            self.log.debug("Using pg_auth file from master.")
+            try:
+                os.rename(dst_authfile, "%s.old" % dst_authfile)
+                self.exec_cmd(["cp", src_authfile, dst_authfile])
+            except Exception, e:
+                self.log.warning("Unable to restore pg_auth file: %s" % e)
+
     def slave_pause(self, waitcomplete=0):
         """Pause the WAL apply, wait until last file applied if needed"""
         self.assert_valid_role(SLAVE)
@@ -1190,26 +1424,49 @@
             backups = self.get_backup_list(self.cf.get("full_backup"))
             if backups:
                 print "\nList of backups:\n"
-                print "%-15s %-24s %-10s %-24s" % \
+                print "%-15s %-24s %-11s %-24s" % \
                     ("Backup set", "Timestamp", "Label", "First WAL")
-                print "%s %s %s %s" % (15*'-', 24*'-', 10*'-',24*'-')
+                print "%s %s %s %s" % (15*'-', 24*'-', 11*'-',24*'-')
                 for backup in backups:
-                    lbl = get_backup_label(backup)
-                    print "%-15s %-24.24s %-10.10s %-24s%s" % \
-                        (os.path.basename(backup), lbl.start_time, lbl.label_string,
-                        lbl.first_wal, lbl.fromslave and "*" or "")
+                    lbl = BackupLabel(backup)
+                    print "%-15s %-24.24s %-11.11s %-24s" % \
+                        (os.path.basename(backup), lbl.start_time,
+                        lbl.label_string, lbl.first_wal)
                 print
             else:
                 print "\nNo backups found.\n"
 
     def get_first_walname(self,backupdir):
         """Returns the name of the first needed WAL segment for backupset"""
-        label = get_backup_label(backupdir)
+        label = BackupLabel(backupdir)
         if not label.first_wal:
             self.log.error("WAL name not found at %s" % backupdir)
             return None
         return label.first_wal
 
+    def last_restart_point(self,walname):
+        """
+        Determine the WAL file of the last restart point (recovery checkpoint).
+        For 8.3 this could be done with %r parameter to restore_command, for 8.2
+        we need to consult control file (parse pg_controldata output).
+        """
+        slave_data = self.cf.get("slave_data")
+        backup_label = os.path.join(slave_data, "backup_label")
+        if os.path.exists(backup_label):
+            # Label file still exists, use it for determining the restart point
+            lbl = BackupLabel(slave_data)
+            self.log.debug("Last restart point from backup_label: %s" % lbl.first_wal)
+            return lbl.first_wal
+
+        ctl = PgControlData(self.cf.get("slave_bin", ""), ".", True)
+        if not ctl.is_valid:
+            # No restart point information, use the given wal name
+            self.log.warning("Unable to determine last restart point")
+            return walname
+
+        self.log.debug("Last restart point: %s" % ctl.wal_name)
+        return ctl.wal_name
+
     def order_backupdirs(self,prefix,a,b):
         """Compare the backup directory indexes numerically"""
         prefix = os.path.abspath(prefix)
@@ -1271,11 +1528,10 @@
             if archive_command:
                 cmdline = archive_command.replace("$BACKUPDIR", last)
                 self.log.info("Executing archive_command: " + cmdline)
-                if not self.not_really:
-                    rc = os.WEXITSTATUS(os.system(cmdline))
-                    if rc != 0:
-                        self.log.error("Backup archiving returned %d, exiting!" % rc)
-                        sys.exit(1)
+                rc = self.exec_system(cmdline)
+                if rc != 0:
+                    self.log.error("Backup archiving returned %d, exiting!" % rc)
+                    sys.exit(1)
 
             self.log.info("Removing expired backup directory: %s" % last)
             if self.not_really:
@@ -1302,11 +1558,11 @@
         completed_wals = self.cf.get("completed_wals")
         partial_wals = self.cf.get("partial_wals")
 
-        self.log.debug("cleaning completed wals since %s" % last_applied)
+        self.log.debug("cleaning completed wals before %s" % last_applied)
         last = self.del_wals(completed_wals, last_applied)
         if last:
             if os.path.isdir(partial_wals):
-                self.log.debug("cleaning partial wals since %s" % last)
+                self.log.debug("cleaning partial wals before %s" % last)
                 self.del_wals(partial_wals, last)
             else:
                 self.log.warning("partial_wals dir does not exist: %s"
