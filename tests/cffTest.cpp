#include <cstdio>
#include <cstdint>
#include <cassert>

namespace CffTest {
    
    [[obfusc::cff]] int CffTestVal(int numLoops) {
        int ret = 0;
        for (int i = 0; i < numLoops; i++) {
            ret++;
        }

        return ret;
    }
    
    
    void CffTestAll() {
        int ret = CffTestVal(100);
        printf("Cff Ret: %d\n", ret);
    }
}

#ifndef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    CffTest::CffTestAll();
    return 0;
}

#endif