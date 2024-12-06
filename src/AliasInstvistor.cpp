//

//
#include "AliasInstvistor.h"
#include "utils.h"
#include <memory>
#include <utility>
using namespace llvm;
//AliasInstVistor::AliasInstVistor(AliasContext* &ctx, Module &m)
//    : Analyzer(ctx, m){}


void AliasInstVistor::visitGetElementPtrInst(GetElementPtrInst &I) {
    if(I.hasAllZeroIndices()) return;
    //outs()<<"alias vistor"<<"\n";
    Value *srcPointer = I.getPointerOperand();
//    OP<<*I.getResultElementType()<<"\n";
//    OP<<*I.getSourceElementType()<<"\n";
    //get the offset
    auto currtype = I.getSourceElementType();

//    OP<<"/----------pointer source :"<<"-------\\\n";
    auto val = I.getPointerOperand();
//    OP<<I<<" with cast\n";
//    //skip all cast process and get the original pointer type instruction
//    OP<<*I.stripPointerCasts()<<" no cast \n";
//    OP<<*val<<"\n";
    auto srcreg = I.getPointerOperand();
//    OP<<"\\----------pointer source :"<<"-------/\n";
    for(auto it= I.op_begin()+2; it!=I.op_end(); ++it){
        if(auto stype = dyn_cast<StructType>(currtype)){
            OP<<"find the structure type"<<**it<<"\n";
            auto idxint = dyn_cast<ConstantInt>(it->get())->getZExtValue();
            auto dl = mod.getDataLayout();
            auto sl = dl.getStructLayout(stype);
            OP<<"element offset: " <<sl->getElementOffset(idxint)<<"\n";
            //we find a filed by  reference a pointer
            //therefore, we need to create a pointerto filed to represent
            //this field and the source pointer which point to this offset
            auto src = currCtx->findOpReg(val);
            if(hasPointsTo(src)){
                //
                OP<<"has points to\n";
                auto destreg = currCtx->getDestReg(&I);

                for(auto &pt: src->pointstoset){
                    //dstfieldId is the whole filed offset
                    //and getelementptr will add a new offset
                    //
                    OP<<"offset find:"<<pt.dstfieldId+sl->getElementOffset(idxint)<<"\n";
                    destreg->addPointsTo(pt.target,pt.dstfieldId+sl->getElementOffset(idxint),&I);
                }
            } else {
                //we want to create the reg points to the field
                //we need to log the offset
                //currCtx->createRegMemPair(I.getPointerOperand());

                //auto srcreg = currCtx->findOpReg(src);
                OP<<"/------------ visiting GEP inst---------\\\n";
                OP<<I<<"\n";
                auto src = I.getPointerOperand();
//                Value* result;
//                for(auto &aliasobj : currCtx->localobjects.aliasinstset){
//                    auto it = aliasobj.second.find(src);
//                    if(it!=aliasobj.second.end()){
//                        //we find
//                        OP<<"we find the level 1 pointer source\n";
//                        OP<<*aliasobj.first<<"\n";
//                        OP<<"field offset found\n";
//                        OP<<"offset: "<<sl->getElementOffset(idxint)<<"\n";
//                        result = aliasobj.first;
//                    }
//                }

                //OP<<*src<<"\n";
                //while(currCtx->findOpReg(src)== nullptr)

                OP<<"\\------------ visiting GEP inst---------//\n";


                //end of the add field
            }
        }
    }

}

void AliasInstVistor::visitLoadInst(LoadInst &I) {
    auto src = I.getPointerOperand();
    auto srcreg = currCtx->findOpReg(src);
    if(!hasPointsTo(srcreg)) return;
    OP<<"/------------ visiting load inst---------\\\n";
    OP<<I<<"\n";
    OP<<*src<<"\n";
    if(isa<AllocaInst>(*src)){
        //we find an alias object which uses a vreg to represnt the memory
        currCtx->localobjects.aliasinstset[src].insert(&I);
    }
    OP<<"\\------------ visiting load inst---------//\n";

    if(!hasPointsTo(srcreg)) return;
    bool ensure2Lpt = I.getType()->isPointerTy() &&(isa<GlobalObject>(src)  );
    RegObject* dstreg = nullptr;
    AliasObject *newobj = nullptr;

}

void AliasInstVistor::visitAllocaInst(AllocaInst &I) {
    //OP<<"creating the context"<<I<<"\n";
    currCtx->createRegMemPair(&I);
}
void AliasInstVistor::visitICmpInst(ICmpInst &I) {
    return;
    OP<<"/--------- ICMP analyzing-----------\\\n";
    OP<<I<<"\n";
    OP<<*I.getOperand(0)<<"\n";
    OP<<"\\--------- ICMP analyzing-----------/\n";
}