# 请遵循官方说明: https://github.com/rr-debugger/rr/wiki/Building-And-Installing
# sudo dnf install \
#   ccache cmake make gcc gcc-c++ gdb libgcc libgcc.i686 \
#   glibc-devel glibc-devel.i686 libstdc++-devel libstdc++-devel.i686 \
#   python3-pexpect man-pages ninja-build capnproto capnproto-libs capnproto-devel

# 以下是在笔者的CentOS上的指令
yum install -y https://download-ib01.fedoraproject.org/pub/epel/7/x86_64/Packages/p/python36-ptyprocess-0.5.1-7.el7.noarch.rpm

yum install -y https://download-ib01.fedoraproject.org/pub/epel/7/x86_64/Packages/p/python36-pexpect-4.8.0-1.el7.noarch.rpm

# https://capnproto.org/install.html
capn_dir=/tmp/capn
mkdir -p $capn_dir
cd $capn_dir
curl -O https://capnproto.org/capnproto-c++-0.8.0.tar.gz
tar zxf capnproto-c++-0.8.0.tar.gz
cd capnproto-c++-0.8.0
./configure
make -j6 check
make install
rm -rf $capn_dir
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/:/usr/share/pkgconfig/

rr_dir=/tmp/rr
mkdir -p $rr_dir
cd $rr_dir
git clone https://github.com/rr-debugger/rr.git
mkdir obj && cd obj
cmake ../rr
make -j8
make install
rm -rf $rr_dir

# echo "kernel.perf_event_paranoid=1" >> /etc/sysctl.conf
# sysctl -p
