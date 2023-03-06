#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class MbaPass : public IObfuscationPass {
    public:
        MbaPass();
        ~MbaPass() override;

        bool obfuscate(llvm::Module& mod, llvm::Function& func) override;

    private:
        bool RunMbaAdd(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr);
        bool RunMbaSub(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr);
    };
}