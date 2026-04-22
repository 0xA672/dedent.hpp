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
Leading/trailing blank lines are removed automatically; internal blank lines become a single `\n`.
```cpp
constexpr auto poem = d::dedent(R"(

    Roses are red,
    Violets are blue,

    Sugar is sweet,
    And so are you.

)");
// Result: "Roses are red,\nViolets are blue,\n\nSugar is sweet,\nAnd so are you.\n"
```
