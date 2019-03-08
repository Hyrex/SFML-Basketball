#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include "b2Actor2D.h"
#include "Defines.h"

class Application;
class FTickHandle;
class FAssetLoader;


struct FRenderWindowData
{
	int Width = 1000;
	int Height = 728;
	int BitsPerPixel;
	
	std::string WindowName = "BasketBallSimulator";

	inline SFML::VideoMode	GetVideoModeFromData()	{	return SFML::VideoMode(Width, Height, BitsPerPixel);	}
	inline std::string		GetWindowName()			{	return WindowName;	}
};

class Application
{
public:
	Application();
	~Application();

	int Initialize();
	void BeginPlay();
	void Tick(const float DeltaTime);
	void EndPlay();

private:

	FRenderWindowData RenderWindowData;
	FTickHandle* TickHandle;
	FAssetLoader* AssetLoader;
	SFML::RenderWindow AppWindow;

	SFML::Texture t;
	SFML::RectangleShape s;
	SFML::Text p;
	SFML::Music* BGM;
	
	//Box2D
	b2Vec2 Gravity; 
	b2World* World;

	std::vector<b2Actor2D*> b2Actors;
	std::vector<SFML::Shape*> RenderShapes;

	//
};
