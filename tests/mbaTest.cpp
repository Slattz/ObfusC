#include <cstdio>
#include <iostream>
#include <cstdint>
#include <cassert>

namespace MbaTest {
    template<typename T>
    [[obfusc::mba]] T Add(T a, T b) {
        return a + b;
    }

    template<typename T>
    [[obfusc::mba]] T Sub(T a, T b) {
        return a - b;
    }

    template<typename T>
    [[obfusc::mba]] T Mul(T a, T b) {
        return a * b;
    }

    template<typename T>
    [[obfusc::mba]] T Div(T a, T b) {
        return a / b;
    }

    template<typename T>
    void TestBinaryOperator(T a, T b, T result, T (*op)(T, T)) {
        T testVal = op(a, b);
        printf("A: %lld B: %lld, Expected Res: %llu, Actual Res: %lld\n", a, b,result, testVal);
        assert(testVal == result);
    }

    void MbaTestAdd() {
        TestBinaryOperator<uint8_t>(200, 16, 216, Add<uint8_t>);
        TestBinaryOperator<uint16_t>(32123, 12321, 44444, Add<uint16_t>);
        TestBinaryOperator<uint32_t>(987654321, 123456789, 1111111110, Add<uint32_t>);
        TestBinaryOperator<uint64_t>(2'305'843'009'213'693'951, 2'305'843'009'213'693'951, 4'611'686'018'427'387'902, Add<uint64_t>);
    }

    void MbaTestSub() {
        TestBinaryOperator<uint8_t>(220, 20, 200, Sub<uint8_t>);
        TestBinaryOperator<uint16_t>(32123, 12321, 19802, Sub<uint16_t>);
        TestBinaryOperator<uint32_t>(987654321, 123456789, 864197532, Sub<uint32_t>);
        TestBinaryOperator<uint64_t>(3'305'843'009'213'693'951, 2'305'843'009'213'693'951, 1'000'000'000'000'000'000, Sub<uint64_t>);
    }

    void MbaTestAll() {
        MbaTestAdd();
        MbaTestSub();
    }
}

#ifndef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    MbaTest::MbaTestAll();
    return 0;
}

#endif