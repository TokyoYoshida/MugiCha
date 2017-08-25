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


#include "mugicha.h"
#include "stack.h"
#include "support.h"

#include "func.h"
#include "mugicha_compiler.h"

#include "llvm_builder.h"

llvm::Type *getLLVMTypeByMugichaType(TYPE type, std::shared_ptr<MugichaScopeInfo> scope) {
  switch(type.kind){
    case INT:
    case BOOLTYPE:
      return llvm::Type::getInt32Ty(*scope->getContext());
    case DOUBLE:
      return llvm::Type::getDoubleTy(*scope->getContext());
    case STRING:
      return llvm::Type::getInt8PtrTy(*scope->getContext());
    case KLASS:
      TMP_DEBUGS(type.klass->name);
      auto structDef = scope->getStructDefMap()->get(type.klass->name);
      TMP_DEBUGS(structDef->getDefName().c_str());
      return structDef->getStructPtr();
  }
}

MugichaScopeInfo::MugichaScopeInfo() {
  context_ = std::make_shared<llvm::LLVMContext>();
  module_ = std::make_shared<LLVMModuleBuilder>("mugicha", context_);
  struct_def_map_ = std::make_shared<LLVMStructDefMap>(module_);
  var_map_ = std::make_shared<LLVMLocalVariableMap>(module_, struct_def_map_);
}

MugichaScopeInfo::MugichaScopeInfo(std::shared_ptr<MugichaScopeInfo> old_scope) {
  context_ = old_scope->context_;
  module_ = old_scope->module_;
  var_map_ = old_scope->var_map_;
  struct_def_map_ = old_scope->struct_def_map_;
}

std::shared_ptr<LLVMModuleBuilder> MugichaScopeInfo::getModuleBuilder(){
  return module_;
}

llvm::Module *MugichaScopeInfo::getModule(){
  return module_->getModule();
}

llvm::LLVMContext *MugichaScopeInfo::getContext(){
  return context_.get();
}

llvm::IRBuilder<> *MugichaScopeInfo::getBuilder()
{
  return module_->getBuilder();
}

std::shared_ptr<LLVMExprBuilder> MugichaScopeInfo::makeExprBuilder()
{
  std::shared_ptr<LLVMExprBuilder> builder = std::make_shared<LLVMExprBuilder>(this->getFuncBuilder());

  return builder;
}

llvm::Function *MugichaScopeInfo::getFunc()
{
  llvm::Function *func = this->getBuilder()->GetInsertBlock()->getParent();
  return func;
}

std::shared_ptr<LLVMFuncBuilder> MugichaScopeInfo::getFuncBuilder()
{
  std::shared_ptr<LLVMFuncBuilder> funcBuilder = std::make_shared<LLVMFuncBuilder>(module_, this->getFunc());

  return funcBuilder;
}

std::shared_ptr<LLVMLocalVariableMap> MugichaScopeInfo::getVarMap()
{
  return var_map_;
}

std::shared_ptr<LLVMStructDefMap> MugichaScopeInfo::getStructDefMap()
{
  return struct_def_map_;
}

llvm::Value *exec_def_var_codegen(ASTNODE *ap , std::shared_ptr<MugichaScopeInfo> scope)
{
  auto expr = scope->makeExprBuilder();

TMP_DEBUGL;
  if(ap->type.kind == KLASS){
    TMP_DEBUGL;
    auto strutDef = scope->getStructDefMap()->get(ap->type.klass->name);
    TMP_DEBUGS(ap->type.klass->name);
    TMP_DEBUGP(strutDef);
    TMP_DEBUGL;
    scope->getVarMap()->makeStruct(ap->sym->name ,strutDef);
    TMP_DEBUGL;
  } else {
    TMP_DEBUGL;
    scope->getVarMap()->makeVariable(ap->sym->name ,ap->type);
    TMP_DEBUGL;
  }

  TMP_DEBUGL;
  return expr->makeConst(-1); // TODO
}

llvm::Value *exec_set_var_codegen(ASTNODE *ap , std::shared_ptr<MugichaScopeInfo> scope)
{
  auto expr = scope->makeExprBuilder();
  TMP_DEBUGL;

  auto ret = eval_node_codegen(ap->left, scope);
  TMP_DEBUGL;

  auto target = new VariableIndicator(ap->sym->name);
  scope->getVarMap()->set(target ,ret);
TMP_DEBUGL;
  return ret;
}

