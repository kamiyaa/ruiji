# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

DESCRIPTION="Reverse image searching program using iqdb.org"
HOMEPAGE="https://github.com/Kamiyaa/ruiji"
SRC_URI="https://github.com/Kamiyaa/${PN}/archive/${PV}.tar.gz"

LICENSE="LGPLv3"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="+color"

DEPEND="
	net-misc/curl
	sys-devel/make
	sys-devel/gcc
"
RDEPEND="${DEPEND}"

IUSE="+color"

src_unpack() {
	unpack ${A}
}

src_compile() {
	cd src/
	if [[ -f Makefile ]] ; then
		emake || die "emake failed"
	fi
}

src_install() {
	cd src/
	if [[ -f Makefile ]] ; then
		emake DESTDIR="${D}" PREFIX="${D%/}" install
	fi
}
