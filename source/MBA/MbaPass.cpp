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
                llvm::outs() << "Op 0: " << *binOp << "\n";
                llvm::Value* op1 = Substitute(irBuilder, binOp->getType(), binOp->getOperand(0));
                //llvm::Value* op2 = Substitute(irBuilder, binOp->getType(), binOp->getOperand(1));
                llvm::Value* op2 = binOp->getOperand(1);

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

    llvm::Value* MbaPass::Substitute(llvm::IRBuilder<>& irBuilder, llvm::Type* type, llvm::Value* operand, size_t numRecursions) {
        int randType = rand() % SubstituteType::Max;

        llvm::outs() << "Op: " << numRecursions << "\t" << *operand << "\n";

        if (numRecursions >= s_RecursiveAmount) {
            return operand;
        }

        //randType = SubstituteType::Add;
        if (randType == SubstituteType::Add) {
            int randNum = rand()%(type->getIntegerBitWidth()*32);

            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto b = irBuilder.CreateMul(operand, randVal);
            auto a = Substitute(irBuilder, type, irBuilder.CreateSub(operand, b), numRecursions+1);
            auto instr = irBuilder.CreateAdd(a, b);

            return instr;
        }

        else if (randType == SubstituteType::Subtract) {
            int randNum = rand()%(type->getIntegerBitWidth()*32);

            auto randVal = llvm::ConstantInt::get(type, randNum);
            auto b = irBuilder.CreateMul(operand, randVal);
            auto a = Substitute(irBuilder, type, irBuilder.CreateAdd(operand, b), numRecursions+1);
            auto instr = irBuilder.CreateSub(a, b);

            return instr;
        }

        else {
            return operand;
        }
    }
    
}