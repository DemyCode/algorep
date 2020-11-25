.PHONY: doc build test debug

build:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build; make raft -j

debug:
	mkdir -p debug
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build; make raft -j

test: build
	./tests/run_tests.sh $(filter-out $@,$(MAKECMDGOALS))

doc:
	doxygen Doxyfile

%:
	@:
