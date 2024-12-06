//

//

#ifndef PASSANALYSISTEST_ANALYZER_H
#define PASSANALYSISTEST_ANALYZER_H
#include "utils.h"
template<typename ContextClass>
struct AnalyzerBase{
    ContextClass * &currCtx;
    Module &mod;

public:
    AnalyzerBase(ContextClass* &ctx,Module &m)
        :currCtx(ctx),mod(m){}

    virtual ~AnalyzerBase() {}
    //virtual void visit(Instruction &I);
    //virtual void visitCallInst(Instruction &I) {};
    virtual void visitGetElementPtrInst(GetElementPtrInst &I) {};
    virtual void visitLoadInst(LoadInst &I) {};
    virtual void visitAllocaInst(AllocaInst &I) {};
    virtual void visitICmpInst(ICmpInst &I) {};
};

#endif //PASSANALYSISTEST_ANALYZER_H
