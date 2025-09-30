#!/bin/sh
mkdir -p ./licence
BUILD_DIR=$(pwd)

(cd ../source/; zip -ur $BUILD_DIR/data.ex data || true)