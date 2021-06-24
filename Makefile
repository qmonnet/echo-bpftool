default:

clean:
	$(RM) -- *.o
	$(MAKE) -C app clean

.PHONY: clean default
