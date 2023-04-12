#!/bin/sh

SOURCE_PATH="$(pwd)"
RELEASE_DIR="$SOURCE_PATH/openssl_release"
OPENSSL_LIB_PATH="$RELEASE_DIR/lib"
OPENSSL_cLIB_PATH="$RELEASE_DIR/lib_copy"

# ./Configure darwin64-x86_64-cc shared no-comp --openssldir=$RELEASE_DIR --prefix=$RELEASE_DIR
# make
# make install

rm -rf $OPENSSL_cLIB_PATH
mkdir -p $OPENSSL_cLIB_PATH

cp $OPENSSL_LIB_PATH/libcrypto.1.1.dylib $OPENSSL_cLIB_PATH/libcrypto.dylib
cp $OPENSSL_LIB_PATH/libssl.1.1.dylib $OPENSSL_cLIB_PATH/libssl.dylib

cd $OPENSSL_cLIB_PATH

install_name_tool -id @rpath/libcrypto.dylib libcrypto.dylib
install_name_tool -id @rpath/libssl.dylib libssl.dylib

install_name_tool -change $OPENSSL_LIB_PATH/libcrypto.1.1.dylib @rpath/libcrypto.dylib libssl.dylib

cd $SOURCE_PATH