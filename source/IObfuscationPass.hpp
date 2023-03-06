#pragma once
#include <llvm/Passes/PassBuilder.h>

class IObfuscationPass {
public:
    virtual bool obfuscate(llvm::Module& mod) = 0;
protected:
    IObfuscationPass() {}
    virtual ~IObfuscationPass() {}
};