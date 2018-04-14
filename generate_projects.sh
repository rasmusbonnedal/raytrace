#!/bin/bash

if hash ninja; then
	GENERATOR=" -G Ninja"
fi

mkdir build
pushd build

mkdir release
pushd release
cmake $GENERATOR -DCMAKE_BUILD_TYPE=Release ../..
popd

mkdir debug
pushd debug
cmake $GENERATOR -DCMAKE_BUILD_TYPE=Debug ../..
popd

popd
