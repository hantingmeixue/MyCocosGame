#ifndef _MONSTER_ROBBIT_H_
#define _MONSTER_ROBBIT_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MonsterBase.h"

class Box2dSprite;
class MarioSprite;
class MonsterTortoise;

class MonsterRobbit : public MonsterBase
{
public:
	MonsterRobbit(b2World * pWorld);
	~MonsterRobbit();

	static MonsterRobbit * create(const char *pszFileName, b2World * pWorld);
	static MonsterRobbit * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

public:
	//设置状态为无效
	virtual void setInvalidStatus();
	//设置状态为正常
	virtual void setNormalStatus();
	//马里奥碰触
	virtual void marioContact(MarioSprite * pMario);
	//防守龟滚动碰撞
	virtual void defendTorRollContact(MonsterTortoise * pTortoise);
	//改变速度方向
	virtual void changeVelocityDirection();

public:
	//位置校验定时器
	void checkPosSchedule(float dt);
	//移除框架定时器
	void removeFixtureSchedule(float dt);
	//移除自己
	void removeSelf();

private:
	Vec2 m_lastPos;	//上一时刻位置
	bool m_bLeft;	//是否向左走
	bool m_bIsDie;	//是否已死
};

#endif // !_MONSTER_ROBBIT_H_


