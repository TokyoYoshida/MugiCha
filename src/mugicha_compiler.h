#ifndef LLVM_COMPILER_H
#define LLVM_COMPILER_H 1

#include "llvm_builder.h"

#ifndef __cplusplus

void mugicha_compile(ASTNODE *rootp);

#endif

#ifdef __cplusplus

class MugichaScopeInfo {
  std::shared_ptr<llvm::LLVMContext> context_;
  std::shared_ptr<LLVMModuleBuilder> module_;
  std::shared_ptr<LLVMLocalVariableMap> var_map_;
  std::shared_ptr<LLVMStructDefMap> struct_def_map_;

public:
  MugichaScopeInfo() ;

  MugichaScopeInfo(std::shared_ptr<MugichaScopeInfo> old_scope) ;

  std::shared_ptr<LLVMModuleBuilder> getModuleBuilder();

  llvm::Module *getModule();

  llvm::LLVMContext *getContext();

  llvm::IRBuilder<> *getBuilder();

  llvm::Function *getFunc();

  std::shared_ptr<LLVMFuncBuilder> getFuncBuilder();

  std::shared_ptr<LLVMExprBuilder> makeExprBuilder();

  std::shared_ptr<LLVMLocalVariableMap> getVarMap();

  std::shared_ptr<LLVMStructDefMap> getStructDefMap();
};

llvm::Type *getLLVMTypeByMugichaType(TYPE type,llvm::LLVMContext *context);

llvm::Value *eval_node_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope);
LLVMStructDef::FieldDef getFieldDef(LLVMModuleBuilder *module, ASTNODE *ap);

TYPE GLOBAL_ANY_TYPE;

#endif /* __cplusplus */


#endif
