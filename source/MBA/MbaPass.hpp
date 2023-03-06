#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class MbaPass : public IObfuscationPass {
    public:
        MbaPass();
        ~MbaPass() override;

        bool obfuscate(llvm::Module& mod) override;
    };
}