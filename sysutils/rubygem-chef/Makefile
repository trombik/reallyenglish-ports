# Ports collection makefile for:	rubygem-chef
# Date created:				19 Mar 2010
# Whom:					Renaud Chaput <renchap@cocoa-x.com>
#
# $FreeBSD$
#

PORTNAME=	chef
PORTVERSION=	0.8.16
PORTREVISION=	3
CATEGORIES=	sysutils
MASTER_SITES=	RG

MAINTAINER=	renchap@cocoa-x.com
COMMENT=	A systems integration framework. Contains the client part

RUN_DEPENDS=	rubygem-mixlib-log>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-log \
		rubygem-mixlib-config>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-config \
		rubygem-mixlib-cli>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-cli \
		rubygem-mixlib-authentication>=1.1.2:${PORTSDIR}/devel/rubygem-mixlib-authentication \
		rubygem-extlib>=0.9.14:${PORTSDIR}/devel/rubygem-extlib \
		rubygem-json>=1.1.9:${PORTSDIR}/devel/rubygem-json \
		rubygem-moneta>=0.6.0:${PORTSDIR}/devel/rubygem-moneta \
		rubygem-erubis>=0.0.0:${PORTSDIR}/www/rubygem-erubis \
		rubygem-bunny>=0.6.0:${PORTSDIR}/net/rubygem-bunny \
		rubygem-ohai>=0.5.0:${PORTSDIR}/sysutils/rubygem-ohai \
		bsdadminscripts>0:${PORTSDIR}/sysutils/bsdadminscripts

USE_RUBY=	yes
USE_RUBYGEMS=	yes
RUBYGEM_AUTOPLIST=	yes

PLIST_FILES=	bin/chef-client \
		bin/shef \
		bin/chef-solo \
		bin/knife

SUB_LIST=	RUBY=${RUBY}
USE_RC_SUBR=	chef_client

post-install:
	@${PATCH} -p0 -d ${PREFIX}/${GEM_LIB_DIR} < ${PATCHDIR}/lib-chef-provider-service-freebsd.rb.patch
	@${PATCH} -p0 -d ${PREFIX}/${GEM_LIB_DIR} < ${PATCHDIR}/lib-chef-provider-package-pkgupgrade.rb.patch
	@${PATCH} -p0 -d ${PREFIX}/${GEM_LIB_DIR} < ${PATCHDIR}/lib-chef-providers.rb
	@${PATCH} -p0 -d ${PREFIX}/${GEM_LIB_DIR} < ${PATCHDIR}/lib-chef-platform.patch
	@${FIND} ${PREFIX}/${GEM_LIB_DIR} -name "*.orig" -delete

.include <bsd.port.mk>
