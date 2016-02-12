#ifndef _GAME_HERO_H_
#define _GAME_HERO_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"

class Hero : public CCBox2dSprite
{
public:
	Hero(b2World * pWorld)
		:CCBox2dSprite(pWorld){}
	static Hero * create(const char *pszFileName, b2World * pWorld);
	static Hero * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	void ReadyFlying();
	void WorldScroll(float dt);

	void hit();
	void flying();
	void jump();
	void drop();
	void die();

};

#endif	//_GAME_HERO_H_
