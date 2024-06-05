#!/bin/bash

# Make sure riscvdb is built
ROOTDIR=../..
if [ ! -f "${ROOTDIR}/build/riscvdb" ]; then
    pushd ${ROOTDIR}
    ./build.sh
    popd
fi

# Make sure example is built
make

${ROOTDIR}/build/riscvdb -x ./run.rvdb_script
