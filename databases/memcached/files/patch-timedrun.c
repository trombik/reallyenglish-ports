--- ./timedrun.c.orig	2009-10-24 05:38:01.000000000 +0900
+++ ./timedrun.c	2010-07-12 19:35:27.000000000 +0900
@@ -4,6 +4,8 @@
 #include <signal.h>
 #include <sys/wait.h>
 #include <sysexits.h>
+#include <syslog.h>
+#include <stdarg.h>
 
 #include <assert.h>
 
@@ -46,7 +48,7 @@
                 /* On the first iteration, pass the signal through */
                 sig = caught > 0 ? caught : SIGTERM;
                 if (caught == SIGALRM) {
-                   fprintf(stderr, "Timeout.. killing the process\n");
+                   syslog(LOG_ERR, "Timeout.. killing the process\n");
                 }
                 break;
             case 1:
