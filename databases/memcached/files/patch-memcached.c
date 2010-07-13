--- ./memcached.c.orig	2009-11-27 14:45:13.000000000 +0900
+++ ./memcached.c	2010-07-12 19:35:27.000000000 +0900
@@ -21,6 +21,7 @@
 #include <sys/resource.h>
 #include <sys/uio.h>
 #include <ctype.h>
+#include <syslog.h>
 #include <stdarg.h>
 
 /* some POSIX systems need the following definition
@@ -255,7 +256,7 @@
     freetotal = 200;
     freecurr = 0;
     if ((freeconns = calloc(freetotal, sizeof(conn *))) == NULL) {
-        fprintf(stderr, "Failed to allocate connection structures\n");
+        syslog(LOG_ERR, "Failed to allocate connection structures\n");
     }
     return;
 }
@@ -325,7 +326,7 @@
 
     if (NULL == c) {
         if (!(c = (conn *)calloc(1, sizeof(conn)))) {
-            fprintf(stderr, "calloc()\n");
+            syslog(LOG_ERR, "calloc()\n");
             return NULL;
         }
         MEMCACHED_CONN_CREATE(c);
@@ -355,7 +356,7 @@
         if (c->rbuf == 0 || c->wbuf == 0 || c->ilist == 0 || c->iov == 0 ||
                 c->msglist == 0 || c->suffixlist == 0) {
             conn_free(c);
-            fprintf(stderr, "malloc()\n");
+            syslog(LOG_ERR, "malloc()\n");
             return NULL;
         }
 
@@ -378,19 +379,19 @@
 
     if (settings.verbose > 1) {
         if (init_state == conn_listening) {
-            fprintf(stderr, "<%d server listening (%s)\n", sfd,
+            syslog(LOG_NOTICE, "<%d server listening (%s)\n", sfd,
                 prot_text(c->protocol));
         } else if (IS_UDP(transport)) {
-            fprintf(stderr, "<%d server listening (udp)\n", sfd);
+            syslog(LOG_NOTICE, "<%d server listening (udp)\n", sfd);
         } else if (c->protocol == negotiating_prot) {
-            fprintf(stderr, "<%d new auto-negotiating client connection\n",
+            syslog(LOG_NOTICE, "<%d new auto-negotiating client connection\n",
                     sfd);
         } else if (c->protocol == ascii_prot) {
-            fprintf(stderr, "<%d new ascii client connection.\n", sfd);
+            syslog(LOG_NOTICE, "<%d new ascii client connection.\n", sfd);
         } else if (c->protocol == binary_prot) {
-            fprintf(stderr, "<%d new binary client connection.\n", sfd);
+            syslog(LOG_NOTICE, "<%d new binary client connection.\n", sfd);
         } else {
-            fprintf(stderr, "<%d new unknown (%d) client connection\n",
+            syslog(LOG_NOTICE, "<%d new unknown (%d) client connection\n",
                 sfd, c->protocol);
             assert(false);
         }
@@ -503,7 +504,7 @@
     event_del(&c->event);
 
     if (settings.verbose > 1)
-        fprintf(stderr, "<%d connection closed.\n", c->sfd);
+        syslog(LOG_NOTICE, "<%d connection closed.\n", c->sfd);
 
     MEMCACHED_CONN_RELEASE(c->sfd);
     close(c->sfd);
@@ -608,7 +609,7 @@
 
     if (state != c->state) {
         if (settings.verbose > 2) {
-            fprintf(stderr, "%d: going from %s to %s\n",
+            syslog(LOG_NOTICE, "%d: going from %s to %s\n",
                     c->sfd, state_text(c->state),
                     state_text(state));
         }
@@ -754,14 +755,14 @@
 
     if (c->noreply) {
         if (settings.verbose > 1)
-            fprintf(stderr, ">%d NOREPLY %s\n", c->sfd, str);
+            syslog(LOG_NOTICE, ">%d NOREPLY %s\n", c->sfd, str);
         c->noreply = false;
         conn_set_state(c, conn_new_cmd);
         return;
     }
 
     if (settings.verbose > 1)
-        fprintf(stderr, ">%d %s\n", c->sfd, str);
+        syslog(LOG_NOTICE, ">%d %s\n", c->sfd, str);
 
     len = strlen(str);
     if ((len + 2) > c->wsize) {
@@ -902,14 +903,14 @@
 
     if (settings.verbose > 1) {
         int ii;
-        fprintf(stderr, ">%d Writing bin response:", c->sfd);
+        syslog(LOG_NOTICE, ">%d Writing bin response:", c->sfd);
         for (ii = 0; ii < sizeof(header->bytes); ++ii) {
             if (ii % 4 == 0) {
-                fprintf(stderr, "\n>%d  ", c->sfd);
+                syslog(LOG_NOTICE, "\n>%d  ", c->sfd);
             }
-            fprintf(stderr, " 0x%02x", header->bytes[ii]);
+            syslog(LOG_NOTICE, " 0x%02x", header->bytes[ii]);
         }
-        fprintf(stderr, "\n");
+        syslog(LOG_NOTICE, "\n");
     }
 
     add_iov(c, c->wbuf, sizeof(header->response));
@@ -950,11 +951,11 @@
     default:
         assert(false);
         errstr = "UNHANDLED ERROR";
-        fprintf(stderr, ">%d UNHANDLED ERROR: %d\n", c->sfd, err);
+        syslog(LOG_ERR, ">%d UNHANDLED ERROR: %d\n", c->sfd, err);
     }
 
     if (settings.verbose > 1) {
-        fprintf(stderr, ">%d Writing an error: %s\n", c->sfd, errstr);
+        syslog(LOG_ERR, ">%d Writing an error: %s\n", c->sfd, errstr);
     }
 
     len = strlen(errstr);
@@ -1006,12 +1007,12 @@
 
     if (settings.verbose > 1) {
         int i;
-        fprintf(stderr, "incr ");
+        syslog(LOG_NOTICE, "incr ");
 
         for (i = 0; i < nkey; i++) {
-            fprintf(stderr, "%c", key[i]);
+            syslog(LOG_NOTICE, "%c", key[i]);
         }
-        fprintf(stderr, " %lld, %llu, %d\n",
+        syslog(LOG_NOTICE, " %lld, %llu, %d\n",
                 (long long)req->message.body.delta,
                 (long long)req->message.body.initial,
                 req->message.body.expiration);
@@ -1163,11 +1164,11 @@
 
     if (settings.verbose > 1) {
         int ii;
-        fprintf(stderr, "<%d GET ", c->sfd);
+        syslog(LOG_NOTICE, "<%d GET ", c->sfd);
         for (ii = 0; ii < nkey; ++ii) {
-            fprintf(stderr, "%c", key[ii]);
+            syslog(LOG_NOTICE, "%c", key[ii]);
         }
-        fprintf(stderr, "\n");
+        syslog(LOG_NOTICE, "\n");
     }
 
     it = item_get(key, nkey);
@@ -1345,11 +1346,11 @@
 
     if (settings.verbose > 1) {
         int ii;
-        fprintf(stderr, "<%d STATS ", c->sfd);
+        syslog(LOG_NOTICE, "<%d STATS ", c->sfd);
         for (ii = 0; ii < nkey; ++ii) {
-            fprintf(stderr, "%c", subcommand[ii]);
+            syslog(LOG_NOTICE, "%c", subcommand[ii]);
         }
-        fprintf(stderr, "\n");
+        syslog(LOG_NOTICE, "\n");
     }
 
     if (nkey == 0) {
@@ -1422,13 +1423,13 @@
 
         if (nsize != c->rsize) {
             if (settings.verbose > 1) {
-                fprintf(stderr, "%d: Need to grow buffer from %lu to %lu\n",
+                syslog(LOG_NOTICE, "%d: Need to grow buffer from %lu to %lu\n",
                         c->sfd, (unsigned long)c->rsize, (unsigned long)nsize);
             }
             char *newm = realloc(c->rbuf, nsize);
             if (newm == NULL) {
                 if (settings.verbose) {
-                    fprintf(stderr, "%d: Failed to grow buffer.. closing connection\n",
+                    syslog(LOG_ERR, "%d: Failed to grow buffer.. closing connection\n",
                             c->sfd);
                 }
                 conn_set_state(c, conn_closing);
@@ -1444,7 +1445,7 @@
             memmove(c->rbuf, c->rcurr, c->rbytes);
             c->rcurr = c->rbuf;
             if (settings.verbose > 1) {
-                fprintf(stderr, "%d: Repack input buffer\n", c->sfd);
+                syslog(LOG_NOTICE, "%d: Repack input buffer\n", c->sfd);
             }
         }
     }
@@ -1458,7 +1459,7 @@
 static void handle_binary_protocol_error(conn *c) {
     write_bin_error(c, PROTOCOL_BINARY_RESPONSE_EINVAL, 0);
     if (settings.verbose) {
-        fprintf(stderr, "Protocol error (opcode %02x), close connection %d\n",
+        syslog(LOG_ERR, "Protocol error (opcode %02x), close connection %d\n",
                 c->binary_header.request.opcode, c->sfd);
     }
     c->write_and_go = conn_closing;
@@ -1476,7 +1477,7 @@
                                    NULL, 0, &c->sasl_conn);
         if (result != SASL_OK) {
             if (settings.verbose) {
-                fprintf(stderr, "Failed to initialize SASL conn.\n");
+                syslog(LOG_ERR, "Failed to initialize SASL conn.\n");
             }
             c->sasl_conn = NULL;
         }
@@ -1504,7 +1505,7 @@
     if (result != SASL_OK) {
         /* Perhaps there's a better error for this... */
         if (settings.verbose) {
-            fprintf(stderr, "Failed to list SASL mechanisms.\n");
+            syslog(LOG_ERR, "Failed to list SASL mechanisms.\n");
         }
         write_bin_error(c, PROTOCOL_BINARY_RESPONSE_AUTH_ERROR, 0);
         return;
