#include "MbaPass.hpp"

namespace obfusc {
    MbaPass::MbaPass() {}
    MbaPass::~MbaPass() {}

    bool MbaPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        llvm::outs() << "Hello from: "<< __PRETTY_FUNCTION__ << "\n";

        return true;
    }

}