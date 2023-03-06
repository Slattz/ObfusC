#pragma once
#include <vector>
#include <llvm/ADT/StringRef.h>
#include "IObfuscationPass.hpp"

namespace obfusc {
    class FuncAttributeStore {
    public:
        static FuncAttributeStore& GetInstance();
        bool IsAttrStored(llvm::StringRef& attr);
        IObfuscationPass* GetAttrPass(llvm::StringRef&& attr);
        void StoreAttributeInfo(const char* name, IObfuscationPass* pass);
        
    private:
        FuncAttributeStore() {}

        struct AttrStoreInfo {
            const char* name;
            IObfuscationPass* pass;
            size_t size;
        };
        std::vector<AttrStoreInfo> m_info;
    };
}
