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

        static constexpr size_t s_RecursiveAmount = 100;

        uint64_t GetSignedMax(llvm::Type* type);
        llvm::Value* GenStackAlignmentCode(llvm::IRBuilder<>& irBuilder, llvm::Type* newType, llvm::Value* operand);
        llvm::Value* Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Type* origType, llvm::Value* operand, size_t numRecursions = 0);
    };
}