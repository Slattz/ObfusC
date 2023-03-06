#pragma once
#include <llvm/Passes/PassBuilder.h>

class IObfuscationPass {
public:
    virtual bool obfuscate(llvm::Module& mod, llvm::Function& func) = 0;
protected:
    IObfuscationPass() {}
    virtual ~IObfuscationPass() {}
};