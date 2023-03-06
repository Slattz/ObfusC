#include "MbaPass.hpp"

namespace obfusc {
    MbaPass::MbaPass() {}
    MbaPass::~MbaPass() {}

    bool MbaPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        bool changed = false;

        for (auto& block : func) {
            for (auto instruction = block.begin(); instruction != block.end(); instruction++) {
                llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction);
                if (!binOp)
                    continue;

                unsigned opCode = binOp->getOpcode();
                if (opCode == llvm::Instruction::Add) {
                    changed |= RunMbaAdd(block, instruction);
                }
                else if (opCode == llvm::Instruction::Sub) {
                    changed |= RunMbaSub(block, instruction);
                }
            }
        }

        return changed;
    }

    //Currently based on https://github.com/banach-space/llvm-tutor/blob/main/lib/MBAAdd.cpp
    bool MbaPass::RunMbaAdd(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr) {
        llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(itr);
        // if (!binOp->getType()->isIntegerTy() || binOp->getType()->getIntegerBitWidth() != 8) {
            // return false;
        // }

        // Constants used in building the instruction for substitution
        auto Val39 = llvm::ConstantInt::get(binOp->getType(), 39);
        auto Val151 = llvm::ConstantInt::get(binOp->getType(), 151);
        auto Val23 = llvm::ConstantInt::get(binOp->getType(), 23);
        auto Val2 = llvm::ConstantInt::get(binOp->getType(), 2);
        auto Val111 = llvm::ConstantInt::get(binOp->getType(), 111);

        llvm::IRBuilder irBuilder(binOp);


        // Build an instruction representing `(((a ^ b) + 2 * (a & b)) * 39 + 23) * 151 + 111`
        llvm::Instruction* newInstrs =
            // E = e5 + 111
            llvm::BinaryOperator::CreateAdd(
                Val111,
                // e5 = e4 * 151
                irBuilder.CreateMul(Val151,
                    // e4 = e2 + 23
                    irBuilder.CreateAdd(Val23,
                        // e3 = e2 * 39
                        irBuilder.CreateMul(Val39,
                            // e2 = e0 + e1
                            irBuilder.CreateAdd(
                                // e0 = a ^ b
                                irBuilder.CreateXor(binOp->getOperand(0), binOp->getOperand(1)),
                                // e1 = 2 * (a & b)
                                irBuilder.CreateMul(Val2, 
                                    irBuilder.CreateAnd(binOp->getOperand(0),binOp->getOperand(1))
                                )
                            )
                        ) // e3 = e2 * 39
                    ) // e4 = e2 + 23
                ) // e5 = e4 * 151
            ); // E = e5 + 111

        llvm::ReplaceInstWithInst(block.getInstList(), itr, newInstrs);

        return true;
    }

    //Currently based on https://github.com/banach-space/llvm-tutor/blob/main/lib/MBASub.cpp
    bool MbaPass::RunMbaSub(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr) {
        llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(itr);
        llvm::IRBuilder irBuilder(binOp);

        // Create an instruction representing (a + ~b) + 1
        llvm::Instruction* newInstrs = llvm::BinaryOperator::CreateAdd(
            irBuilder.CreateAdd(binOp->getOperand(0), irBuilder.CreateNot(binOp->getOperand(1))),
            llvm::ConstantInt::get(binOp->getType(), 1)
        );

        // Replace `(a - b)` (original instructions) with `(a + ~b) + 1` (the new instruction)
        llvm::ReplaceInstWithInst(block.getInstList(), itr, newInstrs);
        return true;
    }
}