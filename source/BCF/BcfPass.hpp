#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class BcfPass : public IObfuscationPass {
    public:
        BcfPass();
        ~BcfPass() override;

        bool obfuscate(llvm::Module& mod) override;
    };
}