#ifndef _SWAMM_SPRITE_H_
#define _SWAMM_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

enum enSwammType
{
	SwammType_Swamm = 0,	//蘑菇
	SwammType_Flower,		//鲜花
};

class SwammSprite : public Box2dSprite
{
public:
	SwammSprite(b2World * pWorld);
	static SwammSprite * create(const char *pszFileName, b2World * pWorld);
	static SwammSprite * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	virtual void update(float dt);

	//开始执行动作
	void beginAction(enSwammType type);

private:
	enSwammType m_swammType;	//蘑菇种类
	Vec2 m_oldPos;	//旧位置
	bool m_bLeft;	//是否向左
};

#endif // !_SWAMM_SPRITE_H_
