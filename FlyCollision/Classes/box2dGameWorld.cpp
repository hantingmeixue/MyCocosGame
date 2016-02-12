#include "box2dGameWorld.h"
#include "GLES-Render.h"
#include "shaders/ccGLStateCache.h"
#include "CCBox2dSprite.h"
#include <algorithm>

Box2dGameWorld::Box2dGameWorld(const b2Vec2 & gravity)
	:m_bDebugDraw(false)
{
	m_pWorld = new b2World(gravity);
	m_pWorld->SetAllowSleeping(true);
	m_pWorld->SetContinuousPhysics(true);
	//m_pWorld->SetAutoClearForces(false);

	m_pDebugDraw = new GLESDebugDraw(PTM_RATIO);
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;
	m_pDebugDraw->SetFlags(flags);

}

Box2dGameWorld::~Box2dGameWorld()
{
	delete m_pWorld;
	delete m_pDebugDraw;
}

void Box2dGameWorld::SetDebugDraw(bool bDraw)
{
	if (m_bDebugDraw == bDraw)
		return ;

	m_bDebugDraw = bDraw;
	if (bDraw)
	{
		m_pWorld->SetDebugDraw(m_pDebugDraw);
	}
	else
	{
		m_pWorld->SetDebugDraw(NULL);
	}
}

void Box2dGameWorld::SetDrawFlags(unsigned int floags)
{
	m_pDebugDraw->SetFlags(floags);
}

void Box2dGameWorld::BeginDraw()
{
	if (!m_bDebugDraw)
		return ;

	cocos2d::ccGLEnableVertexAttribs( cocos2d::kCCVertexAttribFlag_Position );

	kmGLPushMatrix();

	m_pWorld->DrawDebugData();

	kmGLPopMatrix();
}

b2Fixture * Box2dGameWorld::CreateEdgeBody(CCBox2dSprite * pSprite, const CCPoint & point, float fWidth)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);
	pSprite->setPTMRatio(PTM_RATIO);

	b2EdgeShape shape;
	shape.Set(b2Vec2(0.0f, 0.0f), b2Vec2(fWidth / PTM_RATIO, 0.0f));
	return pBody->CreateFixture(&shape, 0.0f);
}

//polygonShape body
b2Fixture * Box2dGameWorld::CreatePolygonBody(CCBox2dSprite * pSprite, const CCPoint & point, b2BodyType type)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	bd.type = type;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);
	pSprite->setPTMRatio(PTM_RATIO);

	b2PolygonShape shape;
	CCSize size = pSprite->getContentSize();
	shape.SetAsBox(size.width / 2 / PTM_RATIO, size.height / 2 / PTM_RATIO);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return pBody->CreateFixture(&fd);
}

b2Fixture * Box2dGameWorld::CreatePolygonBody(CCBox2dSprite * pSprite, const CCPoint & point, b2BodyType type, float fWidth, float fHeight)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	bd.type = type;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);
	pSprite->setPTMRatio(PTM_RATIO);

	b2PolygonShape shape;
	shape.SetAsBox(fWidth / 2 / PTM_RATIO, fHeight / 2 / PTM_RATIO);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return pBody->CreateFixture(&fd);
}

b2Fixture * Box2dGameWorld::CreatePolygonBody(CCBox2dSprite * pSprite, const CCPoint & point, b2BodyType type, const b2Vec2 * vertices, int nCount)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	bd.type = type;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);

	pSprite->setPTMRatio(PTM_RATIO);
	b2PolygonShape shape;
	b2Vec2 * tempVert = new b2Vec2[nCount];
	memcpy(tempVert, vertices, sizeof(b2Vec2) * nCount);
	TransferB2Vec2(tempVert, nCount);
	shape.Set(tempVert, nCount);

	delete [] tempVert;

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return pBody->CreateFixture(&fd);
}

//circleShape body
b2Fixture * Box2dGameWorld::CreateCircleBody(CCBox2dSprite * pSprite, const CCPoint & point, b2BodyType type)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	bd.type = type;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);
	pSprite->setPTMRatio(PTM_RATIO);

	b2CircleShape shape;
	CCSize size = pSprite->getContentSize();
	shape.m_radius = min(size.width, size.height) / 2 / PTM_RATIO;
	shape.m_p.SetZero();

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return pBody->CreateFixture(&fd);
}

b2Fixture * Box2dGameWorld::CreateCircleBody(CCBox2dSprite * pSprite, const CCPoint & point, b2BodyType type, const CCPoint & center, float fRadius)
{
	b2BodyDef bd;
	bd.position.Set(point.x / PTM_RATIO, point.y / PTM_RATIO);
	bd.userData = pSprite;
	bd.type = type;
	b2Body * pBody = m_pWorld->CreateBody(&bd);
	pSprite->setBody(pBody);
	pSprite->setPTMRatio(PTM_RATIO);

	b2CircleShape shape;
	shape.m_radius = fRadius / PTM_RATIO;
	shape.m_p.Set(center.x / PTM_RATIO, center.y / PTM_RATIO);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return pBody->CreateFixture(&fd);
}

b2Fixture * Box2dGameWorld::AddPolygonFixture(b2Body * body, const b2Vec2 * vertices, int nCount)
{
	b2Assert(body != NULL);

	b2PolygonShape shape;
	b2Vec2 * tempVert = new b2Vec2[nCount];
	memcpy(tempVert, vertices, sizeof(b2Vec2) * nCount);
	TransferB2Vec2(tempVert, nCount);
	shape.Set(tempVert, nCount);

	delete [] tempVert;

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return body->CreateFixture(&fd);
}

b2Fixture * Box2dGameWorld::AddCircleFixture(b2Body * body, const CCPoint & center, float fRadius)
{
	b2Assert(body != NULL);

	b2CircleShape shape;
	shape.m_p.Set(center.x / PTM_RATIO, center.y / PTM_RATIO);
	shape.m_radius = fRadius / PTM_RATIO;

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 20.0f;

	return body->CreateFixture(&fd);
}
