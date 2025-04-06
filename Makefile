# Do not overwrite this file/regenerate it via Cmake!
output-bin=vmem-cache-sim

# Absolute hero: https://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
objects=$(call rwildcard,src,*.c)

vmem-cache-sim:
	$(CC) -flto -static $(CFLAGS) -o $(output-bin) $(LDFLAGS) $(objects) -lm

build-dev:
	$(CC) --debug -Wall -Wextra -Wpedantic  $(CFLAGS) -o $(output-bin) $(LDFLAGS) $(objects) -lm

build-release: vmem-cache-sim

clean:
	rm $(output-bin)
