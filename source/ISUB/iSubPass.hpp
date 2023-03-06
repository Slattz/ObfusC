#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class iSubPass : public IObfuscationPass {
    public:
        iSubPass();
        ~iSubPass() override;

        bool obfuscate(llvm::Module& mod, llvm::Function& func) override;
    };
}