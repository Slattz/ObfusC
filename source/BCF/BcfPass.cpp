#include "BcfPass.hpp"
#include <llvm/Transforms/Utils/Cloning.h>

//Heavily based on Obfuscator-LLVM
//https://github.com/obfuscator-llvm/obfuscator/blob/llvm-4.0/lib/Transforms/Obfuscation/BogusControlFlow.cpp

namespace obfusc {
    BcfPass::BcfPass() {}
    BcfPass::~BcfPass() {}

    bool BcfPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        bool changed = false;

        std::vector<llvm::BasicBlock*> basicBlocks; //make copy of blocks
        for (auto& block : func) {
            basicBlocks.push_back(&block);
        }

        //For every block copied
        for (llvm::BasicBlock* block : basicBlocks) {
            auto instruction = block->begin();
            if (block->getFirstNonPHIOrDbgOrLifetime()) {
                instruction = static_cast<llvm::BasicBlock::iterator>(block->getFirstNonPHIOrDbgOrLifetime()); //skip past PHI instructions
            }

            llvm::BasicBlock* origBlock = block->splitBasicBlock(instruction); //Get original block
            llvm::BasicBlock* changedBlock = MakeBogusBlock(origBlock, func); //Get Bogus/altered version of same block

            //Erase from parents for each block
            changedBlock->getTerminator()->eraseFromParent();
            block->getTerminator()->eraseFromParent();

            llvm::Type* int32Type = llvm::Type::getInt32Ty(mod.getContext());

            //Create global variables for comparison
            llvm::GlobalVariable* x = new llvm::GlobalVariable(mod, int32Type, false, llvm::GlobalValue::LinkageTypes::InternalLinkage, llvm::ConstantInt::get(int32Type, 1));
            llvm::GlobalVariable* y = new llvm::GlobalVariable(mod, int32Type, false, llvm::GlobalValue::LinkageTypes::InternalLinkage, llvm::ConstantInt::get(int32Type, 3));

            //Load values from global vars
            llvm::LoadInst* opX = new llvm::LoadInst(int32Type, x, "", block);
            llvm::LoadInst* opY = new llvm::LoadInst(int32Type, y, "", block);

            //Create compare instruction
            llvm::ICmpInst* cmpInstr = new llvm::ICmpInst(*block, llvm::CmpInst::Predicate::ICMP_ULT, opX, opY);

            //Create branches to each block based on cmp instr
            llvm::BranchInst::Create(origBlock, changedBlock, cmpInstr, block);
            llvm::BranchInst::Create(origBlock, changedBlock);

            auto endInstr = origBlock->end();
            endInstr--; //Get actual last instr
            auto endBlock = origBlock->splitBasicBlock(endInstr);
            origBlock->getTerminator()->eraseFromParent(); //remove from parent block

            //Make cmp instruction
            cmpInstr = new llvm::ICmpInst(*origBlock, llvm::CmpInst::Predicate::ICMP_ULT, opX, opY);
            llvm::BranchInst::Create(endBlock, changedBlock, cmpInstr, origBlock); //Loop back from bogus block to changed block

            changed = true;
        }

        return changed;
    }

    llvm::BasicBlock* BcfPass::MakeBogusBlock(llvm::BasicBlock* originalBlock, llvm::Function& func) {
        llvm::ValueToValueMapTy valMap;
        llvm::BasicBlock* retBlock = llvm::CloneBasicBlock(originalBlock, valMap, "", &func);

        //Start remapping information for each instruction
        auto origInstruction = originalBlock->begin();
        for (auto instruction = retBlock->begin(); instruction != retBlock->end(); instruction++) {
            for (auto op = instruction->op_begin(); op != instruction->op_end(); ++op) {
                llvm::Value* val = llvm::MapValue(*op, valMap); //Map operations of new instrs
                if (val) {
                    *op = val;
                }
            }

            llvm::PHINode* phiNode = llvm::dyn_cast<llvm::PHINode>(instruction);
            if (phiNode) { //Remap PHI nodes 
                for (unsigned int i = 0; i != phiNode->getNumIncomingValues(); ++i) {
                    llvm::Value* val = llvm::MapValue(phiNode->getIncomingBlock(i), valMap);
                    if (val) {
                        phiNode->setIncomingBlock(i, llvm::cast<llvm::BasicBlock>(val));
                    }
                }
            }

            llvm::SmallVector<std::pair<unsigned int, llvm::MDNode*>> metadata;
            instruction->getAllMetadata(metadata);

            //Needed for DWARF symbols
            instruction->setDebugLoc(origInstruction->getDebugLoc());
            origInstruction++;
            
            /* Start altering blocks!! */

            if (instruction->isBinaryOp()) { //Swap operators for binary instrs
                llvm::Value* op0 = instruction->getOperand(0);
                llvm::Value* op1 = instruction->getOperand(1);

                instruction->setOperand(0, op1);
                instruction->setOperand(1, op0);
            }

            //Make Load instrs into Store instrs
            else if (instruction->getOpcode() == llvm::Instruction::Load) {
                llvm::Value* op0 = instruction->getOperand(0);
                llvm::AllocaInst* allocIns = (llvm::AllocaInst*)op0;
                llvm::Type* opType = allocIns->getAllocatedType();

                auto randVal = llvm::ConstantInt::get(opType, GetRandomNumber(opType)); //Get random number for storing

                llvm::StoreInst* newOp = new llvm::StoreInst(randVal, op0, true, &*instruction);
                instruction = instruction->eraseFromParent();
            }

            //Make Store instrs into Load instrs
            else if (instruction->getOpcode() == llvm::Instruction::Store) {
                llvm::Value* op0 = instruction->getOperand(0);
                llvm::Value* op1 = instruction->getOperand(1);

                llvm::LoadInst* newOp = new llvm::LoadInst(op0->getType(), op1, "", &*instruction);
                instruction = instruction->eraseFromParent();
            }
        }

        return retBlock;
    }
}