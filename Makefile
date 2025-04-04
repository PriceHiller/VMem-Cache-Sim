CC=clang
output-bin=vmem-cache-sim
CCflags=-Wall -Wextra -Wpedantic -O2 -lm

# Absolute hero: https://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
objects=$(call rwildcard,src,*.c)

build:
	$(CC) --debug $(CCflags) -o $(output-bin) $(objects)

build-dev: build

build-release:
	$(CC) $(CCflags) -o $(output-bin) $(objects)
