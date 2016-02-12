#ifndef _BOX2D_GAME_WORLD_H_
#define _BOX2D_GAME_WORLD_H_

#include <list>
#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "CCBox2dSprite.h"

#define PTM_RATIO 32

class GLESDebugDraw;
class cocos2d::CCPoint;

class Box2dGameWorld
{
public:
	Box2dGameWorld(const b2Vec2 & gravity);
	~Box2dGameWorld();

	inline b2World * GetWorld();

	//Draw debug world
	void SetDebugDraw(bool bDraw);
	void SetDrawFlags(unsigned int floags);
	void BeginDraw();

	//go the physics step
	inline void Step();
	inline void Step(float fTimeStep, int velocityIterations, int positionIterations);


	//create body functions
	// edageShape body
	b2Fixture * CreateEdgeBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, float fWidth);
	b2Fixture * CreateEdgeBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, const b2Vec2 * vertices, int nCount)
	{}
	//polygonShape body
	b2Fixture * CreatePolygonBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, b2BodyType type);
	b2Fixture * CreatePolygonBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, b2BodyType type, float fWidth, float fHeight);
	b2Fixture * CreatePolygonBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, b2BodyType type, const b2Vec2 * vertices, int nCount);

	//circleShape body
	b2Fixture * CreateCircleBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, b2BodyType type);
	b2Fixture * CreateCircleBody(CCBox2dSprite * pSprite, const cocos2d::CCPoint & point, b2BodyType type, const cocos2d::CCPoint & center, float fRadius);

	//add fixture with body
	b2Fixture * AddPolygonFixture(b2Body * body, const b2Vec2 * vertices, int nCount);
	b2Fixture * AddCircleFixture(b2Body * body, const CCPoint & center, float fRadius);

	inline void TransferB2Vec2(b2Vec2 * vertices, int nCount);

	//change boy

private:
	//prevent copy
	Box2dGameWorld(const Box2dGameWorld &);
	Box2dGameWorld & operator = (const Box2dGameWorld &);

private:
	b2World * m_pWorld;
	bool m_bDebugDraw;
	GLESDebugDraw * m_pDebugDraw;

};

b2World * Box2dGameWorld::GetWorld()
{
	return m_pWorld;
}

void Box2dGameWorld::Step()
{
	Step(1.0f / 60.0f, 10, 8);
}

void Box2dGameWorld::Step(float fTimeStep, int velocityIterations, int positionIterations)
{
	m_pWorld->Step(fTimeStep, velocityIterations, positionIterations);
}

void Box2dGameWorld::TransferB2Vec2(b2Vec2 * vertices, int nCount)
{
	for (int i = 0; i < nCount; ++i)
	{
		vertices[i].x /= PTM_RATIO;
		vertices[i].y /= PTM_RATIO;
	}
}

#endif	//_BOX2D_GAME_WORLD_H_
