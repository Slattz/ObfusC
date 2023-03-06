#include "MbaPass.hpp"

namespace obfusc {
    MbaPass::MbaPass() {}
    MbaPass::~MbaPass() {}

    bool MbaPass::obfuscate(llvm::Module& mod) {
        return true;
    }

}