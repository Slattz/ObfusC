#include "CffPass.hpp"
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/Utils/Local.h>

//Heavily based on Obfuscator-LLVM
//https://github.com/obfuscator-llvm/obfuscator/blob/llvm-4.0/lib/Transforms/Obfuscation/Flattening.cpp

namespace obfusc {
    CffPass::CffPass() {}
    CffPass::~CffPass() {}

    bool CffPass::obfuscate(llvm::Module& mod, llvm::Function& func) {
        // Copy original blocks
        std::vector<llvm::BasicBlock *> origBB;
        for (auto& block : func) {
            if (isa<llvm::InvokeInst>(block.getTerminator())) {
                return false;
            }
            origBB.push_back(&block);
        }

        // Nothing to flatten
        if (origBB.size() < 2) {
            return false;
        }

        // Remove first BB
        origBB.erase(origBB.begin());

        // Get a pointer on the first BB
        llvm::Function::iterator tmp = func.begin();
        llvm::BasicBlock* insert = &*tmp;

        // If main begin with an if
        llvm::BranchInst* br = nullptr;
        if (isa<llvm::BranchInst>(insert->getTerminator())) {
            br = cast<llvm::BranchInst>(insert->getTerminator());
        }

        if ((br != nullptr && br->isConditional()) || insert->getTerminator()->getNumSuccessors() > 1) {
            llvm::BasicBlock::iterator i = insert->end();
	        --i;

            if (insert->size() > 1) {
                --i;
            }

            llvm::BasicBlock* tmpBB = insert->splitBasicBlock(i, "first");
            origBB.insert(origBB.begin(), tmpBB);
        }

        // Remove jump
        insert->getTerminator()->eraseFromParent();

        // Create switch variable and set as it
        llvm::AllocaInst* switchVar = new llvm::AllocaInst(llvm::Type::getInt32Ty(func.getContext()), 0, "switchVar", insert);
        new llvm::StoreInst(llvm::ConstantInt::get(llvm::Type::getInt32Ty(func.getContext()), 0), switchVar, insert);

        // Create main loop
        llvm::BasicBlock* loopEntry = llvm::BasicBlock::Create(func.getContext(), "loopEntry", &func, insert);
        llvm::BasicBlock* loopEnd = llvm::BasicBlock::Create(func.getContext(), "loopEnd", &func, insert);

        llvm::LoadInst* load = new llvm::LoadInst(llvm::Type::getInt32Ty(func.getContext()), switchVar, "switchVar", loopEntry);

        // Move first BB on top
        insert->moveBefore(loopEntry);
        llvm::BranchInst::Create(loopEntry, insert);

        // loopEnd jump to loopEntry
        llvm::BranchInst::Create(loopEntry, loopEnd);

        llvm::BasicBlock* swDefault = llvm::BasicBlock::Create(func.getContext(), "switchDefault", &func, loopEnd);
        llvm::BranchInst::Create(loopEnd, swDefault);

        // Create switch instruction itself and set condition
        llvm::SwitchInst* switchI = llvm::SwitchInst::Create(&(*(func.begin())), swDefault, 0, loopEntry);
        switchI->setCondition(load);

        // Remove branch jump from 1st BB and make a jump to the while
        func.begin()->getTerminator()->eraseFromParent();

        llvm::BranchInst::Create(loopEntry, &(*(func.begin())));

        // Put all BB in the switch
        for (std::vector<llvm::BasicBlock *>::iterator b = origBB.begin(); b != origBB.end(); ++b) {
            llvm::BasicBlock* i = *b;

            // Move the BB inside the switch (only visual, no code logic)
            i->moveBefore(loopEnd);

            // Add case to switch
            llvm::ConstantInt* numCase = llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(switchI->getCondition()->getType(), switchI->getNumCases()));
            switchI->addCase(numCase, i);
        }

