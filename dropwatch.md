```bash
~/Downloads$ git clone https://github.com/nhorman/dropwatch.git
cd dropwatch

sudo apt-get install -y libnl-3-dev libnl-genl-3-dev libreadline-dev binutils-dev
sudo apt install autoconf
sudo apt install pkg-config libtool build-essential libnl-3-dev libnl-genl-3-dev  libreadline-dev libpcap-dev binutils-dev

./autogen.sh

./configure 

make

```