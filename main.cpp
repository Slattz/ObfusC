
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "common.hpp"

struct FuncAnnotationsParser : llvm::PassInfoMixin<FuncAnnotationsParser> {
    enum AnnotationOperands {
        FUNCTION_OPERAND = 0,
        ANNOTATE_OPERAND = 1,
        FILE_OPERAND = 2
    };

    // Takes IR unit to run the pass on Module and the corresponding manager
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager&) {
        llvm::GlobalVariable* globalAttrs = M.getGlobalVariable("llvm.global.annotations");
        if (globalAttrs) {
            llvm::ConstantArray* array = llvm::cast<llvm::ConstantArray>(globalAttrs->getOperand(0));
            for (llvm::Value* operand : array->operands()) {
                if (llvm::ConstantStruct* constant = llvm::dyn_cast<llvm::ConstantStruct>(operand)) {
                    if (constant->getNumOperands() >= 2) {
                        if (llvm::Function* func = llvm::cast<llvm::Function>(constant->getOperand(AnnotationOperands::FUNCTION_OPERAND))) {
                            if (llvm::GlobalVariable* globalStrPtr = llvm::cast<llvm::GlobalVariable>(constant->getOperand(AnnotationOperands::ANNOTATE_OPERAND))) {
                                if (llvm::ConstantDataArray* strArray = llvm::dyn_cast<llvm::ConstantDataArray>(globalStrPtr->getOperand(0))) {
                                    func->addFnAttr(strArray->getAsString());
                                }
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
