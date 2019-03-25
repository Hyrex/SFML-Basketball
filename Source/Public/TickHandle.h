#pragma once
#include <SFML/System.hpp>
#include "Defines.h"
#include <ctime>

class Application;

class FTickHandle
{
public:
	FTickHandle();
	~FTickHandle();

	/* 
	 * Binding instance of application into tick handle and call tick here with delta step. 
	 * Ideally, should forward dilation to object instance, but the main tick should tick at normal delta step.
	 */
	bool BindApplication(Application* Object);

	/*Wrapper function to instruct Tick().*/
	void BeginTick();

	/* Tick body */
	void Tick();

	/* Handle Finalization after last Tick called.*/
	void EndTick();

	float GetElapsedTime() const { return ElapsedTime;  }
	
	void ClearTimer() { ElapsedTime = 0.0f;  }

private:

	float ElapsedTime = 0.0f;
	float TimeElapsedSinceLastFrame = 0.0f;
	SFML::Clock FixedUpdateClock;
	Application* ContextObject;
};

