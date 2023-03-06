#pragma once
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

class IObfuscationPass {
public:
    virtual bool obfuscate(llvm::Module& mod, llvm::Function& func) = 0;
protected:
    IObfuscationPass() {}
    virtual ~IObfuscationPass() {}
};