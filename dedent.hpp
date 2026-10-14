#ifndef DEDENT_HPP
#define DEDENT_HPP

#include <array>
#include <cstddef>
#include <limits>
#include <string_view>

namespace d {
namespace dt {

constexpr bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

constexpr size_t stop = std::string_view::npos;

// Visual width of character `c` at column `col`, or `stop` for any
// character that ends the leading whitespace run. A tab advances to the
// next multiple of TabWidth (tab stops), so its width depends on the
// column it starts at.
template <size_t TW>
constexpr size_t width(size_t col, char c) {
    if (c == ' ') return 1;
    if (c == '\t') return TW - (col % TW);
    return stop;
}

template <size_t TW>
constexpr size_t leading_width(std::string_view line) {
    size_t w = 0;
    for (char c : line) {
        size_t step = width<TW>(w, c);
        if (step == stop) break;
        w += step;
    }
    return w;
}

constexpr bool is_blank(std::string_view line) {
    for (char c : line)
        if (c != '\n' && !is_space(c))
            return false;
    return true;
}

template <size_t TW>
constexpr size_t chars_to_skip(std::string_view line, size_t max_width) {
    size_t skip = 0, col = 0;
    for (char c : line) {
        size_t step = width<TW>(col, c);
        if (step == stop) break;
        if (col + step > max_width) break;
        col += step;
        ++skip;
    }
    return skip;
}

} // namespace dt

template <size_t N>
[[nodiscard]] constexpr std::string_view to_sv(const std::array<char, N>& a) noexcept {
    size_t len = 0;
    while (len < N && a[len] != '\0') ++len;
    return std::string_view(a.data(), len);
}

template <size_t N>
[[nodiscard]] constexpr std::string_view sv(const std::array<char, N>& a) noexcept {
    return to_sv(a);
}

// Rejects `d::sv(d::dedent(...))`: the view would outlive the temporary
// array it points into. Bind the result of `dedent` to a named variable
// first.
template <size_t N>
constexpr std::string_view sv(const std::array<char, N>&&) = delete;

template <size_t TW = 4, size_t N>
[[nodiscard]] constexpr std::array<char, N> dedent(const char (&in)[N]) noexcept {
    static_assert(TW > 0, "TabWidth must be greater than zero");
    static_assert(TW < std::numeric_limits<size_t>::max() / 2,
                  "TabWidth is too large");

    if (N <= 1) {
        std::array<char, N> res{};
        return res;
    }

    std::string_view input(in, N - 1);

    struct LineRef { size_t off, len; };
    std::array<LineRef, N> lines{};
    size_t line_count = 0, p = 0;
    while (p < input.size()) {
        size_t nl = input.find('\n', p);
        if (nl == std::string_view::npos) {
            lines[line_count++] = {p, input.size() - p};
            break;
        } else {
            lines[line_count++] = {p, nl - p + 1};
            p = nl + 1;
        }
    }

    // Trim leading and trailing blank lines.
    size_t first = 0, last = line_count;
    while (first < last && dt::is_blank({input.data() + lines[first].off, lines[first].len})) ++first;
    while (last > first && dt::is_blank({input.data() + lines[last - 1].off, lines[last - 1].len})) --last;

    if (first >= last) {
        std::array<char, N> res{};
        return res;
    }

    // Common indent: minimum leading width over all non-blank lines.
    // `first < last` guarantees that at least one non-blank line exists.
    size_t common_indent = std::string_view::npos;
    for (size_t i = first; i < last; ++i) {
        std::string_view line(input.data() + lines[i].off, lines[i].len);
        if (!dt::is_blank(line)) {
            size_t w = dt::leading_width<TW>(line);
            if (w < common_indent) common_indent = w;
        }
    }

    std::array<char, N> res{};
    size_t out = 0;
    for (size_t i = first; i < last; ++i) {
        std::string_view line(input.data() + lines[i].off, lines[i].len);
        // Internal blank lines collapse to a single line break. Such a
        // line always ends with '\n' here: trailing blank lines were
        // trimmed above, so it has a following line.
        if (dt::is_blank(line)) {
            if (line.size() >= 2 && line[line.size() - 2] == '\r') res[out++] = '\r';
            res[out++] = '\n';
            continue;
        }
        size_t skip = dt::chars_to_skip<TW>(line, common_indent);
        for (size_t j = skip; j < line.size(); ++j)
            res[out++] = line[j];
    }
    res[out] = '\0';
    return res;
}

} // namespace d

#endif
