.PHONY: all build sign test runtest clean

all: build

build:
	make -C src

sign: build
	make -C src jpdh.jar.asc

test: build
	make -C test

runtest: test
	make -C test run

clean:
	make -C src clean
	make -C test clean
