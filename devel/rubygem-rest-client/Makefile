# New ports collection makefile for:	rubygem-rest-client
# Date created:		2010-11-29
# Whom:			Tomoyuki Sakurai <tomoyukis@smer.jp.reallyenglish.com>
#
# $FreeBSD$
#

PORTNAME=	rest-client
PORTVERSION=	1.6.1
#PORTREVISION=	0
#PORTEPOCH=	0
CATEGORIES=	devel rubygems
MASTER_SITES=	RG
#MASTER_SITE_SUBDIR=
#PKGNAMEPREFIX=
#PKGNAMESUFFIX=
#DISTNAME=
#EXTRACT_SUFX=
#DISTFILES=
#DIST_SUBDIR=	${PORTNAME}
#EXTRACT_ONLY=

MAINTAINER=	tomoyukis@reallyenglish.com
COMMENT=	simple Simple HTTP and REST client for Ruby

RUN_DEPENDS=	rubygem-mime-types>0:${PORTSDIR}/misc/rubygem-mime-types

USE_RUBY=	yes
USE_RUBYGEMS=	yes
RUBYGEM_AUTOPLIST=	yes
PLIST_FILES=	bin/restclient

.include <bsd.port.pre.mk>
.include <bsd.port.post.mk>
