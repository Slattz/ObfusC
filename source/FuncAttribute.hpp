#pragma once

#include <clang/Sema/ParsedAttr.h>
#include <clang/Sema/Sema.h>
#include <clang/Sema/SemaDiagnostic.h>
#include "FuncAttributeStore.hpp"
#include "IObfuscationPass.hpp"


namespace obfusc {
    template<typename passType, const char... obfName>
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
            if ((!D->getDeclContext()->isFileContext())) { //Check if the decl is at file scope
                if (D->getDeclContext()->getDeclKind() != clang::Decl::Kind::CXXRecord) { //or if it's a lambda (other CXXRecords are covered by diagAppertainsToDecl)
                    std::string attrStr(Attr.getAttrName()->deuglifiedName().data());
                    attrStr.append(" attribute only allowed at file scope and on lambdas");

                    unsigned ID = S.getDiagnostics().getDiagnosticIDs()->getCustomDiagID(clang::DiagnosticIDs::Error, llvm::StringRef(attrStr));
                    S.Diag(Attr.getLoc(), ID);
                    return AttributeNotApplied;
                }
            }
        
            if (Attr.getNumArgs() > OptArgs) {
                std::string attrStr(Attr.getAttrName()->deuglifiedName().data());
                attrStr.append("attribute only accepts no arguments");

                unsigned ID = S.getDiagnostics().getDiagnosticIDs()->getCustomDiagID(clang::DiagnosticIDs::Error, llvm::StringRef(attrStr));
                S.Diag(Attr.getLoc(), ID);
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

            FuncAttributeStore::GetInstance().StoreAttributeInfo(nameStr, new passType());
        }
    };

    //name ## Pass class (e.g. MbaPass) needs to be included before using this macro.
    #define NEW_FUNC_ATTR(name, ...) \
        static_assert(std::is_convertible<name ## Pass*, IObfuscationPass*>::value, #name "Pass must inherit IObfuscationPass as public"); \
        class name ## Attribute : public FuncAttribute<name ## Pass, __VA_ARGS__> { }; \
        static clang::ParsedAttrInfoRegistry::Add<name ## Attribute> name ## Clang("obfusc_" #name, "") 


}
