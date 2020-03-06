include ./scripts/FRAMEWORK

CXX=g++-4.8
CC=gcc

PREFETCHER_FILE=src/prefetcher-${PREFETCHER}.cc

ifdef DEBUG
M5_SUFFIX=debug
NO_FAST_ALLOC=True
else
M5_SUFFIX=opt
NO_FAST_ALLOC=False
endif

M5_TEST=ammp

export

all: compile test

compile: ${PREFETCHER_FILE}
	./scripts/compile.sh

.PHONY: test
test:
	./scripts/test_prefetcher.py

.PHONY: single-test
single-test: compile
	build/ALPHA_SE/m5.${M5_SUFFIX} --trace-flags="HWPrefetch" -re --outdir=output/${M5_TEST}-user \
		/opt/prefetcher/m5/configs/example/se.py     \
		--checkpoint-dir=lib/cp --checkpoint-restore=1000000000 --at-instruction         \
		--caches --l2cache --standard-switch --warmup-insts=10000000 --max-inst=10000000 \
		--l2size=1MB --bench=${M5_TEST} --prefetcher=on_access=true:policy=proxy

.PHONY: gdb
gdb: compile
	gdb --args build/ALPHA_SE/m5.${M5_SUFFIX} --remote-gdb-port=0 -re                    \
	    --outdir=output/${M5_TEST}-user /opt/prefetcher/m5/configs/example/se.py         \
	    --checkpoint-dir=lib/cp --checkpoint-restore=1000000000 --at-instruction         \
	    --caches --l2cache --standard-switch --warmup-insts=10000000 --max-inst=10000000 \
	    --l2size=1MB --bench=${M5_TEST} --prefetcher=on_access=true:policy=proxy

.PHONY: clean
clean:
	rm -Rf build
	rm -Rf output
	rm -Rf stats.txt
