.PHONY: doc build test debug

doc:
	doxygen Doxyfile

build:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd build; make raft -j

debug:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build; make raft -j

test: build
	./tests/run_tests.sh $(filter-out $@,$(MAKECMDGOALS))

%:
	@:
