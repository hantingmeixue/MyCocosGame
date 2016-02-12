#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

CCBox2dSprite::CCBox2dSprite(b2World * pWorld)
	:m_pWorld(pWorld)
	,m_bIgnoreBodyRotation(false)
	,m_pBody(NULL)
	,m_fPTMRatio(0.0f)
	//,m_bGravity(true)
{

}
CCBox2dSprite::~CCBox2dSprite()
{
	m_pWorld->DestroyBody(m_pBody);
}

CCBox2dSprite* CCBox2dSprite::create(b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::createWithTexture(CCTexture2D *pTexture, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::createWithTexture(CCTexture2D *pTexture, const CCRect& rect, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::createWithSpriteFrameName(const char *pszSpriteFrameName, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::create(const char *pszFileName, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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

CCBox2dSprite* CCBox2dSprite::create(const char *pszFileName, const CCRect& rect, b2World * pWorld)
{
    CCBox2dSprite* pRet = new CCBox2dSprite(pWorld);
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
bool CCBox2dSprite::isDirty()
{
    return true;
}

bool CCBox2dSprite::isIgnoreBodyRotation() const
{
    return m_bIgnoreBodyRotation;
}

void CCBox2dSprite::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    m_bIgnoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const CCPoint& CCBox2dSprite::getPosition()
{
    updatePosFromPhysics();
	return CCNode::getPosition();
}

void CCBox2dSprite::getPosition(float* x, float* y)
{
    updatePosFromPhysics();
	return CCNode::getPosition(x, y);
}

float CCBox2dSprite::getPositionX()
{
    updatePosFromPhysics();
    return m_obPosition.x;
}

float CCBox2dSprite::getPositionY()
{
    updatePosFromPhysics();
    return m_obPosition.y;
}

b2Body* CCBox2dSprite::getBody() const
{
    return m_pBody;
}

void CCBox2dSprite::setBody(b2Body *pBody)
{
    m_pBody = pBody;
}

float CCBox2dSprite::getPTMRatio() const
{
    return m_fPTMRatio;
}

void CCBox2dSprite::setPTMRatio(float fRatio)
{
    m_fPTMRatio = fRatio;
}

// Override the setters and getters to always reflect the body's properties.
void CCBox2dSprite::updatePosFromPhysics()
{
    b2Vec2 pos = m_pBody->GetPosition();
    float x = pos.x * m_fPTMRatio;
    float y = pos.y * m_fPTMRatio;

	if (getParent())
		m_obPosition = getParent()->convertToNodeSpace(ccp(x, y));
	else
		m_obPosition = ccp(x, y);
}

void CCBox2dSprite::setPosition(const CCPoint &pos)
{
    float angle = m_pBody->GetAngle();
	CCPoint point = getParent()->convertToWorldSpace(pos);
    m_pBody->SetTransform(b2Vec2(point.x / m_fPTMRatio, point.y / m_fPTMRatio), angle);
}

float CCBox2dSprite::getRotation()
{
    return (m_bIgnoreBodyRotation ? CCSprite::getRotation() :
            CC_RADIANS_TO_DEGREES(m_pBody->GetAngle()));
}

void CCBox2dSprite::setRotation(float fRotation)
{
    if (m_bIgnoreBodyRotation)
    {
        CCSprite::setRotation(fRotation);
    }
    else
    {
        b2Vec2 p = m_pBody->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(fRotation);
        m_pBody->SetTransform(p, radians);
    }
}

// returns the transform matrix according the Box2D Body values
CCAffineTransform CCBox2dSprite::nodeToParentTransform()
{
    b2Vec2 pos  = m_pBody->GetPosition();

	float x = pos.x * m_fPTMRatio;
	float y = pos.y * m_fPTMRatio;

	{
		CCPoint point = getParent()->convertToNodeSpace(ccp(x, y));
		x = point.x;
		y = point.y;
	}

	if (m_bIgnoreAnchorPointForPosition)
    {
		x += m_obAnchorPointInPoints.x;
		y += m_obAnchorPointInPoints.y;
	}
	
	// Make matrix
	float radians = m_pBody->GetAngle();
	float c = cosf(radians);
	float s = sinf(radians);
	
	// Although scale is not used by physics engines, it is calculated just in case
	// the sprite is animated (scaled up/down) using actions.
	// For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
	if (!m_obAnchorPointInPoints.equals(CCPointZero))
    {
		x += ((c * -m_obAnchorPointInPoints.x * m_fScaleX) + (-s * -m_obAnchorPointInPoints.y * m_fScaleY));
		y += ((s * -m_obAnchorPointInPoints.x * m_fScaleX) + (c * -m_obAnchorPointInPoints.y * m_fScaleY));
	}
    
	// Rot, Translate Matrix
	m_sTransform = CCAffineTransformMake( c * m_fScaleX,	s * m_fScaleX,
									     -s * m_fScaleY,	c * m_fScaleY,
									     x,	y );
	
	return m_sTransform;
}

