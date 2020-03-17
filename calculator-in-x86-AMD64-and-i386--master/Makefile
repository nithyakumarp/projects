
all:
	make calc || true
	make calc32 || true
	make calc64 || true

calc32: calc32.S
	gcc -m32 -g -o calc32 calc32.S

calc64: calc64.S
	gcc -g -no-pie -o calc64 calc64.S

clean:
	rm -f calc32 calc64