llvm::Value *exec_get_var_codegen(ASTNODE *ap , std::shared_ptr<MugichaScopeInfo> scope)
{
DEBUGL;
  auto expr = scope->makeExprBuilder();
  DEBUGL;

  auto target = new VariableIndicator(ap->sym->name);
  DEBUGL;
  auto ret = scope->getVarMap()->get(target);
  DEBUGL;

  return ret;
}

llvm::Value *exec_calc_codegen(ASTNODE *ap,OPERATION calc_mode , std::shared_ptr<MugichaScopeInfo> scope)
{
  auto expr = scope->makeExprBuilder();

  // VALUE ret;
  // VALUE lhs ,rhs;
  // char *s;
  // char *rhs_str;


  auto lhs = eval_node_codegen(ap->left  ,scope);
  auto rhs = eval_node_codegen(ap->right ,scope);
  llvm::AddrSpaceCastInst::BinaryOps ops;

  auto type =  lhs->getType();
  switch(calc_mode){
    case ADD:
      if(type->isIntegerTy()){
        ops = llvm::Instruction::Add;
      } else if(type->isDoubleTy()){
        ops = llvm::Instruction::FAdd;
      } else if(type->isPointerTy()){
        ASSERT_FAIL_BLOCK(); // TODO String Ready
      } else {
        ASSERT_FAIL_BLOCK();
      }
      break;
    case SUB:
      if(type->isIntegerTy()){
        ops = llvm::Instruction::Sub;
      } else if(type->isDoubleTy()){
        ops = llvm::Instruction::FSub;
      } else {
        ASSERT_FAIL_BLOCK();
      }
      break;
    case MUL:
      if(type->isIntegerTy()){
        ops = llvm::Instruction::Mul;
      } else if(type->isDoubleTy()){
        ops = llvm::Instruction::FMul;
      } else {
        ASSERT_FAIL_BLOCK();
      }
      break;
    case DIV:
      if(type->isIntegerTy()){
        ops = llvm::Instruction::SDiv;
      } else if(type->isDoubleTy()){
        ops = llvm::Instruction::FDiv;
      } else {
        ASSERT_FAIL_BLOCK();
      }
      break;
    default:
      ASSERT_FAIL_BLOCK();
  }

  return expr->makeCalcOp(ops ,lhs, rhs);

}

llvm::Value *exec_print_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto moduleBuilder = scope->getModuleBuilder();
  auto expr = scope->makeExprBuilder();

  auto targetValue = eval_node_codegen(ap->left, scope);
  std::vector<llvm::Value *> values;

  llvm::Value *formatStr;
  auto type =  targetValue->getType();
  if( type->isIntegerTy()) {
    formatStr = expr->makeConst("value = %d\n"); // TODO type switch
  } else if( type->isDoubleTy()) {
    formatStr = expr->makeConst("value = %f\n"); // TODO type switch
  } else if( type->isPointerTy()) {
    formatStr = expr->makeConst("value = %s\n"); // TODO type switch
  }

  values.push_back(formatStr);


  values.push_back(targetValue);

  return moduleBuilder->makePrintf(values);

}

llvm::Value *exec_def_func_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> old_scope)
{
  std::shared_ptr<MugichaScopeInfo> new_scope = std::make_shared<MugichaScopeInfo>(old_scope);

  auto context = new_scope->getContext();
  auto module = new_scope->getModule();

  auto funcInfo = lookup_func(ap->sym);

  std::vector<llvm::Type *> argTypes; // TODO : now arg is provisional.

  auto defArgs = funcInfo->def_args;
  if( defArgs ){
    TMP_DEBUGI(defArgs->type.kind);
    auto argType = getLLVMTypeByMugichaType(defArgs->type, new_scope);
    TMP_DEBUGL;
    argTypes.push_back(argType);
    TMP_DEBUGL;
  }

  auto retType = getLLVMTypeByMugichaType(funcInfo->type, new_scope);
  auto func_type =
    llvm::FunctionType::get(retType, argTypes, false);

  auto func = std::make_shared<LLVMFuncBuilder>(new_scope->getModuleBuilder(), func_type, funcInfo->sym->name);

  auto f = lookup_func(ap->sym);

  std::shared_ptr<LLVMExprBuilder> expr = std::make_shared<LLVMExprBuilder>(func);

  auto &argList = func->getFunc()->getArgumentList();
  auto varMap = new_scope->getVarMap();

  if( defArgs ){
    TMP_DEBUGL;
    auto iter = argList.begin();
    llvm::Value *argVal = static_cast<llvm::Value*>( &*iter);

    auto argName =defArgs->sym->name;
    varMap->makeVariable(argName, defArgs->type);
    auto target = new VariableIndicator(argName); // TODO this memory needs free after process
    varMap->set(target, argVal);
    TMP_DEBUGL;
  }

  TMP_DEBUGL;
  auto ret = eval_node_codegen(f->body, new_scope);
  TMP_DEBUGL;

  func->makeReturn(ret);

  return ret;
}

