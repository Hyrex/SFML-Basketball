#include "b2Actor2D.h"
#include "Application.h"

b2Actor2D::b2Actor2D(Application* Package, b2World* WorldContext, const std::string Name, EActorShapeType ShapeType, const Eb2ShapeType BodyType, SFML::Vector2f Size, SFML::Vector2f Location, const float Rotation, const bool bIsDynamicBody, const bool bGenerateOverlaps)
{
	if (!Package) return;
	if (!WorldContext) return;

	this->Package = Package;

	ObjectName = Name;
	ObjectShapes.ShapeType = ShapeType;
	CollisionType = BodyType;
	
	MakeShapeInstance(ShapeType);
	ObjectShapeCache = ObjectShapes.Get();
	SetShapeProperties(ShapeType,Size);
	if (ObjectShapes.Get())
	{
		ObjectShapes.Get()->setOrigin(Size * 0.5f);
		ObjectShapes.Get()->setFillColor(SFML::Color(128, 128, 128, 255));
		ObjectShapes.Get()->setOutlineThickness(1);
		ObjectShapes.Get()->setOutlineColor(sf::Color::Black);
	}

	BodyDefinition = new b2BodyDef();
	BodyDefinition->position = b2Vec2(Location.x / PIXEL_PER_METER, Location.y / PIXEL_PER_METER);
	BodyDefinition->type = bIsDynamicBody ? b2_dynamicBody : b2_staticBody;

	MakeB2ShapeInstance(BodyType);
	SetB2ShapeProperties(BodyType, Size);

	if(BodyShape)
	{
		FixtureDefinition = new b2FixtureDef();
		FixtureDefinition->shape = BodyShape;
		FixtureDefinition->density = 0.5f;
		FixtureDefinition->friction = 0.5f;
		FixtureDefinition->restitution = 0.5f;
		FixtureDefinition->isSensor = bGenerateOverlaps; // change it to MakeSensor and get data.
	}
	
	BodyInstance = WorldContext->CreateBody(BodyDefinition);
	BodyInstance->CreateFixture(FixtureDefinition);
	BodyInstance->SetUserData(this);

	this->bGenerateOverlaps = bGenerateOverlaps;
	bIsDynamicObject = bIsDynamicBody;
}

b2Actor2D::~b2Actor2D()
{
	SAFE_DELETE(BodyDefinition);
	SAFE_DELETE(FixtureDefinition);
	SAFE_DELETE(BodyShape);
}

void b2Actor2D::Tick()
{
	// Box2D uses radians for rotation, SFML uses degree
	// Snap rendering according to Box2D BodyInstance.
	ObjectShapes.Get()->setRotation(BodyInstance->GetAngle() * 180 / b2_pi);
	ObjectShapes.Get()->setPosition(BodyInstance->GetPosition().x*PIXEL_PER_METER, BodyInstance->GetPosition().y*PIXEL_PER_METER);

	if (TickCallback != 0)
	{
		TickCallback(this);
	}
}

void b2Actor2D::BeginOverlap(b2Actor2D* OverlappedActor)
{
	if (bGenerateOverlaps && OverlappedActor)
	{
		if (OnBeginOverlapCallback)
		{
			OnBeginOverlapCallback(OverlappedActor);
		}
	}
}

void b2Actor2D::EndOverlap(b2Actor2D* OverlappedActor)
{
	if (bGenerateOverlaps && OverlappedActor)
	{
		if (OnEndOverlapCallback)
		{
			OnEndOverlapCallback(OverlappedActor);
		}

	}
}

void b2Actor2D::BindOnBeginoverlap(void (*Callback)(b2Actor2D* OverlappedActor))
{
	OnBeginOverlapCallback = Callback;
}

void b2Actor2D::BindOnEndOverlap(void (*Callback)(b2Actor2D* OverlappedActor))
{
	OnEndOverlapCallback = Callback;
}

void b2Actor2D::BindOnTick(void(*TickFunction)(b2Actor2D* Actor))
{
	TickCallback = TickFunction;
}

void b2Actor2D::MakeShapeInstance(const EActorShapeType ShapeType)
{
	
	switch (ShapeType)
	{
		case EActorShapeType::EST_Rectangle:	ObjectShapes.RectangleShape = new SFML::RectangleShape(); 	break;
		case EActorShapeType::EST_Circle:		ObjectShapes.CircleShape = new SFML::CircleShape();			break;
		case EActorShapeType::EST_Convex:		ObjectShapes.ConvexShape = new SFML::ConvexShape();			break;
	}
}

void b2Actor2D::SetShapeProperties(const EActorShapeType ShapeType, SFML::Vector2f Size)
{
	if (!ObjectShapeCache) ObjectShapeCache = ObjectShapes.Get();
	switch (ShapeType)
	{
		case EActorShapeType::EST_Circle:
			if (SFML::CircleShape* pShape = dynamic_cast<SFML::CircleShape*>(ObjectShapeCache))
			{
				pShape->setRadius(Size.x);
			}

			break;
		case EActorShapeType::EST_Rectangle:
			if (SFML::RectangleShape* pShape = dynamic_cast<SFML::RectangleShape*>(ObjectShapeCache))
			{
				pShape->setSize(Size);
			}

			break;
	
	}
}

void b2Actor2D::MakeB2ShapeInstance(const Eb2ShapeType BodyType)
{
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		BodyShape = new b2ChainShape(); break;
		case Eb2ShapeType::ECT_Edge:		BodyShape = new b2EdgeShape(); break;
		case Eb2ShapeType::ECT_Polygon:		BodyShape = new b2PolygonShape(); break;
		case Eb2ShapeType::ECT_Circle:		BodyShape = new b2CircleShape(); break;
	}
}

void b2Actor2D::SetB2ShapeProperties(const Eb2ShapeType BodyType, SFML::Vector2f Size)
{
	if (!BodyShape) return;
	switch (BodyType)
	{
		case Eb2ShapeType::ECT_Chain:		
		case Eb2ShapeType::ECT_Edge:		
			//Not supported yet.
			break;
		case Eb2ShapeType::ECT_Polygon:	
		{
			if(b2PolygonShape* pShape = dynamic_cast<b2PolygonShape*>(BodyShape))
			{
				pShape->SetAsBox((Size.x * 0.5f) / PIXEL_PER_METER, (Size.y * 0.5f) / PIXEL_PER_METER);
			}
			
			break;
		}
			
		case Eb2ShapeType::ECT_Circle:		
		{
			if (b2CircleShape* pShape = dynamic_cast<b2CircleShape*>(BodyShape))
			{
				pShape->m_radius = (Size.x * 0.5f) / PIXEL_PER_METER;
			}

			break;
		}
	}
}
