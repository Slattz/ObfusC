#include "FuncAttribute.hpp"

namespace obfusc {
    bool FuncAttribute::diagAppertainsToDecl(clang::Sema& S, const clang::ParsedAttr& Attr, const clang::Decl* D) const {
        if (!clang::isa<clang::FunctionDecl>(D)) { //This attribute appertains to functions only.
            S.Diag(Attr.getLoc(), clang::diag::warn_attribute_wrong_decl_type_str) << Attr << "functions";
            return false;
        }
        return true;
    }

    clang::ParsedAttrInfo::AttrHandling FuncAttribute::handleDeclAttribute(clang::Sema& S, clang::Decl* D, const clang::ParsedAttr& Attr) const {
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

        llvm::outs() << "Here 03" << "\n";
        D->addAttr(clang::AnnotateAttr::Create(S.Context, Attr.getAttrName()->deuglifiedName(), nullptr, 0, Attr.getRange()));
        return AttributeApplied;
    }
}