llvm::Value *exec_def_method_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> old_scope)
{
  std::shared_ptr<MugichaScopeInfo> new_scope = std::make_shared<MugichaScopeInfo>(old_scope);

  auto context = new_scope->getContext();
  auto module = new_scope->getModule();

  auto funcInfo = lookup_func(ap->sym);

  std::vector<llvm::Type *> argTypes; // TODO : now arg is provisional.

  auto defArgs = funcInfo->def_args;
  if( defArgs ){
    TMP_DEBUGL;
    auto recieverType = getLLVMTypeByMugichaType(ap->reciever_type, new_scope);
    TMP_DEBUGL;
    argTypes.push_back(recieverType);
    TMP_DEBUGL;
    auto argType = getLLVMTypeByMugichaType(defArgs->type, new_scope);
    argTypes.push_back(argType);
  }

  auto retType = getLLVMTypeByMugichaType(funcInfo->type, new_scope);
  auto func_type =
    llvm::FunctionType::get(retType, argTypes, false);

  auto func = std::make_shared<LLVMFuncBuilder>(new_scope->getModuleBuilder(), func_type, funcInfo->sym->name);

  auto f = lookup_func(ap->sym);

  std::shared_ptr<LLVMExprBuilder> expr = std::make_shared<LLVMExprBuilder>(func);

  auto &argList = func->getFunc()->getArgumentList();
  auto varMap = new_scope->getVarMap();

  if( defArgs ){
    TMP_DEBUGL;
    auto iter = argList.begin();
    llvm::Value *recieverVal = static_cast<llvm::Value*>( &*iter);
    varMap->makeVariable("this", ap->reciever_type);
    auto reciever = new VariableIndicator("this");
    varMap->set(reciever, recieverVal);

    llvm::Value *argVal = static_cast<llvm::Value*>( &*++iter);

    auto argName =defArgs->sym->name;
    varMap->makeVariable(argName, defArgs->type);
    auto target = new VariableIndicator(argName); // TODO this memory needs free after process
    varMap->set(target, argVal);
    TMP_DEBUGL;
  }

  TMP_DEBUGL;
  auto ret = eval_node_codegen(f->body, new_scope);
  TMP_DEBUGL;

  func->makeReturn(ret);

  return ret;
}

llvm::Value *exec_call_func_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto module = scope->getModuleBuilder();

// TODO argument
  // if( ap->set_args != NULL){
  //   eval_node(f->def_args);
  //   eval_node(ap->set_args);
  // }

  std::vector<llvm::Value *> argValues;
  std::vector<llvm::Type *> argTypes;

  auto context = module->getContext();
  auto funcInfo = lookup_func(ap->sym);

  auto defArgs = funcInfo->def_args;
  if( defArgs ){
    TMP_DEBUGL;
    auto argType = getLLVMTypeByMugichaType(defArgs->type,scope);
    TMP_DEBUGL;
    argTypes.push_back(argType);
    TMP_DEBUGL;
  }

  if( ap->set_args ){ // TODO only single arg. multi arg requre.
    TMP_DEBUGL;
    auto setValue = eval_node_codegen(ap->set_args->left, scope);
    TMP_DEBUGL;
    argValues.push_back(setValue);
    TMP_DEBUGL;
  }
  TMP_DEBUGL;

  auto retType = getLLVMTypeByMugichaType(funcInfo->type, scope);
  llvm::FunctionType *funcType =
    llvm::FunctionType::get(retType, argTypes, true);
  llvm::Constant *callFunc =
    module->getModule()->getOrInsertFunction(funcInfo->sym->name, funcType);

    TMP_DEBUGL;
  auto ret = module->getBuilder()->CreateCall(callFunc, argValues);

  TMP_DEBUGL;
  return ret;
}

