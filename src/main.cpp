#include "color_scheme.h"
#include "myrand.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

int main(int argc, const char **argv) {
  const char *help_msg = "Usage: color-scheme [OPTION] FILE\n"
                         "Generate color scheme from image, based on k-means algorithm.\n"
                         "\n"
                         "Options:\n"
                         "  -h, --help    display this help and exit\n"
                         "  -c, --color   enable colorful printing\n"
                         "  --sample      sample size\n"
                         "  --cluster     number of clusters for k-means algorithm\n"
                         "  --seed        RNG seed, negative for random seed\n";

  const char *filename = nullptr;
  int samples = 1000;
  int clusters = 8;
  int seed = -1;
  bool help = false;
  bool colorful = false;

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    const char *next_arg = argv[i + 1];
    const char *key = nullptr;
    const char *value = nullptr;

    if (arg[0] == '-') {
      if (arg[1] == '-') {
        if (arg[2]) {
          key = arg + 2;
        } else {
          i++;
        }
      } else {
        key = arg + 1;
      }
      value = next_arg;
    } else {
      value = arg;
    }

    if (key) {
      if (!strcmp(key, "h") || !strcmp(key, "help")) {
        help = true;
      } else if (!strcmp(key, "c") || !strcmp(key, "color")) {
        colorful = true;
      } else if (!strcmp(key, "sample")) {
        samples = atoi(value);
        i++;
      } else if (!strcmp(key, "cluster")) {
        clusters = atoi(value);
        i++;
      } else if (!strcmp(key, "seed")) {
        seed = atoi(value);
        i++;
      }
    } else if (value) {
      filename = value;
    }
  }

  if (help) {
    printf("%s", help_msg);
    exit(0);
  }
  if (!filename) {
    exit(1);
  }

  MyRand rng = seed < 0 ? MyRand() : MyRand(seed);
  auto scheme = color_scheme(filename, clusters, samples, rng);
  for (auto &color : scheme) {
    unsigned char R = std::round(color.first.r);
    unsigned char G = std::round(color.first.g);
    unsigned char B = std::round(color.first.b);
    unsigned char r = ~R;
    unsigned char g = ~G;
    unsigned char b = ~B;
    if (colorful) {
      std::printf("\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm  #%02x%02x%02x | %5.2f%%  \x1b[0m\n", r, g, b, R, G, B, R, G,
                  B, color.second * 100);
    } else {
      std::printf("#%02x%02x%02x | %5.2f%%\n", R, G, B, color.second * 100);
    }
  }
  return 0;
}
