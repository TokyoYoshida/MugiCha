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
#include "symbol.h"

llvm::PointerType *getOpequePtrType(llvm::LLVMContext *context){
  auto opequeType = llvm::Type::getInt8Ty(*context);
  auto opequePtrType = llvm::PointerType::getUnqual(opequeType);

  return opequePtrType;
}

llvm::Type *astType2LLVMType(std::shared_ptr<LLVMModuleBuilder> module, TYPEKIND kind){
  switch(kind){
    case INT:
    case BOOLTYPE:
      return llvm::Type::getInt32Ty(*module->getContext());
    case DOUBLE:
      return llvm::Type::getDoubleTy(*module->getContext());
    case STRING:
      return llvm::Type::getInt8PtrTy(*module->getContext());
    default:
      ASSERT_FAIL_BLOCK();
  }
}

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

llvm::Value *LLVMModuleBuilder::makeMalloc(llvm::Constant *AllocSize, llvm::Type *DestTy){
  // printf の関数準備
  auto ITy = llvm::Type::getInt32Ty(*context_);
  auto castedAllocSize = llvm::ConstantExpr::getTruncOrBitCast(AllocSize, ITy);
  std::vector<llvm::Value *>  fmalloc_args({castedAllocSize});
  std::vector<llvm::Type *> args;
  args.push_back(builder_->getInt32Ty());
  llvm::FunctionType *printfType =
    llvm::FunctionType::get(llvm::Type::getInt8PtrTy(*context_), args, true);
  llvm::Constant *func =
      module_->getOrInsertFunction("malloc", printfType);

  return builder_->CreateCall(func, fmalloc_args);
}

llvm::AllocaInst *LLVMModuleBuilder::makeMallocForType(llvm::Type *DestTy){
  auto AllocSize = llvm::ConstantExpr::getSizeOf(DestTy);
  auto inst = makeMalloc(AllocSize, DestTy);
  auto destPtrTy = llvm::PointerType::getUnqual(DestTy);
  return (llvm::AllocaInst *)builder_->CreateBitCast(inst, destPtrTy);
}

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

LLVMStorage::LLVMStorage(std::shared_ptr<LLVMModuleBuilder> module){
  TMP_DEBUGP(module.get());
  module_ = module;
}

LLVMHeapStorage::LLVMHeapStorage(std::shared_ptr<LLVMModuleBuilder> module) : LLVMStorage(module) {
}

llvm::AllocaInst *LLVMHeapStorage::CreateAlloc(llvm::Type *DestTy){
  TMP_DEBUGL;
  auto ret = module_->makeMallocForType(DestTy);
  TMP_DEBUGL;
  return ret;
}

LLVMVariable::LLVMVariable(std::shared_ptr<LLVMModuleBuilder> module, TYPE type){
  type_ = type;
  module_ = module;

  TMP_DEBUGI(type.kind);
  DEBUGS(get_type_description(type.kind));
  switch(type.kind){
    case INT:
    case BOOLTYPE:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getInt32Ty(*module->getContext()), 0);
      break;
    case DOUBLE:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getDoubleTy(*module->getContext()), 0);
      break;
    case STRING:
      value_ = module->getBuilder()->CreateAlloca(llvm::Type::getInt8PtrTy(*module->getContext()), 0);
      break;
    case ARRAY:
    case KLASS:
      value_ = module->getBuilder()->CreateAlloca(getOpequePtrType(module->getContext()), 0);
      break;
    default:
      ASSERT_FAIL_BLOCK();
  }
  TMP_DEBUGL;
}

void LLVMVariable::set(llvm::Value *newVal){
TMP_DEBUGL;
    module_->getBuilder()->CreateStore(newVal, value_);
}

llvm::Value *LLVMVariable::get(){
  TMP_DEBUGL;
  return module_->getBuilder()->CreateLoad(value_);
}

TYPE LLVMVariable::getType(){
  return type_;
}

LLVMStructDef::LLVMStructDef(std::shared_ptr<LLVMModuleBuilder> module, std::string def_name, FieldDef fields){

  module_ = module;
  def_name_ = def_name;
  fields_ = fields;

  std::vector<llvm::Type*> fieldsvec;
  for(auto itr = fields.begin(); itr != fields.end(); ++itr) {
    auto type =  astType2LLVMType(module_, itr->type.kind);
    fieldsvec.push_back(type);
  }

  structTy = llvm::StructType::create(fieldsvec, def_name, false);
  structPtr = llvm::PointerType::getUnqual(structTy);
}

llvm::StructType *LLVMStructDef::getStructTy(){
  return structTy;
}

