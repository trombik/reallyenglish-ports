--- ./assoc.c.orig	2009-10-24 05:38:01.000000000 +0900
+++ ./assoc.c	2010-07-12 19:35:27.000000000 +0900
@@ -24,6 +24,8 @@
 #include <string.h>
 #include <assert.h>
 #include <pthread.h>
+#include <syslog.h>
+#include <stdarg.h>
 
 static pthread_cond_t maintenance_cond = PTHREAD_COND_INITIALIZER;
 
@@ -61,7 +63,7 @@
 void assoc_init(void) {
     primary_hashtable = calloc(hashsize(hashpower), sizeof(void *));
     if (! primary_hashtable) {
-        fprintf(stderr, "Failed to init hashtable.\n");
+        syslog(LOG_ERR, "Failed to init hashtable.\n");
         exit(EXIT_FAILURE);
     }
 }
@@ -122,7 +124,7 @@
     primary_hashtable = calloc(hashsize(hashpower + 1), sizeof(void *));
     if (primary_hashtable) {
         if (settings.verbose > 1)
-            fprintf(stderr, "Hash table expansion starting\n");
+            syslog(LOG_NOTICE, "Hash table expansion starting\n");
         hashpower++;
         expanding = true;
         expand_bucket = 0;
@@ -214,7 +216,7 @@
                 expanding = false;
                 free(old_hashtable);
                 if (settings.verbose > 1)
-                    fprintf(stderr, "Hash table expansion done\n");
+                    syslog(LOG_NOTICE, "Hash table expansion done\n");
             }
         }
 
@@ -241,7 +243,7 @@
     }
     if ((ret = pthread_create(&maintenance_tid, NULL,
                               assoc_maintenance_thread, NULL)) != 0) {
-        fprintf(stderr, "Can't create thread: %s\n", strerror(ret));
+        syslog(LOG_ERR, "Can't create thread: %s\n", strerror(ret));
         return -1;
     }
     return 0;
