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

    bool MbaPass::RunMbaAdd(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr) {
        return false;
    }

    bool MbaPass::RunMbaSub(llvm::BasicBlock& block, llvm::BasicBlock::iterator& itr) {
        llvm::BinaryOperator* binOp = llvm::dyn_cast<llvm::BinaryOperator>(itr);
        llvm::IRBuilder<> irBuilder(binOp);

        // Create an instruction representing (a + ~b) + 1
        llvm::Instruction* newInstrs = llvm::BinaryOperator::CreateAdd(
            irBuilder.CreateAdd(binOp->getOperand(0), irBuilder.CreateNot(binOp->getOperand(1))),
            llvm::ConstantInt::get(binOp->getType(), 1)
        );

        // Replace `(a - b)` (original instructions) with `(a + ~b) + 1`
        // (the new instruction)
        llvm::ReplaceInstWithInst(block.getInstList(), itr, newInstrs);
        return true;
    }
}