#!/bin/bash

export DEPS_DIR="build_cmake_deps"
mkdir ${DEPS_DIR} && cd ${DEPS_DIR}

# we use wget to fetch the cmake binaries
wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.20.2/cmake-3.20.2-linux-x86_64.tar.gz

# this is optional, but useful:
# do a quick md5 check to ensure that the archive we downloaded did not get compromised
md5sum cmake-3.20.2-linux-x86_64.tar.gz
echo "ebc26503469f12bf1e956c564fcfa82a *cmake-3.20.2-linux-x86_64.tar.gz" > cmake_md5.txt
md5sum -c cmake_md5.txt

# extract the binaries; the output here is quite lengthy,
# so we swallow it to not clutter up the travis console
tar -xvf cmake-3.20.2-linux-x86_64.tar.gz
mv cmake-3.20.2-linux-x86_64/* .
rmdir cmake-3.20.2-linux-x86_64
