#include <iostream>
#include <string>
#include <cmath>
#include <omp.h>
#include "arrayff.hxx"
#include "draw.hxx"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cout << "Usage: ./heat_omp <grid_size>\n";
    return 1;
  }

  const int npix = atoi(argv[1]);
  Array<float, 2> h(npix, npix), g(npix, npix);

  fix_boundaries2(h);
  dump_array<float, 2>(h, "plate0.fit");

  double start_time = omp_get_wtime(); // Start Timer

  // FIX 1: Lock the temporal workload to exactly 1000 iterations for all grid sizes
  for (int iter = 0; iter < 1000; ++iter) {

    // FIX 2: Explicitly chunk the row distribution to prevent False Sharing in cache
    #pragma omp parallel for schedule(static)
    for (int y = 1; y < npix-1; ++y) {
      for (int x = 1; x < npix-1; ++x) {
        g(y, x) = 0.25 * (h(y, x-1) + h(y, x+1) + h(y-1, x) + h(y+1,x));
      }
    }

    fix_boundaries2(g);

    #pragma omp parallel for schedule(static)
    for (int y = 0; y < npix; ++y) {
      for (int x = 0; x < npix; ++x) {
        // Bypassing the convergence check to keep computation purely constant
        h(y, x) = g(y, x);
      }
    }
  } // End of fixed loop

  double end_time = omp_get_wtime(); // Stop Timer
  dump_array<float, 2>(h, "plate1.fit");

  std::cout << "Grid Size: " << npix << "x" << npix << " | Threads: " << omp_get_max_threads()
            << " | Time: " << (end_time - start_time) << "s\n";
  return 0;
}