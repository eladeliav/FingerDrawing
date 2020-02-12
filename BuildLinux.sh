#!/bin/sh

echo "About to build to linuxBuild"
cd linuxBuild/
cmake -DCMAKE_PREFIX_PATH="~/Desktop/Documents/UniSockets/linuxBuild;~/Qt/5.12.6/gcc_64" ..
make -j 4
echo "Build Complete with exit code" $?