llvm::PointerType *LLVMStructDef::getStructPtr(){
  return structPtr;
}

std::string LLVMStructDef::getDefName(){
  return def_name_;
}

int LLVMStructDef::filedName2Index(std::string field_name){
  return getFieldDistance(fields_, field_name);
}

LLVMStructDefMap::LLVMStructDefMap(std::shared_ptr<LLVMModuleBuilder> module){
  module_ = module;
}

void LLVMStructDefMap::set(std::string name, LLVMStructDef *struct_def){
  TMP_DEBUGL;
  map[name] = struct_def;
}

LLVMStructDef *LLVMStructDefMap::get(std::string name){
  TMP_DEBUGL;
  return map[name];
}

void LLVMStructDefMap::makeStructDef(std::string def_name, LLVMStructDef::FieldDef  fields){
  LLVMStructDef *sd = new LLVMStructDef(module_, def_name, fields);
  map[def_name] = sd;
}

LLVMStructInitializer::LLVMStructInitializer(std::shared_ptr<LLVMModuleBuilder> module, LLVMStructDef *struct_def){
  type.kind = KLASS;
  type.klass = lookup_symbol(struct_def->getDefName().c_str());
  TMP_DEBUGS(type.klass->name);
}

LLVMStruct::LLVMStruct(std::shared_ptr<LLVMModuleBuilder> module,LLVMStructDef *struct_def, std::shared_ptr<LLVMStorage> storage) :  LLVMStructInitializer(module, struct_def) ,LLVMVariable(module, type) {
TMP_DEBUGL;
  struct_def_ = struct_def;
  storage_ = storage;
  auto iBuilder = module_->getBuilder();
  TMP_DEBUGL;
  TMP_DEBUGP(storage_.get());
  auto alloca_inst= storage->CreateAlloc(struct_def->getStructTy());
  TMP_DEBUGL;

  auto ptr = llvm::PointerType::getUnqual(struct_def->getStructPtr());
  auto castedVal = iBuilder->CreateBitCast(value_, ptr);

  TMP_DEBUGL;

  iBuilder->CreateStore(alloca_inst, castedVal);
  TMP_DEBUGL;
}

void LLVMStruct::set(std::string member_name, llvm::Value *newVal){
TMP_DEBUGL;
  auto iBuilder = module_->getBuilder();

  TMP_DEBUGL;
  auto ptrptr = llvm::PointerType::getUnqual(struct_def_->getStructPtr());
  auto castedVal = iBuilder->CreateBitCast(value_, ptrptr);
  TMP_DEBUGL;
  auto ptr = iBuilder->CreateLoad(castedVal);
  TMP_DEBUGL;
  auto field_i = struct_def_->filedName2Index(member_name);

  TMP_DEBUGL;
  auto structTy = struct_def_->getStructTy();
  TMP_DEBUGL;
  auto gep =  iBuilder->CreateStructGEP(structTy, ptr, field_i);
  TMP_DEBUGL;
  auto store = iBuilder->CreateStore(newVal, gep);
  TMP_DEBUGL;
}

void LLVMStruct::set(llvm::Value *newVal){
  TMP_DEBUGL;

  module_->getBuilder()->CreateStore(newVal, value_);
  TMP_DEBUGL;
  // value_ = (llvm::AllocaInst *)newVal;
}

llvm::Value *LLVMStruct::get(std::string member_name){
TMP_DEBUGL;
  auto iBuilder = module_->getBuilder();
  auto structTy = struct_def_->getStructTy();

  auto ptrptr = llvm::PointerType::getUnqual(struct_def_->getStructPtr());
  auto castedVal = iBuilder->CreateBitCast(value_, ptrptr);
  auto ptr = iBuilder->CreateLoad(castedVal);
  auto field_i = struct_def_->filedName2Index(member_name);

  auto load = iBuilder->CreateLoad( iBuilder->CreateStructGEP(structTy, ptr, field_i));

  return load;
}

llvm::Value *LLVMStruct::get(){
  return module_->getBuilder()->CreateLoad(value_);
  // return value_;
}

Klass::Klass(std::shared_ptr<LLVMModuleBuilder> module, LLVMStructDef *struct_def, int klass_id, std::shared_ptr<LLVMStorage> storage) : LLVMStruct(module ,struct_def, storage){
  llvm::Value *kid = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*module_->getContext()), klass_id);
  set(KLASS_ID_VAL_NAME, kid);
}


llvm::ArrayType *LLVMArray::getArrayType(){
  auto elem_type = astType2LLVMType(module_, type_.elem_kind);
  auto atype = llvm::ArrayType::get(elem_type, size_);

  return atype;
}

