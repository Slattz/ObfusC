#pragma once

#include "FuncAttribute.hpp"

namespace obfusc {
    class MbaAttribute : public FuncAttribute {
    public:
        MbaAttribute() : FuncAttribute() { 
            static constexpr Spelling S[] = {{clang::ParsedAttr::AS_GNU, "mba"},
                                    {clang::ParsedAttr::AS_CXX11, "mba"},
                                    {clang::ParsedAttr::AS_CXX11, "obfusc::mba"}};
            Spellings = S;
        }
    };
}

