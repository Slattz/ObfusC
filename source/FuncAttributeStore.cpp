#include "FuncAttribute.hpp"
#include <cstring>

namespace obfusc {
    FuncAttributeStore& FuncAttributeStore::GetInstance() {
        static FuncAttributeStore x;
        return x;
    }

    bool FuncAttributeStore::IsAttrStored(llvm::StringRef& attr) {
        if (attr.size() < 2) {
            return false;
        }

        const char* attrCStr = attr.data();

        if (attr[0] == '\"') {
            attrCStr = attr.substr(1, attr.size()-2).data();
        }

        for (auto& info : m_info) {
            if (strncasecmp(info.name, attrCStr, info.size) == 0) {
                return true;
            }
        }
        
        return false;
    }

    IObfuscationPass* FuncAttributeStore::GetAttrPass(llvm::StringRef&& attr) {
        if (attr.size() < 2) {
            return nullptr;
        }

        const char* attrCStr = attr.data();

        if (attr[0] == '\"') {
            attrCStr = attr.substr(1, attr.size()-2).data();
        }

        for (auto& info : m_info) {
            if (strncasecmp(info.name, attrCStr, info.size) == 0) {
                return info.pass;
            }
        }
        
        return nullptr;
    }

    void FuncAttributeStore::StoreAttributeInfo(const char* name, IObfuscationPass* pass) {
        m_info.push_back({name, pass, strlen(name)});
    }
}
