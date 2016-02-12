#ifndef __CC_BOX2D_SPRITE_H__
#define __CC_BOX2D_SPRITE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

class Box2dSprite : public cocos2d::Sprite
{
public:
	Box2dSprite(b2World * pWorld);
	~Box2dSprite();

	static Box2dSprite* create(b2World * pWorld);
	/** Creates an sprite with a texture.
	The rect used will be the size of the texture.
	The offset will be (0,0).
	*/
	static Box2dSprite* createWithTexture(Texture2D *pTexture, b2World * pWorld);

	/** Creates an sprite with a texture and a rect.
	The offset will be (0,0).
	*/
	static Box2dSprite* createWithTexture(Texture2D *pTexture, const Rect& rect, b2World * pWorld);

	/** Creates an sprite with an sprite frame. */
	static Box2dSprite* createWithSpriteFrame(SpriteFrame *pSpriteFrame, b2World * pWorld);

	/** Creates an sprite with an sprite frame name.
	An CCSpriteFrame will be fetched from the CCSpriteFrameCache by name.
	If the CCSpriteFrame doesn't exist it will raise an exception.
	@since v0.9
	*/
	static Box2dSprite* createWithSpriteFrameName(const char *pszSpriteFrameName, b2World * pWorld);

	/** Creates an sprite with an image filename.
	The rect used will be the size of the image.
	The offset will be (0,0).
	*/
	static Box2dSprite* create(const char *pszFileName, b2World * pWorld);

	/** Creates an sprite with an image filename and a rect.
	The offset will be (0,0).
	*/
	static Box2dSprite* create(const char *pszFileName, const Rect& rect, b2World * pWorld);

	virtual bool isDirty();

	/** Keep the sprite's rotation separate from the body. */
	bool isIgnoreBodyRotation() const;
	void setIgnoreBodyRotation(bool bIgnoreBodyRotation);

	virtual const Vec2& getPosition() const;
	virtual Vec3 getPosition3D() const;
	virtual void getPosition(float* x, float* y);
	virtual float getPositionX();
	virtual float getPositionY();
	virtual void setPosition(const Vec2 &position);
	virtual void setPosition3D(const Vec3 &position);
	virtual float getRotation();
	virtual void setRotation(float fRotation);
	virtual void syncPhysicsTransform() const;

	/** Body accessor when using box2d */
	inline b2Body* getBody() const;
	inline void setBody(b2Body *pBody);
	inline float getPTMRatio() const;
	inline void setPTMRatio(float fPTMRatio);

	inline void EnableGravity(bool bGravity);

	virtual void onEnter() override;
	virtual void onExit() override;

protected:
	const Vec2& getPosFromPhysics() const;
	void afterUpdate(EventCustom *event);

private:
	bool    m_bIgnoreBodyRotation;
	b2World * m_pWorld;
	b2Body  * m_pBody;
	float   m_fPTMRatio;
	//Vec2 m_physicPosion;
	cocos2d::EventListenerCustom* _syncTransform;
};

void Box2dSprite::EnableGravity(bool bGravity)
{
	m_pBody->SetGravityScale(bGravity ? 1.0f : 0.0f);
	if (bGravity)
		m_pBody->SetAwake(true);
}

b2Body* Box2dSprite::getBody() const
{
	return m_pBody;
}

void Box2dSprite::setBody(b2Body *pBody)
{
	m_pBody = pBody;
}

float Box2dSprite::getPTMRatio() const
{
	return m_fPTMRatio;
}

void Box2dSprite::setPTMRatio(float fRatio)
{
	m_fPTMRatio = fRatio;
}

#endif // __CC_BOX2D_SPRITE_H__
