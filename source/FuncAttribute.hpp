#pragma once

#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "FuncAttributeStore.hpp"

namespace obfusc {
    template<const char... obfName>
    class FuncAttribute : public clang::ParsedAttrInfo {
    public:
        virtual bool diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const override {
            if (!clang::isa<clang::FunctionDecl>(D)) { //This attribute appertains to functions only.
                S.Diag(Attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type_str) << Attr << "functions";
                return false;
            }
            return true;
        }

        virtual AttrHandling handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const override {
            if (!D->getDeclContext()->isFileContext()) { //Check if the decl is at file scope.
                unsigned ID = S.getDiagnostics().getDiagnosticIDs()->getCustomDiagID(clang::DiagnosticIDs::Error, Attr.getAttrName()->deuglifiedName());
                S.Diag(Attr.getLoc(), ID) << " attribute only allowed at file scope";
                return AttributeNotApplied;
            }
        
            if (Attr.getNumArgs() > OptArgs) {
                unsigned ID = S.getDiagnostics().getDiagnosticIDs()->getCustomDiagID(clang::DiagnosticIDs::Error, Attr.getAttrName()->deuglifiedName());
                S.Diag(Attr.getLoc(), ID) << "attribute only accepts no arguments";
                return AttributeNotApplied;
            }

            D->addAttr(clang::AnnotateAttr::Create(S.Context, Attr.getAttrName()->deuglifiedName(), nullptr, 0, Attr.getRange()));
            return AttributeApplied;
        }
    
    protected:
        inline FuncAttribute() {
            OptArgs = 0;
            static constexpr const char nameStr[] = { obfName..., '\0' };
            static constexpr const char cxxNameStr[] = {'o', 'b', 'f', 'u', 's', 'c', ':', ':', obfName..., '\0' };

            static const Spelling S[] {{clang::ParsedAttr::AS_GNU, nameStr},
                        {clang::ParsedAttr::AS_CXX11, nameStr},
                        {clang::ParsedAttr::AS_CXX11, cxxNameStr}};

            Spellings = S;

            FuncAttributeStore::GetInstance().StoreAttributeName(nameStr);
        }
    };

    #define NEW_FUNC_ATTR(name, ...) \
        class name ## Attribute : public FuncAttribute<__VA_ARGS__> { }; \
        static clang::ParsedAttrInfoRegistry::Add<name ## Attribute> name ## Clang("obfusc_" #name, "")

}
