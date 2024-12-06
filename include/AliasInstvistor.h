//

//

#ifndef PASSANALYSISTEST_ALIASINSTVISTOR_H
#define PASSANALYSISTEST_ALIASINSTVISTOR_H
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"
#include "AliasContext.h"
#include "AnalyzerBase.h"
using namespace llvm;
struct AliasInstVistor : public AnalyzerBase<AliasContext>{
    //
public:
    AliasInstVistor(AliasContext* &ctx,Module &m): AnalyzerBase<AliasContext>(ctx,m){
        AliasContext::setupGlobals(m);
    }
    ~AliasInstVistor() {}
    //void visit(Module &m);
    //void visit(Function &f);
    //void visit(Instruction &I) override;
    //virtual void visitCallInst(Instruction &I) override;
    virtual void visitGetElementPtrInst(GetElementPtrInst &I) override;
    virtual void visitLoadInst(LoadInst &I) override;
    virtual void visitAllocaInst(AllocaInst &I) override;
    virtual void visitICmpInst(ICmpInst &I) override;

};

#endif //PASSANALYSISTEST_ALIASINSTVISTOR_H