llvm::Value *exec_call_method_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto module = scope->getModuleBuilder();

// TODO argument
  // if( ap->set_args != NULL){
  //   eval_node(f->def_args);
  //   eval_node(ap->set_args);
  // }

  std::vector<llvm::Value *> argValues;
  std::vector<llvm::Type *> argTypes;

  auto context = module->getContext();
  auto funcInfo = lookup_func(ap->sym);

  auto defArgs = funcInfo->def_args;
  if( defArgs ){
    auto recieverType = getLLVMTypeByMugichaType(ap->reciever_type, scope);
    argTypes.push_back(recieverType);
    TMP_DEBUGL;
    auto argType = getLLVMTypeByMugichaType(defArgs->type,scope);
    TMP_DEBUGL;
    argTypes.push_back(argType);
    TMP_DEBUGL;
  }

  if( ap->set_args ){ // TODO only single arg. multi arg requre.
    auto target = new VariableIndicator(ap->reciever->name);
    auto recieverVal = scope->getVarMap()->get(target);
    argValues.push_back(recieverVal);
    TMP_DEBUGL;
    auto setValue = eval_node_codegen(ap->set_args->left, scope);
    TMP_DEBUGL;
    argValues.push_back(setValue);
    TMP_DEBUGL;
  }
  TMP_DEBUGL;

  auto retType = getLLVMTypeByMugichaType(funcInfo->type, scope);
  llvm::FunctionType *funcType =
    llvm::FunctionType::get(retType, argTypes, true);
  llvm::Constant *callFunc =
    module->getModule()->getOrInsertFunction(funcInfo->sym->name, funcType);

    TMP_DEBUGL;
  auto ret = module->getBuilder()->CreateCall(callFunc, argValues);

  TMP_DEBUGL;
  return ret;
}

LLVMStructDef::FieldDef getFieldDef(llvm::Module *module, ASTNODE *ap)
{

  if(ap->op == DEF_VAR){
    LLVMStructDef::FieldDef f = {{
      ap->sym->name,
      ap->type
    }};

    return f;
  } else {
    LLVMStructDef::FieldDef ret;

    auto l = getFieldDef(module, ap->left);
    auto r = getFieldDef(module, ap->right);
    ret.insert(l.begin(), l.end());
    ret.insert(r.begin(), r.end());

    return ret;
  }
}

llvm::Value *exec_def_class_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  // TODO make this function
  // auto vals = eval_def_vars(ap->def_vars);
TMP_DEBUGL;
  auto context = scope->getContext();
  TMP_DEBUGL;
  auto module = scope->getModule();
  TMP_DEBUGL;

  llvm::FunctionType *FT =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), /*not vararg*/false);

    TMP_DEBUGL;
    //test struct
  auto iBuilder = scope->getBuilder();
  TMP_DEBUGL;
  auto block = iBuilder->GetInsertBlock();
  TMP_DEBUGL;

  auto intTy  = llvm::Type::getInt32Ty(*context);
  TMP_DEBUGL;
  // LLVMStructDef::FieldDef fields = {
  //   {"x" ,INT},
  //   {"y" ,INT},
  // };
  auto fields = getFieldDef(module, ap->def_vars);
  TMP_DEBUGL;

  scope->getStructDefMap()->makeStructDef(ap->sym->name, fields);

  TMP_DEBUGL;
  // auto structTy = llvm::StructType::create(fields, "Testx", false);

  // ここから下の宣言がないとStructの宣言も作られないが、この場所で宣言すべきではない
  // auto newinst    = new llvm::AllocaInst(structTy);
  // block->getInstList().push_back(newinst);

  return NULL;
}

llvm::Value *exec_set_member_var_codegen(ASTNODE *ap , std::shared_ptr<MugichaScopeInfo> scope)
{
  auto expr = scope->makeExprBuilder();

  auto ret = eval_node_codegen(ap->left, scope);

  auto target = (VariableIndicator *)new StructIndicator(ap->sym->name, ap->member->name);
  scope->getVarMap()->set(target, ret);

  return ret;
}

