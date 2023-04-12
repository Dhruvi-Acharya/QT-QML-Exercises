#!/bin/sh

# comment below line to pkg test only
./build_mac.sh beta_version

# Make DMG
./makeDmg.sh
