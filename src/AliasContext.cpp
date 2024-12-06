//

//
#include "AliasContext.h"
#include "llvm/IR/Module.h"

ObjectMap AliasContext::globalobjects;

void AliasContext::setupGlobals(Module &m) {
    for(Function &func:m){
        globalobjects.createRegMemPair(&func);
    }
    //OP<<"global list size:" <<m.getGlobalList().size()<<"\n";
    for(GlobalVariable &var : m.getGlobalList()){
        OP<<var.getName()<<"\n";
        globalobjects.createRegMemPair(&var);
        if(var.hasInitializer()){
            auto init = var.getInitializer();
            OP<<*init<<"\n";
        }
    }

}
