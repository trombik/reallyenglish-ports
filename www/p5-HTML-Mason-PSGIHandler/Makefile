# New ports collection makefile for:	p5-HTML-Mason-PSGIHandler
# Date created:		2011-01-30
# Whom:			Tomoyuki Sakurai <tomoyukis@reallyenglish.com>
#
# $FreeBSD$
#

PORTNAME=	HTML-Mason-PSGIHandler
PORTVERSION=	0.51
#PORTREVISION=	0
CATEGORIES=	www
# http://search.cpan.org/CPAN/authors/id/A/AB/ABH/HTML-Mason-PSGIHandler-0.52.tar.gz
MASTER_SITES=	CPAN/../../authors/id/A/AB/ABH
PKGNAMEPREFIX=	p5-

MAINTAINER=	tomoyukis@smer.jp.reallyenglish.com
COMMENT=	PSGI handler for HTML::Mason

BUILD_DEPENDS=	\
	p5-CGI-PSGI>0:${PORTSDIR}/www/p5-CGI-PSGI \
	p5-HTML-Mason>0:${PORTSDIR}/www/p5-HTML-Mason

USE_PERL5=	Yes
PERL_CONFIGURE=	Yes
MAN3=	HTML::Mason::PSGIHandler.3

.include <bsd.port.mk>
