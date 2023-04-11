#include <cstdio>
#include <cstdint>

namespace MbaTest {
    void MbaTestAll();
}

namespace CffTest {
    void CffTestAll();
}

namespace iSubTest {
    void iSubTestAll();
}

namespace BcfTest {
    void BcfTestAll();
}

#ifdef OBFUSC_TEST_BUILD_ALL

int main(int argc, char *argv[]) {
    MbaTest::MbaTestAll();
    CffTest::CffTestAll();
    iSubTest::iSubTestAll();
    BcfTest::BcfTestAll();
    return 0;
}

#endif