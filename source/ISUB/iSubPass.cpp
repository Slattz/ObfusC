#include "iSubPass.hpp"

namespace obfusc {
    iSubPass::iSubPass() {}
    iSubPass::~iSubPass() {}

    bool iSubPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        return true;
    }

}