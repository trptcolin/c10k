.PHONY: test

all: echo

clean:
	make -f build/MakefileEcho.mk clean

test:
	make -f build/MakefileEcho.mk acceptance-test
