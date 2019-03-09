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
	SFML::CircleShape* CircleShape;
	SFML::RectangleShape* RectangleShape;
	SFML::ConvexShape* ConvexShape;
	EActorShapeType ShapeType;
	
	FShapeCollection()
	{
		CircleShape = nullptr;
		RectangleShape = nullptr;
		ConvexShape = nullptr;
	}

	~FShapeCollection() 
	{	
		SAFE_DELETE(CircleShape); 
		SAFE_DELETE(RectangleShape); 
		SAFE_DELETE(ConvexShape);
		LOG("ObjectInstance shape collection destroyed\n");
	}

	SFML::Shape* Get()
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

class b2Actor2D
{
public:

	b2Actor2D(Application* Package, b2World* WorldContext, const std::string Name, const EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size = SFML::Vector2f(1,1), SFML::Vector2f Location = SFML::Vector2f(0,0), const float Rotation = 0.0f, const bool bIsDynamicBody = false, const bool bGenerateOverlaps = false);
	~b2Actor2D();

	virtual void Tick();
	std::string GetObjectName() const { return ObjectName;  }
	SFML::Shape* GetShape() { return ObjectShapes.Get(); }
	b2FixtureDef* GetFixtureDefinition() { return FixtureDefinition; }
	b2Body*	GetBodyInstance() { return BodyInstance; }
	Application* GetPackage() { return Package; }
	bool IsDynamic() const { return bIsDynamicObject; }

	void SetInitLocation(b2Vec2 Location) { InitialPosition = Location; }
	void ResetLocation();

	void BeginOverlap(b2Actor2D* OverlappedActor);
	void EndOverlap(b2Actor2D* OverlappedActor);
	void BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor));
	void BindOnTick(void(*TickFunction)(b2Actor2D* Actor));

private:
	Application* Package;
	std::string ObjectName;
	FShapeCollection ObjectShapes;	// Act like display component
	SFML::Shape* ObjectShapeCache;	// Do not invoke delete, handled in ObjectShapes.

	b2Body* BodyInstance;
	b2BodyDef* BodyDefinition;
	b2Shape* BodyShape;				// Act as collision component
	Eb2ShapeType CollisionType;
	b2FixtureDef* FixtureDefinition;
	b2Vec2 InitialPosition;
	float InitialRotation;

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

