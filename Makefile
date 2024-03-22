.PHONY: clean always
.PHONY: build-all build-interpreter build-tests
.PHONY: run-tests

clean:
	-rm -r out
always: clean
	mkdir -p out
build-all: always build-interpreter build-tests
build-interpreter: out/main
build-tests: out/test_main
run-tests: build-tests
	./out/test_main

out/main:
	gcc src/main.c src/interpreter.c src/svimpl.c src/tokenizer.c src/parser.c -o out/main
out/test_main:
	gcc tests/test_main.c tests/munit/munit.c src/interpreter.c src/svimpl.c src/tokenizer.c src/parser.c -o out/test_main
