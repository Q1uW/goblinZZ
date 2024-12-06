//

//

#ifndef PASSANALYSISTEST_TAINTOBJECT_H
#define PASSANALYSISTEST_TAINTOBJECT_H
#include "llvm/IR/Instructions.h"
#include "Context.h"
#include <set>

struct PointsTo{
    Instruction * destinst;

};
struct FieldObject{
    bool tained;
    Context ctx;

    bool ignoresink;
    std::set<PointsTo> pointsto;
    int taintflag, sinktaint;
    Instruction *tainter, *sinktainter;


};
#endif //PASSANALYSISTEST_TAINTOBJECT_H
