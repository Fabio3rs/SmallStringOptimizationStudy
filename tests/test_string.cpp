#include "../src/string.hpp"
#include <atomic>
#include <gtest/gtest.h>
#include <new>

static std::atomic<int64_t> allocations{0};
static std::atomic<int64_t> deallocations{0};

void *operator new(std::size_t n) {
    ++allocations;
    return malloc(n);
}
void operator delete(void *p) noexcept {
    ++deallocations;
    free(p);
}

TEST(SSOImpl, NewStringIsEmpty) {
    ssostr::string nstr;

    EXPECT_TRUE(nstr.empty());
}

TEST(SSOImpl, NewStringDoesNotAllocate) {
    const int64_t oldallocations = allocations;

    ssostr::string nstr;

    EXPECT_TRUE(nstr.empty());
    EXPECT_EQ(allocations, oldallocations);
}

TEST(SSOImpl, SmallStringIsNotEmpty) {
    ssostr::string nstr("small str");

    EXPECT_FALSE(nstr.empty());
}

TEST(SSOImpl, SmallStringDoesNotAllocate) {
    const int64_t oldallocations = allocations;

    ssostr::string nstr("small str");

    EXPECT_FALSE(nstr.empty());

    EXPECT_EQ(allocations, oldallocations);
}

TEST(SSOImpl, SmallStringAppendDoesNotAllocate) {
    const int64_t oldallocations = allocations;

    ssostr::string nstr("small str");

    EXPECT_FALSE(nstr.empty());

    nstr.append("0123456");

    EXPECT_EQ(allocations, oldallocations);
}

TEST(SSOImpl, SmallStringBigAppendAllocateOnce) {
    const int64_t oldallocations = allocations;

    ssostr::string nstr("small str");

    EXPECT_FALSE(nstr.empty());
    EXPECT_EQ(nstr.capacity(), 31uL);

    nstr.append("0123456789101112131415161718192021");

    EXPECT_EQ(nstr.capacity(), 64uL - 1);
    EXPECT_EQ(nstr.size(), 43uL);

    EXPECT_EQ(allocations, oldallocations + 1);
}

TEST(SSOImpl, SmallStringDualBigAppendAllocateTwice) {
    const int64_t oldallocations = allocations;
    const int64_t olddeallocations = deallocations;

    {
        ssostr::string nstr("small str");
        std::cout << nstr.c_str() << std::endl << std::endl;

        EXPECT_FALSE(nstr.empty());
        EXPECT_EQ(nstr.size(), 9uL);
        EXPECT_EQ(nstr.capacity(), 31uL);

        nstr.append("0123456789101#1213141516171819202$");
        std::cout << nstr.c_str() << std::endl << std::endl;

        nstr.append(
            "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz");
        std::cout << nstr.c_str() << std::endl << std::endl;

        EXPECT_EQ(nstr.capacity(), 128uL - 1);
        EXPECT_EQ(nstr.size(), 104uL);

        EXPECT_EQ(allocations, oldallocations + 2);

        EXPECT_EQ(deallocations, olddeallocations + 1);

        constexpr char strtmp[] = {
            "small "
            "str0123456789101#1213141516171819202$"
            "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"};

        std::cout << nstr.c_str() << std::endl << std::endl;
        std::cout << strtmp << std::endl << std::endl;
        EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
    }

    EXPECT_EQ(deallocations, olddeallocations + 2);
}

