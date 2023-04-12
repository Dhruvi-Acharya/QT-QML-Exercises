#!/bin/sh

SOURCE_PATH="$(pwd)"
RELEASE_DIR="$SOURCE_PATH/openldap_release"

# If there is a version upgrade of openssl, you must change LDFLAGS, CPPFLAGS according to changed directory structure
# Note. Make sure that CPPFLAGS for openssl shouldn't be like "$SOURCE_PATH/openssl/include/openssl" because openldap check openssl/ssl.h
export LDFLAGS="-L/$SOURCE_PATH/openssl/lib"
export CPPFLAGS="-I/$SOURCE_PATH/openssl/include"

# Run this script on Mac with Intel processor
./configure --prefix=$RELEASE_DIR --enable-slapd=no --with-tls=openssl

make depend
make
make install

