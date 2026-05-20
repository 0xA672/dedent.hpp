#ifndef DEDENT_HPP
#define DEDENT_HPP

#include <array>
#include <string_view>
#include <cstddef>

namespace d {
namespace dt {
    constexpr bool sp(char c) {
        return c == ' ' || c == '\t';
    }
    constexpr size_t lw(std::string_view sv, size_t tw = 4) {
        size_t w = 0;
        for (char c : sv) {
            if (c == ' ') w += 1;
            else if (c == '\t') w += tw;
            else break;
        }
        return w;
    }
    constexpr bool bl(std::string_view sv) {
        for (char c : sv) if (c != '\n' && !sp(c)) return false;
        return true;
    }
    constexpr size_t cs(std::string_view sv, size_t mx, size_t tw = 4) {
        size_t sc = 0, aw = 0;
        for (char c : sv) {
            if (c == ' ') {
                if (aw + 1 > mx) break;
                aw += 1; ++sc;
            } else if (c == '\t') {
                if (aw + tw > mx) break;
                aw += tw; ++sc;
            } else break;
        }
        return sc;
    }
}

template <size_t N>
constexpr std::string_view to_sv(const std::array<char, N>& a) {
    size_t len = 0;
    while (len < N && a[len] != '\0') ++len;
    return std::string_view(a.data(), len);
}

template <size_t N, size_t TW = 4>
constexpr std::array<char, N> dedent(const char (&in)[N]) {
    using namespace dt;
    constexpr size_t tw = TW;
    std::string_view sv(in, N - 1);

    struct Li { size_t off, len; };
    std::array<Li, N> ls{};
    size_t lc = 0, p = 0;
    while (p < sv.size()) {
        size_t nl = sv.find('\n', p);
        if (nl == std::string_view::npos) {
            ls[lc++] = {p, sv.size() - p};
            break;
        } else {
            ls[lc++] = {p, nl - p + 1};
            p = nl + 1;
        }
    }

    size_t fst = 0, lst = lc;
    while (fst < lst && bl({sv.data() + ls[fst].off, ls[fst].len})) ++fst;
    while (lst > fst && bl({sv.data() + ls[lst-1].off, ls[lst-1].len})) --lst;

    if (fst >= lst) {
        std::array<char, N> res{};
        return res;
    }

    size_t ciw = std::string_view::npos;
    for (size_t i = fst; i < lst; ++i) {
        std::string_view ln(sv.data() + ls[i].off, ls[i].len);
        if (!bl(ln)) {
            size_t w = lw(ln, tw);
            if (w < ciw) ciw = w;
        }
    }
    if (ciw == std::string_view::npos) ciw = 0;

    std::array<char, N> res{};
    size_t out = 0;
    for (size_t i = fst; i < lst; ++i) {
        std::string_view ln(sv.data() + ls[i].off, ls[i].len);
        if (bl(ln)) {
            if (!ln.empty() && ln.back() == '\n') res[out++] = '\n';
        } else {
            size_t sk = cs(ln, ciw, tw);
            for (size_t j = sk; j < ln.size(); ++j) res[out++] = ln[j];
        }
    }
    res[out] = '\0';
    return res;
}

}

#endif
