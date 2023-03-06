#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class CffPass : public IObfuscationPass {
    public:
        CffPass();
        ~CffPass() override;

        bool obfuscate(llvm::Module& mod) override;
    };
}