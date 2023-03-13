#pragma once
#include "IObfuscationPass.hpp"

namespace obfusc {
    class MbaPass : public IObfuscationPass {
    public:
        MbaPass();
        ~MbaPass() override;

        bool obfuscate(llvm::Module& mod, llvm::Function& func) override;

    private:
        enum SubstituteType : int {
            Add = 0,
            Subtract,
            Multiply,
            Divide,
            Max
        };

        static constexpr size_t s_RecursiveAmount = 3;

        llvm::Value* Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Value* operand, size_t numRecursions = 0);
        bool RunMbaAdd(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr);
        bool RunMbaSub(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr);
    };
}