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
DEBUGL;
  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();

  DEBUGL;
  llvm::Value *val = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), value);
  DEBUGL;

  return val;
}

llvm::Value *LLVMExprBuilder::makeConst(llvm::StringRef str){
  llvm::IRBuilder<> *builder = func_->getModuleBuilder()->getBuilder();

  llvm::Value *val = builder->CreateGlobalStringPtr(str);

  return val;
}

// 値を作成して呼び出し元で使用するパターン
llvm::Instruction *LLVMExprBuilder::makeCalcOp(llvm::AddrSpaceCastInst::BinaryOps ops,llvm::Value *lhs,llvm::Value *rhs){
DEBUGL;
  llvm::LLVMContext *context = func_->getModuleBuilder()->getContext();
  llvm::BasicBlock *block = func_->getBasicBlock();

  DEBUGL;
  llvm::Instruction *inst = llvm::BinaryOperator::Create(ops, lhs, rhs, "calcresult");

  DEBUGL;
  block->getInstList().push_back(inst); //この操作を挟まないとうまく行かず

  DEBUGL;
  return inst;
}

LLVMLocalVariable::LLVMLocalVariable(std::shared_ptr<LLVMModuleBuilder> module, std::string name, TYPE type){
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
    default:
      ASSERT_FAIL_BLOCK();
  }
}

void LLVMLocalVariable::set(llvm::Value *newVal){
  module_->getBuilder()->CreateStore(newVal, value_);
}

llvm::Value *LLVMLocalVariable::get(){
  return module_->getBuilder()->CreateLoad(value_);
}

LLVMLocalVariableMap::LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module){
  module_ = module;
}

void LLVMLocalVariableMap::makeVariable(std::string name ,TYPE type){
  map[name] = new LLVMLocalVariable(module_, name, type);
}

void LLVMLocalVariableMap::set(std::string name, llvm::Value *newVal){
  map[name]->set(newVal);
}

llvm::Value *LLVMLocalVariableMap::get(std::string name){
  return map[name]->get();
}