KlassDef::KlassDef(std::shared_ptr<LLVMModuleBuilder> module, std::string name, FieldDef fields) : LLVMStructDef(module, name, fields) {
}

void KlassDef::addMethod(std::string name, std::shared_ptr<LLVMFuncBuilder> func){
  method_map[def_name_] = func;
}

int KlassDef::getMethodId(std::string name){
  auto iter = method_map.find(def_name_);
  return std::distance(method_map.begin(), iter);
}

int KlassDef::getMethodCount(){
  return method_map.size();
}

std::map<std::string, std::shared_ptr<LLVMFuncBuilder>> KlassDef::getMethodMap(){
  return method_map;
}

KlassDefMap::KlassDefMap(std::shared_ptr<LLVMModuleBuilder> module){
  module_ = module;
}

void KlassDefMap::set(std::string name, KlassDef *klass_def){
  TMP_DEBUGS(name.c_str());
  TMP_DEBUGP(&map);
    map[name] = klass_def;
}

int KlassDefMap::size(){
  return map.size();
}

KlassDef *KlassDefMap::get(std::string name){
TMP_DEBUGL;
TMP_DEBUGS(name.c_str());
TMP_DEBUGP(&map);
  auto def = map[name];

  if(!def) ASSERT_FAIL_BLOCK();

  return def;
}

void KlassDefMap::makeKlassDef(std::string name, KlassDef::FieldDef fields){
  TMP_DEBUGS(name.c_str());
  TMP_DEBUGP(&map);
  map[name] = new KlassDef(module_, name, fields);
  TMP_DEBUGP(&map);
}

int KlassDefMap::getDefId(std::string name){
  auto iter = map.find(name);
  return std::distance(map.begin(), iter);
}

std::map<std::string, KlassDef *> KlassDefMap::getMap(){
  return map;
}

LLVMArray::LLVMArray(std::shared_ptr<LLVMModuleBuilder> module, TYPE type, int size) : LLVMVariable(module, type) {
TMP_DEBUGL;
  size_ = size;
  auto iBuilder = module_->getBuilder();
  auto atype = getArrayType();

  alloca_inst= iBuilder->CreateAlloca(atype, 0);
  TMP_DEBUGL;
  //
  auto ptr = llvm::PointerType::getUnqual(getArrayPtr());

  auto castedVal = iBuilder->CreateBitCast(value_, ptr);
  //
  TMP_DEBUGL;

  iBuilder->CreateStore(alloca_inst, castedVal);
  TMP_DEBUGL;
}

void LLVMArray::set(llvm::Value *index, llvm::Value *newVal){
TMP_DEBUGL;
  auto iBuilder = module_->getBuilder();

  TMP_DEBUGL;
  auto ptrptr = llvm::PointerType::getUnqual(getArrayPtr());
  auto castedVal = iBuilder->CreateBitCast(value_, ptrptr);
  TMP_DEBUGL;
  auto ptr = iBuilder->CreateLoad(castedVal);
  TMP_DEBUGL;
  llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(index);
  if (!CI) {
    ASSERT_FAIL_BLOCK();
  }
  auto field_i = CI->getSExtValue();
  // auto field_i = index;

  TMP_DEBUGL;
  auto structTy = getArrayType();
  TMP_DEBUGL;
  auto gep =  iBuilder->CreateStructGEP(structTy, ptr, field_i);

  auto store = iBuilder->CreateStore(newVal, gep);
  TMP_DEBUGL;
}

llvm::Value *LLVMArray::get(llvm::Value *index){
TMP_DEBUGL;
  auto iBuilder = module_->getBuilder();
  TMP_DEBUGL;
  auto structTy = getArrayType();

  TMP_DEBUGL;
  auto ptrptr = llvm::PointerType::getUnqual(getArrayPtr());
  TMP_DEBUGL;
  auto castedVal = iBuilder->CreateBitCast(value_, ptrptr);
  TMP_DEBUGL;
  auto ptr = iBuilder->CreateLoad(castedVal);
  TMP_DEBUGL;
  llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(index);
  if (!CI) {
    ASSERT_FAIL_BLOCK();
  }
  auto field_i = CI->getSExtValue();
  TMP_DEBUGI(field_i);

  TMP_DEBUGL;
  auto gep =  iBuilder->CreateStructGEP(structTy, ptr, field_i);
  auto load = iBuilder->CreateLoad( gep);
  TMP_DEBUGL;

  return load;
}

