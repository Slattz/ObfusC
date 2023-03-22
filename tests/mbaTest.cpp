#include <cstdio>
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
    void TestBinaryOperator(T a, T b, T expectedRes, T (*op)(T, T)) {
        T testVal = op(a, b);
        printf("A: %lu B: %lu, Expected Res: %lu, Actual Res: %lu\n", static_cast<uint64_t>(a), static_cast<uint64_t>(b), static_cast<uint64_t>(expectedRes), static_cast<uint64_t>(testVal));
        assert(testVal == expectedRes);
    }

    void MbaTestAdd() {
        TestBinaryOperator<uint8_t>(200, 16, 216, Add<uint8_t>);
        TestBinaryOperator<uint16_t>(32'123, 12'321, 44'444, Add<uint16_t>);
        TestBinaryOperator<uint32_t>(987'654'321, 123'456'789, 1'111'111'110, Add<uint32_t>);
        TestBinaryOperator<uint64_t>(2'305'843'009'213'693'951, 2'305'843'009'213'693'951, 4'611'686'018'427'387'902, Add<uint64_t>);
    }

    void MbaTestSub() {
        TestBinaryOperator<uint8_t>(220, 20, 200, Sub<uint8_t>);
        TestBinaryOperator<uint16_t>(32'123, 12'321, 19'802, Sub<uint16_t>);
        TestBinaryOperator<uint32_t>(987'654'321, 123'456'789, 864'197'532, Sub<uint32_t>);
        TestBinaryOperator<uint64_t>(3'305'843'009'213'693'951, 2'305'843'009'213'693'951, 1'000'000'000'000'000'000, Sub<uint64_t>);
    }

    void MbaTestMul() {
        TestBinaryOperator<uint8_t>(120, 2, 240, Mul<uint8_t>);
        TestBinaryOperator<uint16_t>(600, 100, 60'000, Mul<uint16_t>);
        TestBinaryOperator<uint32_t>(600'000, 1'000, 600'000'000, Mul<uint32_t>);
        TestBinaryOperator<uint64_t>(2'305'843'009'213'693'951, 2, 4'611'686'018'427'387'902, Mul<uint64_t>);
    }

    void MbaTestDiv() {
        TestBinaryOperator<uint8_t>(240, 2, 120, Div<uint8_t>);
        TestBinaryOperator<uint16_t>(60'000, 100, 600, Div<uint16_t>);
        TestBinaryOperator<uint32_t>(600'000'000, 1'000, 600'000, Div<uint32_t>);
        TestBinaryOperator<uint64_t>(4'611'686'018'427'387'902, 2, 2'305'843'009'213'693'951, Div<uint64_t>);
    }

    void MbaTestAll() {
        MbaTestAdd();
        MbaTestSub();
        MbaTestMul();
        MbaTestDiv();
    }
}

#ifndef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    MbaTest::MbaTestAll();
    return 0;
}

#endif