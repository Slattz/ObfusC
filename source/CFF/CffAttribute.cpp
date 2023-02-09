#include "CffAttribute.hpp"

namespace obfusc {
    static clang::ParsedAttrInfoRegistry::Add<CffAttribute> mbaClang("obfusc_cff", "");
}