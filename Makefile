all: hello echo

hello: clean-hello
	@echo "Building Hello World"
	@$(CC) -O3 src/hello.c -o build/hello

clean-hello:
	rm -rf build/hello
	mkdir -p build

echo: clean-echo
	@echo "Building echo server"
	@$(CC) -O3 src/echo.c -o build/echo

clean-echo:
	rm -rf build/echo
	mkdir -p build

test: echo
	test/echo_test.sh
