//

//

#ifndef PASSANALYSISTEST_CONTEXT_H
#define PASSANALYSISTEST_CONTEXT_H
#include "utils.h"
#include <string>
#include <vector>

using namespace std;
template <typename ContextClass>
struct Context {
    Function *func;
    Instruction *inst;
    std::vector<ContextClass*> childrencontext;
    ContextClass *self;
    ContextClass *parent;
    bool inside_loop;
    Context(Instruction *I,Function *f)
        :func(f),inst(I),parent(nullptr),self(nullptr)
    {
        self = static_cast<ContextClass*>(this);
    }
    // second true -> create a children
    //      false -> find the children context
    std::pair<ContextClass *, bool> getOrCreateChildCtx(Instruction * inst, Function *func){
        for(auto ctxptr: childrencontext){
            if(ctxptr->inst == inst && ctxptr->func== func){
                return std::make_pair(ctxptr, false);
            }
        }
        auto ret = new ContextClass(inst, func);
        ret->parent = self;
        childrencontext.push_back(ret);
        return std::make_pair(ret,true);
    }
};


#endif //PASSANALYSISTEST_CONTEXT_H
