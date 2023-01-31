#include "MbaAttribute.hpp"

namespace obfusc {
    static clang::ParsedAttrInfoRegistry::Add<MbaAttribute> mbaClang("obfusc_mba", "");
}