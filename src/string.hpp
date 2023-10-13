#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace ssostr {
static constexpr uint8_t NO_SSOFLAG = 1 << 7;

class string {
  public:
    struct __attribute__((aligned(1))) owning_string_view {
        char *ptr{nullptr};
        std::size_t sizeval{0};
        std::size_t capacityval{0};
    };

    struct __attribute__((aligned(1))) sso_t {
        char buffer[31];
        std::uint8_t flags{0u};
    };

    union internal_data_t {
        owning_string_view copyval;
        sso_t ssoval;

        internal_data_t() : ssoval{} {}
    } internal_data_o;

    constexpr bool is_sso() const {
        return (internal_data_o.ssoval.flags & NO_SSOFLAG) == 0;
    }

    constexpr char *get_buffer() {
        if (is_sso()) {
            return internal_data_o.ssoval.buffer;
        }

        return internal_data_o.copyval.ptr;
    }

    constexpr size_t sso_size() const {
        return 31 - static_cast<size_t>(internal_data_o.ssoval.flags & 31);
    }

    constexpr std::size_t sso_capacity() const { return 31; }

    constexpr std::size_t capacity_no_sso() const {
        return internal_data_o.copyval.capacityval &
               ~(static_cast<size_t>(NO_SSOFLAG) << 56uLL);
    }

    constexpr std::size_t size_no_sso() const {
        return internal_data_o.copyval.sizeval;
    }

    static constexpr std::size_t align_size(std::size_t nsize) {
        return ((nsize + 63) / 64) * 64;
    }

    void cleanup() {
        if (!is_sso()) {
            delete[] internal_data_o.copyval.ptr;
            internal_data_o.copyval.ptr = nullptr;
        }

        internal_data_o = internal_data_t{};
    }

    void set_ptr_capacity(char *ptr, size_t allocated) {
        internal_data_o.ssoval.flags =
            NO_SSOFLAG; // Disables clang-tidy false-positive memory leak
                        // warning
        internal_data_o.copyval.ptr = ptr;
        internal_data_o.copyval.capacityval =
            (allocated - 1); // allocated mem - 1 to get capacity in characters
    }

    void allocate_mem(std::size_t nsize) {
        cleanup();

        if (nsize > sso_capacity()) {
            nsize = align_size(nsize + 1);
            set_ptr_capacity(new char[nsize], nsize);
            internal_data_o.copyval.sizeval = 0;
        }
    }

    void realloc_grow(size_t nsize) {
        if (nsize > capacity()) {
            nsize = align_size(nsize + 1);
            char *nptr = new char[nsize];
            size_t tmpsizevar = size();

            {
                char *ibuff = get_buffer();
                std::copy(ibuff, ibuff + tmpsizevar, nptr);
                nptr[tmpsizevar] = 0;
            }

            if (!is_sso()) {
                try {
                    cleanup();
                } catch (...) {
                }
            }

            set_ptr_capacity(nptr, nsize);
            internal_data_o.copyval.sizeval = tmpsizevar;
        }
    }

    void internal_set_size(std::size_t nsize) {
        if (is_sso()) {
            internal_data_o.ssoval.flags =
                static_cast<std::uint8_t>(sso_capacity() - nsize);
        } else {
            internal_data_o.copyval.sizeval = nsize;
        }
    }

    constexpr void handle_copy(const char *ptr, std::size_t nsize) {
        if (nsize > capacity()) {
            allocate_mem(nsize);
        }

        char *buffer = get_buffer();
        std::copy(ptr, ptr + nsize, buffer);
        buffer[nsize] = 0;

        internal_set_size(nsize);
    }

  public:
    constexpr const char *c_str() const {
        if (is_sso()) {
            return internal_data_o.ssoval.buffer;
        }

        return internal_data_o.copyval.ptr;
    }

    constexpr std::string_view as_sv() const {
        return std::string_view(c_str(), size());
    }

    constexpr std::size_t size() const {
        if (is_sso()) {
            return sso_size();
        }

        return size_no_sso();
    }

    constexpr std::size_t capacity() const {
        if (is_sso()) {
            return sso_capacity();
        }

        return capacity_no_sso();
    }

    void reserve(size_t n) { realloc_grow(n); }

    constexpr char &operator[](size_t i) { return get_buffer()[i]; }

    constexpr char *data() { return get_buffer(); }

    constexpr bool empty() const noexcept { return size() == 0; }

    // Comparators
    int compare(std::string_view str) const {
        return std::strncmp(c_str(), str.data(), size());
    }

    int compare(const string &str) const {
        return std::strncmp(c_str(), str.c_str(), size());
    }

    bool operator==(std::string_view str) const { return compare(str) == 0; }

    bool operator==(const string &str) const { return compare(str) == 0; }

    friend bool operator<(const string &rhs, const string &lhs) {
        return rhs.compare(lhs) < 0;
    }

    // Append
    string &operator+=(const string &str) {
        append(std::string_view(str.c_str(), str.size()));
        return *this;
    }

    string &operator+=(std::string_view str) {
        append(str);
        return *this;
    }

    string &operator+=(char c) {
        append(std::string_view(&c, 1));
        return *this;
    }

    void append(std::string_view inputval) {
        std::size_t inputvaluesize = inputval.size();
        std::size_t newsize = inputvaluesize + size();
        realloc_grow(newsize);

        char *current_buffer = get_buffer();
        std::size_t current_size = size();
        std::copy(inputval.data(), inputval.data() + inputvaluesize,
                  std::addressof(current_buffer[current_size]));

        current_buffer[newsize] = 0;

        internal_set_size(newsize);
    }

    // Assign
    string &operator=(const string &str) {
        if (std::addressof(str) == this) {
            return *this;
        }

        handle_copy(str.c_str(), str.size());

        return *this;
    }

    string &operator=(std::string_view str) {
        if (str.data() == c_str()) {
            return *this;
        }

        handle_copy(str.data(), str.size());

        return *this;
    }

    explicit string(std::string_view str) : string() {
        handle_copy(str.data(), str.size());
    }

    explicit string(const char *str) : string() {
        handle_copy(str, std::strlen(str));
    }

    explicit string(const string &str) : string() {
        handle_copy(str.c_str(), str.size());
    }

    string() { internal_data_o.ssoval.flags = 31; }

    ~string() { cleanup(); }
};
} // namespace ssostr

namespace std {
template <> struct hash<ssostr::string> {
    size_t operator()(const ssostr::string &str) const {
        return hash<std::string_view>()(std::string_view(
            str.c_str(),
            str.size())); // std::hash using std::string_view to minimize copies
    }
};
} // namespace std

static_assert(sizeof(ssostr::string) == 32,
              "ssostr::string deve ter o tamanho 32");
