# New ports collection makefile for:	p5-Template-Plugin-YAML
# Date created:		2009-03-28
# Whom:			Tomoyuki Sakurai <vherry@trombik.org>
#
# $FreeBSD$
#

PORTNAME=	Template-Plugin-YAML
PORTVERSION=	1.23
CATEGORIES=	textproc perl5
MASTER_SITES=	CPAN
PKGNAMEPREFIX=	p5-

MAINTAINER=	cherry@trombik.org
COMMENT=	Template plugin for YAML

RUN_DEPENDS=	p5-Template-Toolkit>0:${PORTSDIR}/www/p5-Template-Toolkit \
	p5-YAML>0:${PORTSDIR}/textproc/p5-YAML

PERL_CONFIGURE=	Yes
MAN3=	Template::Plugin::YAML.3

.include <bsd.port.mk>
