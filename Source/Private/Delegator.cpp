#include "Delegator.h"
#include <algorithm>

Delegator::Delegator()
{
}

Delegator::~Delegator()
{
}

void Delegator::BindFunctionUnique(FuncPtrNoParam Function)
{
}

void Delegator::RemoveFunction(std::string FunctionName)
{
}

void Delegator::IsBound(FuncPtrNoParam Function)
{
	//auto x = [&Function]() { Function; };

	//std::find(InvocationList.begin(), InvocationList.end(), x);
}

void Delegator::Invoke()
{
	//for (auto It : InvocationList)
	{	
		//if (It.second)
		{
		//	It.second();
		}
	}
} 

//void Delegator::BindFunction(std::pair<std::string, VoidFuncPtr> F)
//{
//}
//
//std::pair<std::string, void*()> Delegator::GenerateFuncNamePair(VoidFuncPtr Function)
//
//	std::string m = "test";
//	std::pair<std::string, void*()> Instance;
//	return std::make_pair<std::string, void*()>(m, nullptr);
//
