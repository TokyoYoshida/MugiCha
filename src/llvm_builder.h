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

class LLVMVariable {
  std::shared_ptr<LLVMModuleBuilder> module_;
  llvm::Value *value_;
  int flg; // todo make-class-update

  public:
  LLVMVariable(std::shared_ptr<LLVMModuleBuilder> module, std::string name, TYPE type);

  void set(llvm::Value *newVal);

  llvm::Value *get();
};

class LLVMStructDef {
  private:
    llvm::StructType *structTy;
    std::string def_name_;
    std::vector<llvm::Type*> fields_;

  public:
    LLVMStructDef(std::string def_name, std::vector<llvm::Type*>  fields);

    llvm::StructType *getStructTy();
};

class LLVMStructDefMap {
  public:
  std::map<std::string, LLVMStructDef *> map;

  LLVMStructDefMap();

  void set(std::string name, LLVMStructDef *struct_def);

  LLVMStructDef *get(std::string name);

  void makeStructDef(std::string def_name, std::vector<llvm::Type*>  fields);
};

class LLVMStruct : public LLVMVariable {
private:
  LLVMStructDef *struct_def_;

  public:
  LLVMStruct(std::shared_ptr<LLVMModuleBuilder> module, LLVMStructDef *struct_def, std::string name);

  void set(std::string member_name, llvm::Value *newVal);

  llvm::Value *get(std::string member_name);
};

class LLVMVariableMap;

class VariableIndicator {
  protected:
  std::string name_;

  public:
  VariableIndicator(std::string name);

  void set(LLVMVariableMap *target, llvm::Value *newVal); // visitor of Visitor Pattern
  llvm::Value *get(LLVMVariableMap *target); // visitor of Visitor Pattern
};

class StructIndicator : public VariableIndicator {
  std::string member_name_;

  public:
    StructIndicator(std::string var_name, std::string member_name);

    void set(LLVMVariableMap *target, llvm::Value *newVal); // visitor of Visitor Pattern
    llvm::Value *get(LLVMVariableMap *target); // visitor of Visitor Pattern
};

class LLVMVariableMap {
  public:
  std::shared_ptr<LLVMModuleBuilder> module_;
  std::map<std::string, LLVMVariable *> map;

  LLVMVariableMap(std::shared_ptr<LLVMModuleBuilder> module);

  virtual void makeVariable(std::string name ,TYPE type) = 0;

  void set(VariableIndicator target, llvm::Value *newVal); // acceotor of Visitor Pattern

  llvm::Value *get(VariableIndicator target);

  LLVMVariable *getVariable(std::string name);
};

class LLVMLocalVariableMap : public LLVMVariableMap {
  public:
  LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module);

  virtual void makeVariable(std::string name ,TYPE type);
  void makeStruct(std::string name, LLVMStructDef *structDef);
};

llvm::Value *makePrintf(std::shared_ptr<LLVMModuleBuilder> module,std::shared_ptr<LLVMExprBuilder> builder, std::string printStr); // TODO delete lator

#endif /* __cplusplus */

#endif
