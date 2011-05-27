# New ports collection makefile for:	mod_security
# Date created:				4 June 2003
# Whom:					Marcelo Araujo <araujo@FreeBSD.org>
#
# $FreeBSD: ports/www/mod_security/Makefile,v 1.34 2009/12/18 22:37:08 pav Exp $
#

PORTNAME=	mod_security
PORTVERSION=	2.6.0
CATEGORIES=	www security
MASTER_SITES=	SF/mod-security/modsecurity-apache/${PORTVERSION}
DISTNAME=	${PORTNAME:S/_//:S/2//}-apache_${PORTVERSION}

MAINTAINER=	tomoyukis@reallyenglish.com
COMMENT=	An intrusion detection and prevention engine

MAKE_JOBS_SAFE=	yes

LIB_DEPENDS+=	pcre.0:${PORTSDIR}/devel/pcre \
		apr-1:${PORTSDIR}/devel/apr1

USE_APACHE=	2.2+
GNU_CONFIGURE=	yes
CONFIGURE_TARGET=	--build=${MACHINE_ARCH}-portbld-freebsd${OSREL}
USE_GNOME=	libxml2
DOCS=		CHANGES LICENSE README.TXT modsecurity.conf-minimal
SUB_FILES+=	mod_security2.conf
SUB_LIST+=	APACHEETCDIR="${APACHEETCDIR}"
PLIST_FILES+=	${APACHEMODDIR}/mod_security2.so
OPTIONS=	LUA "Embedded Lua language support" off \
		MLOGC "Build ModSecurity Log Collector" off

.include <bsd.port.pre.mk>

.if defined(WITH_MLOGC)
PLIST_FILES+=	bin/mlogc \
				bin/mlogc-batch-load.pl
.endif

.if defined(WITH_LUA)
IGNORE=		LUA support is not tested
USE_LUA=	5.1+
CONFIGURE_ARGS+=	--with-lua=${LOCALBASE}
LIB_DEPENDS+=	lua-5.1.1:${PORTSDIR}/lang/lua
.else
CONFIGURE_ARGS+=	--without-lua
.endif
.if defined(WITH_MLOGC)
LIB_DEPENDS+=	curl:${PORTSDIR}/ftp/curl
CONFIGURE_ARGS+=	--with-curl=${LOCALBASE}
.else
CONFIGURE_ARGS+=	--without-curl --disable-mlogc
.endif
REINPLACE_ARGS=	-i ""
AP_EXTRAS+=	-DWITH_LIBXML2

CONFIGURE_ARGS+=	--with-apxs=${APXS} --with-pcre=${LOCALBASE}

post-patch:
.if defined(WITH_LUA)
	${REINPLACE_CMD} -e 's|%%LUA_VER%%|${LUA_VER}|' ${WRKSRC}/configure
.endif
	${REINPLACE_CMD} -e 's|/usr/local|${LOCALBASE}|g' ${WRKSRC}/configure

do-install:
	make -C ${WRKSRC} install

post-install:
	${RM} ${LOCALBASE}/lib/mod_security2.so
.if !defined(NOPORTDOCS)
	${MKDIR} ${DOCSDIR}
	${INSTALL_DATA} ${WRKSRC}/doc/Reference_Manual.html ${DOCSDIR}
	${INSTALL_DATA} ${WRKSRC}/doc/CHANGES ${DOCSDIR}
.endif
.if defined(WITH_MLOGC)
	${INSTALL_PROGRAM} ${WRKSRC}/mlogc/mlogc ${PREFIX}/bin/
.endif

.include <bsd.port.post.mk>
