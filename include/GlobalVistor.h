//

//

#ifndef PASSANALYSISTEST_GLOBALVISTOR_H
#define PASSANALYSISTEST_GLOBALVISTOR_H
#include "llvm/IR/InstVisitor.h"
#include "AliasInstvistor.h"
#include "AnalyzerBase.h"
#include "utils.h"
#include <memory>


using namespace std;
using namespace llvm;
template <typename ContextClass>
class GlobalVistor : public InstVisitor<GlobalVistor<ContextClass>>{
    Module &module;
    Function &function;
    //vector<vector<BasicBlock*>> SCCList;
    vector<unique_ptr<AnalyzerBase<ContextClass>>> analyzer;
    typedef InstVisitor<GlobalVistor> VisitorBase;
    typedef AnalyzerBase<ContextClass> AnalyzerTy;

    ContextClass* currentCtx;
    vector<unique_ptr<ContextClass>> contexts;
    VisitorBase *super;
public:
    GlobalVistor(Module &M, Function &F)
        : module(M), function(F),currentCtx(nullptr)
    {
        super = static_cast<VisitorBase *>(this);
        contexts.push_back(unique_ptr<ContextClass>(new ContextClass(nullptr,&F)));
    }

    template<typename T>
    T* addCallback(){
        static_assert(
                std::is_base_of<AnalyzerTy,T>::value,
                "Type T error"
                );
       auto analysis =  new T(currentCtx,module);
       analyzer.push_back(unique_ptr<AnalyzerTy>(analysis));
       return analysis;
    }
    ContextClass* pushContext(Instruction &inst, Function &func){
        auto tmp = currentCtx->getOrCreateChildCtx(&inst,&func);
        if(tmp.second) contexts.push_back(
                unique_ptr<ContextClass>(tmp.first)
        );
        currentCtx = tmp.first;
        return currentCtx->parent;
    }
    ContextClass* popContext(){
        auto child = currentCtx;
        currentCtx = child->parent;
        return child;
    }
    void analyze(){
        //set the current function's context by using the module entry function
        currentCtx = contexts[0].get();
        //auto a = make_unique<AliasInstVistor>(currentCtx,module);
        analyze(function);
    }
    void analyze(Function &func){
        //used for the current function
        if(func.isDeclaration()){
            return;
        }
        //for each function we need to create a SCCList to save the
        //basic block information in order to visit the unit
        vector<vector<BasicBlock*>> SCCList;
        SCCList.clear();
        getSCCList(func,SCCList);
        //
        int scc_cnt = 0;
        for(auto &currSCC: SCCList){
            if(currSCC.size()>1){
                //we find the loop or some structure
                // we don't want to travel a loop multi-times
                //set the context as inside the loop
                scc_cnt++;
                this->currentCtx->inside_loop = true;
                visitSCC(currSCC);
            } else{
                this->currentCtx->inside_loop = false;
                visitSCC(currSCC);
            }
        }
    }
    void visitSCC(std::vector<BasicBlock*> &currSCC){
        for(auto &BB : currSCC){
            super->visit(BB);
        }
    }

    void visitBasicBlock(BasicBlock &BB){
        //be called by the InstVistor::visit(BasicBlock)
        //called by the visitscc
//        for(auto &I:BB){
//            outs()<<I<<"\n";
//        }
    }
    void visit(Instruction &I){
        super->visit(I);
    }

    void visit(Function& F) {
        OP<<F.getName()<<"\n";
    }
    void visitCallInst(CallInst &I) {
//        for(auto &currCallback: analyzer){
//            currCallback->visitCallInst(I);
//        }

        Function* currFunc = I.getCalledFunction();
        if(currFunc){
            //we find the callee function inside the current function which we are analyzing
            processCalledFunction(I, currFunc);
        }
    }
    inline void processCalledFunction(CallInst &I, Function *currFunc){

        if(currFunc->isDeclaration()){
            return;
        }
        auto parentCtx = pushContext(I,*currFunc);
        analyze(*currFunc);
        auto childCtx = popContext();

    }
    void visitGetElementPtrInst(GetElementPtrInst &I){
        for(auto &currCallback: analyzer){
            currCallback->visitGetElementPtrInst(I);
        }
    }
    void visitLoadInst(LoadInst &I){
        for(auto &currCallback: analyzer){
            currCallback->visitLoadInst(I);
        }
    }
    void visitAllocaInst(AllocaInst &I){
        for(auto &currCallback: analyzer){
            currCallback->visitAllocaInst(I);
        }
    }
    void visitICmpInst(ICmpInst &I){
        for(auto &currCallback: analyzer){
            currCallback->visitICmpInst(I);
        }
    }
};
#endif //PASSANALYSISTEST_GLOBALVISTOR_H
