#include "iSubPass.hpp"

namespace obfusc {
    iSubPass::iSubPass() {}
    iSubPass::~iSubPass() {}

    bool iSubPass::obfuscate(llvm::Module& mod) {
        return true;
    }

}