#include "FuncAnnotationsParser.hpp"
#include "FuncAttributeStore.hpp"

namespace obfusc {
    llvm::PreservedAnalyses FuncAnnotationsParser::run(llvm::Module& M, llvm::ModuleAnalysisManager&) {
        llvm::GlobalVariable* globalAttrs = M.getGlobalVariable("llvm.global.annotations"); //Get llvm.global.annotations from IR data
        if (!globalAttrs) {
            return llvm::PreservedAnalyses::all();
        }
        
        llvm::ConstantArray* array = llvm::cast<llvm::ConstantArray>(globalAttrs->getOperand(0)); //Get array of operands    
        for (llvm::Value* operand : array->operands()) { //Get each operand
            llvm::ConstantStruct* constant = llvm::dyn_cast<llvm::ConstantStruct>(operand); //Cast operand to a struct (i.e. the annotation struct)
            if (!constant || constant->getNumOperands() < 2) { //Must be at least two operands (FUNCTION_OPERAND and ANNOTATE_OPERAND) 
                continue;
            }
            
            llvm::Function* func = llvm::cast<llvm::Function>(constant->getOperand(AnnotationOperands::FUNCTION_OPERAND)); //Get function
            llvm::GlobalVariable* globalStrPtr = llvm::cast<llvm::GlobalVariable>(constant->getOperand(AnnotationOperands::ANNOTATE_OPERAND));
            if (!func || !globalStrPtr) {
                continue;
            }

            if (llvm::ConstantDataArray* strArray = llvm::dyn_cast<llvm::ConstantDataArray>(globalStrPtr->getOperand(0))) { //Get Annotation str
                llvm::StringRef str = strArray->getAsString();
                if (FuncAttributeStore::GetInstance().IsAttrStored(str)) {
                    func->addFnAttr(str); //add Annotation to function
                }
            }
        }

        return llvm::PreservedAnalyses::all();
    }
}
