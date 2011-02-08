# New ports collection makefile for:	reallyenglish-munin-plugins
# Date created:		2011-02-05
# Whom:			Tomoyuki Sakurai <tomoyukis@reallyenglish.com>
#
# $FreeBSD$
#

PORTNAME=	reallyenglish-munin-plugins
PORTVERSION=	1.1
#PORTREVISION=	0
CATEGORIES=	sysutils
MASTER_SITES=	http://dist.reallyenglish.com/${PORTNAME}/
EXTRACT_SUFX=	.tgz

MAINTAINER=	tomoyukis@reallyenglish.com
COMMENT=	3rd-party munin plugins for reallyenglish

RUN_DEPENDS=	p5-libwww>0:${PORTSDIR}/www/p5-libwww

MAKE_ARGS=	DESTDIR=${PREFIX}

# included plugins:
#
# unbound: http://www.unbound.net/documentation/howto_statistics.html
# http://exchange.munin-monitoring.org/plugins/zfs-filesystem-graph/details
# http://anders.fupp.net/plugins/munin/open_sockets
# http://trull.org/~alex/src/FreeBSD/muninplugins
.include <bsd.port.mk>
