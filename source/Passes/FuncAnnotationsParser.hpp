#pragma once
#include <llvm/Passes/PassBuilder.h>

namespace obfusc {
struct FuncAnnotationsParser : llvm::PassInfoMixin<FuncAnnotationsParser> {
    enum AnnotationOperands {
        FUNCTION_OPERAND = 0,
        ANNOTATE_OPERAND = 1,
        FILENAME_OPERAND = 2
    };

    // Takes IR unit to run the pass on Module and the corresponding manager
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager&);

    // If false, pass is skipped for functions decorated with the optnone attribute (e.g. -O0).
    static bool isRequired() { return true; }
};
}