@@ -1566,7 +1567,7 @@
     mech[nkey] = 0x00;
 
     if (settings.verbose)
-        fprintf(stderr, "mech:  ``%s'' with %d bytes of data\n", mech, vlen);
+        syslog(LOG_NOTICE, "mech:  ``%s'' with %d bytes of data\n", mech, vlen);
 
     const char *challenge = vlen == 0 ? NULL : ITEM_data((item*) c->item);
 
@@ -1588,7 +1589,7 @@
         /* This code is pretty much impossible, but makes the compiler
            happier */
         if (settings.verbose) {
-            fprintf(stderr, "Unhandled command %d with challenge %s\n",
+            syslog(LOG_NOTICE, "Unhandled command %d with challenge %s\n",
                     c->cmd, challenge);
         }
         break;
@@ -1597,7 +1598,7 @@
     item_unlink(c->item);
 
     if (settings.verbose) {
-        fprintf(stderr, "sasl result code:  %d\n", result);
+        syslog(LOG_NOTICE, "sasl result code:  %d\n", result);
     }
 
     switch(result) {
@@ -1617,7 +1618,7 @@
         break;
     default:
         if (settings.verbose)
-            fprintf(stderr, "Unknown sasl response:  %d\n", result);
+            syslog(LOG_NOTICE, "Unknown sasl response:  %d\n", result);
         write_bin_error(c, PROTOCOL_BINARY_RESPONSE_AUTH_ERROR, 0);
         pthread_mutex_lock(&c->thread->stats.mutex);
         c->thread->stats.auth_cmds++;
@@ -1646,7 +1647,7 @@
     }
 
     if (settings.verbose > 1) {
-        fprintf(stderr, "authenticated() in cmd 0x%02x is %s\n",
+        syslog(LOG_NOTICE, "authenticated() in cmd 0x%02x is %s\n",
                 c->cmd, rv ? "true" : "false");
     }
 
@@ -1842,18 +1843,18 @@
     if (settings.verbose > 1) {
         int ii;
         if (c->cmd == PROTOCOL_BINARY_CMD_ADD) {
-            fprintf(stderr, "<%d ADD ", c->sfd);
+            syslog(LOG_NOTICE, "<%d ADD ", c->sfd);
         } else if (c->cmd == PROTOCOL_BINARY_CMD_SET) {
-            fprintf(stderr, "<%d SET ", c->sfd);
+            syslog(LOG_NOTICE, "<%d SET ", c->sfd);
         } else {
-            fprintf(stderr, "<%d REPLACE ", c->sfd);
+            syslog(LOG_NOTICE, "<%d REPLACE ", c->sfd);
         }
         for (ii = 0; ii < nkey; ++ii) {
-            fprintf(stderr, "%c", key[ii]);
+            syslog(LOG_NOTICE, "%c", key[ii]);
         }
 
-        fprintf(stderr, " Value len is %d", vlen);
-        fprintf(stderr, "\n");
+        syslog(LOG_NOTICE, " Value len is %d", vlen);
+        syslog(LOG_NOTICE, "\n");
     }
 
     if (settings.detail_enabled) {
@@ -1925,7 +1926,7 @@
     vlen = c->binary_header.request.bodylen - nkey;
 
     if (settings.verbose > 1) {
-        fprintf(stderr, "Value len is %d\n", vlen);
+        syslog(LOG_NOTICE, "Value len is %d\n", vlen);
     }
 
     if (settings.detail_enabled) {
@@ -2000,7 +2001,7 @@
     assert(c != NULL);
 
     if (settings.verbose > 1) {
-        fprintf(stderr, "Deleting %s\n", key);
+        syslog(LOG_NOTICE, "Deleting %s\n", key);
     }
 
     if (settings.detail_enabled) {
@@ -2061,7 +2062,7 @@
         process_bin_complete_sasl_auth(c);
         break;
     default:
-        fprintf(stderr, "Not handling substate %d\n", c->substate);
+        syslog(LOG_ERR, "Not handling substate %d\n", c->substate);
         assert(0);
     }
 }
@@ -2139,7 +2140,7 @@
             pthread_mutex_unlock(&c->thread->stats.mutex);
 
             if(settings.verbose > 1) {
-                fprintf(stderr, "CAS:  failure: expected %llu, got %llu\n",
+                syslog(LOG_ERR, "CAS:  failure: expected %llu, got %llu\n",
                         (unsigned long long)ITEM_get_cas(old_it),
                         (unsigned long long)ITEM_get_cas(it));
             }
@@ -2614,7 +2615,7 @@
 
 
                 if (settings.verbose > 1)
-                    fprintf(stderr, ">%d sending key %s\n", c->sfd, ITEM_key(it));
+                    syslog(LOG_ERR, ">%d sending key %s\n", c->sfd, ITEM_key(it));
 
                 /* item_get() has incremented it->refcount for us */
                 pthread_mutex_lock(&c->thread->stats.mutex);
@@ -2655,7 +2656,7 @@
     }
 
     if (settings.verbose > 1)
-        fprintf(stderr, ">%d END\n", c->sfd);
+        syslog(LOG_NOTICE, ">%d END\n", c->sfd);
 
     /*
         If the loop was terminated because of out-of-memory, it is not
@@ -2951,7 +2952,7 @@
     MEMCACHED_PROCESS_COMMAND_START(c->sfd, c->rcurr, c->rbytes);
 
     if (settings.verbose > 1)
-        fprintf(stderr, "<%d %s\n", c->sfd, command);
+        syslog(LOG_NOTICE, "<%d %s\n", c->sfd, command);
 
     /*
      * for commands set/add/replace, we build an item and read the data
@@ -3075,7 +3076,7 @@
         }
 
         if (settings.verbose > 1) {
-            fprintf(stderr, "%d: Client using the %s protocol\n", c->sfd,
+            syslog(LOG_NOTICE, "%d: Client using the %s protocol\n", c->sfd,
                     prot_text(c->protocol));
         }
     }
@@ -3092,7 +3093,7 @@
                 memmove(c->rbuf, c->rcurr, c->rbytes);
                 c->rcurr = c->rbuf;
                 if (settings.verbose > 1) {
-                    fprintf(stderr, "%d: Realign input buffer\n", c->sfd);
+                    syslog(LOG_NOTICE, "%d: Realign input buffer\n", c->sfd);
                 }
             }
 #endif
@@ -3102,14 +3103,14 @@
             if (settings.verbose > 1) {
                 /* Dump the packet before we convert it to host order */
                 int ii;
-                fprintf(stderr, "<%d Read binary protocol data:", c->sfd);
+                syslog(LOG_NOTICE, "<%d Read binary protocol data:", c->sfd);
                 for (ii = 0; ii < sizeof(req->bytes); ++ii) {
                     if (ii % 4 == 0) {
-                        fprintf(stderr, "\n<%d   ", c->sfd);
+                        syslog(LOG_NOTICE, "\n<%d   ", c->sfd);
                     }
-                    fprintf(stderr, " 0x%02x", req->bytes[ii]);
+                    syslog(LOG_NOTICE, " 0x%02x", req->bytes[ii]);
                 }
-                fprintf(stderr, "\n");
+                syslog(LOG_NOTICE, "\n");
             }
 
             c->binary_header = *req;
@@ -3119,7 +3120,7 @@
 
             if (c->binary_header.request.magic != PROTOCOL_BINARY_REQ) {
                 if (settings.verbose) {
-                    fprintf(stderr, "Invalid magic:  %x\n",
+                    syslog(LOG_ERR, "Invalid magic:  %x\n",
                             c->binary_header.request.magic);
                 }
                 conn_set_state(c, conn_closing);
@@ -3262,7 +3263,7 @@
             char *new_rbuf = realloc(c->rbuf, c->rsize * 2);
             if (!new_rbuf) {
                 if (settings.verbose > 0)
-                    fprintf(stderr, "Couldn't realloc input buffer\n");
+                    syslog(LOG_ERR, "Couldn't realloc input buffer\n");
                 c->rbytes = 0; /* ignore what we read */
                 out_string(c, "SERVER_ERROR out of memory reading request");
                 c->write_and_go = conn_closing;
@@ -3392,7 +3393,7 @@
         if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
             if (!update_event(c, EV_WRITE | EV_PERSIST)) {
                 if (settings.verbose > 0)
-                    fprintf(stderr, "Couldn't update event\n");
+                    syslog(LOG_ERR, "Couldn't update event\n");
                 conn_set_state(c, conn_closing);
                 return TRANSMIT_HARD_ERROR;
             }
@@ -3434,7 +3435,7 @@
                     stop = true;
                 } else if (errno == EMFILE) {
                     if (settings.verbose > 0)
-                        fprintf(stderr, "Too many open connections\n");
+                        syslog(LOG_ERR, "Too many open connections\n");
                     accept_new_conns(false);
                     stop = true;
                 } else {
@@ -3458,7 +3459,7 @@
         case conn_waiting:
             if (!update_event(c, EV_READ | EV_PERSIST)) {
                 if (settings.verbose > 0)
-                    fprintf(stderr, "Couldn't update event\n");
+                    syslog(LOG_ERR, "Couldn't update event\n");
                 conn_set_state(c, conn_closing);
                 break;
             }
@@ -3514,7 +3515,7 @@
                     */
                     if (!update_event(c, EV_WRITE | EV_PERSIST)) {
                         if (settings.verbose > 0)
-                            fprintf(stderr, "Couldn't update event\n");
+                            syslog(LOG_ERR, "Couldn't update event\n");
                         conn_set_state(c, conn_closing);
                     }
                 }
@@ -3562,7 +3563,7 @@
             if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                 if (!update_event(c, EV_READ | EV_PERSIST)) {
                     if (settings.verbose > 0)
-                        fprintf(stderr, "Couldn't update event\n");
+                        syslog(LOG_ERR, "Couldn't update event\n");
                     conn_set_state(c, conn_closing);
                     break;
                 }
@@ -3571,7 +3572,7 @@
             }
             /* otherwise we have a real error, on which we close the connection */
             if (settings.verbose > 0) {
-                fprintf(stderr, "Failed to read, and not due to blocking:\n"
+                syslog(LOG_ERR, "Failed to read, and not due to blocking:\n"
                         "errno: %d %s \n"
                         "rcurr=%lx ritem=%lx rbuf=%lx rlbytes=%d rsize=%d\n",
                         errno, strerror(errno),
@@ -3613,7 +3614,7 @@
             if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                 if (!update_event(c, EV_READ | EV_PERSIST)) {
                     if (settings.verbose > 0)
-                        fprintf(stderr, "Couldn't update event\n");
+                        syslog(LOG_ERR, "Couldn't update event\n");
                     conn_set_state(c, conn_closing);
                     break;
                 }
@@ -3622,7 +3623,7 @@
             }
             /* otherwise we have a real error, on which we close the connection */
             if (settings.verbose > 0)
-                fprintf(stderr, "Failed to read, and not due to blocking\n");
+                syslog(LOG_ERR, "Failed to read, and not due to blocking\n");
             conn_set_state(c, conn_closing);
             break;
 
@@ -3635,7 +3636,7 @@
             if (c->iovused == 0 || (IS_UDP(c->transport) && c->iovused == 1)) {
                 if (add_iov(c, c->wcurr, c->wbytes) != 0) {
                     if (settings.verbose > 0)
-                        fprintf(stderr, "Couldn't build response\n");
+                        syslog(LOG_ERR, "Couldn't build response\n");
                     conn_set_state(c, conn_closing);
                     break;
                 }
@@ -3646,7 +3647,7 @@
         case conn_mwrite:
           if (IS_UDP(c->transport) && c->msgcurr == 0 && build_udp_headers(c) != 0) {
             if (settings.verbose > 0)
-              fprintf(stderr, "Failed to build UDP headers\n");
+              syslog(LOG_ERR, "Failed to build UDP headers\n");
             conn_set_state(c, conn_closing);
             break;
           }
@@ -3680,7 +3681,7 @@
                     conn_set_state(c, c->write_and_go);
                 } else {
                     if (settings.verbose > 0)
-                        fprintf(stderr, "Unexpected state %d\n", c->state);
+                        syslog(LOG_ERR, "Unexpected state %d\n", c->state);
                     conn_set_state(c, conn_closing);
                 }
                 break;
@@ -3723,7 +3724,7 @@
     /* sanity */
     if (fd != c->sfd) {
         if (settings.verbose > 0)
-            fprintf(stderr, "Catastrophic: event fd doesn't match conn fd!\n");
+            syslog(LOG_ERR, "Catastrophic: event fd doesn't match conn fd!\n");
         conn_close(c);
         return;
     }
@@ -3783,7 +3784,7 @@
     }
 
     if (settings.verbose > 1)
-        fprintf(stderr, "<%d send buffer was %d, now %d\n", sfd, old_size, last_good);
+        syslog(LOG_NOTICE, "<%d send buffer was %d, now %d\n", sfd, old_size, last_good);
 }
 
 /**
@@ -3816,7 +3817,7 @@
     error= getaddrinfo(settings.inter, port_buf, &hints, &ai);
     if (error != 0) {
         if (error != EAI_SYSTEM)
-          fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
+          syslog(LOG_ERR, "getaddrinfo(): %s\n", gai_strerror(error));
         else
           perror("getaddrinfo()");
         return 1;
@@ -3910,7 +3911,7 @@
             if (!(listen_conn_add = conn_new(sfd, conn_listening,
                                              EV_READ | EV_PERSIST, 1,
                                              transport, main_base))) {
-                fprintf(stderr, "failed to create listening connection\n");
+                syslog(LOG_ERR, "failed to create listening connection\n");
                 exit(EXIT_FAILURE);
             }
             listen_conn_add->next = listen_conn;
@@ -3995,7 +3996,7 @@
     if (!(listen_conn = conn_new(sfd, conn_listening,
                                  EV_READ | EV_PERSIST, 1,
                                  local_transport, main_base))) {
-        fprintf(stderr, "failed to create listening connection\n");
+        syslog(LOG_ERR, "failed to create listening connection\n");
         exit(EXIT_FAILURE);
     }
 
@@ -4168,13 +4169,13 @@
         return;
 
     if ((fp = fopen(pid_file, "w")) == NULL) {
-        fprintf(stderr, "Could not open the pid file %s for writing\n", pid_file);
+        syslog(LOG_ERR, "Could not open the pid file %s for writing\n", pid_file);
         return;
     }
 
     fprintf(fp,"%ld\n", (long)pid);
     if (fclose(fp) == -1) {
-        fprintf(stderr, "Could not close the pid file %s.\n", pid_file);
+        syslog(LOG_ERR, "Could not close the pid file %s.\n", pid_file);
         return;
     }
 }
@@ -4184,7 +4185,7 @@
       return;
 
   if (unlink(pid_file) != 0) {
-      fprintf(stderr, "Could not remove the pid file %s.\n", pid_file);
+      syslog(LOG_ERR, "Could not remove the pid file %s.\n", pid_file);
   }
 
 }
@@ -4231,16 +4232,16 @@
         arg.mha_cmd = MHA_MAPSIZE_BSSBRK;
 
         if (memcntl(0, 0, MC_HAT_ADVISE, (caddr_t)&arg, 0, 0) == -1) {
-            fprintf(stderr, "Failed to set large pages: %s\n",
+            syslog(LOG_ERR, "Failed to set large pages: %s\n",
                     strerror(errno));
-            fprintf(stderr, "Will use default page size\n");
+            syslog(LOG_ERR, "Will use default page size\n");
         } else {
             ret = 0;
         }
     } else {
-        fprintf(stderr, "Failed to get supported pagesizes: %s\n",
+        syslog(LOG_ERR, "Failed to get supported pagesizes: %s\n",
                 strerror(errno));
-        fprintf(stderr, "Will use default page size\n");
+        syslog(LOG_ERR, "Will use default page size\n");
     }
 
     return ret;
@@ -4279,6 +4280,9 @@
     /* set stderr non-buffering (for running under, say, daemontools) */
     setbuf(stderr, NULL);
 
+    /* add LOG_PERROR as we are not daemonized yet */
+    openlog("memcached", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
+
     /* process arguments */
     while (-1 != (c = getopt(argc, argv,
           "a:"  /* access mask for unix socket */
@@ -4358,7 +4362,7 @@
         case 'R':
             settings.reqs_per_event = atoi(optarg);
             if (settings.reqs_per_event == 0) {
-                fprintf(stderr, "Number of requests per event must be greater than 0\n");
+                syslog(LOG_ERR, "Number of requests per event must be greater than 0\n");
                 return 1;
             }
             break;
@@ -4371,21 +4375,21 @@
         case 'f':
             settings.factor = atof(optarg);
             if (settings.factor <= 1.0) {
-                fprintf(stderr, "Factor must be greater than 1\n");
+                syslog(LOG_ERR, "Factor must be greater than 1\n");
                 return 1;
             }
             break;
         case 'n':
             settings.chunk_size = atoi(optarg);
             if (settings.chunk_size == 0) {
-                fprintf(stderr, "Chunk size must be greater than 0\n");
+                syslog(LOG_ERR, "Chunk size must be greater than 0\n");
                 return 1;
             }
             break;
         case 't':
             settings.num_threads = atoi(optarg);
             if (settings.num_threads <= 0) {
-                fprintf(stderr, "Number of threads must be greater than 0\n");
+                syslog(LOG_ERR, "Number of threads must be greater than 0\n");
                 return 1;
             }
             /* There're other problems when you get above 64 threads.
@@ -4393,7 +4397,7 @@
              * default.
              */
             if (settings.num_threads > 64) {
-                fprintf(stderr, "WARNING: Setting a high number of worker"
+                syslog(LOG_ERR, "WARNING: Setting a high number of worker"
                                 "threads is not recommended.\n"
                                 " Set this value to the number of cores in"
                                 " your machine or less.\n");
@@ -4401,7 +4405,7 @@
             break;
         case 'D':
             if (! optarg || ! optarg[0]) {
-                fprintf(stderr, "No delimiter specified\n");
+                syslog(LOG_ERR, "No delimiter specified\n");
                 return 1;
             }
             settings.prefix_delimiter = optarg[0];
@@ -4427,7 +4431,7 @@
             } else if (strcmp(optarg, "ascii") == 0) {
                 settings.binding_protocol = ascii_prot;
             } else {
-                fprintf(stderr, "Invalid value for binding protocol: %s\n"
+                syslog(LOG_ERR, "Invalid value for binding protocol: %s\n"
                         " -- should be one of auto, binary, or ascii\n", optarg);
                 exit(EX_USAGE);
             }
@@ -4447,15 +4451,15 @@
                 settings.item_size_max = atoi(optarg);
             }
             if (settings.item_size_max < 1024) {
-                fprintf(stderr, "Item max size cannot be less than 1024 bytes.\n");
+                syslog(LOG_ERR, "Item max size cannot be less than 1024 bytes.\n");
                 return 1;
             }
             if (settings.item_size_max > 1024 * 1024 * 128) {
-                fprintf(stderr, "Cannot set item size limit higher than 128 mb.\n");
+                syslog(LOG_ERR, "Cannot set item size limit higher than 128 mb.\n");
                 return 1;
             }
             if (settings.item_size_max > 1024 * 1024) {
-                fprintf(stderr, "WARNING: Setting item max size above 1MB is not"
+                syslog(LOG_ERR, "WARNING: Setting item max size above 1MB is not"
                     " recommended!\n"
                     " Raising this limit increases the minimum memory requirements\n"
                     " and will decrease your memory efficiency.\n"
@@ -4464,13 +4468,13 @@
             break;
         case 'S': /* set Sasl authentication to true. Default is false */
 #ifndef ENABLE_SASL
-            fprintf(stderr, "This server is not built with SASL support.\n");
+            syslog(LOG_ERR, "This server is not built with SASL support.\n");
             exit(EX_USAGE);
 #endif
             settings.sasl = true;
             break;
         default:
-            fprintf(stderr, "Illegal argument \"%c\"\n", c);
+            syslog(LOG_ERR, "Illegal argument \"%c\"\n", c);
             return 1;
         }
     }
@@ -4480,7 +4484,7 @@
             settings.binding_protocol = binary_prot;
         } else {
             if (settings.binding_protocol != binary_prot) {
-                fprintf(stderr, "ERROR: You cannot allow the ASCII protocol while using SASL.\n");
+                syslog(LOG_ERR, "ERROR: You cannot allow the ASCII protocol while using SASL.\n");
                 exit(EX_USAGE);
             }
         }
@@ -4513,7 +4517,7 @@
          */
 
         if ((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_cur == 0) {
-            fprintf(stderr, "failed to ensure corefile creation\n");
+            syslog(LOG_ERR, "failed to ensure corefile creation\n");
             exit(EX_OSERR);
         }
     }
@@ -4524,7 +4528,7 @@
      */
 
     if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) {
-        fprintf(stderr, "failed to getrlimit number of files\n");
+        syslog(LOG_ERR, "failed to getrlimit number of files\n");
         exit(EX_OSERR);
     } else {
         int maxfiles = settings.maxconns;
@@ -4533,7 +4537,7 @@
         if (rlim.rlim_max < rlim.rlim_cur)
             rlim.rlim_max = rlim.rlim_cur;
         if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
-            fprintf(stderr, "failed to set rlimit for open files. Try running as root or requesting smaller maxconns value.\n");
+            syslog(LOG_ERR, "failed to set rlimit for open files. Try running as root or requesting smaller maxconns value.\n");
             exit(EX_OSERR);
         }
     }
@@ -4541,15 +4545,15 @@
     /* lose root privileges if we have them */
     if (getuid() == 0 || geteuid() == 0) {
         if (username == 0 || *username == '\0') {
-            fprintf(stderr, "can't run as root without the -u switch\n");
+            syslog(LOG_ERR, "can't run as root without the -u switch\n");
             exit(EX_USAGE);
         }
         if ((pw = getpwnam(username)) == 0) {
-            fprintf(stderr, "can't find the user %s to switch to\n", username);
+            syslog(LOG_ERR, "can't find the user %s to switch to\n", username);
             exit(EX_NOUSER);
         }
         if (setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0) {
-            fprintf(stderr, "failed to assume identity of user %s\n", username);
+            syslog(LOG_ERR, "failed to assume identity of user %s\n", username);
             exit(EX_OSERR);
         }
     }
@@ -4566,7 +4570,7 @@
             perror("Failed to ignore SIGHUP");
         }
         if (daemonize(maxcore, settings.verbose) == -1) {
-            fprintf(stderr, "failed to daemon() in order to daemonize\n");
+            syslog(LOG_ERR, "failed to daemon() in order to daemonize\n");
             exit(EXIT_FAILURE);
         }
     }
@@ -4576,11 +4580,11 @@
 #ifdef HAVE_MLOCKALL
         int res = mlockall(MCL_CURRENT | MCL_FUTURE);
         if (res != 0) {
-            fprintf(stderr, "warning: -k invalid, mlockall() failed: %s\n",
+            syslog(LOG_ERR, "warning: -k invalid, mlockall() failed: %s\n",
                     strerror(errno));
         }
 #else
-        fprintf(stderr, "warning: -k invalid, mlockall() not supported on this platform.  proceeding without.\n");
+        syslog(LOG_ERR, "warning: -k invalid, mlockall() not supported on this platform.  proceeding without.\n");
 #endif
     }
 
@@ -4639,7 +4643,7 @@
 
             portnumber_file = fopen(temp_portnumber_filename, "a");
             if (portnumber_file == NULL) {
-                fprintf(stderr, "Failed to open \"%s\": %s\n",
+                syslog(LOG_ERR, "Failed to open \"%s\": %s\n",
                         temp_portnumber_filename, strerror(errno));
             }
         }
