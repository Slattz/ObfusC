
#include "IObfuscationPass.hpp"

IObfuscationPass::IObfuscationPass() {
    std::random_device rd;  //Get hardware specific random device
    m_randGen64.seed(rd()); //Seed Mersenne Twister random generator  
}

/* Get a random number that is half the types bit length (e.g. 64 bit for 128 bit values) */
uint64_t IObfuscationPass::GetRandomNumber(llvm::Type* type) {
    uint64_t modNum = UCHAR_MAX; //UCHAR_MAX covers 8-bit and 16-bit
    if (type->getIntegerBitWidth() == 32) { //32-bit
        modNum = USHRT_MAX;
    } else if (type->getIntegerBitWidth() >= 64) { //64-bit and 128-bit
        modNum = UINT_MAX;
    }

    return m_randGen64()%modNum;
}