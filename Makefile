CFLAGS := -std=c99 -ggdb
SOURCES := src/main.c src/interpreter.c src/svimpl.c \
					 src/tokenizer.c src/parser.c src/ast_helper.c
TEST_SOURCES := tests/test_main.c tests/munit/munit.c

.PHONY: clean always
.PHONY: build-all build-interpreter build-tests
.PHONY: run-tests

clean:
	-rm -r out
always:
	mkdir -p out
build-all: build-interpreter build-tests
build-interpreter: clean always out/main
build-tests: clean always out/test_main
run-tests: build-tests
	./out/test_main

out/main:
	gcc $(SOURCES) $(CFLAGS) -o out/main
out/test_main:
	gcc $(TEST_SOURCES) $(SOURCES) $(CFLAGS) -o out/test_main
