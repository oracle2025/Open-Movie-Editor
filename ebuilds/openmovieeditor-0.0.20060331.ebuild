# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

DESCRIPTION="Open Movie Editor is designed to be a simple tool, that provides basic movie making capabilites."
SRC_URI="mirror://sourceforge/${PN}/${P}.tar.gz"
HOMEPAGE="http://openmovieeditor.sourceforge.net"

SLOT="0"
LICENSE="GPL-2"
KEYWORDS="~amd64 ~ppc ~x86"

IUSE=""
DEPEND=">=media-libs/libquicktime-0.9.7
        x11-libs/fltk
        media-libs/gavl
        media-libs/libsndfile"

src_install () {
    make DESTDIR=${D} install || die
    dodoc AUTHORS ChangeLog NEWS README TODO
}
