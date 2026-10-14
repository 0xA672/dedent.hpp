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
// Treat a tab as advancing to the next multiple of 2 columns
constexpr auto text = d::dedent<2>(R"(
	First line
		Indented line
)");
```
### Handling Blank Lines
* Blank lines at the very beginning and end of the input (containing only spaces, tabs, `\r`, `\f`, or `\v`) are removed entirely.
* Internal blank lines are preserved as a single line break: `\n` for LF input, `\r\n` for CRLF input.
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
`TabWidth`: tab stop distance in columns. A `\t` advances to the next multiple of `TabWidth`. Tabs are never partially removed, so with mixed space/tab indentation a tab may be kept even when the spaces before it are trimmed. Defaults to `4`.

### Return Value
`std::array<char, N>` where `N` is the size of the input literal (including its terminating `'\0'`). The dedented text starts at index 0, is terminated by `'\0'`, and the rest of the array is zero‑filled. Use `d::sv` to obtain a `std::string_view` of the dedented text.

Bind the result to a named variable before calling `d::sv`; `d::sv(d::dedent(...))` is rejected at compile time because the view would refer to a temporary array:
```cpp
constexpr auto code = d::dedent(R"(...)");
constexpr auto view = d::sv(code);  // ok
```

### `d::sv(const std::array<char, N>& a)`
### Purpose
Converts the array returned by `dedent` into a `std::string_view` for convenient output or further processing. The rvalue overload is deleted, so a view into a temporary array cannot be created by accident.

Return Value
`std::string_view` covering the dedented string (excluding the null terminator).

## Implementation Details
The library uses internal helper functions inside the `d::dt` namespace, all of which are `constexpr`:
* `is_space(char)`: Checks whether a character is `' '`, `'\t'`, `'\r'`, `'\f'`, or `'\v'`.
* `width<TabWidth>(col, char)`: Returns the visual width of a leading‑whitespace character at column `col` (a `\t` advances to the next multiple of `TabWidth`), or a sentinel for any character that ends the leading whitespace run.
* `leading_width<TabWidth>(string_view)`: Computes the total visual width of a line's leading whitespace.
* `is_blank(string_view)`: Determines if a line contains only whitespace.
* `chars_to_skip<TabWidth>(string_view, max_width)`: Returns the number of leading characters to skip so the remaining visual width does not exceed `max_width`.

## Requirements
* C++17 or later (uses std::string_view, enhanced constexpr, etc.).
* Built and tested in CI with GCC, Clang, AppleClang, and MSVC (see `.github/workflows/ci.yml`).

# License
This project is licensed under the MIT License. See the LICENSE file for details.
