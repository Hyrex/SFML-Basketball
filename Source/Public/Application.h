#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>

#include "Defines.h"

class FTickHandle;
class FAssetLoader;
class b2Actor2D;
class b2Actor2DContactListener;

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

	b2World* GetWorld() const { return World.get(); }
	FTickHandle* GetTickHandle() const { return TickHandle.get();  }

private:

	std::shared_ptr<FTickHandle> TickHandle;
	std::shared_ptr<FAssetLoader> AssetLoader;
	FRenderWindowData RenderWindowData;
	SFML::RenderWindow AppWindow;

	SFML::Music* BGM;
	
	//Box2D
	b2Vec2 Gravity; 
	std::shared_ptr<b2World> World;
	std::shared_ptr<b2Actor2DContactListener> b2ActorContactListner;


	std::vector<std::shared_ptr<SFML::Shape>> RenderShapes;
	std::vector<std::shared_ptr<b2Actor2D>> b2Actors;
	std::vector<std::shared_ptr<b2Actor2D>> BallPools_ThatStilLWork;

	SFML::Vertex AngleIndicators[2];

	std::shared_ptr<SFML::RectangleShape> ChargeGaugeMax;		// Cached pointer, no need clear. Cleared via RenderShapes.
	std::shared_ptr<SFML::RectangleShape> ChargeGaugeProgress;	// Cached pointer, no need clear. Cleared via RenderShapes.

	bool bLeftMouseKeyDown = false;
	bool bLeftMousePressed = false;
	bool bRightMousePressed = false;
	bool bMiddleMousePressed = false;

	std::shared_ptr<b2Actor2D> PivotCache;		// Cached pointer, no need clear. Cleared via b2Actors.
	std::shared_ptr<b2Actor2D> WheelCache;		// Cached pointer, no need clear. Cleared via b2Actors.

	static void PivotTick(b2Actor2D* Actor);
	static void WheelTick(b2Actor2D* Actor);


	//
};
