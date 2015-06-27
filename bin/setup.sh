#!/usr/bin/env bash

set -e

git submodule init
git submodule update
cd cpputest
./autogen.sh
./configure
make