llvm::Value *exec_get_member_var_codegen(ASTNODE *ap , std::shared_ptr<MugichaScopeInfo> scope)
{
  auto expr = scope->makeExprBuilder();

  auto target = (VariableIndicator *)new StructIndicator(ap->sym->name, ap->member->name);
  auto ret = scope->getVarMap()->get(target);

  return ret;
}


llvm::Value *exec_seq_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto lhs = eval_node_codegen(ap->left, scope);
  auto rhs = eval_node_codegen(ap->right, scope);

  return rhs;
}

llvm::Value *exec_if_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto module = scope->getModuleBuilder();
  auto context = module->getContext();
  auto iBuilder = module->getBuilder();

  auto builder2 = scope->makeExprBuilder();

  // llvm::Value *v1 = builder2->makeConst(1);
  // llvm::Value *v2 = builder2->makeConst(1);
  //
  // llvm::Value *v3 = iBuilder->CreateAlloca(llvm::Type::getInt32Ty(*context), 0, "x");
  // iBuilder->CreateStore(v2, v3);
  // v3 = iBuilder->CreateLoad(v3);
  // // llvm::Instruction *Add1 =   builder2->makeCalcOp(llvm::Instruction::Add, v1,v3);


  // llvm::Value *CondCheck = iBuilder->CreateICmpEQ(
  //     Add1, builder2->makeConst(3), "ifcond");

  auto CondV = eval_node_codegen(ap->condition, scope);

  auto TheFunction = iBuilder->GetInsertBlock()->getParent();
  auto ThenBB = llvm::BasicBlock::Create(*context, "then", TheFunction);
  auto ElseBB = llvm::BasicBlock::Create(*context, "else");
  auto MergeBB = llvm::BasicBlock::Create(*context, "ifcont");


  iBuilder->CreateCondBr(CondV, ThenBB, ElseBB);

  iBuilder->SetInsertPoint(ThenBB);

  auto ThenV = eval_node_codegen(ap->left, scope);

  iBuilder->CreateBr(MergeBB);

  ThenBB = iBuilder->GetInsertBlock();

  TheFunction->getBasicBlockList().push_back(ElseBB);
  iBuilder->SetInsertPoint(ElseBB);

  auto ElseV= eval_node_codegen(ap->right, scope);

  iBuilder->CreateBr(MergeBB);

  ElseBB = iBuilder->GetInsertBlock();

  TheFunction->getBasicBlockList().push_back(MergeBB);
  iBuilder->SetInsertPoint(MergeBB);
  llvm::PHINode *PN = iBuilder->CreatePHI(llvm::Type::getInt32Ty(*context), 2, "iftmp");


  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);

  return PN;
}

llvm::Value *exec_while_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  auto module = scope->getModuleBuilder();
  llvm::LLVMContext *context = module->getContext();
  llvm::IRBuilder<> *iBuilder = module->getBuilder();

  std::shared_ptr<LLVMExprBuilder> builder2 = scope->makeExprBuilder();

  llvm::Function *TheFunction = iBuilder->GetInsertBlock()->getParent();
  llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*context, "cond", TheFunction);
  llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*context, "loop", TheFunction);
  llvm::BasicBlock *AfterBB = llvm::BasicBlock::Create(*context, "afterloop", TheFunction);
  // BasicBlock *LoopBodyBB = BasicBlock::Create(*context, "loopbody", TheFunction);

  iBuilder->CreateBr(CondBB);
  iBuilder->SetInsertPoint(CondBB);

  llvm::Value *CondCheck = eval_node_codegen(ap->condition, scope);
  llvm::Value *CondV = iBuilder->CreateICmpEQ(
      CondCheck, llvm::ConstantInt::getTrue(*context),  "loopcond");
      iBuilder->CreateCondBr(CondV, LoopBB, AfterBB);

  iBuilder->SetInsertPoint(LoopBB);

  llvm::Value *LoopV = eval_node_codegen(ap->left, scope);
  iBuilder->CreateBr(CondBB);

  iBuilder->SetInsertPoint(AfterBB);


  return LoopV;
}

