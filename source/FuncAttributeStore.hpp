#pragma once
#include <vector>
#include "IObfuscationPass.hpp"

namespace obfusc {
    class FuncAttributeStore {
    public:
        static FuncAttributeStore& GetInstance();
        bool IsAttrStored(const char* attr);
        IObfuscationPass* GetAttrPass(const char* attr);
        void StoreAttributeInfo(const char* name, IObfuscationPass* pass);
        
    private:
        FuncAttributeStore();

        struct AttrStoreInfo {
            const char* name;
            IObfuscationPass* pass;
            size_t size;
        };
        std::vector<AttrStoreInfo> m_info;
    };
}
