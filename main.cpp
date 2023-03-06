
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "common.hpp"
#include "FuncAnnotationsParser.hpp"

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
                    MPM.addPass(obfusc::FuncAnnotationsParser());
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
