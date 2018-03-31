#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cassert>
// #include "Timer.h"

#include <thread>
using std::thread;

extern "C" {
#include "ppmb_io.h"
}

struct img {
  int xsize;
  int ysize;
  int maxrgb;
  unsigned char *r;
  unsigned char *g;
  unsigned char *b;
};

struct hists {
  int* hist_r;
  int* hist_g;
  int* hist_b;
};

void print_histogram(FILE *f, int *hist, int N) {
  fprintf(f, "%d\n", N+1);
  for(int i = 0; i <= N; i++) {
    fprintf(f, "%d %d\n", i, hist[i]);
  }
}

void histogram(struct img *input, int firstRow, int lastRow, struct hists h) {
  // we assume hist_r, hist_g, hist_b are zeroed on entry.

  int start = firstRow * (*input).ysize;
  int end = lastRow * (*input).ysize;

  for(int pix = start; pix < end; pix++) {
    h.hist_r[input->r[pix]] += 1;
    h.hist_g[input->g[pix]] += 1;
    h.hist_b[input->b[pix]] += 1;
  }
}

int main(int argc, char *argv[]) {

  // Making sure every input looks as we hope it does 
  // --------------------------------------------------------------------------

  if(argc != 4) {
    printf("Usage: %s input-file output-file threads\n", argv[0]);
    printf("       For single-threaded runs, pass threads = 1\n");
    exit(1);
  }
  
  char *output_file = argv[2];
  char *input_file = argv[1];
  int threads = atoi(argv[3]);

  struct img input;

  if(!ppmb_read(input_file, &input.xsize, &input.ysize, &input.maxrgb, 
		&input.r, &input.g, &input.b)) {
    if(input.maxrgb > 255) {
      printf("Maxrgb %d not supported\n", input.maxrgb);
      exit(1);
    }

    // Setting up the parameters for the algorithm
    // ------------------------------------------------------------------------

    int N = input.xsize;
    int chunk_size = (int) ((N / threads) + 1);

    int start = 0;
    int end = chunk_size;

    thread threadArray[threads];
    struct hists resultHists[threads];

    for (int i = 0; i < threads; i++)
    {
        struct hists h;

        h.hist_r = (int *) calloc(input.maxrgb+1, sizeof(int));
        h.hist_g = (int *) calloc(input.maxrgb+1, sizeof(int));
        h.hist_b = (int *) calloc(input.maxrgb+1, sizeof(int));

        threadArray[i] = thread(histogram, input, start, end, h);
        start = start + chunk_size;
        end = (N > end + chunk_size) ? (end + chunk_size) : N;

        resultHists[i] = h;
    }

    for (int i = 0; i < threads; i++)
    {
        threadArray[i].join();
    }

    // ggc::Timer t("histogram");

    // t.start();
    // histogram(&input, hist_r, hist_g, hist_b);
    // t.stop();




    // FILE *out = fopen(output_file, "w");
    // if(out) {
    //   print_histogram(out, hist_r, input.maxrgb);
    //   print_histogram(out, hist_g, input.maxrgb);
    //   print_histogram(out, hist_b, input.maxrgb);
    //   fclose(out);
    // } else {
    //   fprintf(stderr, "Unable to output!\n");
    // }
    // printf("Time: %llu ns\n", t.duration());
  }  
}