TEST(SSOImpl, SmallStringSizeEquals) {
    const char strtmp[] = {"small str"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    EXPECT_EQ(strnlen(strtmp, sizeof(strtmp)), nstr.size());
}

TEST(SSOImpl, SmallStringStrEquals) {
    const char strtmp[] = {"small str"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
}

TEST(SSOImpl, SmallStringCopyStrEquals) {
    const char strtmp[] = {"small str"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    ssostr::string nstrcopy(nstr);

    EXPECT_FALSE(nstrcopy.empty());

    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
    EXPECT_EQ(std::strncmp(strtmp, nstrcopy.c_str(), sizeof(strtmp)), 0);
}

TEST(SSOImpl, SmallStringCopyOperatorStrEquals) {
    const char strtmp[] = {"small str"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    ssostr::string nstrcopy;

    nstrcopy = nstr;

    EXPECT_FALSE(nstrcopy.empty());

    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
    EXPECT_EQ(std::strncmp(strtmp, nstrcopy.c_str(), sizeof(strtmp)), 0);
}

TEST(SSOImpl, BigStringCopyOperatorStrEquals) {
    const char strtmp[] = {
        "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    ssostr::string nstrcopy;

    nstrcopy = nstr;

    EXPECT_FALSE(nstrcopy.empty());

    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
    EXPECT_EQ(std::strncmp(strtmp, nstrcopy.c_str(), sizeof(strtmp)), 0);
}

TEST(SSOImpl, BigStringCopyOperatorAppendStrEquals) {
    const char strtmp[] = {
        "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    ssostr::string nstrcopy;

    nstrcopy = nstr;

    EXPECT_FALSE(nstrcopy.empty());

    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
    EXPECT_EQ(std::strncmp(strtmp, nstrcopy.c_str(), sizeof(strtmp)), 0);

    nstrcopy.append("9876543210-1-2-3-4-5-6-7-8-9-10");

    EXPECT_EQ(nstrcopy.capacity(), 127);
    EXPECT_EQ(nstr.capacity(), 63);
}

TEST(SSOImpl, BigStringAppendCopyOperatorStrEquals) {
    const int64_t olddeallocations = deallocations;

    {
        const char strtmp[] = {
            "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"};

        ssostr::string nstr(strtmp);

        EXPECT_FALSE(nstr.empty());
        EXPECT_EQ(deallocations, olddeallocations);

        const char *bigstr =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi "
            "ornare "
            "luctus ligula, eu facilisis est aliquet sed. Vestibulum non "
            "tellus at "
            "sem mollis viverra nec ut ipsum. Praesent orci sem, ornare non "
            "leo "
            "sed, sodales dapibus ipsum. Aenean id purus nec mauris mattis "
            "maximus. Aenean id lacus sapien. Pellentesque et interdum nunc, "
            "vel "
            "suscipit elit. Suspendisse quis pretium ante, eget fringilla "
            "justo. "
            "Cras pretium orci vel nibh pulvinar, in lobortis diam auctor. "
            "Pellentesque ut lacus finibus, placerat quam eu, lacinia leo. "
            "Etiam "
            "viverra fermentum odio ut iaculis. Vestibulum vel odio sed enim "
            "interdum tempor. Aenean tincidunt diam vel euismod egestas.";

        nstr.append(bigstr);

        EXPECT_EQ(deallocations, olddeallocations + 1);

        ssostr::string nstrcopy;

        nstrcopy = nstr;

        EXPECT_EQ(deallocations, olddeallocations + 1);

        EXPECT_FALSE(nstrcopy.empty());
        EXPECT_EQ(nstrcopy.capacity(), 767);
        EXPECT_EQ(nstrcopy.size(), 719);
        EXPECT_EQ(
            std::strcmp(
                nstrcopy.c_str(),
                "0123456789101112131415161718192021 abcdefghijklmnopqrstuvwxyz"
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                "Morbi "
                "ornare "
                "luctus ligula, eu facilisis est aliquet sed. Vestibulum non "
                "tellus at "
                "sem mollis viverra nec ut ipsum. Praesent orci sem, ornare "
                "non "
                "leo "
                "sed, sodales dapibus ipsum. Aenean id purus nec mauris mattis "
                "maximus. Aenean id lacus sapien. Pellentesque et interdum "
                "nunc, "
                "vel "
                "suscipit elit. Suspendisse quis pretium ante, eget fringilla "
                "justo. "
                "Cras pretium orci vel nibh pulvinar, in lobortis diam auctor. "
                "Pellentesque ut lacus finibus, placerat quam eu, lacinia leo. "
                "Etiam "
                "viverra fermentum odio ut iaculis. Vestibulum vel odio sed "
                "enim "
                "interdum tempor. Aenean tincidunt diam vel euismod egestas."),
            0);
    }
    
    EXPECT_EQ(deallocations, olddeallocations + 3);
}

TEST(SSOImpl, SmallStringStrCapacity) {
    const char strtmp[] = {"small str"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    EXPECT_EQ(nstr.capacity(), 31uL);
}

TEST(SSOImpl, BigStringWillAllocateOnce) {
    const int64_t oldallocations = allocations;

    const char strtmp[] = {
        "small str 0123456789 abcdefghijklmnopqrstuvwxyz 12345678890-"};

    ssostr::string nstr(strtmp);

    EXPECT_FALSE(nstr.empty());

    EXPECT_EQ(allocations, oldallocations + 1);
}

TEST(SSOImpl, BigStringIsEqual) {
    const char strtmp[] = {
        "small str 0123456789 abcdefghijklmnopqrstuvwxyz 12345678890-"};

    ssostr::string nstr(strtmp);

    EXPECT_EQ(strnlen(strtmp, sizeof(strtmp)), nstr.size());
    EXPECT_EQ(strnlen(strtmp, sizeof(strtmp)), strlen(nstr.c_str()));
    EXPECT_EQ(std::strncmp(strtmp, nstr.c_str(), sizeof(strtmp)), 0);
}

TEST(SSOImpl, BigStringWillDeAllocateOnceWhenDestructed) {
    const int64_t olddeallocations = deallocations;

    {
        const char strtmp[] = {
            "small str 0123456789 abcdefghijklmnopqrstuvwxyz 12345678890-"};

        ssostr::string nstr(strtmp);

        EXPECT_FALSE(nstr.empty());
    }

    EXPECT_EQ(deallocations, olddeallocations + 1);
}
