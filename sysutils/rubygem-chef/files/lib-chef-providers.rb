--- lib/chef/providers.rb
+++ lib/chef/providers.rb
@@ -57,6 +57,7 @@ require 'chef/provider/package/zypper'
 require 'chef/provider/service/arch'
 require 'chef/provider/service/debian'
 require 'chef/provider/service/freebsd'
+require 'chef/provider/package/pkgupgrade'
 require 'chef/provider/service/gentoo'
 require 'chef/provider/service/init'
 require 'chef/provider/service/redhat'
