#include "string.hpp"
#include <iostream>
#include <cassert>

int main() {

    std::cout << "Hello world!" << std::endl;
    {
        const char strtmp[] = {"small str"};

        ssostr::string nstr(strtmp);

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;

        nstr.append("-pato.");

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;
    }

    std::cout << std::endl;

    {
        const char strtmp[] = {"med str 01234567891011121314156"};

        ssostr::string nstr(strtmp);

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;

        nstr.append("-pato.");

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;
    }

    std::cout << std::endl;

    {
        const char strtmp[] = {"med1 str 01234567891011121314156"};

        ssostr::string nstr(strtmp);

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;

        nstr.append("-pato.");

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;
    }

    std::cout << std::endl;

    {
        const char strtmp[] = {
            "big str 012345678910111213141567891011123315649498"};

        ssostr::string nstr(strtmp);

        nstr.reserve(128);

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;

        nstr.append("-pato voador aquatico que voa.");

        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        std::cout << nstr.c_str() << std::endl;
    }

    std::cout << std::endl;

    {
        ssostr::string nstr("small str");
        std::cout << nstr.c_str() << std::endl;
        assert(nstr.size() == 9);

        nstr.append("0123456789101#1213141516171819202$");
        std::cout << nstr.c_str() << std::endl;
        std::cout << nstr.capacity() << std::endl;
        assert(nstr.size() == 43);
        assert(nstr.capacity() == 63);

        nstr.append(
            "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz");
        std::cout << nstr.c_str() << std::endl;
        std::cout << nstr.capacity() << std::endl;
        std::cout << nstr.size() << std::endl;
        assert(nstr.size() == 104);
        assert(nstr.capacity() == 127);

        constexpr char strtmp[] = {
            "small "
            "str0123456789101#1213141516171819202$"
            "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"};

        std::cout << nstr.c_str() << std::endl;
        std::cout << strtmp << std::endl;
        assert(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)) == 0);
    }

    return 0;
}
