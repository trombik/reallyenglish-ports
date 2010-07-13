--- ./thread.c.orig	2009-11-26 09:37:49.000000000 +0900
+++ ./thread.c	2010-07-12 19:35:27.000000000 +0900
@@ -10,6 +10,8 @@
 #include <errno.h>
 #include <string.h>
 #include <pthread.h>
+#include <syslog.h>
+#include <stdarg.h>
 
 #define ITEMS_PER_ALLOC 64
 
@@ -170,7 +172,7 @@
     pthread_attr_init(&attr);
 
     if ((ret = pthread_create(&thread, &attr, func, arg)) != 0) {
-        fprintf(stderr, "Can't create thread: %s\n",
+        syslog(LOG_ERR, "Can't create thread: %s\n",
                 strerror(ret));
         exit(1);
     }
@@ -192,7 +194,7 @@
 static void setup_thread(LIBEVENT_THREAD *me) {
     me->base = event_init();
     if (! me->base) {
-        fprintf(stderr, "Can't allocate event base\n");
+        syslog(LOG_ERR, "Can't allocate event base\n");
         exit(1);
     }
 
@@ -202,7 +204,7 @@
     event_base_set(me->base, &me->notify_event);
 
     if (event_add(&me->notify_event, 0) == -1) {
-        fprintf(stderr, "Can't monitor libevent notify pipe\n");
+        syslog(LOG_ERR, "Can't monitor libevent notify pipe\n");
         exit(1);
     }
 
@@ -221,7 +223,7 @@
     me->suffix_cache = cache_create("suffix", SUFFIX_SIZE, sizeof(char*),
                                     NULL, NULL);
     if (me->suffix_cache == NULL) {
-        fprintf(stderr, "Failed to create suffix cache\n");
+        syslog(LOG_ERR, "Failed to create suffix cache\n");
         exit(EXIT_FAILURE);
     }
 }
@@ -258,7 +260,7 @@
 
     if (read(fd, buf, 1) != 1)
         if (settings.verbose > 0)
-            fprintf(stderr, "Can't read from libevent pipe\n");
+            syslog(LOG_ERR, "Can't read from libevent pipe\n");
 
     item = cq_pop(me->new_conn_queue);
 
@@ -267,11 +269,11 @@
                            item->read_buffer_size, item->transport, me->base);
         if (c == NULL) {
             if (IS_UDP(item->transport)) {
-                fprintf(stderr, "Can't listen for events on UDP socket\n");
+                syslog(LOG_ERR, "Can't listen for events on UDP socket\n");
                 exit(1);
             } else {
                 if (settings.verbose > 0) {
-                    fprintf(stderr, "Can't listen for events on fd %d\n",
+                    syslog(LOG_ERR, "Can't listen for events on fd %d\n",
                         item->sfd);
                 }
                 close(item->sfd);
