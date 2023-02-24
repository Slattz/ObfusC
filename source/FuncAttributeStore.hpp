#pragma once
#include <vector>

namespace obfusc {
    class FuncAttributeStore {
    public:
        static FuncAttributeStore& GetInstance() {
            static FuncAttributeStore x;
            return x;
        }

        const std::vector<const char*>& GetAttributeNames() {
            return m_names;
        }

        void StoreAttributeName(const char* name) {
            m_names.push_back(name);
        }
        
    private:
        std::vector<const char*> m_names;
    };
}
