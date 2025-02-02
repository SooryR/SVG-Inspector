# libSVGParse: A C Library for Parsing and Manipulating SVG Images

`libSVGParse` is a C library designed to parse Scalable Vector Graphics (SVG) files using `libxml2`, and to represent the SVG data in a user-friendly, easily accessible data structure. This allows developers to quickly extract, analyze, and manipulate SVG elements programmatically.

## Features

*   **SVG Parsing:** Parses SVG files into a custom in-memory data structure.
*   **Element Extraction:** Provides functions to extract all rectangles, circles, paths, and groups, including nested groups.
*   **Attribute Access:** Allows easy access to element attributes and their values.
*   **Data Manipulation:** Enables adding, setting, and modifying SVG components.
*   **Validation:** Offers SVG validation against a schema file, plus additional checks based on the data structure.
*   **JSON Conversion:** Can convert different lists and elements to a JSON string.
*   **Easy to Use API:** Provides a simple and straightforward API for accessing and manipulating the data structure.
*   **Dynamic Library:** Can be used as a shared dynamic library, and included in C-based projects.

## Prerequisites

*   **libxml2:** You must have `libxml2` installed on your system. You can usually install it via your system's package manager (e.g., `apt-get install libxml2-dev` on Debian/Ubuntu, or `brew install libxml2` on macOS).

## Installation

1.  **Clone the Repository:**
    ```bash
    git clone [your-repository-url]
    cd [your-repository-directory]
    ```
2.  **Compile the Library:**
    ```bash
    make
    ```
    This will create a shared library file `libsvgparse.so` (or `libsvgparse.dylib` on macOS) in the root folder.
3.  **Set `LD_LIBRARY_PATH` (Linux/macOS):**
    To use the shared dynamic library, you need to tell your system where to find it:
    ```bash
    export LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH
    ```
   *(Replace `$(pwd)` with the actual path to your library if necessary)*. You may want to add this to your `~/.bashrc` or `~/.zshrc` file for persistence.

## Usage

Here's a basic example demonstrating how to use the library:

```c
#include "SVGParser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if (argc != 2) {
        printf("Usage: %s <svg_file>\n", argv[0]);
        return 1;
    }
  SVGimage *img = createValidSVGimage(argv[1], "svg.xsd"); // Replace with path to your SVG and Schema file.
  if (!img)
  {
    printf("Error Creating SVGimage Struct\n");
    return 1;
  }

    char *img_to_string = SVGimageToString(img);
    if(img_to_string == NULL)
    {
        printf("Error getting the string value of the struct\n");
        deleteSVGimage(img);
        return 1;
    }
    printf("%s\n", img_to_string);
    free(img_to_string);
    deleteSVGimage(img);
    return 0;
}
```
