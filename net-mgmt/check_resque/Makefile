# New ports collection makefile for:	check_resque
# Date created:		2011-02-16
# Whom:			Tomoyuki Sakurai <tomoyukis@reallyenglish.com>
#
# $FreeBSD$
#

PORTNAME=	check_resque
PORTVERSION=	1.1
CATEGORIES=	net-mgmt
MASTER_SITES=	https://download.github.com/
DISTNAME=	trombik-${PORTNAME}-${PORTVERSION}-0-g44fba07

MAINTAINER=	tomoyukis@smer.jp.reallyenglish.com
COMMENT=	Nagios/Icinga plugins for resque

RUN_DEPENDS=	${SITE_PERL}/Nagios/Plugin.pm:${PORTSDIR}/net-mgmt/p5-Nagios-Plugin \
	${SITE_PERL}/Redis.pm:${PORTSDIR}/databases/p5-Redis

USE_PERL5=	Yes
WRKSRC=	${WRKDIR}/trombik-${PORTNAME}-d7dbe97

MAKE_ARGS=	DESTDIR=${PREFIX}
LICENSE=	BSD

.include <bsd.port.mk>
