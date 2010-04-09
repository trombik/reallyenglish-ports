add syslog support (add "syslog = true" to .ini file)
--- python/skytools/scripting.py.orig	2009-09-03 12:15:20.000000000 +0900
+++ python/skytools/scripting.py	2009-09-03 12:48:31.000000000 +0900
@@ -133,6 +133,13 @@
         hdlr.setFormatter(fmt)
         log.addHandler(hdlr)
 
+    # syslog support via /dev/log
+    syslog = cf.getfile("syslog", "")
+    if syslog:
+        sysloghdlr = logging.handlers.SysLogHandler(address = syslog)
+        sysloghdlr.setFormatter(logging.Formatter('%(name)s[%(process)s]: %(message)s'))
+        log.addHandler(sysloghdlr)
+
     # if skylog.ini is disabled or not available, log at least to stderr
     if not got_skylog:
         hdlr = logging.StreamHandler()
