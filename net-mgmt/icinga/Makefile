# New ports collection makefile for:	icinga
# Date created:				24 May 2010
# Whom:				Mitsuru YOSHIDA <mitsuru@riken.jp>
#
# $FreeBSD$
#

PORTNAME=	icinga
PORTVERSION=	1.0.1
CATEGORIES=	net-mgmt
MASTER_SITES=	SF

MAINTAINER=	mitsuru@riken.jp
COMMENT=	Extremely powerful network monitoring system

LIB_DEPENDS=	gd.4:${PORTSDIR}/graphics/gd
RUN_DEPENDS=	${LOCALBASE}/libexec/nagios/check_nagios:${PORTSDIR}/net-mgmt/nagios-plugins

USE_ICONV=	yes
USE_PERL5_BUILD=	yes
USE_AUTOTOOLS=	autoconf:262 libltdl:22
USE_RC_SUBR=	icinga

CONFLICTS=	nagios-[123].* nagios-devel-*

OPTIONS=	EMBEDDED_PERL "Enable embedded Perl [requires Perl 5.8.0+]" off \
		NANOSLEEP "Use nanosleep in event timing" off \
		EVENT_BROKER "Enable event broker functionality" off \

GNU_CONFIGURE=	yes

ICINGAUSER?=	icinga
ICINGAGROUP?=	icinga
ICINGADIR?=	/var/spool/icinga

ICINGAUID=	183
ICINGAGID=	${ICINGAUID}

ICINGAWWWDIR?=	www/icinga
ICINGAHTMURL?=	/icinga
ICINGACGIURL?=	${ICINGAHTMURL}/cgi-bin

.include <bsd.port.pre.mk>

CPPFLAGS=	-I${LOCALBASE}/include -fPIC
CFLAGS+=	${CPPFLAGS}

CONFIGURE_ARGS=	--with-command-user=${ICINGAUSER} \
		--with-command-group=${WWWGRP} \
		--with-icinga-user=${ICINGAUSER} \
		--with-icinga-group=${ICINGAGROUP} \
		--with-htmurl=${ICINGAHTMURL} \
		--with-cgiurl=${ICINGACGIURL} \
		--sbindir=${PREFIX}/${ICINGAWWWDIR}/cgi-bin \
		--libexecdir=${PREFIX}/libexec/nagios \
		--datarootdir=${PREFIX}/${ICINGAWWWDIR} \
		--datadir=${PREFIX}/${ICINGAWWWDIR} \
		--sysconfdir=${PREFIX}/etc/icinga \
		--localstatedir=${ICINGADIR} \
		--with-httpd-conf=${PREFIX}/etc \
		--with-checkresult-dir=${ICINGADIR}/checkresults \
		--disable-statuswrl

CONFIGURE_ENV=	CPPFLAGS="${CPPFLAGS}" \
		CFLAGS="${CFLAGS}" \
		LDFLAGS="${LDFLAGS}" \
		LIBS="-L${LOCALBASE}/lib" \
		PERL=${PERL}

MAKE_JOBS_UNSAFE=	yes

INSTALL_TARGET=	install install-commandmode install-config

PLIST_SUB=	ICINGADIR=${ICINGADIR} \
		ICINGAWWWDIR=${ICINGAWWWDIR} \
		ICINGAUSER=${ICINGAUSER} \
		ICINGAGROUP=${ICINGAGROUP} \
		ICINGAUID=${ICINGAUID} \
		ICINGAGID=${ICINGAGID} \
		ICINGAHTMURL=${ICINGAHTMURL} \
		ICINGACGIURL=${ICINGACGIURL} \
		WWWGRP=${WWWGRP}

SUB_FILES=	pkg-install \
		pkg-deinstall \
		pkg-message

# XXX: Don't remove PREFIX from SUB_LIST here.
SUB_LIST=	PREFIX=${PREFIX} \
		${PLIST_SUB}

USE_PHP=	yes

.if defined(WITH_EMBEDDED_PERL)
USE_PERL5=	5.8.0+
CONFIGURE_ARGS+=	--enable-embedded-perl \
		--with-perlcache
PLIST_SUB+=	EMBEDDED_PERL=""
.else
PLIST_SUB+=	EMBEDDED_PERL="@comment "
.endif

.if defined(WITH_NANOSLEEP)
CONFIGURE_ARGS+=	--enable-nanosleep
.endif

.if defined(WITH_EVENT_BROKER)
CONFIGURE_ARGS+=	--enable-event-broker
.else
CONFIGURE_ARGS+=	--disable-event-broker
.endif

pre-install:
	@${SH} ${PKGINSTALL} ${DISTNAME} PRE-INSTALL

post-install:
	@${CHMOD} 775 ${ICINGADIR} ${ICINGADIR}/archives ${ICINGADIR}/checkresults ${ICINGADIR}/rw
	@${CHOWN} ${ICINGAUSER}:${ICINGAGROUP} ${ICINGADIR} ${ICINGADIR}/archives ${ICINGADIR}/checkresults
	@${CHOWN} ${ICINGAUSER}:${WWWGRP} ${ICINGADIR}/rw
	@${CAT} ${PKGMESSAGE}

.include <bsd.port.post.mk>
