// ----------------------------------------------------------------------------
// Imports
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cassert>
#include "Timer.h"
#include <chrono>

#include <thread>
using std::thread;

#include <atomic>
using std::atomic;

#include <string>
#include <sstream>
#include <iostream>
using std::cout;
using std::endl;

extern "C" {
#include "ppmb_io.h"
}

// ----------------------------------------------------------------------------
// Utility structs for images, histogram containers
// ----------------------------------------------------------------------------

struct img {
	int xsize;
	int ysize;
	int maxrgb;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
};

struct hists {
	atomic<int>* hist_r;
	atomic<int>* hist_g;
	atomic<int>* hist_b;
};

// ----------------------------------------------------------------------------
// Histogram tools for calculating hists and printing
// ----------------------------------------------------------------------------

void histogram(struct img *input, int firstRow, int lastRow, struct hists *h) 
{
	int start = (firstRow == 0) ? 0 : (firstRow - 1) * (*input).ysize;
	int end = (lastRow - 1) * (*input).ysize;

	for(int pix = start; pix < end; pix++)
	{
		(*h).hist_r[input->r[pix]] += 1;
		(*h).hist_g[input->g[pix]] += 1;
		(*h).hist_b[input->b[pix]] += 1;
	}
	return;
}

void print_histogram(FILE *f, atomic<int>* hist, int N)
{
	fprintf(f, "%d\n", N+1);
	for(int i = 0; i <= N; i++) 
	{
		fprintf(f, "%d %d\n", i, hist[i].load());
	}
	return;
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

	// Checking to make sure input is what we're hoping for 

	if(argc != 4) 
	{
		printf("Usage: %s input-file output-file threads\n", argv[0]);
		printf("For single-threaded runs, pass threads = 1\n");
		exit(1);
	}
	
	char *output_file = argv[2];
	char *input_file = argv[1];
	int threads = atoi(argv[3]);

	struct img input;

	if(!ppmb_read(input_file, &input.xsize, &input.ysize, &input.maxrgb, 
	&input.r, &input.g, &input.b)) 
	{
		if(input.maxrgb > 255) 
		{
			printf("Maxrgb %d not supported\n", input.maxrgb);
			exit(1);
		}

		// Setting up the parameters for the algorithm beforehand

		int N = input.xsize;
		int chunk_size = (int) ((N / threads) + 1);

		int start = 0;
		int end = chunk_size;

		thread* threadArray = new thread[threads];

		struct hists h;
		h.hist_r = new atomic<int>[input.maxrgb+1]();
		h.hist_g = new atomic<int>[input.maxrgb+1]();
		h.hist_b = new atomic<int>[input.maxrgb+1]();

		// Actually run the algorithm! One chunk of rows for every thread used

		ggc::Timer t("histogram");
		t.start();

		for (int i = 0; i < threads; i++)
		{
			threadArray[i] = thread(histogram, &input, start, end, &h);
			start = start + chunk_size;
			end = (N > end + chunk_size) ? (end + chunk_size) : N + 1;
		}

		for (int i = 0; i < threads; i++)
		{
			threadArray[i].join();
		}

		t.stop();

		// Print all the output to whatever file was entered for us

		FILE *out = fopen(output_file, "w");
		if(out) {
			print_histogram(out, h.hist_r, input.maxrgb);
			print_histogram(out, h.hist_g, input.maxrgb);
			print_histogram(out, h.hist_b, input.maxrgb);
			fclose(out);
		} else {
			fprintf(stderr, "Unable to output!\n");
		}
		printf("Time: %llu ns\n", t.duration());
	}
	return 0;
}
