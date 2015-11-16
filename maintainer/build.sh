#!/bin/sh
###############################################################################
# Copyright (C) NEngine Team
# 
# This file is part of CSA-Rainbow-Table-Tool.
# 
# CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with CSA-Rainbow-Table-Tool.  If not, see <http://www.gnu.org/licenses/>.
# 
# Author: NEngine Team
# 
###############################################################################

error() {
 echo "$@"
 exit 1
}

CONF_PATH=$PWD; export CONF_PATH

if [ "$BUILD_DIR" = "" ] ; then
 BUILD_DIR=/mnt/cache/.$USER/; export BUILD_DIR
fi

NBCORE=$(cat /proc/cpuinfo | grep processor | wc -l)
NBCORE=$(expr $NBCORE + 1)

export CC="ccache gcc"
export CXX="ccache g++"

CFLAGS="${CFLAGS}"; export CFLAGS
CXXFLAGS="${CXXFLAGS}"; export CXXFLAGS

MACHINE=$(uname -r)

mkdir -p ${BUILD_DIR}/build-${MACHINE} || \
 error "[ERROR]: can't create build dir: ${BUILD_DIR}/build-${MACHINE}"
rm -f build build_current
ln -s "${BUILD_DIR}" build
ln -s "${BUILD_DIR}/build-${MACHINE}" build_current

cd ${BUILD_DIR}/build-${MACHINE} && \
 \
${CONF_PATH}/configure --prefix=/usr \
 --enable-corei7 \
 --with-opencl --with-cuda=no \
 --with-tbb \
 --without-fltk --with-gtk --with-debug || error "[ERROR]: configure failed"

## generate current build script

cat <<EOF > ${CONF_PATH}/build_current.sh
#!/bin/sh
CCACHE_DIR=${CCACHE_DIR}
export CCACHE_DIR
mkdir -p ${CCACHE_DIR}
cd ${BUILD_DIR}/build-${MACHINE} && make -j${NBCORE}
EOF
chmod +x ${CONF_PATH}/build_current.sh

## check if we want to run compilation right now
if [ "$1" = "make" ]; then
 make -j3
 exit $?
fi

exit 0
