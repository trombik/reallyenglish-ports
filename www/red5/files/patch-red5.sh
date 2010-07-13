--- ./red5.sh.orig	2010-02-22 05:27:53.000000000 +0900
+++ ./red5.sh	2010-07-12 13:41:51.000000000 +0900
@@ -1,7 +1,7 @@
-#!/bin/bash
+#!/bin/sh
 
 if [ -z "$RED5_HOME" ]; then 
-  export RED5_HOME=`pwd`; 
+  export RED5_HOME=$( dirname $0 )
 fi
 
 P=":" # The default classpath separator
