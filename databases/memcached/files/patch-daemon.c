--- ./daemon.c.orig	2009-08-30 08:00:58.000000000 +0900
+++ ./daemon.c	2010-07-12 19:37:01.000000000 +0900
@@ -38,6 +38,8 @@
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
+#include <syslog.h>
+#include <stdarg.h>
 
 #include "memcached.h"
 
@@ -85,5 +87,12 @@
             }
         }
     }
+
+    if (settings.verbose > 1) {
+        openlog("memcached", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
+    } else {
+        openlog("memcached", LOG_PID | LOG_NDELAY, LOG_USER);
+    }
+
     return (0);
 }
