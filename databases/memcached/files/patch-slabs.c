--- ./slabs.c.orig	2010-07-12 18:24:32.000000000 +0900
+++ ./slabs.c	2010-07-12 18:25:56.000000000 +0900
@@ -20,6 +20,8 @@
 #include <string.h>
 #include <assert.h>
 #include <pthread.h>
+#include <syslog.h>
+#include <stdarg.h>
 
 /* powers-of-N allocation structures */
 
@@ -109,7 +111,7 @@
             mem_current = mem_base;
             mem_avail = mem_limit;
         } else {
-            fprintf(stderr, "Warning: Failed to allocate requested memory in"
+            syslog(LOG_ERR, "Warning: Failed to allocate requested memory in"
                     " one large chunk.\nWill allocate in smaller chunks\n");
         }
     }
@@ -125,7 +127,7 @@
         slabclass[i].perslab = settings.item_size_max / slabclass[i].size;
         size *= factor;
         if (settings.verbose > 1) {
-            fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u\n",
+            syslog(LOG_NOTICE, "slab class %3d: chunk size %9u perslab %7u\n",
                     i, slabclass[i].size, slabclass[i].perslab);
         }
     }
@@ -134,7 +136,7 @@
     slabclass[power_largest].size = settings.item_size_max;
     slabclass[power_largest].perslab = 1;
     if (settings.verbose > 1) {
-        fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u\n",
+        syslog(LOG_NOTICE, "slab class %3d: chunk size %9u perslab %7u\n",
                 i, slabclass[i].size, slabclass[i].perslab);
     }
 
