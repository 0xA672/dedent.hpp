# dedent.hpp
> Compile‑time string dedentation.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Standard: C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Header Only](https://img.shields.io/badge/header--only-yes-green.svg)](.)

## Usage
### Example
```cpp
#include <iostream>
#include "dedent.hpp"

int main() {
    constexpr auto code = d::dedent(R"(
        int main() {
            return 0;
        }
    )");
    std::cout << d::sv(code);
}
```
### Output
```rust
int main() {
    return 0;
}
```
### Custom Tab Width
```cpp
// Treat '\t' as 2 spaces instead of the default 4
constexpr auto text = d::dedent<2>(R"(
	First line
		Indented line
)");
```
### Handling Blank Lines
* Blank lines at the very beginning and end of the input (containing only spaces, tabs, `\r`, `\f`, or `\v`) are removed entirely.
* Internal blank lines are preserved; each contributes a single `\n` to the output.
```cpp
constexpr auto poem = d::dedent(R"(

    Roses are red,
    Violets are blue,

    Sugar is sweet,
    And so are you.

)");
// Resulting string_view content:
// "Roses are red,\nViolets are blue,\n\nSugar is sweet,\nAnd so are you.\n"
```

## API Reference
### `d::dedent<size_t TabWidth = 4>(const char (&in)[N])`
### Purpose
Compile‑time dedentation of the raw string literal `in`, also trimming surrounding blank lines.

### Template Parameter
`TabWidth`: Visual column count for a `\t` character. Defaults to `4`.

### Return Value
`std::array<char, M>` where `M` is the length of the dedented string plus one (for the null terminator). Suitable for constructing a `std::string_view` or a C‑style string.

### `d::sv(const std::array<char, N>& a)`
### Purpose
Converts the array returned by `dedent` into a `std::string_view` for convenient output or further processing.

Return Value
`std::string_view` covering the dedented string (excluding the null terminator).

## Implementation Details
The library uses internal helper functions inside the `d::dt` namespace, all of which are `constexpr`:
* `sp(char)`: Checks for whitespace characters (, `\t`, `\r`, `\f`, `\v`).
* `cw(char, TabWidth)`: Returns the visual leading width of a character.
* `lw(string_view, TabWidth)`: Computes the total visual width of leading whitespace in a line.
* `bl(string_view)`: Determines if a line is entirely blank.
* `cs(string_view, maxWidth, TabWidth)`: Returns the number of leading characters to skip without exceeding `maxWidth`.

## Requirements
* C++17 or later (uses std::string_view, enhanced constexpr, etc.).
* Supported on GCC 7+, Clang 5+, and MSVC 2017 15.7+.

# License
This project is licensed under the MIT License. See the LICENSE file for details.
