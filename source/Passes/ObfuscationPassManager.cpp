#include "ObfuscationPassManager.hpp"
#include "IObfuscationPass.hpp"
#include "FuncAttributeStore.hpp"

namespace obfusc {
    llvm::PreservedAnalyses ObfuscationPassManager::run(llvm::Module& mod, llvm::ModuleAnalysisManager&) {
        bool changed = false;
        for (auto& func : mod.getFunctionList()) { //Get all functions in module
            for (auto& attrs : func.getAttributes()) { //Get each attribute lists attached to function
                for (auto& attr : attrs) { //Get attributes one by one
                    if (attr.isStringAttribute()) { //If attribute is a string
                        IObfuscationPass* pass = FuncAttributeStore::GetInstance().GetAttrPass(llvm::StringRef(attr.getAsString().data())); //Check if attr is an obfuscation pass
                        if (pass) {
                            changed |= pass->obfuscate(mod, func); //Call obfuscate func
                        }
                    }
                }
            }
        }

        if (changed) {
            return llvm::PreservedAnalyses::none();
        }
        return llvm::PreservedAnalyses::all();
    }
}