#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PARENT_DIR=$(dirname $SCRIPT_DIR)
TARGET_DIR=$PARENT_DIR/verilator

if [ -d $TARGET_DIR ]; then
    echo "Found existing verilator installation: $TARGET_DIR"
    echo "Aborting installation."
    exit -1
fi

# install prerequisites
# sudo apt-get install git perl python3 make
# sudo apt-get install g++  # Alternatively, clang
# sudo apt-get install libgz  # Non-Ubuntu (ignore if gives error)
# sudo apt-get install libfl2  # Ubuntu only (ignore if gives error)
# sudo apt-get install libfl-dev  # Ubuntu only (ignore if gives error)
# sudo apt-get install zlibc zlib1g zlib1g-dev  # Ubuntu only (ignore if gives error)
# sudo apt-get install ccache  # If present at build, needed for run
# sudo apt-get install libgoogle-perftools-dev numactl
# sudo apt-get install perl-doc
# sudo apt-get install git autoconf flex bison

cd $PARENT_DIR
git clone https://github.com/verilator/verilator

cd verilator
export VERILATOR_ROOT=`pwd`
echo "VERILATOR_ROOT=$VERILATOR_ROOT"
git checkout v4.224
autoconf
./configure
make -j `nproc`