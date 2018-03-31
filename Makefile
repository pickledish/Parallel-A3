all: histogram

histogram: histogram.cpp ppmb_io.a
	gcc -O3 $^ -o $@ -lm

histo_private: histo_private.cpp ppmb_io.a
	gcc -O3 $^ -o $@ -lm

ppmb_io.a: ppmb_io.o 
	ar rs $@ $<

.phony: clean

clean:
	rm -f ppmb_io.a ppmb_io.o histogram

