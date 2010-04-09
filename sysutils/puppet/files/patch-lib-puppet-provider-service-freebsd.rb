--- lib/puppet/provider/service/freebsd.rb.orig	2009-04-12 14:12:33.202241322 +0000
+++ lib/puppet/provider/service/freebsd.rb	2009-04-12 14:23:25.452014547 +0000
@@ -15,10 +15,14 @@
     def self.defpath
         superclass.defpath
     end
+
+    def rcfile
+        File.join(@@rcconf_dir, @model[:name]).gsub(/-/, "_")
+    end
     
     # remove service file from rc.conf.d to disable it
     def disable
-        rcfile = File.join(@@rcconf_dir, @model[:name])
+        rcfile = self.rcfile
         if File.exists?(rcfile)
             File.delete(rcfile)
         end
@@ -26,7 +30,7 @@
     
     # if the service file exists in rc.conf.d then it's already enabled
     def enabled?
-        rcfile = File.join(@@rcconf_dir, @model[:name])
+        rcfile = self.rcfile
         if File.exists?(rcfile)
             return :true
         end
@@ -40,8 +44,8 @@
         if not File.exists?(@@rcconf_dir)
             Dir.mkdir(@@rcconf_dir)
         end
-        rcfile = File.join(@@rcconf_dir, @model[:name])
-        open(rcfile, 'w') { |f| f << "%s_enable=\"YES\"\n" % @model[:name] }
+        rcfile = self.rcfile
+        open(rcfile, 'w') { |f| f << "%s_enable=\"YES\"\n" % @model[:name].gsub(/-/, "_") }
     end
     
     # Override stop/start commands to use one<cmd>'s and the avoid race condition
