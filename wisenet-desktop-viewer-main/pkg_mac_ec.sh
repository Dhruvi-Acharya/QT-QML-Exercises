#!/bin/sh

# comment below line to pkg test only
./build_mac.sh ec_version

# Make DMG
./makeDmg.sh
