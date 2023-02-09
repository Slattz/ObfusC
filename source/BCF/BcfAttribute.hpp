#pragma once

#include "FuncAttribute.hpp"

namespace obfusc {
    class BcfAttribute : public FuncAttribute {
    public:
        BcfAttribute() : FuncAttribute() { 
            static constexpr Spelling S[] = {{clang::ParsedAttr::AS_GNU, "bcf"},
                                    {clang::ParsedAttr::AS_CXX11, "bcf"},
                                    {clang::ParsedAttr::AS_CXX11, "obfusc::bcf"}};
            Spellings = S;
        }
    };
}

