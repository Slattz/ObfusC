#pragma once
#include <llvm/Passes/PassBuilder.h>

namespace obfusc {
    struct ObfuscationPassManager : llvm::PassInfoMixin<ObfuscationPassManager> {
        // Takes IR unit to run the pass on Module and the corresponding manager
        llvm::PreservedAnalyses run(llvm::Module& mod, llvm::ModuleAnalysisManager&);

        // If false, pass is skipped for functions decorated with the optnone attribute (e.g. -O0).
        static bool isRequired() { return true; }
    };
}