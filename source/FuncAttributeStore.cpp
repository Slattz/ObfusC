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

    IObfuscationPass* FuncAttributeStore::GetAttrPass(const char* attr) {
        if (!attr || strlen(attr) < 1) {
            return nullptr;
        }

        for (auto& info : m_info) {
            if (strncasecmp(info.name, attr, info.size) == 0) {
                return info.pass;
            }
        }
        
        return nullptr;
    }

    void FuncAttributeStore::StoreAttributeInfo(const char* name, IObfuscationPass* pass) {
        m_info.push_back({name, pass, strlen(name)});
    }
}
