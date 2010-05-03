ignore EBUSY when data directory is a mount point.
this is usefull when you snapshot data directory.
--- python/walmgr.py	2010-01-15 05:47:23.000000000 +0900
+++ python/walmgr.py	2010-05-03 12:24:31.000000000 +0900
@@ -1284,13 +1285,28 @@
         if createbackup and os.path.isdir(data_dir):
             self.log.info("Move %s to %s" % (data_dir, bak))
             if not self.not_really:
-                os.rename(data_dir, bak)
+                # mimic "mv"
+                self.exec_rsync(["--delete", os.path.join(data_dir,""), bak], True)
+                try:
+                    shutil.rmtree(data_dir)
+                except OSError as ex:
+                    if ex.errno == errno.EBUSY:
+                        self.log.info("ingnoring EBUSY, assuming %s is a mountpoint." % (data_dir))
+                    else:
+                        raise
 
         # move new data, copy if setname specified
         self.log.info("%s %s to %s" % (setname and "Copy" or "Move", full_dir, data_dir))
         if not self.not_really:
             if not setname:
-                os.rename(full_dir, data_dir)
+                self.exec_rsync(["--delete", os.path.join(full_dir, ""), data_dir], True)
+                try:
+                    shutil.rmtree(full_dir)
+                except OSError as ex:
+                    if ex.errno == errno.EBUSY:
+                        self.log.info("ingnoring EBUSY, assuming %s is a mountpoint." % (full_dir))
+                    else:
+                        raise
             else:
                 self.exec_rsync(["--delete", "--no-relative", "--exclude=pg_xlog/*",
                     os.path.join(full_dir,""), data_dir], True)
