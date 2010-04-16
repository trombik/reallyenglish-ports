# Ports collection makefile for:	rubygem-chef
# Date created:				19 Mar 2010
# Whom:					Renaud Chaput <renchap@cocoa-x.com>
#
# $FreeBSD$
#

PORTNAME=	chef
PORTVERSION=	0.8.8
CATEGORIES=	sysutils
MASTER_SITES=	http://gemcutter.org/gems/

MAINTAINER=	renchap@cocoa-x.com
COMMENT=	A systems integration framework. Contains the client part

BUILD_DEPENDS=	rubygem-mixlib-log>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-log \
		rubygem-mixlib-config>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-config \
		rubygem-mixlib-cli>=1.1.0:${PORTSDIR}/devel/rubygem-mixlib-cli \
		rubygem-mixlib-authentication>=1.1.2:${PORTSDIR}/devel/rubygem-mixlib-authentication \
		rubygem-extlib>=0.9.14:${PORTSDIR}/devel/rubygem-extlib \
		rubygem-json>=1.1.9:${PORTSDIR}/devel/rubygem-json \
		rubygem-moneta>=0.6.0:${PORTSDIR}/devel/rubygem-moneta \
		rubygem-erubis>=0.0.0:${PORTSDIR}/www/rubygem-erubis \
		rubygem-bunny>=0.6.0:${PORTSDIR}/net/rubygem-bunny \
		rubygem-ohai>=0.5.0:${PORTSDIR}/sysutils/rubygem-ohai
RUN_DEPENDS=	${BUILD_DEPENDS}

USE_RUBY=	yes
USE_RUBYGEMS=	yes
RUBYGEM_AUTOPLIST=	yes

PLIST_FILES=	bin/chef-client \
		bin/shef \
		bin/knife

SUB_LIST=	RUBY=${RUBY}
USE_RC_SUBR=	chef_client

# we care about not passing -A to allow HTTP redirects
FETCH_ARGS=     -pRr

.include <bsd.port.mk>
