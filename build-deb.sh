#!/bin/sh

PKGFOLDER=jack-sine-1.0.0

mkdir -p ${PKGFOLDER}
cp -r src/* ${PKGFOLDER}
tar -czf ${PKGFOLDER}.tar.gz --exclude .git ${PKGFOLDER}

cd ${PKGFOLDER}

debuild -b -us -uc
