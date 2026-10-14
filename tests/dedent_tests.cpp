#include "dedent.hpp"

#include <array>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

// Every check below is compile-time: if this translation unit builds, the
// tests pass. The binary only exists so that CTest has something to run.
namespace {

// --- README example ----------------------------------------------------------

constexpr const char readme_literal[] = R"(
    int main() {
        return 0;
    }
)";

constexpr auto readme_code = d::dedent(readme_literal);
static_assert(d::sv(readme_code) == "int main() {\n    return 0;\n}\n");

// The return type is std::array<char, N> where N is the size of the input
// literal (including '\0'), not the dedented length.
static_assert(std::tuple_size_v<decltype(readme_code)> == sizeof(readme_literal));

// --- README poem example: internal blank lines -------------------------------

constexpr auto poem = d::dedent(R"(

    Roses are red,
    Violets are blue,

    Sugar is sweet,
    And so are you.

)");
static_assert(d::sv(poem) ==
    "Roses are red,\nViolets are blue,\n\nSugar is sweet,\nAnd so are you.\n");

// --- Internal blank lines collapse to a single line break --------------------

// Regression: a blank line indented wider than the common indent used to
// keep the leftover spaces.
constexpr auto wide_blank = d::dedent("    a\n      \n    b\n");
static_assert(d::sv(wide_blank) == "a\n\nb\n");

// Blank line indented narrower than the common indent.
constexpr auto narrow_blank = d::dedent("    a\n  \n    b\n");
static_assert(d::sv(narrow_blank) == "a\n\nb\n");

// Leading and trailing blank lines are removed, whatever their width.
constexpr auto blank_edges = d::dedent("      \n\n    a\n\t\n      \n");
static_assert(d::sv(blank_edges) == "a\n");

// --- Line endings ------------------------------------------------------------

constexpr auto crlf = d::dedent("    a\r\n    b\r\n");
static_assert(d::sv(crlf) == "a\r\nb\r\n");

// CRLF blank lines keep their '\r'.
constexpr auto crlf_blank = d::dedent("    a\r\n      \r\n    b\r\n");
static_assert(d::sv(crlf_blank) == "a\r\n\r\nb\r\n");

// A CRLF blank line inside LF input keeps its '\r' too.
constexpr auto mixed_eol = d::dedent("    a\n\r\n    b\n");
static_assert(d::sv(mixed_eol) == "a\n\r\nb\n");

// --- Tabs: tab stops ---------------------------------------------------------

// " \t" reaches column 4 exactly like "    " does, so both lines dedent fully.
constexpr auto tab_stops = d::dedent<4>(" \tx\n    y\n");
static_assert(d::sv(tab_stops) == "x\ny\n");

// Pure tab indentation.
constexpr auto tabs = d::dedent("\tint x;\n\tint y;\n");
static_assert(d::sv(tabs) == "int x;\nint y;\n");

// Custom tab widths (also exercises the README example).
constexpr auto tw2 = d::dedent<2>("\tFirst\n\t\tSecond\n");
static_assert(d::sv(tw2) == "First\n\tSecond\n");

constexpr auto tw1 = d::dedent<1>("\tx\n\ty\n");
static_assert(d::sv(tw1) == "x\ny\n");

constexpr auto tw8 = d::dedent<8>("\t\tx\n\t\ty\n");
static_assert(d::sv(tw8) == "x\ny\n");

// Tabs are never partially removed: a tab that would overshoot the common
// indent is kept even though it starts before it.
constexpr auto partial_tab = d::dedent<4>(" \tabc\n   def\n");
static_assert(d::sv(partial_tab) == "\tabc\ndef\n");

// --- Misc --------------------------------------------------------------------

// No trailing newline.
constexpr auto no_newline = d::dedent("    a\n    b");
static_assert(d::sv(no_newline) == "a\nb");

// Empty and blank-only inputs.
constexpr auto empty = d::dedent("");
static_assert(d::sv(empty).empty());

constexpr auto just_newline = d::dedent("\n");
static_assert(d::sv(just_newline).empty());

constexpr auto all_blank = d::dedent("\n  \n\t \n");
static_assert(d::sv(all_blank).empty());

// Single unindented character.
constexpr auto one_char = d::dedent("x");
static_assert(d::sv(one_char) == "x");

// Trailing whitespace on non-blank lines is preserved.
constexpr auto trailing_ws = d::dedent("    a  \n    b\t\n");
static_assert(d::sv(trailing_ws) == "a  \nb\t\n");

// Relative indentation is preserved.
constexpr auto nested = d::dedent("    if (x) {\n        y();\n    }\n");
static_assert(d::sv(nested) == "if (x) {\n    y();\n}\n");

// A non-blank line without indentation reduces the common indent to zero.
constexpr auto zero_indent = d::dedent("    a\nb\n    c\n");
static_assert(d::sv(zero_indent) == "    a\nb\n    c\n");

// --- API: d::sv rejects rvalue arrays ----------------------------------------

template <class T, class = void>
struct sv_accepts : std::false_type {};

template <class T>
struct sv_accepts<T, std::void_t<decltype(d::sv(std::declval<T>()))>> : std::true_type {};

static_assert(sv_accepts<std::array<char, 4>&>::value, "lvalue arrays are accepted");
static_assert(!sv_accepts<std::array<char, 4>>::value, "rvalue arrays are rejected");

} // namespace

int main() {
    // Runtime smoke test: same code path without constant evaluation.
    auto code = d::dedent("    hello\n    world\n");
    return d::sv(code) == "hello\nworld\n" ? 0 : 1;
}