        // Recalculate switchVar
        for (std::vector<llvm::BasicBlock*>::iterator b = origBB.begin(); b != origBB.end(); ++b) {
            llvm::BasicBlock* i = *b;

            // If it's a non-conditional jump
            if (i->getTerminator()->getNumSuccessors() == 1) {
                // Get successor and delete terminator
                llvm::BasicBlock* succ = i->getTerminator()->getSuccessor(0);
                i->getTerminator()->eraseFromParent();

                // Get next case
                llvm::ConstantInt* numCase = switchI->findCaseDest(succ);

                // If next case == default case (switchDefault)
                if (numCase == NULL) {
                    numCase = cast<llvm::ConstantInt>(llvm::ConstantInt::get(switchI->getCondition()->getType(), switchI->getNumCases()));
                }

                // Update switchVar and jump to the end of loop
                new llvm::StoreInst(numCase, load->getPointerOperand(), i);
                llvm::BranchInst::Create(loopEnd, i);
            }

            // If it's a conditional jump
            else if (i->getTerminator()->getNumSuccessors() == 2) {
                // Get next cases
                llvm::ConstantInt* numCaseTrue = switchI->findCaseDest(i->getTerminator()->getSuccessor(0));
                llvm::ConstantInt* numCaseFalse = switchI->findCaseDest(i->getTerminator()->getSuccessor(1));

                // Check if next case == default case (switchDefault)
                if (numCaseTrue == NULL) {
                    numCaseTrue = llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(switchI->getCondition()->getType(), switchI->getNumCases()));
                }

                if (numCaseFalse == NULL) {
                    numCaseFalse = llvm::cast<llvm::ConstantInt>(llvm::ConstantInt::get(switchI->getCondition()->getType(), switchI->getNumCases()));
                }

                // Create a SelectInst
                llvm::BranchInst *br = llvm::cast<llvm::BranchInst>(i->getTerminator());
                llvm::SelectInst *sel = llvm::SelectInst::Create(br->getCondition(), numCaseTrue, numCaseFalse, "", i->getTerminator());

                // Erase terminator
                i->getTerminator()->eraseFromParent();

                // Update switchVar and jump to the end of loop
                new llvm::StoreInst(sel, load->getPointerOperand(), i);
                llvm::BranchInst::Create(loopEnd, i);
            }
        }

        fixStack(func);
        //func.viewCFG();

        return true;
    }

    //Based on lib/Transforms/Scalar/Reg2Mem.cpp
    static bool valueEscapes(const llvm::Instruction& Inst) {
        const llvm::BasicBlock* BB = Inst.getParent();
        for (const llvm::User* U : Inst.users()) {
            const llvm::Instruction* UI = llvm::cast<llvm::Instruction>(U);
            if (UI->getParent() != BB || llvm::isa<llvm::PHINode>(UI)) {
                return true;
            }
        }
        return false;
    }

    void CffPass::fixStack(llvm::Function& func) {
        // Try to remove phi node and demote reg to stack
        std::vector<llvm::PHINode *> tmpPhi;
        std::vector<llvm::Instruction *> tmpReg;
        llvm::BasicBlock& bbEntry = *(func.begin());

        while (true) {
            tmpPhi.clear();
            tmpReg.clear();

            for (auto& block : func) {
                for (auto& instruction : block) {
                    if (llvm::isa<llvm::PHINode>(&instruction)) {
                        tmpPhi.push_back(llvm::cast<llvm::PHINode>(&instruction));
                    }

                    else if (!(llvm::isa<llvm::AllocaInst>(&instruction) && instruction.getParent() == &bbEntry) &&
                            (valueEscapes(instruction) || instruction.isUsedOutsideOfBlock(&block)))
                    {
                        tmpReg.push_back(&instruction);
                    }
                }
            }

            for (llvm::Instruction* reg : tmpReg) {
                llvm::DemoteRegToStack(*reg, false, func.begin()->getTerminator());
            }

            for (llvm::PHINode* phi : tmpPhi) {
                llvm::DemotePHIToStack(phi, func.begin()->getTerminator());
            }

            if (tmpReg.size() == 0 || tmpPhi.size() == 0) {
                break;
            }
        }
    }
}