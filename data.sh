#!/bin/sh
mkdir -p ./licence
BUILD_DIR=$(pwd)
cp ../src/licence/* ./licence/
(cd ../src/; zip -ur $BUILD_DIR/data.ex data || true)
(cd ../src/exengine; zip -ur $BUILD_DIR/data.ex data || true)