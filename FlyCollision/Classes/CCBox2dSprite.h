#ifndef __CC_BOX2D_SPRITE_H__
#define __CC_BOX2D_SPRITE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

USING_NS_CC;

class CCBox2dSprite : public cocos2d::CCSprite
{
public:
	CCBox2dSprite(b2World * pWorld);
	~CCBox2dSprite();

	static CCBox2dSprite* create(b2World * pWorld);
	/** Creates an sprite with a texture.
	The rect used will be the size of the texture.
	The offset will be (0,0).
	*/
	static CCBox2dSprite* createWithTexture(CCTexture2D *pTexture, b2World * pWorld);

	/** Creates an sprite with a texture and a rect.
	The offset will be (0,0).
	*/
	static CCBox2dSprite* createWithTexture(CCTexture2D *pTexture, const CCRect& rect, b2World * pWorld);

	/** Creates an sprite with an sprite frame. */
	static CCBox2dSprite* createWithSpriteFrame(CCSpriteFrame *pSpriteFrame, b2World * pWorld);

	/** Creates an sprite with an sprite frame name.
	An CCSpriteFrame will be fetched from the CCSpriteFrameCache by name.
	If the CCSpriteFrame doesn't exist it will raise an exception.
	@since v0.9
	*/
	static CCBox2dSprite* createWithSpriteFrameName(const char *pszSpriteFrameName, b2World * pWorld);

	/** Creates an sprite with an image filename.
	The rect used will be the size of the image.
	The offset will be (0,0).
	*/
	static CCBox2dSprite* create(const char *pszFileName, b2World * pWorld);

	/** Creates an sprite with an image filename and a rect.
	The offset will be (0,0).
	*/
	static CCBox2dSprite* create(const char *pszFileName, const CCRect& rect, b2World * pWorld);

	virtual bool isDirty();

	/** Keep the sprite's rotation separate from the body. */
	bool isIgnoreBodyRotation() const;
	void setIgnoreBodyRotation(bool bIgnoreBodyRotation);

	virtual const CCPoint& getPosition();
	virtual void getPosition(float* x, float* y);
	virtual float getPositionX();
	virtual float getPositionY();
	virtual void setPosition(const CCPoint &position);
	virtual float getRotation();
	virtual void setRotation(float fRotation);
	virtual CCAffineTransform nodeToParentTransform();

	/** Body accessor when using box2d */
	b2Body* getBody() const;
	void setBody(b2Body *pBody);
	float getPTMRatio() const;
	void setPTMRatio(float fPTMRatio);

	inline void EnbaleGravity(bool bGravity);

protected:
	void updatePosFromPhysics();

private:
	bool    m_bIgnoreBodyRotation;
	b2World * m_pWorld;
	b2Body  * m_pBody;
	float   m_fPTMRatio;

};

void CCBox2dSprite::EnbaleGravity(bool bGravity)
{
	m_pBody->SetGravityScale(bGravity ? 1.0f : 0.0f);
	if (bGravity)
		m_pBody->SetAwake(true);
}

#endif // __CC_BOX2D_SPRITE_H__
