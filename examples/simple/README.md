# Simple Example

A minimal example demonstrating how to use mquickjs in an ESP-IDF project.

## Overview

This example shows how to:

- Initialize a QuickJS JavaScript context with 10KB of memory
- Define custom JavaScript functions (`print()`, `Date.now()`, `gc()`)
- Execute JavaScript code from C
- Handle JavaScript exceptions
- Clean up resources

## Code Structure

### `main/main.c`

The main application file contains:

1. **Helper Functions**

   - `get_time_ms()` - Returns current time in milliseconds
   - `js_print()` - Implements the `print()` function in JavaScript
   - `js_date_now()` - Implements `Date.now()` for getting current timestamp
   - `js_performance_now()` - Implements `performance.now()`
   - `js_gc()` - Implements garbage collection
   - Stub functions for `load()`, `setTimeout()`, and `clearTimeout()`

2. **Main Application**
   - `app_main()` - Entry point that:
     - Allocates 10KB of memory for the JavaScript context
     - Initializes a QuickJS context with the standard library
     - Evaluates a sample JavaScript script
     - Handles any exceptions that occur during execution
     - Cleans up and frees resources

### `main/CMakeLists.txt`

ESP-IDF component configuration that:

- Registers the component with source file `main.c`
- Specifies the include directories
- Declares dependency on the `mquickjs` component

### `CMakeLists.txt`

Top-level CMake configuration for the ESP-IDF project.

## Sample Script

The example executes a script to generate a Mandelbrot set visualization in the console:

```javascript
/* Mandelbrot set */
function mandelbrot(center_x, center_y, scale, w, h, max_it) {
  // ... setup ...
  for (y1 = 0; y1 < h; y1++) {
    s = "";
    for (x1 = 0; x1 < w; x1++) {
      // ... calculate c0 and c ...
      s +=
        "\x1b[" +
        (c0 >= 8 ? 82 + c0 : 30 + c0) +
        ";" +
        (c >= 8 ? 92 + c : 40 + c) +
        "m\u2580";
    }
    s += "\x1b[0m";
    print(s);
  }
}

print("Generating Mandelbrot set...");
var start = Date.now();
mandelbrot(-0.75, 0.0, 2.0, 80, 25, 50);
print("Time: " + (Date.now() - start) + "ms");
```

This demonstrates:

- **Math**: heavy floating point calculations
- **Loops**: nested loops for pixel generation
- **Arrays**: color palette lookup
- **Strings**: complex string concatenation with ANSI escape codes and Unicode
- **Performance**: measures execution time

## Building and Running

### Prerequisites

- ESP-IDF toolkit installed and configured
- Target microcontroller connected

### Build

```bash
idf.py build
```

### Flash

```bash
idf.py -p PORT flash
```

### Monitor

```bash
idf.py -p MONITOR
```

Replace `PORT` with your device's serial port.

## Output

When executed, the program should output the Mandelbrot set in ASCII/ANSI art, followed by the execution time:

```
I (xxx) example: script executed successfully.
Generating Mandelbrot set...
[ ... Colorful Mandelbrot Set Visualization ... ]
Time: <time-in-ms>ms
```

**Note**: Ensure your terminal supports ANSI color codes and UTF-8 output to view the Mandelbrot set correctly.

## Memory Configuration

The example allocates 10KB of memory for the JavaScript context. This can be adjusted by modifying the `JS_MEMORY_SIZE` variable in `main.c`:

```c
const size_t JS_MEMORY_SIZE = 10 * 1024; // 10KB
```

Larger allocations provide more memory for JavaScript variables and objects, but consume more of your device's RAM.

## License

This example is part of the esp-mquickjs project.
