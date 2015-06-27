all: echo

clean:
	rm -rf out/*
	mkdir -p out

echo: clean
	@echo "Building echo server"
	@$(CC) -D_GNU_SOURCE -std=c11 -O3 -Wall src/tcp_server.c src/echo.c -o out/echo

test: echo
	test/echo_test.sh
