#pragma once
#include "Application.h"

int main()
{
	Application* ApplicationInstance = new Application();
	
	if (ApplicationInstance)
	{
		if(ApplicationInstance->Initialize())
			ApplicationInstance->BeginPlay();
	}
	delete ApplicationInstance;

	return 0;
}