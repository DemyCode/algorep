.PHONY: build test

build:
	mkdir -p build
	cd build; cmake ..
	cd build; make raft

test:
	./tests/run_tests.sh
