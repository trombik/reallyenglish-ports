Use DERIVE with min=0 to avoid huge spike when the value is reset

--- gmetad/rrd_helpers.c.orig	2010-06-07 19:46:40.000000000 +0900
+++ gmetad/rrd_helpers.c	2010-06-07 19:51:03.000000000 +0900
@@ -82,7 +82,7 @@
 
    switch( slope) {
    case GANGLIA_SLOPE_POSITIVE:
-     data_source_type = "COUNTER";
+     data_source_type = "DERIVE";
      break;
 
    case GANGLIA_SLOPE_ZERO:
@@ -101,7 +101,7 @@
    argv[argc++] = "--start";
    sprintf(start, "%u", process_time-1);
    argv[argc++] = start;
-   sprintf(sum,"DS:sum:%s:%d:U:U",
+   sprintf(sum,"DS:sum:%s:%d:0:U",
            data_source_type,
            heartbeat);
    argv[argc++] = sum;
