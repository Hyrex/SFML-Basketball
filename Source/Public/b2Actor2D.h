#pragma once
#include <SFML\Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Defines.h"

static const float PIXEL_PER_METER = 32.0f;
class Application;

enum EActorShapeType
{
	EST_Circle,
	EST_Rectangle,
	EST_Convex,
};

enum Eb2ShapeType
{
	ECT_Polygon,
	ECT_Circle,
	ECT_Edge,
	ECT_Chain,
};

struct FShapeCollection
{
	std::shared_ptr<SFML::CircleShape> CircleShape;
	std::shared_ptr<SFML::RectangleShape> RectangleShape;
	std::shared_ptr<SFML::ConvexShape> ConvexShape;
	EActorShapeType ShapeType;

	std::shared_ptr<SFML::Shape> Get()
	{
		switch (ShapeType)
		{
			case EActorShapeType::EST_Rectangle: return RectangleShape;
			case EActorShapeType::EST_Circle: return CircleShape;
			case EActorShapeType::EST_Convex: return ConvexShape;	
		}
		return nullptr;
	}
};

struct Fb2ActorSpawnParam
{
	Application* Package;
	std::shared_ptr<b2World> WorldContext;
	std::string Name;
	EActorShapeType ShapeType;
	Eb2ShapeType BodyType;
	SFML::Vector2f Size; 
	SFML::Vector2f Location;
	float Rotation; 
	bool bIsDynamicBody; 
	bool bGenerateOverlaps;
	bool bAutoActivate;
};

class b2Actor2D
{
public:

	b2Actor2D(Application* Package, std::shared_ptr<b2World> WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size = SFML::Vector2f(1,1), SFML::Vector2f Location = SFML::Vector2f(0,0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false, const bool bAutoActivate = true);
	b2Actor2D(Fb2ActorSpawnParam SpawnParam);

	virtual void Tick();
	std::string GetObjectName() const { return ObjectName;  }
	std::shared_ptr<SFML::Shape>	GetShape()				{ return ObjectShapes.Get(); }
	std::shared_ptr<b2FixtureDef>	GetFixtureDefinition()	{ return FixtureDefinition; }
	
	b2Body*	GetBodyInstance() const { return BodyInstance; }
	Application* GetPackage() const { return Package; }
	bool IsDynamic() const { return bIsDynamicObject; }

	void SetInitLocation(b2Vec2 Location) { InitialPosition = Location; }
	void SetInitRotation(float Rotation) { InitialRotation = Rotation; }
	void ResetToInitTransform();

	void BeginOverlap(b2Actor2D* OverlappedActor);
	void EndOverlap(b2Actor2D* OverlappedActor);
	void BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnTick(void(*TickFunction)(b2Actor2D* Actor));

	void Activate();
	void MakeInactive();

	static b2Vec2 Tob2Vec2Location(SFML::Vector2f Location) { return b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER); };

private:
	std::string ObjectName;
	Application* Package;

	FShapeCollection ObjectShapes;	// Act like display component
	
	b2Body* BodyInstance;
	std::shared_ptr<SFML::Shape> ObjectShapeCache;	// Do not invoke delete, handled in ObjectShapes.
	std::shared_ptr<b2BodyDef>  BodyDefinition;
	std::shared_ptr<b2Shape> BodyShape;				// Act as collision component
	std::shared_ptr<b2FixtureDef> FixtureDefinition;

	Eb2ShapeType CollisionType;
	b2Vec2 InitialPosition;
	float InitialRotation;

	bool bIsActive = false;
	bool bGenerateOverlaps = false;
	bool bIsDynamicObject = false;
	void (*OnBeginOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void (*OnEndOverlapCallback)(b2Actor2D* OverlappedActor) = 0;
	void (*TickCallback)(b2Actor2D* Actor) = 0;

	void MakeShapeInstance(const EActorShapeType ShapeType);
	void SetShapeProperties(const EActorShapeType ShapeType, SFML::Vector2f Size);
	void MakeB2ShapeInstance(const Eb2ShapeType BodyType);
	void SetB2ShapeProperties(const Eb2ShapeType BodyType, SFML::Vector2f Size);
};