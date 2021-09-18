#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace ssostr {
static constexpr uint8_t SSOFLAG = 1 << 7;

class string {
    struct owning_string_view {
        char *ptr{nullptr};
        char pad[8];
        std::size_t sizeval{0};
        std::size_t capacityval{0};
    };

    struct sso_t {
        char buffer[31];
        std::uint8_t flags{0u};
    };

    union internal_data_t {
        owning_string_view copyval;
        sso_t ssoval;

        internal_data_t() : ssoval{} {}
    } internal_data_o;

    constexpr bool is_sso() const {
        return (internal_data_o.ssoval.flags & SSOFLAG) == 0;
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
               ~(static_cast<size_t>(SSOFLAG) << 56uLL);
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
            SSOFLAG; // Disables clang-tidy false-positive memory leak warning
        internal_data_o.copyval.ptr = ptr;
        internal_data_o.copyval.capacityval =
            (allocated - 1) |
            (static_cast<size_t>(SSOFLAG)
             << 56uLL); // allocated mem - 1 to get capacity in characters
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

    void handle_copy(const char *ptr, std::size_t nsize) {
        if (nsize > capacity()) {
            allocate_mem(nsize);
        }

        char *buffer = get_buffer();
        std::copy(ptr, ptr + nsize, buffer);
        buffer[nsize] = 0;

        internal_set_size(nsize);
    }

  public:
    const char *c_str() const {
        if (is_sso()) {
            return internal_data_o.ssoval.buffer;
        }

        return internal_data_o.copyval.ptr;
    }

    std::size_t size() const {
        if (is_sso()) {
            return sso_size();
        }

        return size_no_sso();
    }

    std::size_t capacity() const {
        if (is_sso()) {
            return sso_capacity();
        }

        return capacity_no_sso();
    }

    bool empty() const noexcept { return size() == 0; }

    void reserve(size_t n) { realloc_grow(n); }

    void append(const char *inputval) {
        std::size_t inputvaluesize = std::strlen(inputval);
        std::size_t newsize = inputvaluesize + size();
        realloc_grow(newsize);

        char *current_buffer = get_buffer();
        std::size_t current_size = size();
        std::copy(inputval, inputval + inputvaluesize,
                  std::addressof(current_buffer[current_size]));

        current_buffer[newsize] = 0;

        internal_set_size(newsize);
    }

    string &operator=(const string &str) {
        if (std::addressof(str) == this) {
            return *this;
        }

        handle_copy(str.c_str(), str.size());

        return *this;
    }

    string &operator=(const char *str) {
        if (str == c_str()) {
            return *this;
        }

        handle_copy(str, std::strlen(str));

        return *this;
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

static_assert(sizeof(ssostr::string) == 32,
              "ssostr::string deve ter o tamanho 32");
