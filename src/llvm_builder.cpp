//===- examples/ModuleMaker/ModuleMaker.cpp - Example project ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This programs is a simple example that creates an LLVM module "from scratch",
// emitting it as a bitcode file to standard out.  This is just to show how
// LLVM projects work and to demonstrate some of the LLVM APIs.
//
//===----------------------------------------------------------------------===//

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DataLayout.h"

#include "llvm/ADT/STLExtras.h"

#include "llvm/Bitcode/ReaderWriter.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/type_traits.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/FileSystem.h"

#include "llvm/IR/Argument.h"
#include <iostream>

#include "llvm_builder.h"
#include "support.h"

// class LLVMModuleBuilder {
  // protected:
  // std::shared_ptr<llvm::Module> module_;
  // std::shared_ptr<llvm::LLVMContext> context_;
  // std::shared_ptr<llvm::IRBuilder<>> builder_;

  // public:
LLVMModuleBuilder::LLVMModuleBuilder(std::string name, std::shared_ptr<llvm::LLVMContext> context){
  context_ = context;
  module_  = std::make_shared<llvm::Module>(name, *context);
  builder_ = std::make_shared<llvm::IRBuilder<>>(*context);
}

llvm::Module *LLVMModuleBuilder::getModule(){
  return module_.get();
}

llvm::LLVMContext *LLVMModuleBuilder::getContext(){
  return context_.get();
}

llvm::IRBuilder<> *LLVMModuleBuilder::getBuilder(){
  return builder_.get();
}

llvm::Value *LLVMModuleBuilder::makePrintf(std::vector<llvm::Value *> values){
  // printf の関数準備
  std::vector<llvm::Type *> args;
  args.push_back(llvm::Type::getInt8PtrTy(*context_));
  llvm::FunctionType *printfType =
    llvm::FunctionType::get(builder_->getInt32Ty(), args, true);
  llvm::Constant *printfFunc =
      module_->getOrInsertFunction("printf", printfType);

  return builder_->CreateCall(printfFunc, values);
}
// };

LLVMFuncBuilder::LLVMFuncBuilder(std::shared_ptr<LLVMModuleBuilder> module, llvm::FunctionType *funcType, std::string name){
  module_ = module;
  func_ = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module_->getModule());
  basicBlock_ = llvm::BasicBlock::Create(*module_->getContext(), "EntryBlock", func_);
  module->getBuilder()->SetInsertPoint(basicBlock_);
}

LLVMFuncBuilder::LLVMFuncBuilder(std::shared_ptr<LLVMModuleBuilder> module, llvm::Function *func)
{
  module_ = module;
  func_ = func;
  basicBlock_ = module_->getBuilder()->GetInsertBlock();
  // module->getBuilder()->SetInsertPoint(basicBlock_);
}

// void LLVMFuncBuilder::makeReturn(llvm::Value *value){
// return basicBlock_->getInstList().push_back(llvm::ReturnInst::Create(*module_->getContext(), value));
// }
void LLVMFuncBuilder::makeReturn(llvm::Value *value){
  auto iBuilder = module_->getBuilder();
  iBuilder->GetInsertBlock()->getInstList().push_back(llvm::ReturnInst::Create(*module_->getContext(), value));
  // return basicBlock->getInstList().push_back(llvm::ReturnInst::Create(*module_->getContext(), value));
}

LLVMModuleBuilder *LLVMFuncBuilder::getModuleBuilder(){
return module_.get();
}

llvm::BasicBlock *LLVMFuncBuilder::getBasicBlock(){
return basicBlock_;
}

llvm::Function *LLVMFuncBuilder::getFunc(){
  return func_;
}

LLVMExprBuilder::LLVMExprBuilder(std::shared_ptr<LLVMFuncBuilder> func){
  func_ = func;
}

llvm::Value *LLVMExprBuilder::makeConst(int value){
  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();

  llvm::Value *val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), value);

  return val;
}

llvm::Value *LLVMExprBuilder::makeConst(float value){
  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();

  llvm::Value *val = llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), value);

  return val;
}

llvm::Value *LLVMExprBuilder::makeConst(double value){

  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();


  llvm::Value *val = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), value);


  return val;
}

llvm::Value *LLVMExprBuilder::makeConst(llvm::StringRef str){
  llvm::IRBuilder<> *builder = func_->getModuleBuilder()->getBuilder();

  llvm::Value *val = builder->CreateGlobalStringPtr(str);

  return val;
}

// 値を作成して呼び出し元で使用するパターン
llvm::Instruction *LLVMExprBuilder::makeCalcOp(llvm::AddrSpaceCastInst::BinaryOps ops,llvm::Value *lhs,llvm::Value *rhs){

  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();
  llvm::BasicBlock *block = func_->getBasicBlock();


  llvm::Instruction *inst = llvm::BinaryOperator::Create(ops, lhs, rhs, "calcresult");


  block->getInstList().push_back(inst); //この操作を挟まないとうまく行かず


  return inst;
}

LLVMVariable::LLVMVariable(std::shared_ptr<LLVMModuleBuilder> module, std::string name, TYPE type){
  module_ = module;

  switch(type){
    case INT:
    case BOOLTYPE:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getInt32Ty(*module->getContext()), 0, name);
      break;
    case DOUBLE:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getDoubleTy(*module->getContext()), 0, name);
      break;
    case STRING:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getInt8PtrTy(*module->getContext()), 0, name);
      break;
    case KLASS:
      break;
  }
}

