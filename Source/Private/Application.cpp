#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "TickHandle.h"
#include "AssetLoader.h"

Application::Application()
{
	TickHandle = new FTickHandle();
	AssetLoader = new FAssetLoader();

	Gravity = b2Vec2(0.f, 9.81f);
	World = new b2World(Gravity);
}

Application::~Application()
{
}

void Application::BeginPlay()
{
	while (AppWindow.isOpen())
	{
		if (TickHandle)
			TickHandle->BeginTick();
	}

	EndPlay();
}

int Application::Initialize()
{
	bool bInitChecks = true;
	if (TickHandle)
		bInitChecks &= TickHandle->BindApplication(this);

	if (AssetLoader)
		bInitChecks &= AssetLoader->LoadResources();

	if (bInitChecks)
	{
		if (BGM = FAssetLoader::FindMusic(AssetLoader, RESOURCES_AUDIO_TROLOLO))
		{
			BGM->play();
			BGM->setVolume(10);
			BGM->setLoop(true);
			BGM->play();
		}

#if 0	// demo
		s.setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		s.setSize(SFML::Vector2f((float)RenderWindowData.Width, (float)RenderWindowData.Height));

		p.setFont(*FAssetLoader::FindFont(AssetLoader, RESOURCES_FONT_CHALK));
		p.setCharacterSize(30);
		p.setPosition(0, 0);
		p.setFillColor(SFML::Color::White);
		p.setString("JESUS FUCK!");
#endif

		// Window creation
		AppWindow.create(RenderWindowData.GetVideoModeFromData(), RenderWindowData.GetWindowName());
		AppWindow.setVerticalSyncEnabled(true);
		AppWindow.setActive();

		// Border creations
		const float BorderThickness = 16.0f;
		const float ViewportX = (float)RenderWindowData.Width;
		const float ViewportY = (float)RenderWindowData.Height;
		const SFML::Vector2f XBorder(ViewportX, BorderThickness);
		const SFML::Vector2f YBorder(BorderThickness, ViewportY * 0.7f);
		const SFML::Vector2f UBorderLocation(ViewportX * 0.5f					, BorderThickness * 0.5f);
		const SFML::Vector2f DBorderLocation(ViewportX * 0.5f					, ViewportY - BorderThickness * 0.5f);
		const SFML::Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f);
		const SFML::Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f);


		b2Actor2D* TopBorder = new b2Actor2D(World, "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		TopBorder->GetShape()->setOutlineThickness(-1);
		TopBorder->GetShape()->setOutlineColor(sf::Color::Black);
		TopBorder->GetShape()->setFillColor(sf::Color(100, 100, 100));
		b2Actors.push_back(TopBorder);
		
		b2Actor2D* LeftBorder = new b2Actor2D(World, "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		LeftBorder->GetShape()->setOutlineThickness(-1);
		LeftBorder->GetShape()->setOutlineColor(sf::Color::Black);
		LeftBorder->GetShape()->setFillColor(sf::Color(100, 100, 100));
		b2Actors.push_back(LeftBorder);

		b2Actor2D* RightBorder = new b2Actor2D(World, "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		RightBorder->GetShape()->setOutlineThickness(-1);
		RightBorder->GetShape()->setOutlineColor(sf::Color::Black);
		RightBorder->GetShape()->setFillColor(sf::Color(100, 100, 100));
		b2Actors.push_back(RightBorder);

		
		SFML::RectangleShape* Background = new SFML::RectangleShape(SFML::Vector2f(ViewportX, ViewportY));
		Background->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(Background);

		SFML::RectangleShape* Scoreboard = new SFML::RectangleShape(SFML::Vector2f(ViewportX, ViewportY * .3f));
		Scoreboard->setPosition(0.0f, ViewportY * 0.7f);
		Scoreboard->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_WOOD));
		RenderShapes.push_back(Scoreboard);

		SFML::RectangleShape* ChargeGaugeMax = new SFML::RectangleShape();
		ChargeGaugeMax->setFillColor(SFML::Color(145, 145, 145, 255));
		RenderShapes.push_back(ChargeGaugeMax);

		SFML::RectangleShape* ChargeGaugeProgress = new SFML::RectangleShape();
		ChargeGaugeProgress->setFillColor(SFML::Color::Yellow);
		RenderShapes.push_back(ChargeGaugeProgress);

		// Indicator of what? Fix me
		//SFML::Vertex SAngleIndicator[2];
		//SAngleIndicator[0].color = SFML::Color::Cyan;
		//SAngleIndicator[1].color = SFML::Color::Cyan;

		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		SFML::Vector2f boardSize(8.0f, 200.0f);
		SFML::Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);
		SFML::Vector2f netEdgeSize(8.0f, 90.0f);
		SFML::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533)), offsetY + 16.0f);
		SFML::Vector2f sensorSize(48.0f, 48.0f);
		SFML::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);

		b2Actor2D* boardFrame1 = new b2Actor2D(World, "board1", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, boardSize, boardPos);
		boardFrame1->GetShape()->setOutlineThickness(-1);
		boardFrame1->GetShape()->setOutlineColor(sf::Color::Black);
		boardFrame1->GetShape()->setFillColor(sf::Color(40, 40, 40, 255));
		b2Actors.push_back(boardFrame1);

		b2Actor2D* boardFrame2 = new b2Actor2D(World, "board2", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, netEdgeSize, netEdgePos);
		boardFrame2->GetShape()->setOutlineThickness(-1);
		boardFrame2->GetShape()->setOutlineColor(sf::Color::Black);
		boardFrame2->GetShape()->setFillColor(sf::Color(40, 40, 40, 255));
		boardFrame2->GetBodyInstance()->SetTransform(boardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2Actors.push_back(boardFrame2);

		b2Actor2D* ScoreSensor = new b2Actor2D(World, "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0, false, true);
		ScoreSensor->GetShape()->setOutlineThickness(-1);
		ScoreSensor->GetShape()->setOutlineColor(sf::Color::Black);
		ScoreSensor->GetShape()->setFillColor(sf::Color(255, 255, 0, 100));
		b2Actors.push_back(ScoreSensor);
	}

	return bInitChecks;
}

void Application::Tick(const float DeltaTime)
{
	SFML::Event ExitEvent;

	// Delay PollEvent, avoid to use
	while (AppWindow.pollEvent(ExitEvent));
	{
		if (ExitEvent.type == SFML::Event::Closed)
		{
			AppWindow.close();		
		}
	}

	for (auto i : b2Actors)
	{
		if (i) i->Tick();
	}

	// Rendering
	AppWindow.clear(CORNFLOWER_BLUE);

	for (auto Itr : RenderShapes)
		AppWindow.draw(*Itr);
	
	for (auto Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());
	
	
	AppWindow.display();
}

void Application::EndPlay()
{
	if (TickHandle)
	{
		TickHandle->EndTick();
		delete TickHandle;
	}

	if (AssetLoader)
	{
		AssetLoader->UnloadResources();
		delete AssetLoader;
	}

	for (auto i : b2Actors)
		SAFE_DELETE(i);

	SAFE_DELETE(World);
}
