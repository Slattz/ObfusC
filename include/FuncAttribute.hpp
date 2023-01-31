#pragma once

#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include <string>

namespace obfusc {
    class FuncAttribute : public clang::ParsedAttrInfo {
    public:
        virtual bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override;
        virtual AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override;
    
    protected:
        inline FuncAttribute() {
            OptArgs = 0;
        }
    };
}
