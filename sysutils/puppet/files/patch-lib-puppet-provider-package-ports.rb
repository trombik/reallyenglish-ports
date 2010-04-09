--- lib/puppet/provider/package/ports.rb.orig	2009-08-11 18:16:09.000000000 +0900
+++ lib/puppet/provider/package/ports.rb	2009-08-11 18:14:52.000000000 +0900
@@ -83,6 +83,27 @@
             end
         end
 
+        # apache22  => apache
+        # perl5.10  => perl
+        # openldap24-client => openldap-client
+        name_without_version = ''
+        name_with_client     = ''
+        if name =~ /([^\.]+)[0-9\.]+$/
+            name_without_version = $1
+        end
+        if name =~ /^([^0-9]+)\d+-(server|client)$/
+            name_with_client = $1 + "-" + $2
+        end
+        Puppet.debug "name_with_client is %s" % name_with_client
+        self.class.instances.each do |instance|
+            if instance.name == name_without_version
+                return instance.properties
+            end
+            if instance.name == name_with_client
+                return instance.properties
+            end
+        end
+
         return nil
     end
 
