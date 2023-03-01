#pragma once
#include <vector>

namespace obfusc {
    class FuncAttributeStore {
    public:
        static FuncAttributeStore& GetInstance();
        bool IsAttrStored(const char* attr);
        void StoreAttributeName(const char* attr);
        
    private:
        struct AttrStoreInfo {
            const char* name;
            size_t size;
        };
        std::vector<AttrStoreInfo> m_info;
    };
}
