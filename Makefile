CFLAGS := -ggdb
SOURCES := src/main.c src/interpreter.c src/svimpl.c \
					 src/tokenizer.c src/parser.c src/ast_helper.c \
					 src/b_stacktrace_impl.c
GETOPT_SOURCES := gengetopt/cmdline.c
TEST_SOURCES := tests/test_main.c tests/munit/munit.c

BIN := spaz

#default install directory
ifndef INSTALL_DIR
INSTALL_DIR := /usr/local/bin/
endif

.PHONY: clean always install
.PHONY: build-all build-interpreter build-tests
.PHONY: run-tests
.PHONY: gengetopt

clean:
	-rm -r out
	-rm gengetopt/cmdline.*
always: gengetopt
	mkdir -p out
build-all: build-interpreter build-tests
build-interpreter: clean always out/main
build-tests: clean always out/test_main
run-tests: build-tests
	./out/test_main
debug:
	docker run --rm -it -v $(shell pwd):$(shell pwd) -w $(shell pwd) alpine sh -c "gcc $(SOURCES) $(GETOPT_SOURCES) $(CFLAGS) -o out/$(BIN)_x86"
	docker run --rm -it -e DISPLAY=192.168.1.142:0 -v $(shell pwd):$(shell pwd) -w $(shell pwd) alpine gf2 ./out/$(BIN)_x86
gengetopt:
	mkdir -p gengetopt
	gengetopt --input=config.ggo --include-getopt
	mv cmdline.* gengetopt/
out/main:
	gcc $(SOURCES) $(GETOPT_SOURCES) $(CFLAGS) -o out/$(BIN) -lm
out/test_main:
	gcc $(TEST_SOURCES) $(SOURCES) $(GETOPT_SOURCES) $(CFLAGS) -o out/test_main -lm

install:
	@echo "Installing to $(INSTALL_DIR)"
	sudo install -d $(INSTALL_DIR)
	sudo install -m 557 out/$(BIN) $(INSTALL_DIR)
