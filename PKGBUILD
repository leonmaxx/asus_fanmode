# Maintainer: Leonid Maksymchuk <leonmaxx@gmail.com>
pkgname=asus_fanmode
pkgver=0.1.0
pkgrel=1
pkgdesc="Daemon that controls Fan Boost Mode on ASUS TUF laptops"
arch=('x86_64')
url="https://github.com/leonmaxx/asus_fanmode"
license=('BSD')
source=(git+https://github.com/leonmaxx/asus_fanmode)
md5sums=('00000000000000000000000000000000')

build() {
  cd "${pkgname}"
  make CXXFLAGS="$CXXFLAGS"
}

package() {
  cd "${pkgname}"
  make install DESTDIR="$pkgdir"
}
