# Ports collection makefile for:	rubygem-mixlib-config
# Date created:				18 Mar 2010
# Whom:					renchap@cocoa-x.com
#
# $FreeBSD$
#

PORTNAME=	mixlib-config
PORTVERSION=	1.1.2
CATEGORIES=	devel
MASTER_SITES=	http://gemcutter.org/gems/

MAINTAINER=	renchap@cocoa-x.com
COMMENT=	A class based config mixin, similar to the one found in Chef

USE_RUBY=	yes
USE_RUBYGEMS=	yes
RUBYGEM_AUTOPLIST=  yes

# we care about not passing -A to avoid HTTP redirect
FETCH_ARGS=     -pRr

.include <bsd.port.mk>
