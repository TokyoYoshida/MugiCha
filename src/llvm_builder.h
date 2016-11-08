#ifndef LLVM_BUILDER_H
#define LLVM_BUILDER_H 1

#ifdef __cplusplus
#include "type.h"

class LLVMModuleBuilder {
  protected:
  std::shared_ptr<llvm::Module> module_;
  std::shared_ptr<llvm::LLVMContext> context_;
  std::shared_ptr<llvm::IRBuilder<>> builder_;

  public:
  LLVMModuleBuilder(std::string name, std::shared_ptr<llvm::LLVMContext> context);

  llvm::Module *getModule();

  llvm::LLVMContext *getContext();

  llvm::IRBuilder<> *getBuilder();

  llvm::Value *makePrintf(std::vector<llvm::Value *> values);
};


class LLVMFuncBuilder{
protected:
  std::shared_ptr<LLVMModuleBuilder> module_;
  llvm::Function *func_;
  llvm::BasicBlock *basicBlock_;

  public:
  LLVMFuncBuilder(std::shared_ptr<LLVMModuleBuilder> module, llvm::FunctionType *funcType, std::string name);
  LLVMFuncBuilder(std::shared_ptr<LLVMModuleBuilder> module, llvm::Function *func);

 void makeReturn(llvm::Value *value);

 LLVMModuleBuilder *getModuleBuilder();

  llvm::BasicBlock *getBasicBlock();

  llvm::Function *getFunc();

};

class LLVMExprBuilder {
private:
  std::shared_ptr<LLVMFuncBuilder> func_;
public:
  LLVMExprBuilder(std::shared_ptr<LLVMFuncBuilder> func);

  llvm::Value *makeConst(int value);
  llvm::Value *makeConst(float value);
  llvm::Value *makeConst(double value);
  llvm::Value *makeConst(llvm::StringRef str);

  llvm::Instruction *makeCalcOp(llvm::AddrSpaceCastInst::BinaryOps ops,llvm::Value *lhs,llvm::Value *rhs);

  llvm::Instruction *makeCalcOp(llvm::BasicBlock *block ,llvm::AddrSpaceCastInst::BinaryOps ops,llvm::Value *lhs,llvm::Value *rhs );
};

class LLVMLocalVariable {
  std::shared_ptr<LLVMModuleBuilder> module_;
  llvm::Value *value_;

  public:
  LLVMLocalVariable(std::shared_ptr<LLVMModuleBuilder> module, std::string name, TYPE type);

  void set(llvm::Value *newVal);

  llvm::Value *get();
};

class LLVMLocalVariableMap {
  public:
  std::shared_ptr<LLVMModuleBuilder> module_;
  std::map<std::string, LLVMLocalVariable *> map;

  LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module);

  void makeVariable(std::string name ,TYPE type);

  void set(std::string name, llvm::Value *newVal);

  llvm::Value *get(std::string name);

};

llvm::Value *makePrintf(std::shared_ptr<LLVMModuleBuilder> module,std::shared_ptr<LLVMExprBuilder> builder, std::string printStr); // TODO delete lator

#endif /* __cplusplus */

#endif