llvm::PointerType *LLVMArray::getArrayPtr(){
  auto atype = getArrayType();
  auto ptr = llvm::PointerType::getUnqual(atype);
  return ptr;
}

LLVMVariableMap::LLVMVariableMap(std::shared_ptr<LLVMModuleBuilder> module, std::shared_ptr<LLVMStructDefMap> struct_def_map, std::shared_ptr<KlassDefMap> klass_def_map){
  module_ = module;
  struct_def_map_ = struct_def_map;
  klass_def_map_ = klass_def_map;
}

void LLVMVariableMap::set(VariableIndicator *target, llvm::Value *newVal){
  TMP_DEBUGL;
  target->set(this, newVal);
  TMP_DEBUGL;
}

llvm::Value *LLVMVariableMap::get(VariableIndicator *target){
  TMP_DEBUGL;
  return target->get(this);
}

LLVMVariable *LLVMVariableMap::LLVMVariableMap::getVariable(std::string name){
  TMP_DEBUGP(map[name]);
  return map[name];
}

LLVMLocalVariableMap::LLVMLocalVariableMap(std::shared_ptr<LLVMModuleBuilder> module, std::shared_ptr<LLVMStructDefMap> struct_def_map, std::shared_ptr<KlassDefMap> klass_def_map) : LLVMVariableMap(module ,struct_def_map, klass_def_map){

}

void LLVMLocalVariableMap::makeVariable(std::string name ,TYPE type){
  map[name] = new LLVMVariable(module_, type);
}

void LLVMLocalVariableMap::makeClass(std::string name, KlassDef *klassDef, std::shared_ptr<LLVMStorage> storage){
  TMP_DEBUGL;
  auto klassId = klass_def_map_->getDefId(klassDef->getDefName());
  auto c = new Klass(module_, klassDef, klassId, storage);
  map[name] = c;
  TMP_DEBUGL;
}

void LLVMLocalVariableMap::makeArray(std::string name, TYPE type, int size){
  TMP_DEBUGL;
  map[name] = new LLVMArray(module_, type, size);
  TMP_DEBUGL;
}

VariableIndicator::VariableIndicator(std::string name){
    name_ = name;
}

void VariableIndicator::set(LLVMVariableMap *target, llvm::Value *newVal){
    TMP_DEBUGL;
    auto var = target->getVariable(name_);
    TMP_DEBUGL;
    var->set(newVal);
}

llvm::Value *VariableIndicator::get(LLVMVariableMap *target){
    TMP_DEBUGL;
    auto var = target->getVariable(name_);
    TMP_DEBUGL;
    return var->get();
}

StructIndicator::StructIndicator(std::string name, std::string member_name) : VariableIndicator(name){
    member_name_ = member_name;
}

void StructIndicator::set(LLVMVariableMap *target, llvm::Value *newVal){
    TMP_DEBUGL;
    if( member_name_.empty() ){
      ASSERT_FAIL_BLOCK();
    }
    TMP_DEBUGL;
    TMP_DEBUGS(name_.c_str());
    TMP_DEBUGS(member_name_.c_str());
    auto var = (LLVMStruct *)target->getVariable(name_);
    TMP_DEBUGL;
    TMP_DEBUGP(newVal);
    TMP_DEBUGS(typeid(var).name());
    var->set(member_name_, newVal);
    TMP_DEBUGL;
}

llvm::Value *StructIndicator::get(LLVMVariableMap *target){
    TMP_DEBUGL;
    auto var = (LLVMStruct *)target->getVariable(name_);
    return var->get(member_name_);
}

int getFieldDistance(LLVMStructDef::FieldDef fields,std::string field_name){
  for(auto itr = fields.begin(); itr != fields.end(); ++itr) {
    if(itr->name == field_name){
      return std::distance(fields.begin(), itr);
    }
  }
  return -1;
}

ArrayIndicator::ArrayIndicator(std::string name, llvm::Value *index) : VariableIndicator(name){
    index_ = index;
}

void ArrayIndicator::set(LLVMVariableMap *target, llvm::Value *newVal){
    TMP_DEBUGL;
    TMP_DEBUGL;
    TMP_DEBUGS(name_.c_str());
    auto var = (LLVMArray *)target->getVariable(name_);
    TMP_DEBUGL;
    TMP_DEBUGP(newVal);
    TMP_DEBUGS(typeid(var).name());
    TMP_DEBUGI(index_);
    var->set(index_, newVal);
    TMP_DEBUGL;
}

llvm::Value *ArrayIndicator::get(LLVMVariableMap *target){
    TMP_DEBUGL;
    auto var = (LLVMArray *)target->getVariable(name_);
    return var->get(index_);
}
