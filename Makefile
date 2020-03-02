include ./scripts/FRAMEWORK

CXX=g++-4.8
CC=gcc

PREFETCHER_FILE=src/prefetcher-${PREFETCHER}.cc

export

all: compile test

compile: ${PREFETCHER_FILE}
	./scripts/compile.sh

.PHONY: test
test:
	./scripts/test_prefetcher.py


.PHONY: clean
clean:
	rm -Rf build
	rm -Rf output
	rm -Rf stats.txt
