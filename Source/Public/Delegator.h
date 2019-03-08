#pragma once
#include "Defines.h"

#define FuncNamePair std::pair<std::string, VoidFuncPtr>

class Delegator
{
public:
	Delegator();
	~Delegator();

	void BindFunctionUnique(FuncPtrNoParam Function);
	void RemoveFunction(std::string FunctionName);
	void IsBound(FuncPtrNoParam Function);
	void Invoke();

private:
	//void BindFunction(FuncNamePair F);
	//std::pair<std::string, void*> GenerateFuncNamePair(VoidFuncPtr Function);
	//std::vector<FuncNamePair> InvocationList;
};

