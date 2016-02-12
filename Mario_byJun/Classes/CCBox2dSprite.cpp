#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

Box2dSprite::Box2dSprite(b2World * pWorld)
	:m_pWorld(pWorld)
	,m_bIgnoreBodyRotation(false)
	,m_pBody(nullptr)
	,m_fPTMRatio(0.0f)
	//,m_bGravity(true)
{

}
Box2dSprite::~Box2dSprite()
{
	m_pWorld->DestroyBody(m_pBody);
}

Box2dSprite* Box2dSprite::create(b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

Box2dSprite* Box2dSprite::createWithTexture(Texture2D *pTexture, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithTexture(pTexture))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

Box2dSprite* Box2dSprite::createWithTexture(Texture2D *pTexture, const Rect& rect, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithTexture(pTexture, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

Box2dSprite* Box2dSprite::createWithSpriteFrame(SpriteFrame *pSpriteFrame, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithSpriteFrame(pSpriteFrame))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

Box2dSprite* Box2dSprite::createWithSpriteFrameName(const char *pszSpriteFrameName, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

Box2dSprite* Box2dSprite::create(const char *pszFileName, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

Box2dSprite* Box2dSprite::create(const char *pszFileName, const Rect& rect, b2World * pWorld)
{
    Box2dSprite* pRet = new Box2dSprite(pWorld);
    if (pRet && pRet->initWithFile(pszFileName, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

// this method will only get called if the sprite is batched.
// return YES if the physic's values (angles, position ) changed.
// If you return NO, then nodeToParentTransform won't be called.
bool Box2dSprite::isDirty()
{
    return true;
}

bool Box2dSprite::isIgnoreBodyRotation() const
{
    return m_bIgnoreBodyRotation;
}

void Box2dSprite::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    m_bIgnoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const Vec2& Box2dSprite::getPosition() const
{
	return getPosFromPhysics();
}

Vec3 Box2dSprite::getPosition3D() const
{
	Vec2 pos = getPosition();

	return Vec3(pos.x, pos.y, getPositionZ());
}

void Box2dSprite::getPosition(float* x, float* y)
{
	if (x == nullptr || y == nullptr) {
		return;
	}
	const Vec2& pos = getPosFromPhysics();
	*x = pos.x;
	*y = pos.y;
}

float Box2dSprite::getPositionX()
{
	return getPosFromPhysics().x;
}

float Box2dSprite::getPositionY()
{
	return getPosFromPhysics().y;
}

// Override the setters and getters to always reflect the body's properties.
const Vec2& Box2dSprite::getPosFromPhysics() const
{
	static Vec2 m_physicPosion;
	b2Vec2 pos = m_pBody->GetPosition();
	float x = pos.x * m_fPTMRatio;
	float y = pos.y * m_fPTMRatio;
	m_physicPosion.set(x, y);

	return m_physicPosion;
}

void Box2dSprite::setPosition(const Vec2 &pos)
{
    float angle = m_pBody->GetAngle();
	//Vec2 point = getParent()->convertToWorldSpace(pos);
	m_pBody->SetTransform(b2Vec2(pos.x / m_fPTMRatio, pos.y / m_fPTMRatio), angle);
}

void Box2dSprite::setPosition3D(const Vec3 &position)
{
	setPosition(Vec2(position.x, position.y));
	setPositionZ(position.z);
}

float Box2dSprite::getRotation()
{
    return (m_bIgnoreBodyRotation ? Sprite::getRotation() :
            CC_RADIANS_TO_DEGREES(m_pBody->GetAngle()));
}

void Box2dSprite::setRotation(float fRotation)
{
    if (m_bIgnoreBodyRotation)
    {
        Sprite::setRotation(fRotation);
    }
    else
    {
        b2Vec2 p = m_pBody->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(fRotation);
        m_pBody->SetTransform(p, radians);
    }
}

// returns the transform matrix according the Box2D Body values
void Box2dSprite::syncPhysicsTransform() const
{
    b2Vec2 pos  = m_pBody->GetPosition();

	float x = pos.x * m_fPTMRatio;
	float y = pos.y * m_fPTMRatio;

	if (_ignoreAnchorPointForPosition)
	{
		x += _anchorPointInPoints.x;
		y += _anchorPointInPoints.y;
	}
	
	// Make matrix
	float radians = m_pBody->GetAngle();
	float c = cosf(radians);
	float s = sinf(radians);
	
	if (!_anchorPointInPoints.isZero())
	{
		x += ((c * -_anchorPointInPoints.x * _scaleX) + (-s * -_anchorPointInPoints.y * _scaleY));
		y += ((s * -_anchorPointInPoints.x * _scaleX) + (c * -_anchorPointInPoints.y * _scaleY));
	}
    
	// Rot, Translate Matrix
	float mat[] = { (float)c * _scaleX, (float)s * _scaleX, 0, 0,
		(float)-s * _scaleY, (float)c * _scaleY, 0, 0,
		0, 0, 1, 0,
		x, y, 0, 1 };

	_transform.set(mat);
}

void Box2dSprite::onEnter()
{
	Node::onEnter();
	_syncTransform = Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_AFTER_UPDATE,
		std::bind(&Box2dSprite::afterUpdate, this, std::placeholders::_1));
	_syncTransform->retain();
}

void Box2dSprite::onExit()
{
	if (_syncTransform != nullptr)
	{
		Director::getInstance()->getEventDispatcher()->removeEventListener(_syncTransform);
		_syncTransform->release();
	}
	Node::onExit();
}

void Box2dSprite::afterUpdate(EventCustom *event)
{
	syncPhysicsTransform();

	//b2Vec2 pos = m_pBody->GetPosition();
	//m_physicPosion.set(pos.x * m_fPTMRatio, pos.y * m_fPTMRatio);

	_transformDirty = false;
	_transformUpdated = true;
	setDirtyRecursively(true);
}
