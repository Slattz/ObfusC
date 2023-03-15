#include "MbaPass.hpp"
#include <cstdlib>

namespace obfusc {
    MbaPass::MbaPass() {}
    MbaPass::~MbaPass() {}

    bool MbaPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        bool changed = false;
        srand(2);

        for (auto& block : func) {
            for (auto instruction = block.begin(); instruction != block.end(); instruction++) {
                llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction);
                if (!binOp)
                    continue;

                llvm::IRBuilder irBuilder(binOp);
                llvm::Type* newType = llvm::IntegerType::getInt128Ty(binOp->getType()->getContext());

                llvm::Value* op1 = GenStackAlignmentCode(irBuilder, newType, binOp->getOperand(0));
                llvm::Value* op2 = GenStackAlignmentCode(irBuilder, newType, binOp->getOperand(1));

                op1 = Substitute(irBuilder, newType, op1);
                op2 = Substitute(irBuilder, newType, op2);

                llvm::Instruction* newInstrs;
                unsigned opCode = binOp->getOpcode();
                if (opCode == llvm::Instruction::Add) {
                    newInstrs = llvm::BinaryOperator::CreateAdd(op1, op2);
                }
                // else if (opCode == llvm::Instruction::Sub) {
                    // newInstrs = llvm::BinaryOperator::CreateSub(op1, op2);
                // }
                else {
                    continue;
                }


                llvm::ReplaceInstWithInst(block.getInstList(), instruction, newInstrs);
                changed = true;
            }
        }

        return changed;
    }

    int MbaPass::GetRandomNumber(llvm::Type* type) {
        return rand()%(type->getIntegerBitWidth()*32);
    }

    llvm::Value* MbaPass::GenStackAlignmentCode(llvm::IRBuilder<>& irBuilder, llvm::Type* newType, llvm::Value* operand) {
        llvm::Instruction* alloc = irBuilder.CreateAlloca(newType);
        auto constZero = llvm::ConstantInt::get(newType, 0);
        llvm::Instruction* store = irBuilder.CreateStore(constZero, alloc);
        store = irBuilder.CreateStore(operand, alloc);
        return irBuilder.CreateLoad(newType, alloc);
    }

    llvm::Value* MbaPass::Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Value* operand, size_t numRecursions) {
        llvm::outs() << "Op: " << numRecursions << "\t" << *operand << "\n";
        if (numRecursions >= s_RecursiveAmount) {
            return operand;
        }

        llvm::Instruction* instr = llvm::dyn_cast<llvm::Instruction>(operand);
        if (instr) {
            unsigned int opCode = instr->getOpcode();
            if ((opCode == llvm::Instruction::Add) || (opCode == llvm::Instruction::Sub) || (opCode == llvm::Instruction::UDiv) ||
                (opCode == llvm::Instruction::SDiv) || (opCode == llvm::Instruction::Xor)
            ) {
                operand->mutateType(type);
            }
        }

        int randType = rand() % SubstituteType::Max;
        uint64_t randNum = GetRandomNumber(type);

        if (randType == SubstituteType::Add) {
            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto a = Substitute(irBuilder, type, irBuilder.CreateSub(operand, randVal), numRecursions+1);
            return irBuilder.CreateAdd(a, randVal);
        }

        else if (randType == SubstituteType::Subtract) {
            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto a = Substitute(irBuilder, type, irBuilder.CreateAdd(operand, randVal), numRecursions+1);
            return irBuilder.CreateSub(a, randVal);
        }

        else if (randType == SubstituteType::Divide) {
            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto a = Substitute(irBuilder, type, irBuilder.CreateMul(operand, randVal), numRecursions+1);
            return irBuilder.CreateSDiv(a, randVal);
        }

        else if (randType == SubstituteType::Not) {
            auto a = Substitute(irBuilder, type, irBuilder.CreateNot(operand), numRecursions+1);
            return irBuilder.CreateNot(a);
        }

        else if (randType == SubstituteType::Xor) {
            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto a = Substitute(irBuilder, type, irBuilder.CreateXor(operand, randVal), numRecursions+1);
            return irBuilder.CreateXor(a, randVal);
        }

        return operand;
    }
    
}