void LLVMVariable::set(llvm::Value *newVal){
    module_->getBuilder()->CreateStore(newVal, value_);
}

llvm::Value *LLVMVariable::get(){
  return module_->getBuilder()->CreateLoad(value_);
}

LLVMStructDef::LLVMStructDef(std::string def_name, std::map<std::string, llvm::Type*> fields){

  def_name_ = def_name;

  fields_ = fields;

  std::vector<llvm::Type*> fieldsvec;
  for(auto itr = fields.begin(); itr != fields.end(); ++itr) {
    fieldsvec.push_back(itr->second);
  }

  structTy = llvm::StructType::create(fieldsvec, def_name, false);
}

llvm::StructType *LLVMStructDef::getStructTy(){
  return structTy;
}

int LLVMStructDef::filedName2Index(std::string filed_name){
  auto iter = fields_.find(filed_name);
  return std::distance(fields_.begin(), iter);
}

LLVMStructDefMap::LLVMStructDefMap(){
}

void LLVMStructDefMap::set(std::string name, LLVMStructDef *struct_def){
  map[name] = struct_def;
}

LLVMStructDef *LLVMStructDefMap::get(std::string name){
  TMP_DEBUGS(name.c_str());
  TMP_DEBUGP(strlen(name.c_str()));
  TMP_DEBUGP(&map[name]);
  return map[name];
}

void LLVMStructDefMap::makeStructDef(std::string def_name, std::map<std::string, llvm::Type*>  fields){
TMP_DEBUGL;
TMP_DEBUGS(def_name.c_str());
  LLVMStructDef *sd = new LLVMStructDef(def_name, fields);
  map[def_name] = sd;
  TMP_DEBUGP(&map[def_name]);
  TMP_DEBUGP(strlen(def_name.c_str()));
}

LLVMStruct::LLVMStruct(std::shared_ptr<LLVMModuleBuilder> module,LLVMStructDef *struct_def, std::string name) : LLVMVariable(module, name, KLASS){
  TMP_DEBUGL;
  struct_def_ = struct_def;
  TMP_DEBUGL;
  TMP_DEBUGP(struct_def);
  alloca_inst = new llvm::AllocaInst(struct_def->getStructTy());
  TMP_DEBUGL;
  auto iBuilder = module_->getBuilder();
  auto block = iBuilder->GetInsertBlock();
  block->getInstList().push_back(alloca_inst);
}

void LLVMStruct::set(std::string member_name, llvm::Value *newVal){
  auto field_i = struct_def_->filedName2Index(member_name);

  auto iBuilder = module_->getBuilder();
  auto structTy = struct_def_->getStructTy();
  llvm::Type   *intTy  = llvm::Type::getInt32Ty(*module_->getContext());
  auto store = iBuilder->CreateStore(newVal, iBuilder->CreateStructGEP(structTy,alloca_inst, field_i));
}

llvm::Value *LLVMStruct::get(std::string member_name){
  auto iBuilder = module_->getBuilder();
  auto structTy = struct_def_->getStructTy();

  auto field_i = struct_def_->filedName2Index(member_name);

  auto load = iBuilder->CreateLoad( iBuilder->CreateStructGEP(structTy,alloca_inst, field_i));

  return load;
}

LLVMVariableMap::LLVMVariableMap(std::shared_ptr<LLVMModuleBuilder> module){
  module_ = module;
}

void LLVMVariableMap::set(VariableIndicator *target, llvm::Value *newVal){
  target->set(this, newVal);
}

llvm::Value *LLVMVariableMap::get(VariableIndicator *target){
  return target->get(this);
}

LLVMVariable *LLVMVariableMap::LLVMVariableMap::getVariable(std::string name){
  return map[name];
}

LLVMLocalVariableMap::LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module) : LLVMVariableMap(module){

}

void LLVMLocalVariableMap::makeVariable(std::string name ,TYPE type){
  map[name] = new LLVMVariable(module_, name, type);
}

void LLVMLocalVariableMap::makeStruct(std::string name, LLVMStructDef *structDef){
  TMP_DEBUGL;
  map[name] = new LLVMStruct(module_, structDef, name);
  TMP_DEBUGL;
}

VariableIndicator::VariableIndicator(std::string name){
    name_ = name;
}

void VariableIndicator::set(LLVMVariableMap *target, llvm::Value *newVal){
    auto var = target->getVariable(name_);
    var->set(newVal);
}

llvm::Value *VariableIndicator::get(LLVMVariableMap *target){
    auto var = target->getVariable(name_);
    return var->get();
}

StructIndicator::StructIndicator(std::string name, std::string member_name) : VariableIndicator(name){
    member_name_ = member_name;
}

void StructIndicator::set(LLVMVariableMap *target, llvm::Value *newVal){
    auto var = (LLVMStruct *)target->getVariable(name_);
    var->set(member_name_, newVal);
}

llvm::Value *StructIndicator::get(LLVMVariableMap *target){
    auto var = (LLVMStruct *)target->getVariable(name_);
    return var->get(member_name_);
}
