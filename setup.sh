#!/bin/sh

./clean.sh

cd src
if [ command -v curl >/dev/null 2>&1 ]; then
	curl -s -o libBareMetal.asm https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.asm
	curl -s -o libBareMetal.c https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.c
	curl -s -o libBareMetal.h https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.h
else
	wget -q https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.asm
	wget -q https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.c
	wget -q https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.h
fi
cd ..

mkdir bin

./build.sh
