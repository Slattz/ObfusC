#include "BcfAttribute.hpp"

namespace obfusc {
    static clang::ParsedAttrInfoRegistry::Add<BcfAttribute> mbaClang("obfusc_bcf", "");
}