llvm::Value *exec_cmp_codegen(ASTNODE *ap ,OPERATION comp_mode,  std::shared_ptr<MugichaScopeInfo> scope)
{
  auto lhs = eval_node_codegen(ap->left ,scope);
  auto rhs = eval_node_codegen(ap->right ,scope);

  llvm::Value *ret;
  auto iBuilder = scope->getBuilder();

  switch(comp_mode){
    case CMP_EQ:
      ret = iBuilder->CreateICmpEQ(lhs, rhs, "cond");
      break;
    case CMP_NOTEQ:
    ret = iBuilder->CreateICmpNE(lhs, rhs, "cond");
      break;
    case CMP_GREATER:
    ret = iBuilder->CreateICmpSGT(lhs, rhs, "cond");
      break;
    case CMP_SMALLER:
    ret = iBuilder->CreateICmpSLT(lhs, rhs, "cond");
      break;
    case CMP_GREATEREQ:
    ret = iBuilder->CreateICmpSGE(lhs, rhs, "cond");
      break;
    case CMP_SMALLEREQ:
    ret = iBuilder->CreateICmpSLE(lhs, rhs, "cond");
      break;
    default:
      ASSERT_FAIL("invalid operation.");
  }

  return ret;
}

llvm::Value *eval_node_op_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  DEBUGS(get_op_description(ap->op));
  switch(ap->op){
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      return exec_calc_codegen(ap ,ap->op, scope);
    case SEQ:
      return exec_seq_codegen(ap, scope);
    case NONE:
    ASSERT_FAIL("this block expect never call.");
    case VALUEDATA:
    ASSERT_FAIL("this block expect never call.");
    case CMP_EQ:
    case CMP_NOTEQ:
    case CMP_GREATER:
    case CMP_SMALLER:
    case CMP_GREATEREQ:
    case CMP_SMALLEREQ:
      return exec_cmp_codegen(ap, ap->op, scope);
    case PRINTDATA:
      return exec_print_codegen(ap, scope);
    case DEF_VAR:
      return exec_def_var_codegen(ap, scope);
    case SET_VAR:
      return exec_set_var_codegen(ap, scope);
    case GET_VAR:
      return exec_get_var_codegen(ap, scope);
    case DEF_FUNC:
    return exec_def_func_codegen(ap ,scope);
    case DEF_METHOD:
    return exec_def_method_codegen(ap ,scope);
    case CALL_FUNC:
    return exec_call_func_codegen(ap, scope);
    case CALL_METHOD:
    return exec_call_method_codegen(ap, scope);
    case DEF_CLASS:
    return exec_def_class_codegen(ap ,scope);
    case SET_MEMBER_VAR:
    return exec_set_member_var_codegen(ap, scope);
    case GET_MEMBER_VAR:
    return exec_get_member_var_codegen(ap, scope);
    case IF_STMT:
    return exec_if_codegen(ap, scope);
    case WHILE_STMT:
    return exec_while_codegen(ap, scope);
  }
  ASSERT_FAIL("this block expect never call.");
}

llvm::Value *eval_node_value_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope )
{
  auto expr = scope->makeExprBuilder();

  switch(ap->val.type.kind){
    case INT:
      return expr->makeConst(ap->val.val.i);
      break;
    case DOUBLE:
      return expr->makeConst(ap->val.val.d);
      break;
    case BOOLTYPE:
      return expr->makeConst(ap->val.val.b);
      break;
    case STRING:
      return expr->makeConst(ap->val.val.s);
      break;
    default:
      ASSERT_FAIL_BLOCK();
  }
}

llvm::Value *eval_node_codegen(ASTNODE *ap, std::shared_ptr<MugichaScopeInfo> scope)
{
  if(ap->op == VALUEDATA ) return eval_node_value_codegen(ap, scope);

  return eval_node_op_codegen(ap, scope);
}

void do_compile(ASTNODE *ast_rootp)
{
  DEBUGL;
  std::shared_ptr<MugichaScopeInfo> scope = std::make_shared<MugichaScopeInfo>();

  DEBUGL;
  eval_node_codegen(ast_rootp, scope);
  //
  DEBUGL;
  WriteBitcodeToFile(scope->getModule(), llvm::outs());
}

extern "C" void mugicha_compile(ASTNODE *rootp)
{
  print_ast(0, rootp);

  do_compile(rootp);

  DEBUGS("compile complete.");
}
