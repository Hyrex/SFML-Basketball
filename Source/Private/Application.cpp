#include <SFML/Audio.hpp>

#include "Application.h"
#include "b2Actor2D.h"
#include "b2Actor2DContactListener.h"


Application::Application()
{
	b2ActorContactListner = std::unique_ptr<b2Actor2DContactListener>();

	Gravity = b2Vec2(0.f, 9.81f);
	World = std::make_shared<b2World>(Gravity);
	World->SetContactListener(b2ActorContactListner.get());
}

Application::~Application()
{
}

void Application::BeginPlay()
{
	while (AppWindow.isOpen())
	{
		TickHandle.BeginTick();
	}

	EndPlay();
}

int Application::Initialize()
{
	// Reduce the code length, scope in this function only.
	using namespace sf;
	using namespace std;

	bool bInitChecks = true;
	bInitChecks &= TickHandle.BindApplication(this);
	bInitChecks &= AssetLoader.LoadResources();

	if (bInitChecks)
	{
		if (BGM = FAssetLoader::FindMusic(&AssetLoader, RESOURCES_AUDIO_TROLOLO))
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
		const Vector2f UBorderLocation(ViewportX * 0.5f						, BorderThickness * 0.5f);
		const Vector2f DBorderLocation(ViewportX * 0.5f						, ViewportY - BorderThickness * 0.5f);
		const Vector2f LBorderLocation(BorderThickness * 0.5f				, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2
		const Vector2f RBorderLocation(ViewportX - BorderThickness * 0.5f	, ViewportY * 0.5f - (ViewportY * .15f) ); // 1 - .7f div 2

		// Collapsed function body. Transfering ownership of local unique ptr to the container
		auto b2ActorInit = [this](unique_ptr<b2Actor2D>& p, const Color c) ->void 
		{
			p->GetShape()->setOutlineThickness(-1);
			p->GetShape()->setOutlineColor(Color::Black);
			p->GetShape()->setFillColor(c);
			b2Actors.push_back(move(p));
		};

		unique_ptr<b2Actor2D> TopBorder = make_unique<b2Actor2D>(this, World.get(), "TopBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, UBorderLocation);
		b2ActorInit(TopBorder, Color(100, 100, 100));

		unique_ptr<b2Actor2D> LeftBorder = make_unique<b2Actor2D>(this, World.get(), "LeftBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, LBorderLocation);
		b2ActorInit(LeftBorder , Color(100, 100, 100) );

		unique_ptr<b2Actor2D> RightBorder = make_unique<b2Actor2D>(this, World.get(), "RightBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, YBorder, RBorderLocation);
		b2ActorInit(RightBorder,Color(100, 100, 100));

#if 1 // debug floor!
		unique_ptr<b2Actor2D> BotBorder = make_unique<b2Actor2D>(this, World.get(), "BotBorder", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, XBorder, DBorderLocation);
		b2ActorInit(BotBorder, Color(100, 100, 100));
#endif 
		unique_ptr<RectangleShape> Background = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY));
		Background->setTexture(FAssetLoader::FindTexture(&AssetLoader, RESOURCES_TEXTURE_BACKGROUND));
		RenderShapes.push_back(move(Background));

		unique_ptr<RectangleShape> Scoreboard = make_unique<RectangleShape>(Vector2f(ViewportX, ViewportY * .3f));
		Scoreboard->setPosition(0.0f, ViewportY * 0.7f);
		Scoreboard->setTexture(FAssetLoader::FindTexture(&AssetLoader, RESOURCES_TEXTURE_CHALKBOARD));
		RenderShapes.push_back(move(Scoreboard));

		unique_ptr<RectangleShape> ChargeGaugeMaxUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeMaxUniquePtr->setFillColor(Color(145, 145, 145, 255));
		ChargeGaugeMaxUniquePtr->setSize(Vector2f(160.0f, 8.0f));
		ChargeGaugeMax = ChargeGaugeMaxUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(move(ChargeGaugeMaxUniquePtr));

		unique_ptr<RectangleShape> ChargeGaugeProgressUniquePtr = make_unique<RectangleShape>();
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		ChargeGaugeProgressUniquePtr->setFillColor(Color::Yellow);
		ChargeGaugeProgress = ChargeGaugeProgressUniquePtr.get(); // Fill up the cache pointer, but it is rely on its last moved position!
		RenderShapes.push_back(move(ChargeGaugeProgressUniquePtr));

		for (int i = 0; i < 2; i++)
		{
			AngleIndicators[i].color = (i == 1) ? Color::Cyan : Color::Blue;
		}
		
		// Board
		const float offsetX = ViewportX * 0.98f;
		const float offsetY = ViewportY * 0.35f;
		const Vector2f boardSize(8.0f, 200.0f);
		const Vector2f boardPos(ViewportX * 0.98f, ViewportY * 0.35f);

		unique_ptr<b2Actor2D> BoardFrame1 = make_unique<b2Actor2D>(this, World.get(), "board1", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, boardSize, boardPos);
		b2ActorInit(BoardFrame1, Color(40, 40, 40, 255));

		const Vector2f netEdgeSize(8.0f, 90.0f);
		const Vector2f netEdgePos(offsetX - 48.0f + (netEdgeSize.y / 2 * sin(-0.174533f)), offsetY + 16.0f);
		
		unique_ptr<b2Actor2D> BoardFrame2 = make_unique<b2Actor2D>(this, World.get(), "board2", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, netEdgeSize, netEdgePos);
		BoardFrame2->GetBodyInstance()->SetTransform(BoardFrame2->GetBodyInstance()->GetPosition(), -0.261799388f);
		b2ActorInit(BoardFrame2, Color(40, 40, 40, 255));
	
		const Vector2f sensorSize(48.0f, 48.0f);
		const Vector2f sensorPos((boardPos.x + netEdgePos.x) / 2, netEdgePos.y);
		
		unique_ptr<b2Actor2D> ScoreSensor = make_unique<b2Actor2D>(this, World.get(), "sensor", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, sensorSize, sensorPos, 0.0f, false, true);
		b2ActorInit(ScoreSensor, Color(255, 255, 0, 100));

		MakeTrack();
		MakeProjector();
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

	for (auto& i : b2Actors)
	{
		if (i) i->Tick();
	}

	//for (auto& i : BallPools_ThatStilLWork)
	{
		//if (i) i->Tick();
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

				Fb2ActorSpawnParam SpawnParam;
				SpawnParam.Package = this;
				SpawnParam.WorldContext = World.get();
				SpawnParam.Name = "Ball";
				SpawnParam.ShapeType = EActorShapeType::EST_Circle;
				SpawnParam.BodyType = Eb2ShapeType::ECT_Circle;
				SpawnParam.Size = SFML::Vector2f(32, 32);
				SpawnParam.Location = BallSpawnLocation;
				SpawnParam.Rotation = 0.0f;
				SpawnParam.bIsDynamicBody = true;
				SpawnParam.bGenerateOverlaps = false;
				SpawnParam.bAutoActivate = true;

#if 1
				std::unique_ptr<b2Actor2D> Ball = std::make_unique<b2Actor2D>(SpawnParam);
#elif
				std::unique_ptr<b2Actor2D> Ball = 
					std::make_unique<b2Actor2D>(this, World.get(), "Ball", EST_Circle, ECT_Circle, SFML::Vector2f(32, 32),
					BallSpawnLocation, 0.0f, true, false, true); 
#endif
				Ball->SetInitLocation(BallSpawnLocation);
				Ball->SetInitRotation(0.0f);
				Ball->ResetToInitTransform();
				Ball->GetShape()->setTexture(FAssetLoader::FindTexture(&AssetLoader, RESOURCES_TEXTURE_BASKETBALL));
				//body instance crash.
				Ball->GetBodyInstance()->SetLinearVelocity(b2Vec2(-velocity * sinf(-CurrentRotationAngle * 3.142f / 180.0f), +velocity * cosf(-CurrentRotationAngle * 3.142f / 180.0f)));
				Ball->GetFixtureDefinition()->density = 0.83f;
				Ball->GetFixtureDefinition()->friction = 0.4f;
				Ball->GetFixtureDefinition()->restitution = 0.65f;
				b2Actors.push_back(std::move(Ball));
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
			TickHandle.ClearTimer();
			PivotCache->ResetToInitTransform();
			WheelCache->ResetToInitTransform();

			for (auto& i : BallPools_ThatStilLWork)
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

	for (auto& Itr : RenderShapes)
		AppWindow.draw(*Itr);
	
	for (auto& Itr : b2Actors)
		AppWindow.draw(*Itr->GetShape());

	//for (auto Itr : BallPool->Actors)
		//AppWindow.draw(*Itr->GetShape());

	for (auto& Itr : BallPools_ThatStilLWork)
		AppWindow.draw(*Itr->GetShape());


	AppWindow.draw(AngleIndicators, 2, SFML::Lines);
	AppWindow.display();
}

void Application::EndPlay()
{
	TickHandle.EndTick();
	AssetLoader.UnloadResources();
}

void Application::MakeTrack()
{
	using namespace std;
	using namespace sf;

	// The Track 
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	const int Row = 14;
	const int Column = 2;

	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row*32.0f));
	const Vector2f Size(32.0f, 32.0f);

	unique_ptr<RectangleShape> Tracks[Row][Column];
	for (int i = 0; i < Row; ++i)
	{
		for (int j = 0; j < Column; ++j)
		{
			Tracks[i][j] = make_unique<RectangleShape>();

			Tracks[i][j]->setSize(Size);
			Tracks[i][j]->setPosition(StartLocation.x + j * 32.0f, StartLocation.y + i * 32.0f);
			Tracks[i][j]->setTexture(FAssetLoader::FindTexture(&AssetLoader, RESOURCES_TEXTURE_BOXALT));
			RenderShapes.push_back(move(Tracks[i][j]));
		}
	}
}

void Application::MakeProjector()
{
	using namespace std;
	using namespace sf;

	// Collapsed function body. Transfering ownership of local unique ptr to the container
	auto Setup = [this](unique_ptr<b2Actor2D>& p, const Color c) ->void
	{
		p->GetShape()->setOutlineThickness(-1);
		p->GetShape()->setOutlineColor(Color::Black);
		p->GetShape()->setFillColor(c);
		b2Actors.push_back(move(p));
	};

	// Projector Pivot
	const int Row = 14;
	const float ViewportX = (float)RenderWindowData.Width;
	const float ViewportY = (float)RenderWindowData.Height;
	const Vector2f StartLocation(ViewportX * 0.15f, ViewportY - 16.0f - (Row * 32.0f));
	const Vector2f Location(StartLocation.x + 32.0f, StartLocation.y + (Row / 2)*32.0f);
	unique_ptr<b2Actor2D> Pivot = make_unique<b2Actor2D>(this, World.get(), "Pivot", EActorShapeType::EST_Rectangle, Eb2ShapeType::ECT_Polygon, Vector2f(8.0f, 8.0f), Location, 0.0f, false, false);
	Pivot->BindOnTick(PivotTick);
	PivotCache = Pivot.get();
	Setup(Pivot, Color(0, 0, 255, 100));

	unique_ptr<b2Actor2D> Wheel = make_unique<b2Actor2D>(this, World.get(), "Wheel", EActorShapeType::EST_Circle, Eb2ShapeType::ECT_Circle, Vector2f(128.0f, 128.0f), Location, 0.0f, false, true);
	Wheel->BindOnTick(WheelTick);
	WheelCache = Wheel.get();
	Setup(Wheel, Color(0, 255, 255, 40));
}

void Application::PivotTick(b2Actor2D* Actor)
{
	if (!Actor) return;

	const float ElapsedTime = Actor->GetPackage()->GetTickHandle().GetElapsedTime();
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
