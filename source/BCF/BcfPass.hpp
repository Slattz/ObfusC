#pragma once
#include "IObfuscationPass.hpp"

//Heavily based on Obfuscator-LLVM
//https://github.com/obfuscator-llvm/obfuscator/blob/llvm-4.0/lib/Transforms/Obfuscation/BogusControlFlow.cpp

namespace obfusc {
    class BcfPass : public IObfuscationPass {
    public:
        BcfPass();
        ~BcfPass() override;

        bool obfuscate(llvm::Module& mod, llvm::Function& func) override;

    private:
        llvm::BasicBlock* MakeBogusBlock(llvm::BasicBlock* originalBlock, llvm::Function& func);
    };
}