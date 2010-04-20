--- ezjail.sh	2010/02/18 12:22:26	1.58
+++ ezjail.sh	2010/03/16 23:48:18	1.59
@@ -1,5 +1,5 @@
 #!/bin/sh
-# $Id: ezjail.sh,v 1.58 2010/02/18 12:22:26 cryx Exp $
+# $Id: ezjail.sh,v 1.59 2010/03/16 23:48:18 cryx Exp $
 #
 # $FreeBSD$
 #
@@ -104,7 +104,7 @@ do_cmd()
   [ "${ezjail_pass}" ] && sh /etc/rc.d/jail one${action%crypto} ${ezjail_pass}
 
   # Configure settings that need to be done after the jail has been started
-  if [ "${action}" = "start" ]; then
+  if [ "${action%crypto}" = "start" -o "${action}" = "restart" ]; then
     for ezjail in ${ezjail_list}; do
       ezjail_safename=`echo -n "${ezjail}" | tr -c '[:alnum:]' _`
       # Get the JID of the jail
