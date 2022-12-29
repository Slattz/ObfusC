
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

namespace {

struct HelloWorld : llvm::PassInfoMixin<HelloWorld> {
  // Takes IR unit to run the pass on Module and the corresponding manager
  llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager &) {
    for (auto&& F : M.getFunctionList()) {
      for (auto&& A : F.getAttributes()) {
        //TODO
      }
      llvm::errs() << "Hello from: "<< F.getName() << "\n";
      llvm::errs() << "   number of arguments: " << F.arg_size() << "\n";
    }
    
    return llvm::PreservedAnalyses::all();
  }

  // If false, pass is skipped for functions decorated with the optnone attribute (e.g. -O0).
  static bool isRequired() { return true; }
};
}

llvm::PassPluginLibraryInfo getHelloWorldPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,
        [](llvm::PassBuilder &PB) {
            PB.registerPipelineEarlySimplificationEPCallback(
            [=](llvm::ModulePassManager &MPM, llvm::OptimizationLevel Level) {
                MPM.addPass(HelloWorld());
            });
        }
    };
}

// Dynamic Library Entrypoint
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getHelloWorldPluginInfo();
}
