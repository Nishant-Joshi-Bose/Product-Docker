sudo apt-get install -y gcc \
  pkg-config \
  libarchive13 \
  libarchive-dev \
  libcurl3 \
  libcurl3-dev \
  libgpgme11 \
  libgpgme11-dev \
  autogen \
  autoconf \
  make \
  libtool \
  curl \
  libtool \
  libssl-dev


git clone git://git.yoctoproject.org/opkg /scratch/opkg
cd /scratch/opkg
git reset --hard c5d1ac9f39c2ea42b93f0d2b881db2f224de9a06

./autogen.sh
autoconf

libtoolize --force
aclocal
autoheader
automake --force-missing --add-missing
autoconf
./configure
make
sudo make install
