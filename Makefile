CC = clang
# Absolute hero: https://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
objects=$(call rwildcard,src,*.c)

build:
	$(CC) -lm -o vmem-cache-sim $(objects)
