# Simple Example

A minimal example demonstrating how to use mquickjs in an ESP-IDF project.

## Overview

This example shows how to:
- Initialize a QuickJS JavaScript context with 64KB of memory
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
     - Allocates 64KB of memory for the JavaScript context
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

The example executes the following JavaScript code:
```javascript
print('Hello from mquickjs!');
var a = 1+2;
print('1+2=', a);
print('Time:', Date.now());
```

This demonstrates:
- Using the `print()` function to output text
- Basic variable assignment and arithmetic
- Accessing `Date.now()` to get the current timestamp

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

When executed, the program should output:
```
Initializing mquickjs...
Evaluating script: print('Hello from mquickjs!'); var a = 1+2; print('1+2=', a); print('Time:', Date.now());
Hello from mquickjs!
1+2= 3
Time: <current-timestamp>
Script executed successfully.
Done.
```

## Memory Configuration

The example allocates 64KB of memory for the JavaScript context. This can be adjusted by modifying the `mem_size` variable in `main.c`:

```c
size_t mem_size = 64 * 1024; // Change this value as needed
```

Larger allocations provide more memory for JavaScript variables and objects, but consume more of your device's RAM.

## License

This example is part of the esp-mquickjs project.
