#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR}/..

EP=""
for i in $@; do
    if test "$i" == "--enterprise"; then
        EP="EP"
    fi
done


./Installation/Jenkins/build.sh \
    standard \
    --rpath \
    --parallel 25 \
    --package DEB \
    $SNAP \
    --xcArm arm-linux-gnueabihf \
    --buildDir build-${EP}deb \
    --targetDir /var/tmp/ \
    --noopt \
    $@

cd ${DIR}/..
