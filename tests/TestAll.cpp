#include <cstdio>
#include <cstdint>

namespace MbaTest {
    void MbaTestAll();
}

#ifdef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    MbaTest::MbaTestAll();
    return 0;
}

#endif