#include "FuncAttribute.hpp"
#include <cstring>

namespace obfusc {
    FuncAttributeStore& FuncAttributeStore::GetInstance() {
        static FuncAttributeStore x;
        return x;
    }

    bool FuncAttributeStore::IsAttrStored(const char* attr) {
        if (!attr || strlen(attr) < 1) {
            return false;
        }

        for (auto& info : m_info) {
            if (strncasecmp(info.name, attr, info.size) == 0) {
                return true;
            }
        }
        
        return false;
    }

    void FuncAttributeStore::StoreAttributeName(const char* name) {
        m_info.push_back({name, strlen(name)});
    }
}
