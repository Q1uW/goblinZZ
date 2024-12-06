//

//

#ifndef PASS4DATAFLOW_ALIASCONTEXT_H
#define PASS4DATAFLOW_ALIASCONTEXT_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include "Context.h"
typedef int FieldIDTy;
struct AliasObject;

struct PointsTo{
    FieldIDTy fieldId;
    FieldIDTy dstfieldId;
    AliasObject *target;
    Instruction *propagator;
//    PointsTo* makeCopy(){
//        return new PointsTo{fieldId,dstfieldId,target,propagator};
//    }
    bool pointsToSameObject(PointsTo *ptr1,PointsTo *ptr2) const
    {
        if (ptr1== nullptr || ptr2== nullptr){
            return false;
        } else{
            return ptr1->fieldId==ptr2->fieldId && ptr1->dstfieldId == ptr2->dstfieldId;
        }
    }
    PointsTo(AliasObject *obj, FieldIDTy off, Instruction *inst)
        :dstfieldId(off),target(obj),propagator(inst)
    {}
    bool operator<(const PointsTo &rhs) const{
        if(target== rhs.target){
            return dstfieldId<rhs.dstfieldId;
        }
        return target<rhs.target;
    }
};

//represent the field inside the structure rather the structure itself

struct FieldObject{
    std::set<PointsTo> pointstoset;
    void addPointsTo(AliasObject *object, FieldIDTy off, Instruction *inst){
        pointstoset.insert(PointsTo(object,off,inst));
    }
    void mergePointsTo(FieldObject *src, Instruction *inst){
        for(auto &item: src->pointstoset){
            auto tmp = item;
            tmp.propagator = inst;
            pointstoset.insert(tmp);
        }
    }
};
struct RegObject : public FieldObject{
    Value *represnted;
    RegObject(Value *obj):represnted(obj){}
};
//represent the whole data structure
struct AliasObject{
    //Instruction *inst;
    //the instruction create the object
    //such as the mell
    Value *represent;
    std::map<FieldIDTy,FieldObject> fieldmap;
    bool fake;
    AliasObject(Value *obj)
            :represent(obj){}

    FieldObject* findFieldObject(FieldIDTy fid){
        auto it = fieldmap.find(fid);
        if(it!=fieldmap.end())
            return &(it->second);
        return nullptr;
    }
    FieldObject* getFieldObject(FieldIDTy fid){
        return &(fieldmap[fid]);
    }
    bool isstackobj(){
        if(represent && dyn_cast<AllocaInst>(represent))
            return true;
        else
            return false;
    }
};
struct ObjectMap{
    std::map<Value*,std::unique_ptr<AliasObject>> memmap;
    std::map<Value*,std::unique_ptr<RegObject>> regmap;
    std::map<Value*,std::set<Value*>> aliasinstset;

    template<typename T, typename Map>
    std::pair<T*,bool> getOrCreateObject(Map &map, Value *val){
        auto ins = map.emplace(val,std::unique_ptr<T>(nullptr));
        auto &uptr = ins.first->second;
        if(ins.second) uptr.reset(new T(val));
        return std::make_pair(uptr.get(),ins.second);
    }

    template<typename Map>
    void createAliasMap(Map &map,Value *val){
        map.emplace(val, std::set<Value*>() );

    }
    template<typename T, typename Map>
    T* getNoCreateObject(Map &map, Value *val){
        auto it = map.find(val);
        if(it!=map.end())
            return it->second.get();
        return nullptr;
    }
    std::pair<RegObject*,AliasObject*>createRegMemPair(Value* val){
        auto regs = getOrCreateObject<RegObject>(regmap,val);
        auto mems = getOrCreateObject<AliasObject>(memmap,val);
        createAliasMap(aliasinstset,val);
        if(regs.second || mems.second){
            auto inst = static_cast<Instruction*>(val);
            regs.first->addPointsTo(mems.first,0,inst);
        }
        return std::make_pair(regs.first,mems.first);
    }
    RegObject* getRegObj(Value *val){
        return getOrCreateObject<RegObject>(regmap,val).first;
    }
    RegObject* findRegObj(Value *val){
        return getNoCreateObject<RegObject>(regmap,val);
    }
    AliasObject* findMemObj(Value *val){
        return getNoCreateObject<AliasObject>(memmap,val);
    }
};
struct AliasContext: public  Context<AliasContext>{
    static ObjectMap globalobjects;
    ObjectMap localobjects;
    std::set<Value*> retval;

    AliasContext( Instruction *inst, Function *func): Context(inst,func)
    {}

    std::pair<RegObject *, AliasObject *>
    createRegMemPair(Value *val, bool fake = false){
        auto ret = localobjects.createRegMemPair(val);
        ret.second->fake = fake;
        return ret;
    }

    RegObject* getDestReg(Value *val){
        auto newval = val->stripPointerCasts();
        if(newval!=val) return getDestReg(newval);
        if(isa<GlobalObject>(val))
            return globalobjects.findRegObj(val);
        return localobjects.getRegObj(val);
    }

    RegObject* findOpReg(Value *val){
        auto newval = val->stripPointerCasts();
        if(newval!=val) return findOpReg(newval);
        if(isa<GlobalObject>(val))
            return globalobjects.findRegObj(val);
        auto ret = localobjects.findRegObj(val);
        //if we don't find the virtual register object
        //we need to create the object
        if(!ret){
            ret = createRegMemPair(val, true).first;
        }

        return ret;
    }
    static void setupGlobals(Module &m);


};
inline bool hasPointsTo(FieldObject *reg){
    return reg&&reg->pointstoset.size();
}

#endif //PASS4DATAFLOW_ALIASCONTEXT_H
