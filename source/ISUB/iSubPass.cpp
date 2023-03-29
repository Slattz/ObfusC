#include "iSubPass.hpp"

/*
    Based on Obfuscator-LLVM
    https://github.com/obfuscator-llvm/obfuscator/blob/llvm-4.0/lib/Transforms/Obfuscation/Substitution.cpp
*/

namespace obfusc {
    iSubPass::iSubPass() {}
    iSubPass::~iSubPass() {}

    bool iSubPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        bool changed = false;

        for (auto& block : func) {
            for (auto instruction = block.begin(); instruction != block.end(); instruction++) {
                llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction); 
                if (!binOp) //Is instruction a binary op
                    continue;

                llvm::IRBuilder irBuilder(binOp);

                llvm::Instruction* newInstrs;
                switch (binOp->getOpcode()) {
                case llvm::Instruction::Add: //a = b - (-c)
                    newInstrs = llvm::BinaryOperator::CreateSub(binOp->getOperand(0), irBuilder.CreateNeg(binOp->getOperand(1)));
                    break;

                case llvm::Instruction::FAdd: //a = b - (-c)
                    newInstrs = llvm::BinaryOperator::CreateFSub(binOp->getOperand(0), irBuilder.CreateFNeg(binOp->getOperand(1)));
                    break;

                case llvm::Instruction::Sub: //a = b + (-c)
                    newInstrs = llvm::BinaryOperator::CreateAdd(binOp->getOperand(0), irBuilder.CreateNeg(binOp->getOperand(1)));
                    break;

                case llvm::Instruction::FSub: //a = b + (-c)
                    newInstrs = llvm::BinaryOperator::CreateFAdd(binOp->getOperand(0), irBuilder.CreateFNeg(binOp->getOperand(1)));
                    break;

                case llvm::Instruction::And: //a = b & c  ->  a = (b ^ ~c) & b
                    newInstrs = llvm::BinaryOperator::CreateAnd(irBuilder.CreateXor(binOp->getOperand(0), irBuilder.CreateNot(binOp->getOperand(1))), binOp->getOperand(0));
                    break;

                case llvm::Instruction::Or: //a = b | c  ->  a = (b & c) | (b ^ c)
                    newInstrs = llvm::BinaryOperator::CreateOr(irBuilder.CreateAnd(binOp->getOperand(0), binOp->getOperand(1)), 
                                                                irBuilder.CreateXor(binOp->getOperand(0), binOp->getOperand(1)));
                    break;

                default:
                    continue;
                }

                llvm::ReplaceInstWithInst(block.getInstList(), instruction, newInstrs); //replace old instruction with new obfuscation instructions
                changed = true;
            }
        }

        return changed;
    }

}