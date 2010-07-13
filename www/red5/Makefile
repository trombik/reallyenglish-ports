# New ports collection makefile for:    devel/red5
# Date created:         14 March 2008
# Whom:                 wenheping@gmail.com
#
# $FreeBSD: ports/www/red5/Makefile,v 1.4 2010/02/01 12:52:52 wen Exp $
#

PORTNAME=	red5
PORTVERSION=	0.9.1
#PORTREVISION=	1
CATEGORIES=	www java
MASTER_SITES=	http://www.red5.org/downloads/0_9/

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Red5 is an Open Source flash server

USE_JAVA=	yes
JAVA_VERSION=	1.6
NO_BUILD=	yes
RED5_ROOT=	${PREFIX}/red5
RED5_DIRS=	conf lib webapps log
.include <bsd.port.pre.mk>

do-install:
	${INSTALL} -d ${RED5_ROOT}
.for D in ${RED5_DIRS}
	${INSTALL} -d ${RED5_ROOT}/${D}
.endfor
	${INSTALL_DATA} ${WRKSRC}/boot.jar ${RED5_ROOT}
	${INSTALL_DATA} ${WRKSRC}/red5.jar ${RED5_ROOT}
	${INSTALL_SCRIPT} ${WRKSRC}/red5.sh ${RED5_ROOT}
.for D in ${RED5_DIRS}
	( cd ${WRKSRC} && ${COPYTREE_SHARE} ${D} ${RED5_ROOT} )
.endfor
	${CHOWN} -R ${WWWOWN}:${WWWGRP} ${RED5_ROOT}

post-install:
	${FIND} ${RED5_ROOT} -type f | ${SORT}    | ${SED} 's,^${PREFIX}/,,' >> ${TMPPLIST}
	${FIND} ${RED5_ROOT} -type d | ${SORT} -r | ${SED} 's,^${PREFIX}/,@dirrm ,' >> ${TMPPLIST}
	@${CAT} ${PKGMESSAGE}

.include <bsd.port.post.mk>
