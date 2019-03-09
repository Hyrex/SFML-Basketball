#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"
#include "TickHandle.h"
#include "AssetLoader.h"

Application::Application()
{
	TickHandle = new FTickHandle();
	AssetLoader = new FAssetLoader();
	b2ActorContactListner = new b2Actor2DContactListener();

	Gravity = b2Vec2(0.f, 9.81f);
	World = new b2World(Gravity);
	World->SetContactListener(b2ActorContactListner);

	ChargeGaugeMax = new SFML::RectangleShape();
	ChargeGaugeProgress = new SFML::RectangleShape();
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
			BGM->setVolume(0);
			BGM->setLoop(true);
			BGM->play();
		}

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
		const SFML::Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2
		const SFML::Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2


		b2Actor2D* TopBorder = new b2Actor2D(this, World, "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		TopBorder->GetShape()->setOutlineThickness(-1);
		TopBorder->GetShape()->setOutlineColor(SFML::Color::Black);
		TopBorder->GetShape()->setFillColor(SFML::Color(100, 100, 100));
		b2Actors.push_back(TopBorder);
		
		b2Actor2D* LeftBorder = new b2Actor2D(this, World, "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		LeftBorder->GetShape()->setOutlineThickness(-1);
		LeftBorder->GetShape()->setOutlineColor(SFML::Color::Black);
		LeftBorder->GetShape()->setFillColor(SFML::Color(100, 100, 100));
		b2Actors.push_back(LeftBorder);

		b2Actor2D* RightBorder = new b2Actor2D(this, World, "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		RightBorder->GetShape()->setOutlineThickness(-1);
		RightBorder->GetShape()->setOutlineColor(SFML::Color::Black);
		RightBorder->GetShape()->setFillColor(SFML::Color(100, 100, 100));
		b2Actors.push_back(RightBorder);

#if 1 // debug floor!
		b2Actor2D* BotBorder = new b2Actor2D(this, World, "BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
		BotBorder->GetShape()->setOutlineThickness(-1);
		BotBorder->GetShape()->setOutlineColor(SFML::Color::Black);
		BotBorder->GetShape()->setFillColor(SFML::Color(100, 100, 100));
		b2Actors.push_back(BotBorder);
#endif 

		
		SFML::RectangleShape* Background = new SFML::RectangleShape(SFML::Vector2f(ViewportX, ViewportY));
		Background->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(Background);

		SFML::RectangleShape* Scoreboard = new SFML::RectangleShape(SFML::Vector2f(ViewportX, ViewportY * .3f));
		Scoreboard->setPosition(0.0f, ViewportY * 0.7f);
		Scoreboard->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(Scoreboard);

	
		ChargeGaugeMax->setFillColor(SFML::Color(145, 145, 145, 255));
		ChargeGaugeMax->setSize(SFML::Vector2f(160.0f, 8.0f));
		RenderShapes.push_back(ChargeGaugeMax);

		ChargeGaugeProgress->setFillColor(SFML::Color::Yellow);
		RenderShapes.push_back(ChargeGaugeProgress);

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i] = new SFML::Vertex();
			AngleIndicators[i]->color = (i == 1) ? SFML::Color::Cyan : SFML::Color::Blue;
		}
		

		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		SFML::Vector2f boardSize(8.0f, 200.0f);
		SFML::Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);
		SFML::Vector2f netEdgeSize(8.0f, 90.0f);
		SFML::Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		SFML::Vector2f sensorSize(48.0f, 48.0f);
		SFML::Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);

		b2Actor2D* boardFrame1 = new b2Actor2D(this, World, "board1", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, boardSize, boardPos);
		boardFrame1->GetShape()->setOutlineThickness(-1);
		boardFrame1->GetShape()->setOutlineColor(sf::Color::Black);
		boardFrame1->GetShape()->setFillColor(sf::Color(40, 40, 40, 255));
		b2Actors.push_back(boardFrame1);

		b2Actor2D* boardFrame2 = new b2Actor2D(this, World, "board2", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, netEdgeSize, netEdgePos);
		boardFrame2->GetShape()->setOutlineThickness(-1);
		boardFrame2->GetShape()->setOutlineColor(sf::Color::Black);
		boardFrame2->GetShape()->setFillColor(sf::Color(40, 40, 40, 255));
		boardFrame2->GetBodyInstance()->SetTransform(boardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2Actors.push_back(boardFrame2);

		b2Actor2D* ScoreSensor = new b2Actor2D(this, World, "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0, false, true);
		ScoreSensor->GetShape()->setOutlineThickness(-1);
		ScoreSensor->GetShape()->setOutlineColor(sf::Color::Black);
		ScoreSensor->GetShape()->setFillColor(sf::Color(255, 255, 0, 100));
		b2Actors.push_back(ScoreSensor);

		// The Track 
		const int Row = 14;
		const int Column = 2;

		SFML::Vector2f trackStartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row*32.0f));
		SFML::Vector2f slingPosSize(32.0f, 32.0f);
		SFML::RectangleShape* Tracks[Row][Column];
		for (int i = 0; i < Row; ++i)
		{
			for (int j = 0; j < Column; ++j)
			{
				Tracks[i][j] = new SFML::RectangleShape();

				Tracks[i][j]->setSize(slingPosSize);
				Tracks[i][j]->setPosition(trackStartLocation.x + j * 32.0f, trackStartLocation.y + i * 32.0f);
				Tracks[i][j]->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BOXALT));
				RenderShapes.push_back(Tracks[i][j]);
			}
		}

		// Projector Pivot
		SFML::Vector2f projectorPos(trackStartLocation.x + 32.0f, trackStartLocation.y + (Row / 2)*32.0f);
		SFML::Vector2f restartPos(projectorPos.x / 32.0f, projectorPos.y / 32.0f);
		b2Actor2D* const Pivot = new b2Actor2D(this, World, "Pivot", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, SFML::Vector2f(8.0f, 8.0f), projectorPos, 0.0f, false, false);
		Pivot->GetShape()->setOutlineThickness(-1);
		Pivot->GetShape()->setOutlineColor(SFML::Color::Black);
		Pivot->GetShape()->setFillColor(SFML::Color(0, 0, 255, 100));
		Pivot->BindOnTick(PivotTick);
		PivotCache = Pivot;
		b2Actors.push_back(Pivot);

		b2Actor2D* const Wheel = new b2Actor2D(this, World, "Wheel", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, SFML::Vector2f(128.0f, 128.0f), projectorPos, 0.0f, false, true);
		Wheel->GetShape()->setOutlineThickness(-1);
		Wheel->GetShape()->setOutlineColor(SFML::Color::Black);
		Wheel->GetShape()->setFillColor(SFML::Color(0, 255, 255, 40));
		Wheel->BindOnTick(WheelTick);
		WheelCache = Wheel;
		b2Actors.push_back(Wheel);	
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

	for (auto i : BallPool)
	{
		if (i) i->Tick();
	}
		

	// Update Pivot Position, Mouse Position
	// Dirty implementation, tick for-loop.
	const float pivotX = PivotCache->GetBodyInstance()->GetPosition().x *32.0f;
	const float pivotY = PivotCache->GetBodyInstance()->GetPosition().y *32.0f;
	SFML::Vector2f CurrentPivotLocation(pivotX, pivotY);
	
	SFML::Vector2f MouseLocation = SFML::Vector2f(SFML::Mouse::getPosition(AppWindow));
	SFML::Vector2f offsetMousePos = SFML::Vector2f(MouseLocation.x - 16.0f, MouseLocation.y - 16.0f);

	// Update Rotation Angle
	const float dx = MouseLocation.x - CurrentPivotLocation.x;
	const float dy = MouseLocation.y - CurrentPivotLocation.y;
	const float CurrentRotationAngle = (atan2(dy, dx)) * 180.0f / 3.142f;

	// Left Click to Start Game, Press for Charge Velocity, Release for Discharge Velocity
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Left))
	{
		bLeftMouseKeyDown = true;
		if (!bLeftMousePressed)
		{
			//if (!MGameData.getGameStartFlag())
			//	MGameData.setGameStartFlag(true);

			bLeftMousePressed = true;
		}

		//If the game already Started, Do something else.
		//if (MGameData.getGameStartFlag())
		//{
		//	MGameData.chargeVelocity(deltaTime);
		//}
	}
	else
	{
		//MGameData.dischargeVelocity(deltaTime);
		bLeftMouseKeyDown = false;
		bLeftMousePressed = false;
	}


	// Right Click to Spawn Ball.
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Right))
	{
		if (!bRightMousePressed)
		{
			//if (!MGameData.getGameOverFlag())
			{
			//	float velocity = MGameData.getChargeVelocity();
				float velocity = 20.0f;

				const SFML::Vector2f BallSpawnLocation(pivotX + 32.0f, pivotY - 32.0f);

				b2Actor2D* Ball = new b2Actor2D(this, World, "Ball", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, SFML::Vector2f(32, 32), BallSpawnLocation, 0.0f, true, false);
				Ball->GetShape()->setOutlineThickness(1);
				Ball->GetShape()->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BASKETBALL));
				Ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(-velocity * sinf(-CurrentRotationAngle * 3.142f / 180.0f), +velocity * cosf(-CurrentRotationAngle * 3.142f / 180.0f)));
				Ball->GetFixtureDefinition()->density = 0.83f;
				Ball->GetFixtureDefinition()->friction = 0.4f;
				Ball->GetFixtureDefinition()->restitution = 0.65f;
				//	int index = MGameData.getInactiveObjectIndex();
			//	MGameData.assignPoolObject(c, index);
				BallPool.push_back(Ball);
			}
			bRightMousePressed = true;
		}
	}
	else
	{
		bRightMousePressed = false;
	}
	


	// Middle Button ： Reset
	if (SFML::Mouse::isButtonPressed(SFML::Mouse::Middle))
	{
		if (!bMiddleMousePressed)
		{
			//MGameData.reset();
			//b2Vec2 pos(restartPos.x, restartPos.y);

			// Prevent accumulative cosine input.
			bMiddleMousePressed = true;
			TickHandle->ClearTimer();

			PivotCache->ResetLocation();
			WheelCache->ResetLocation();
		}
	}
	else
	{
		bMiddleMousePressed = false;
	}

	// Update Info Gauge
	float maxVelocity = 60.0f;
	float percentage = 0;// MGameData.getChargeVelocity() / maxVelocity;

	ChargeGaugeMax->setPosition(offsetMousePos);
	ChargeGaugeProgress->setPosition(offsetMousePos);
	ChargeGaugeProgress->setSize(SFML::Vector2f(160.0f * percentage, 8.0f));;
	

	// Update Angle Indicator
	AngleIndicators[0]->position = CurrentPivotLocation;
	AngleIndicators[1]->position = MouseLocation;

	std::cout << "X : " << AngleIndicators[1]->position.x << " Y : " << AngleIndicators[1]->position.y << std::endl;


	// Rendering
	AppWindow.clear(CORNFLOWER_BLUE);

	for (auto Itr : RenderShapes)
		AppWindow.draw(*Itr);
	
	for (auto Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());

	for (auto Itr : BallPool)
		AppWindow.draw(*Itr->GetShape());

	AppWindow.draw(*AngleIndicators, 2, SFML::Lines);
	
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

	for (auto i : BallPool)
		SAFE_DELETE(i);

	for (auto i : RenderShapes)
		SAFE_DELETE(i);

	for (int i = 0; i < 2; i++)
	{
		delete AngleIndicators[i];
	}

	SAFE_DELETE(World);
}

void Application::PivotTick(b2Actor2D* Actor)
{
	if (!Actor) return;

	const float ElapsedTime = Actor->GetPackage()->GetTickHandle()->GetElapsedTime();
	const float cosfTime = cosf(ElapsedTime);
	
	const float deltaY = 3.0f * cosf(ElapsedTime) / 32.0f;
	b2Vec2 pos = Actor->GetBodyInstance()->GetPosition();
	b2Vec2 Location = Actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
	Actor->GetBodyInstance()->SetTransform(Location, Actor->GetBodyInstance()->GetAngle());
}

void Application::WheelTick(b2Actor2D* Actor)
{
	if (!Actor) return;

	b2Vec2 PivotLocation = Actor->GetPackage()->PivotCache->GetBodyInstance()->GetPosition();
	Actor->GetBodyInstance()->SetTransform(PivotLocation, Actor->GetBodyInstance()->GetAngle());
}

b2Actor2D* Application::FindActor(std::string Label)
{
	for (b2Actor2D* i : b2Actors)
	{
		if(i->GetObjectName() == Label)
			return i;
	}
	return nullptr;
	
}
