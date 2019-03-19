#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"
#include "TickHandle.h"
#include "AssetLoader.h"

Application::Application()
{
	TickHandle = std::make_shared<FTickHandle>();
	AssetLoader = std::make_shared<FAssetLoader>();
	b2ActorContactListner = std::make_shared<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(b2ActorContactListner.get());

	ChargeGaugeMax = std::make_shared<SFML::RectangleShape>();
	ChargeGaugeProgress = std::make_shared<SFML::RectangleShape>();
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
	// Reduce the code length, scope in this function only.
	using namespace sf;

	bool bInitChecks = true;
	if (TickHandle)
		bInitChecks &= TickHandle->BindApplication(this);

	if (AssetLoader)
		bInitChecks &= AssetLoader->LoadResources();

	if (bInitChecks)
	{
		if (BGM = FAssetLoader::FindMusic(AssetLoader, RESOURCES_AUDIO_TROLOLO))
		{
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
		const Vector2f XBorder(ViewportX, BorderThickness);
		const Vector2f YBorder(BorderThickness, ViewportY * 0.7f);
		const Vector2f UBorderLocation(ViewportX * 0.5f					, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(ViewportX * 0.5f					, ViewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2
		const Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2


		std::shared_ptr<b2Actor2D> TopBorder = std::make_shared<b2Actor2D>(this, World, "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		TopBorder->GetShape()->setOutlineThickness(-1);
		TopBorder->GetShape()->setOutlineColor(Color::Black);
		TopBorder->GetShape()->setFillColor(Color(100, 100, 100));
		b2Actors.push_back(TopBorder);
		
		std::shared_ptr<b2Actor2D> LeftBorder = std::make_shared<b2Actor2D>(this, World, "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		LeftBorder->GetShape()->setOutlineThickness(-1);
		LeftBorder->GetShape()->setOutlineColor(Color::Black);
		LeftBorder->GetShape()->setFillColor(Color(100, 100, 100));
		b2Actors.push_back(LeftBorder);

		std::shared_ptr<b2Actor2D> RightBorder = std::make_shared<b2Actor2D>(this, World, "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		RightBorder->GetShape()->setOutlineThickness(-1);
		RightBorder->GetShape()->setOutlineColor(Color::Black);
		RightBorder->GetShape()->setFillColor(Color(100, 100, 100));
		b2Actors.push_back(RightBorder);

#if 1 // debug floor!
		std::shared_ptr<b2Actor2D> BotBorder = std::make_shared<b2Actor2D>(this, World, "BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
		BotBorder->GetShape()->setOutlineThickness(-1);
		BotBorder->GetShape()->setOutlineColor(Color::Black);
		BotBorder->GetShape()->setFillColor(Color(100, 100, 100));
		b2Actors.push_back(BotBorder);
#endif 
		std::shared_ptr<RectangleShape> Background = std::make_shared<RectangleShape>(Vector2f(ViewportX, ViewportY));
		Background->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(Background);

		std::shared_ptr<RectangleShape> Scoreboard = std::make_shared<RectangleShape>(Vector2f(ViewportX, ViewportY * .3f));
		Scoreboard->setPosition(0.0f, ViewportY * 0.7f);
		Scoreboard->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(Scoreboard);

		ChargeGaugeMax->setFillColor(Color(145, 145, 145, 255));
		ChargeGaugeMax->setSize(Vector2f(160.0f, 8.0f));
		RenderShapes.push_back(ChargeGaugeMax);

		ChargeGaugeProgress->setFillColor(Color::Yellow);
		RenderShapes.push_back(ChargeGaugeProgress);

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i].color = (i == 1) ? Color::Cyan : Color::Blue;
		}
		
		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		Vector2f boardSize(8.0f, 200.0f);
		Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);
		Vector2f netEdgeSize(8.0f, 90.0f);
		Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		Vector2f sensorSize(48.0f, 48.0f);
		Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);

		std::shared_ptr<b2Actor2D> BoardFrame1 = std::make_shared<b2Actor2D>(this, World, "board1", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, boardSize, boardPos);
		BoardFrame1->GetShape()->setOutlineThickness(-1);
		BoardFrame1->GetShape()->setOutlineColor(Color::Black);
		BoardFrame1->GetShape()->setFillColor(Color(40, 40, 40, 255));
		b2Actors.push_back(BoardFrame1);

		std::shared_ptr<b2Actor2D> BoardFrame2 = std::make_shared<b2Actor2D>(this, World, "board2", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, netEdgeSize, netEdgePos);
		BoardFrame2->GetShape()->setOutlineThickness(-1);
		BoardFrame2->GetShape()->setOutlineColor(Color::Black);
		BoardFrame2->GetShape()->setFillColor(Color(40, 40, 40, 255));
		BoardFrame2->GetBodyInstance()->SetTransform(BoardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2Actors.push_back(BoardFrame2);

		std::shared_ptr<b2Actor2D> ScoreSensor = std::make_shared<b2Actor2D>(this, World, "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
		ScoreSensor->GetShape()->setOutlineThickness(-1);
		ScoreSensor->GetShape()->setOutlineColor(Color::Black);
		ScoreSensor->GetShape()->setFillColor(Color(255, 255, 0, 100));
		b2Actors.push_back(ScoreSensor);

		// The Track 
		const int Row = 14;
		const int Column = 2;

		const Vector2f trackStartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row*32.0f));
		const Vector2f slingPosSize(32.0f, 32.0f);
		std::shared_ptr<RectangleShape> Tracks[Row][Column];
		for (int i = 0; i < Row; ++i)
		{
			for (int j = 0; j < Column; ++j)
			{
				Tracks[i][j] = std::make_shared<RectangleShape>();

				Tracks[i][j]->setSize(slingPosSize);
				Tracks[i][j]->setPosition(trackStartLocation.x + j * 32.0f, trackStartLocation.y + i * 32.0f);
				Tracks[i][j]->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BOXALT));
				RenderShapes.push_back(Tracks[i][j]);
			}
		}

		// Projector Pivot
		const Vector2f projectorPos(trackStartLocation.x + 32.0f, trackStartLocation.y + (Row / 2)*32.0f);
		std::shared_ptr<b2Actor2D> const Pivot = std::make_shared<b2Actor2D>(this, World, "Pivot", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, Vector2f(8.0f, 8.0f), projectorPos, 0.0f, false, false);
		Pivot->GetShape()->setOutlineThickness(-1);
		Pivot->GetShape()->setOutlineColor(Color::Black);
		Pivot->GetShape()->setFillColor(Color(0, 0, 255, 100));
		Pivot->BindOnTick(PivotTick);
		PivotCache = Pivot;
		b2Actors.push_back(Pivot);

		std::shared_ptr<b2Actor2D> const Wheel = std::make_shared<b2Actor2D>(this, World, "Wheel", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, Vector2f(128.0f, 128.0f), projectorPos, 0.0f, false, true);
		Wheel->GetShape()->setOutlineThickness(-1);
		Wheel->GetShape()->setOutlineColor(Color::Black);
		Wheel->GetShape()->setFillColor(Color(0, 255, 255, 40));
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

	for (auto i : BallPools_ThatStilLWork)
	{
		if (i) i->Tick();
	}


	// Update Pivot Position, Mouse Position
	// Dirty implementation, tick for-loop.
	const float pivotX = PivotCache->GetBodyInstance()->GetPosition().x *32.0f;
	const float pivotY = PivotCache->GetBodyInstance()->GetPosition().y *32.0f;
	const SFML::Vector2f CurrentPivotLocation(pivotX, pivotY);
	
	const SFML::Vector2f MouseLocation = SFML::Vector2f(SFML::Mouse::getPosition(AppWindow));
	const SFML::Vector2f offsetMousePos = SFML::Vector2f(MouseLocation.x - 16.0f, MouseLocation.y - 16.0f);

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
	
				/// FIX ME CRASH HERE\

				Fb2ActorSpawnParam SpawnParam;
				SpawnParam.Package = this;
				SpawnParam.WorldContext = World;
				SpawnParam.Name = "Ball";
				SpawnParam.ShapeType = EActorShapeType::EST_Circle;
				SpawnParam.BodyType = Eb2ShapeType::ECT_Circle;
				SpawnParam.Size = SFML::Vector2f(32, 32);
				SpawnParam.Location = BallSpawnLocation;
				SpawnParam.Rotation = 0.0f;
				SpawnParam.bIsDynamicBody = true;
				SpawnParam.bGenerateOverlaps = false;
				SpawnParam.bAutoActivate = true;

				//std::shared_ptr<b2Actor2D> Ball = BallPool->Spawn();
				//std::shared_ptr<b2Actor2D> Ball = std::make_shared<b2Actor2D>(SpawnParam);
				
				std::shared_ptr<b2Actor2D> Ball = std::make_shared<b2Actor2D>(this, World, "Ball", EST_Circle, ECT_Circle, SFML::Vector2f(32, 32),
					BallSpawnLocation, 0.0f, true, false, true); 
				Ball->SetInitLocation(b2Actor2D::Tob2Vec2Location(BallSpawnLocation));
				Ball->SetInitRotation(0);
				Ball->ResetToInitTransform();
				Ball->GetShape()->setOutlineThickness(1);
				Ball->GetShape()->setTexture(FAssetLoader::FindTexture(AssetLoader, RESOURCES_TEXTURE_BASKETBALL));
				Ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(-velocity * sinf(-CurrentRotationAngle * 3.142f / 180.0f), +velocity * cosf(-CurrentRotationAngle * 3.142f / 180.0f)));
				Ball->GetFixtureDefinition()->density = 0.83f;
				Ball->GetFixtureDefinition()->friction = 0.4f;
				Ball->GetFixtureDefinition()->restitution = 0.65f;
				BallPools_ThatStilLWork.push_back(Ball);
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

			// Prevent accumulative cosine input.
			bMiddleMousePressed = true;
			TickHandle->ClearTimer();
			PivotCache->ResetToInitTransform();
			WheelCache->ResetToInitTransform();

			for (auto i : BallPools_ThatStilLWork)
			{
				i->MakeInactive();
			}
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
	AngleIndicators[0].position = CurrentPivotLocation;
	AngleIndicators[1].position = MouseLocation;

	// Rendering
	AppWindow.clear(CORNFLOWER_BLUE);

	for (auto Itr : RenderShapes)
		AppWindow.draw(*Itr);
	
	for (auto Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());

	//for (auto Itr : BallPool->Actors)
		//AppWindow.draw(*Itr->GetShape());

	for (auto Itr : BallPools_ThatStilLWork)
		AppWindow.draw(*Itr->GetShape());


	AppWindow.draw(AngleIndicators, 2, SFML::Lines);
	AppWindow.display();
}

void Application::EndPlay()
{
	if (TickHandle)
	{
		TickHandle->EndTick();
	}

	if (AssetLoader)
	{
		AssetLoader->UnloadResources();
	}
}

void Application::PivotTick(b2Actor2D* Actor)
{
	if (!Actor) return;

	const float ElapsedTime = Actor->GetPackage()->GetTickHandle()->GetElapsedTime();
	const float cosfTime = cosf(ElapsedTime);
	
	const float deltaY = 3.0f * cosf(ElapsedTime) / 32.0f;
	b2Vec2 Location = Actor->GetBodyInstance()->GetPosition() + b2Vec2(0, deltaY);
	Actor->GetBodyInstance()->SetTransform(Location, Actor->GetBodyInstance()->GetAngle());
}

void Application::WheelTick(b2Actor2D* Actor)
{
	if (!Actor) return;

	b2Vec2 PivotLocation = Actor->GetPackage()->PivotCache->GetBodyInstance()->GetPosition();
	Actor->GetBodyInstance()->SetTransform(PivotLocation, Actor->GetBodyInstance()->GetAngle());
}
