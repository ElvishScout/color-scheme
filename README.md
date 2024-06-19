# Color Scheme Generator

### About

This is a command line tool for generating color schemes from a given image.

### Installation

[Xmake](https://github.com/xmake-io/xmake) is recommended for building this project. Alternatively you may use any other tool you like.

### Usage

```
Usage: color-scheme [OPTION] FILE
Generate color scheme from image, based on k-means algorithm.

Options:
  -h, --help    display this help and exit
  -c, --color   enable colorful printing
  --sample      sample size
  --cluster     number of clusters for k-means algorithm
  --seed        RNG seed, negative for random seed
```
