#pragma once

#include "FuncAttribute.hpp"

namespace obfusc {
    class iSubAttribute : public FuncAttribute {
    public:
        iSubAttribute() : FuncAttribute() { 
            static constexpr Spelling S[] = {{clang::ParsedAttr::AS_GNU, "isub"},
                                    {clang::ParsedAttr::AS_CXX11, "isub"},
                                    {clang::ParsedAttr::AS_CXX11, "obfusc::isub"}};
            Spellings = S;
        }
    };
}

