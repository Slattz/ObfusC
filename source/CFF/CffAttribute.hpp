#pragma once

#include "FuncAttribute.hpp"

namespace obfusc {
    class CffAttribute : public FuncAttribute {
    public:
        CffAttribute() : FuncAttribute() { 
            static constexpr Spelling S[] = {{clang::ParsedAttr::AS_GNU, "cff"},
                                    {clang::ParsedAttr::AS_CXX11, "cff"},
                                    {clang::ParsedAttr::AS_CXX11, "obfusc::cff"}};
            Spellings = S;
        }
    };
}

