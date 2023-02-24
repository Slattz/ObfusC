
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "common.hpp"
#include "FuncAttributeStore.hpp"

struct FuncAnnotationsParser : llvm::PassInfoMixin<FuncAnnotationsParser> {
    enum AnnotationOperands {
        FUNCTION_OPERAND = 0,
        ANNOTATE_OPERAND = 1,
        FILENAME_OPERAND = 2
    };

    // Takes IR unit to run the pass on Module and the corresponding manager
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager&) {
        llvm::GlobalVariable* globalAttrs = M.getGlobalVariable("llvm.global.annotations"); //Get llvm.global.annotations from IR data
        if (globalAttrs) {
            llvm::ConstantArray* array = llvm::cast<llvm::ConstantArray>(globalAttrs->getOperand(0)); //Get array of operands
            for (llvm::Value* operand : array->operands()) { //Get each operand
                if (llvm::ConstantStruct* constant = llvm::dyn_cast<llvm::ConstantStruct>(operand)) { //Cast operand to a struct (i.e. the annotation struct)
                    if (constant->getNumOperands() >= 2) { //Must be at least two operands (FUNCTION_OPERAND and ANNOTATE_OPERAND)
                        llvm::Function* func = llvm::cast<llvm::Function>(constant->getOperand(AnnotationOperands::FUNCTION_OPERAND)); //Get function
                        llvm::GlobalVariable* globalStrPtr = llvm::cast<llvm::GlobalVariable>(constant->getOperand(AnnotationOperands::ANNOTATE_OPERAND));
                        if (func && globalStrPtr) {
                            if (llvm::ConstantDataArray* strArray = llvm::dyn_cast<llvm::ConstantDataArray>(globalStrPtr->getOperand(0))) { //Get Annotation str
                                func->addFnAttr(strArray->getAsString()); //add Annotation to function
                            }
                        }
                    }
                }
            }
        }
    
        return llvm::PreservedAnalyses::all();
    }

    // If false, pass is skipped for functions decorated with the optnone attribute (e.g. -O0).
    static bool isRequired() { return true; }
};

struct HelloWorld2 : llvm::PassInfoMixin<HelloWorld2> {
    // Takes IR unit to run the pass on Module and the corresponding manager
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager &) {
        for (auto& L : obfusc::FuncAttributeStore::GetInstance().GetAttributeNames()) {
            llvm::outs() << "Attr Registered: " << L << "\n";
        }

        for (auto&& F : M.getFunctionList()) {
            llvm::outs() << "Hello from: "<< F.getName() << "\n";
            llvm::outs() << "   number of arguments: " << F.arg_size() << "\n";
            llvm::outs() << "   number of attributes: " << F.getAttributes().getNumAttrSets() << "\n";
            for (auto&& A : F.getAttributes()) {
                llvm::outs() << A.getAsString() << "\n";
            }
        }
        return llvm::PreservedAnalyses::all();
    }

    // If false, pass is skipped for functions decorated with the optnone attribute (e.g. -O0).
    static bool isRequired() { return true; }
};

llvm::PassPluginLibraryInfo getHelloWorldPluginInfo() {
    llvm::outs() << "ObfusC Version: " << OBFUSC_VERSION_STR << "\n";
    return {LLVM_PLUGIN_API_VERSION, "ObfusC", OBFUSC_VERSION_STR,
        [](llvm::PassBuilder &PB) {
            PB.registerPipelineEarlySimplificationEPCallback(
                [=](llvm::ModulePassManager &MPM, llvm::OptimizationLevel Level) {
                    MPM.addPass(FuncAnnotationsParser());
                }
            );
            PB.registerOptimizerLastEPCallback(
                [=](llvm::ModulePassManager &MPM, llvm::OptimizationLevel Level) {
                    MPM.addPass(HelloWorld2());
                }
            );
        }
    };
}

// Dynamic Library Entrypoint
extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getHelloWorldPluginInfo();
}
