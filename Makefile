all: hello echo

clean:
	rm -rf build/*
	mkdir -p build

hello: clean-hello
	@echo "Building Hello World"
	@$(CC) -std=c11 -O3 -Wall src/hello.c -o build/hello

clean-hello:
	rm -rf build/hello
	mkdir -p build

echo: clean
	@echo "Building echo server"
	@$(CC) -D_GNU_SOURCE -std=c11 -O3 -Wall src/tcp_server.c src/echo.c -o build/echo

test: echo
	test/echo_test.sh
