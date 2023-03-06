
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "common.hpp"
#include "FuncAnnotationsParser.hpp"
#include "ObfuscationPassManager.hpp"

namespace obfusc {
    llvm::PassPluginLibraryInfo getObfuscPluginInfo() {
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
                        MPM.addPass(obfusc::ObfuscationPassManager());
                    }
                );
            }
        };
    }
}

// Dynamic Library Entrypoint
extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return obfusc::getObfuscPluginInfo();
}
