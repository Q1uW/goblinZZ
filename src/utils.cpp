//

//

#include "utils.h"
void getSCCList(Function &currF, vector<vector<BasicBlock*>> &List){
    Function* F = &currF;
    for(auto I = scc_begin(F),IE = scc_end(F); I!=IE; ++I){
        const vector<BasicBlock *> &constvec = *I;
        vector<BasicBlock*> SCC(constvec.rbegin(), constvec.rend());
        List.push_back(move(SCC));
    }
    reverse(List.begin(),List.end());
}