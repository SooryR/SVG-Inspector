# svg-image-parser

SVG image parser is a parser that uses libxml2 to parse the svg, then store the different elements in a eazy to use data structure.

## Prerequisites

You need to have installed libxml2.

## Installation

Run make in bash in the root folder with the makefile, then copy the libsvgparse.so to where every you need it.

```bash
make
```

To use the shared dynamic library, you will need to add a path to the library to the LD_LIBRARY_PATH environment variable

```bash
export LD_LIBRARY_PATH= (the library path with out brackets) :$LD_LIBRARY_PATH
```

## Usage

```C
#include "SVGParser.c"
#include <stdio.h>

int main(int argc, char **argv)
{
  SVGimage * img;
  char *img_to_string;
  img = createSVGimage(argv[1]); // argv[1] is the path to the SVG image
  img_to_string = SVGimageToString(img);
  print("%d", img_to_string);
}
```

