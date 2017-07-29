
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
  protected:
  std::shared_ptr<LLVMModuleBuilder> module_;
  llvm::Value *value_;

  public:
  LLVMVariable(std::shared_ptr<LLVMModuleBuilder> module, std::string name, TYPE type);

  virtual void set(llvm::Value *newVal);

  virtual llvm::Value *get();
};

class LLVMStructDef {
  public:
    // using FieldDef = std::map<std::string, llvm::Type*>;
    std::shared_ptr<LLVMModuleBuilder> module_;
    using FieldDef = std::map<std::string, TYPE>;
    llvm::StructType *structTy;
    std::string def_name_;
    FieldDef fields_;
    llvm::PointerType *structPtr;

    LLVMStructDef(std::shared_ptr<LLVMModuleBuilder> module, std::string def_name, FieldDef  fields);

    llvm::StructType *getStructTy();
    llvm::PointerType *getStructPtr();
    std::string getDefName();

    int filedName2Index(std::string filed_name);
};

class LLVMStructDefMap {
  public:
    std::shared_ptr<LLVMModuleBuilder> module_;
  std::map<std::string, LLVMStructDef *> map;

  LLVMStructDefMap(std::shared_ptr<LLVMModuleBuilder> module);

  virtual void set(std::string name, LLVMStructDef *struct_def);

  virtual LLVMStructDef *get(std::string name);

  void makeStructDef(std::string def_name, LLVMStructDef::FieldDef  fields);
};

class LLVMStructInitializer {
public:
  TYPE type;

  LLVMStructInitializer(std::shared_ptr<LLVMModuleBuilder> module, LLVMStructDef *struct_def, std::string name);
};

class LLVMStruct : public LLVMStructInitializer ,public LLVMVariable {
private:
  LLVMStructDef *struct_def_;
  llvm::AllocaInst *alloca_inst;
  llvm::AllocaInst *alloca_inst_ptr;

  public:
  LLVMStruct(std::shared_ptr<LLVMModuleBuilder> module, LLVMStructDef *struct_def, std::string name);

  virtual void set(std::string member_name, llvm::Value *newVal);
  virtual void set(llvm::Value *newVal);

  virtual llvm::Value *get(std::string member_name);
  llvm::Value *get();
};

class LLVMVariableMap;

class VariableIndicator {
  protected:
  std::string name_;

  public:
  VariableIndicator(std::string name);

  virtual void set(LLVMVariableMap *target, llvm::Value *newVal); // visitor of Visitor Pattern
  virtual llvm::Value *get(LLVMVariableMap *target); // visitor of Visitor Pattern
};

class StructIndicator : public VariableIndicator {
  std::string member_name_;

  public:
    StructIndicator(std::string var_name, std::string member_name);

    virtual void set(LLVMVariableMap *target, llvm::Value *newVal); // visitor of Visitor Pattern
    virtual llvm::Value *get(LLVMVariableMap *target); // visitor of Visitor Pattern
};

class LLVMVariableMap {
  public:
  std::shared_ptr<LLVMModuleBuilder> module_;
  std::map<std::string, LLVMVariable *> map;

  LLVMVariableMap(std::shared_ptr<LLVMModuleBuilder> module);

  virtual void makeVariable(std::string name ,TYPE type) = 0;

  void set(VariableIndicator *target, llvm::Value *newVal); // acceotor of Visitor Pattern

  llvm::Value *get(VariableIndicator *target);

  LLVMVariable *getVariable(std::string name);
};

class LLVMLocalVariableMap : public LLVMVariableMap {
  public:
  LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module);

  virtual void makeVariable(std::string name ,TYPE type);
  void makeStruct(std::string name, LLVMStructDef *structDef);
};

llvm::Value *makePrintf(std::shared_ptr<LLVMModuleBuilder> module,std::shared_ptr<LLVMExprBuilder> builder, std::string printStr); // TODO delete lator
llvm::Type *astType2LLVMType(std::shared_ptr<LLVMModuleBuilder> module, TYPE type);

#endif /* __cplusplus */

#endif
