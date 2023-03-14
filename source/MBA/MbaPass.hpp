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
            Divide,
            Not,
            Xor,
            Max
        };

        static constexpr size_t s_RecursiveAmount = 5;

        int GetRandomNumber(llvm::Type* type);
        llvm::Value* Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Value* operand, size_t numRecursions = 0);
    };
}