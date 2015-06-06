all: hello echo

clean:
	rm -rf build/*
	mkdir -p build

hello: clean-hello
	@echo "Building Hello World"
	@$(CC) -O3 -Wall src/hello.c -o build/hello

clean-hello:
	rm -rf build/hello
	mkdir -p build

echo: clean
	@echo "Building echo server"
	@$(CC) -O3 -Wall src/echo.c -o build/echo

test: echo
	test/echo_test.sh
