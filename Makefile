.PHONY: build test debug

build:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build; make raft

debug:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build; make raft

test: build
	./tests/run_tests.sh