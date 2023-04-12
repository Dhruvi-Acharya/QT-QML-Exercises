#!/bin/sh

SOURCE_PATH="$(pwd)"
RELEASE_DIR="$SOURCE_PATH/openldap_release"

OPENLDAP_LIB_PATH="$RELEASE_DIR/lib"
OPENLDAP_cLIB_PATH="$RELEASE_DIR/lib_copy"

# If there is a version upgrade of openssl, you must change LDFLAGS, CPPFLAGS according to changed directory structure
# Note. Make sure that CPPFLAGS for openssl shouldn't be like "$SOURCE_PATH/openssl/include/openssl" because openldap check openssl/ssl.h
export LDFLAGS="-L/$SOURCE_PATH/openssl/lib"
export CPPFLAGS="-I/$SOURCE_PATH/openssl/include"

export MACOSX_DEPLOYMENT_TARGET=10.13

# Run this script on Mac with Intel processor
./configure --prefix=$RELEASE_DIR --enable-slapd=no --with-tls=openssl

make depend
make
make install

rm -rf $OPENLDAP_cLIB_PATH
mkdir -p $OPENLDAP_cLIB_PATH

cp $OPENLDAP_LIB_PATH/liblber-2.5.0.dylib $OPENLDAP_cLIB_PATH/liblber.dylib
cp $OPENLDAP_LIB_PATH/libldap-2.5.0.dylib $OPENLDAP_cLIB_PATH/libldap.dylib

cd $OPENLDAP_cLIB_PATH

install_name_tool -id @rpath/liblber.dylib liblber.dylib
install_name_tool -id @rpath/libldap.dylib libldap.dylib

install_name_tool -change $OPENLDAP_LIB_PATH/liblber-2.5.0.dylib @rpath/liblber.dylib libldap.dylib
install_name_tool -change $OPENLDAP_LIB_PATH/libldap-2.5.0.dylib @rpath/liblber.dylib libldap.dylib

cd $SOURCE_PATH
