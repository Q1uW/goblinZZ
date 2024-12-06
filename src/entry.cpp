#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "GlobalVistor.h"
#include "AliasContext.h"
#include "AliasInstvistor.h"
#include <iostream>
using namespace llvm;

namespace {

    class FunctionInfoPass final : public PassInfoMixin<FunctionInfoPass> {
    public:
        PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
            outs() << "Dataflow information Pass"
                   << "\n";
            runOnModule(M);
            return PreservedAnalyses::all();
        }
        bool runOnModule(Module &m){
            for(auto &f : m){
                //outs()<<f.getName()<<"\n";
                if(f.getName().str()=="main"){
                    int a;
                    cin>>a;
                    analyze(m,f);
                }
            }
            return false;
        }
        void analyze(Module &m,Function &f);

    }; // class FunctionInfoPass
    void FunctionInfoPass::analyze(Module &m, Function &f) {
        GlobalVistor<AliasContext> globalVistor(m,f);
        globalVistor.addCallback<AliasInstVistor>();
        globalVistor.analyze();

    }

} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
            .APIVersion = LLVM_PLUGIN_API_VERSION,
            .PluginName = "FunctionInfo",
            .PluginVersion = LLVM_VERSION_STRING,
            .RegisterPassBuilderCallbacks =
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                        [](StringRef Name, ModulePassManager &MPM,
                           ArrayRef<PassBuilder::PipelineElement>) -> bool {
                            if (Name == "function-info") {
                                MPM.addPass(FunctionInfoPass());
                                return true;
                            }
                            return false;
                        });
            } // RegisterPassBuilderCallbacks
    };        // struct PassPluginLibraryInfo
}