#include <cstdio>
#include <cstdint>
#include <cassert>

namespace iSubTest {
    template<typename T>
    [[obfusc::isub]] T Add(T a, T b) {
        return a + b;
    }

    template<typename T>
    [[obfusc::isub]] T Sub(T a, T b) {
        return a - b;
    }
    template<typename T>
    [[obfusc::isub]] T And(T a, T b) {
        return a & b;
    }

    template<typename T>
    [[obfusc::isub]] T Or(T a, T b) {
        return a | b;
    }

    template<typename T>
    void TestBinaryOperator(T a, T b, T expectedRes, T (*op)(T, T)) {
        T testVal = op(a, b);
        printf("A: %lu B: %lu, Expected Res: %lu, Actual Res: %lu\n", static_cast<uint64_t>(a), static_cast<uint64_t>(b), static_cast<uint64_t>(expectedRes), static_cast<uint64_t>(testVal));
        assert(testVal == expectedRes);
    }

    template<typename T>
    void TestFloatBinaryOperator(T a, T b, T expectedRes, T (*op)(T, T)) {
        T testVal = op(a, b);
        printf("A: %f B: %f, Expected Res: %f, Actual Res: %f\n", a, b, expectedRes, testVal);
        assert(testVal == expectedRes);
    }

    void iSubTestAdd() {
        TestBinaryOperator<uint8_t>(200, 16, 216, Add<uint8_t>);
        TestBinaryOperator<uint16_t>(32'123, 12'321, 44'444, Add<uint16_t>);
        TestBinaryOperator<uint32_t>(987'654'321, 123'456'789, 1'111'111'110, Add<uint32_t>);
        TestBinaryOperator<uint64_t>(2'305'843'009'213'693'951, 2'305'843'009'213'693'951, 4'611'686'018'427'387'902, Add<uint64_t>);
        
        TestFloatBinaryOperator<float>(1.0f, 2.1f, 3.1f, Add<float>);
        TestFloatBinaryOperator<double>(1.0, 2.1, 3.1, Add<double>);
    }

    void iSubTestSub() {
        TestBinaryOperator<uint8_t>(220, 20, 200, Sub<uint8_t>);
        TestBinaryOperator<uint16_t>(32'123, 12'321, 19'802, Sub<uint16_t>);
        TestBinaryOperator<uint32_t>(987'654'321, 123'456'789, 864'197'532, Sub<uint32_t>);
        TestBinaryOperator<uint64_t>(3'305'843'009'213'693'951, 2'305'843'009'213'693'951, 1'000'000'000'000'000'000, Sub<uint64_t>);
        
        TestFloatBinaryOperator<float>(3.1f, 2.1f, 1.0f, Sub<float>);
        TestFloatBinaryOperator<double>(3.1, 2.1, 1.0, Sub<double>);
    }

    void iSubTestAnd() {
        TestBinaryOperator<uint8_t>( 2, 6, 2, And<uint8_t>);
        TestBinaryOperator<uint16_t>(2, 6, 2, And<uint16_t>);
        TestBinaryOperator<uint32_t>(2, 6, 2, And<uint32_t>);
        TestBinaryOperator<uint64_t>(2, 6, 2, And<uint64_t>);
    }

    void iSubTestOr() {
        TestBinaryOperator<uint8_t>( 2, 4, 6, Or<uint8_t>);
        TestBinaryOperator<uint16_t>(2, 4, 6, Or<uint16_t>);
        TestBinaryOperator<uint32_t>(2, 4, 6, Or<uint32_t>);
        TestBinaryOperator<uint64_t>(2, 4, 6, Or<uint64_t>);
    }

    void iSubTestAll() {
        iSubTestAdd();
        iSubTestSub();
        iSubTestAnd();
        iSubTestOr();
    }
}

#ifndef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    iSubTest::iSubTestAll();
    return 0;
}

#endif