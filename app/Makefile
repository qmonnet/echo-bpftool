APP := app.o
SRC := $(wildcard *.c)

default: $(APP)

$(APP): $(addsuffix .o, $(basename $(SRC)))
	bpftool gen object $@ $^

.c.o:
	clang -O2 -g -target bpf -c $< -o $@

clean:
	$(RM) -- *.o

.PHONY: clean default
