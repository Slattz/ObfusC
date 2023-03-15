#include "MbaPass.hpp"
#include <cstdlib>
#include <limits>

namespace obfusc {
    MbaPass::MbaPass() {
        std::random_device rd;  //Get hardware specific random device
        m_randGen64.seed(rd()); //Seed Mersenne Twister random generator  
    }

    MbaPass::~MbaPass() {}

    bool MbaPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        bool changed = false;

        for (auto& block : func) {
            for (auto instruction = block.begin(); instruction != block.end(); instruction++) {
                llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction); 
                if (!binOp) //Is instruction a binary op
                    continue;

                llvm::IRBuilder irBuilder(binOp);
                llvm::Type* newType = llvm::IntegerType::getInt128Ty(binOp->getType()->getContext()); //Set type to 128 bit to account for possible int overflows

                llvm::Value* op1 = GenStackAlignmentCode(irBuilder, newType, binOp->getOperand(0));
                llvm::Value* op2 = GenStackAlignmentCode(irBuilder, newType, binOp->getOperand(1));

                op1 = Substitute(irBuilder, newType, op1); //Generate MBA for op1
                op2 = Substitute(irBuilder, newType, op2); //Generate MBA for op2

                llvm::Instruction* newInstrs;
                unsigned opCode = binOp->getOpcode();
                switch (opCode) { //Gen underlying operation
                case llvm::Instruction::Add:
                    newInstrs = llvm::BinaryOperator::CreateAdd(op1, op2);
                    break;

                case llvm::Instruction::Sub:
                    newInstrs = llvm::BinaryOperator::CreateSub(op1, op2);
                    break;

                case llvm::Instruction::Mul:
                    newInstrs = llvm::BinaryOperator::CreateMul(op1, op2);
                    break;

                case llvm::Instruction::UDiv:
                    newInstrs = llvm::BinaryOperator::CreateUDiv(op1, op2);
                    break;

                case llvm::Instruction::SDiv:
                    newInstrs = llvm::BinaryOperator::CreateSDiv(op1, op2);
                    break;

                case llvm::Instruction::And:
                    newInstrs = llvm::BinaryOperator::CreateAnd(op1, op2);
                    break;

                case llvm::Instruction::Or:
                    newInstrs = llvm::BinaryOperator::CreateOr(op1, op2);
                    break;

                case llvm::Instruction::Xor:
                    newInstrs = llvm::BinaryOperator::CreateXor(op1, op2);
                    break;

                case llvm::Instruction::LShr:
                    newInstrs = llvm::BinaryOperator::CreateLShr(op1, op2);
                    break;

                case llvm::Instruction::Shl:
                    newInstrs = llvm::BinaryOperator::CreateShl(op1, op2);
                    break;

                default:
                    continue;
                }

                llvm::ReplaceInstWithInst(block.getInstList(), instruction, newInstrs); //replace old instruction with new obfuscation instructions
                changed = true;
            }

            llvm::outs() << "Function " << func.getName() << " Has Been Obfuscated: " << changed << "\n";
        }

        return changed;
    }

    /* Get a random number that is half the types bit length (e.g. 64 bit for 128 bit values) */
    int MbaPass::GetRandomNumber(llvm::Type* type) {
        uint64_t modNum = UCHAR_MAX; 
        if (type->getIntegerBitWidth() == 16) {
            modNum = UCHAR_MAX;
        } else if (type->getIntegerBitWidth() == 32) {
            modNum = USHRT_MAX;
        } else if (type->getIntegerBitWidth() == 64) {
            modNum = UINT_MAX;
        } else if (type->getIntegerBitWidth() == 128) {
            modNum = ULLONG_MAX;
        }

        return m_randGen64()%modNum;
    }

    /* Gen loads and stores to convert initial value to a 128 bit value (e.g. 32 bit to 128 bit) */
    llvm::Value* MbaPass::GenStackAlignmentCode(llvm::IRBuilder<>& irBuilder, llvm::Type* newType, llvm::Value* operand) {
        llvm::Instruction* alloc = irBuilder.CreateAlloca(newType);
        auto constZero = llvm::ConstantInt::get(newType, 0);
        llvm::Instruction* store = irBuilder.CreateStore(constZero, alloc);
        store = irBuilder.CreateStore(operand, alloc);
        return irBuilder.CreateLoad(newType, alloc);
    }

    /* Recursive function to generate the various mixed arithmetic IR. Essentially creates a reversible tree of IR instructions */
    llvm::Value* MbaPass::Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Value* operand, size_t numRecursions) {
        if (numRecursions >= s_RecursiveAmount) {
            return operand;
        }

        llvm::Instruction* instr = llvm::dyn_cast<llvm::Instruction>(operand);
        if (instr) {
            unsigned int opCode = instr->getOpcode();
            if ((opCode == llvm::Instruction::Add) || (opCode == llvm::Instruction::Sub) || (opCode == llvm::Instruction::UDiv) ||
                (opCode == llvm::Instruction::SDiv) || (opCode == llvm::Instruction::Xor)) 
            {
                operand->mutateType(type);
            }
        }

        int randType = rand() % SubstituteType::Max; //Randomly make obfuscation operation
        auto randVal = llvm::ConstantInt::get(type, GetRandomNumber(type)); //Get random number for each operation

        /* x = (x - randVal) + randVal */
        if (randType == SubstituteType::Add) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateSub(operand, randVal), numRecursions+1);
            return irBuilder.CreateAdd(a, randVal);
        }

        /* x = (x + randVal) - randVal */
        else if (randType == SubstituteType::Subtract) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateAdd(operand, randVal), numRecursions+1);
            return irBuilder.CreateSub(a, randVal);
        }

        /* x = (x * randVal) / randVal */
        else if (randType == SubstituteType::Divide) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateMul(operand, randVal), numRecursions+1);
            return irBuilder.CreateSDiv(a, randVal);
        }

        /* x = ~(~x) */
        else if (randType == SubstituteType::Not) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateNot(operand), numRecursions+1);
            return irBuilder.CreateNot(a);
        }

        /* x = (x ^ randVal) ^ randVal */
        else if (randType == SubstituteType::Xor) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateXor(operand, randVal), numRecursions+1);
            return irBuilder.CreateXor(a, randVal);
        }

        return operand;
    }
    
}