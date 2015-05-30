all: hello

hello: clean
	@echo "Building Hello World"
	@$(CC) -O3 src/hello.c -o build/hello

clean:
	rm -rf build
	mkdir build
