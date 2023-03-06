#include "BcfPass.hpp"

namespace obfusc {
    BcfPass::BcfPass() {}
    BcfPass::~BcfPass() {}

    bool BcfPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        return true;
    }

}