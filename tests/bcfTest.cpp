#include <cstdio>
#include <cstdint>
#include <cassert>

namespace BcfTest {
    
    [[obfusc::bcf]] int BcfTestVal(int a, int b) {
        return a+b+20;
    }
    
    
    void BcfTestAll() {
        int ret = BcfTestVal(10, 10);
        printf("Bcf Ret: %d\n", ret);
    }
}

#ifndef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    BcfTest::BcfTestAll();
    return 0;
}

#endif