--- ./red5-shutdown.sh.orig	2010-07-13 23:45:36.000000000 +0900
+++ ./red5-shutdown.sh	2010-07-13 23:46:58.000000000 +0900
@@ -1,7 +1,7 @@
-#!/bin/bash
+#!/bin/sh
 
 if [ -z "$RED5_HOME" ]; then 
-  export RED5_HOME=`pwd`; 
+  export RED5_HOME=$( dirname $0 ); 
 fi
 
 # JMX options
