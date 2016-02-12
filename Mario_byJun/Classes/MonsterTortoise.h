#ifndef _MONSTER_TORTOISE_H_
#define _MONSTER_TORTOISE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MonsterBase.h"

class Box2dSprite;
class MarioSprite;

//更新乌龟框架状态
enum enUpdateTorFixStatus
{
	UpdateTortoise_Invalid = 0,	//不用更新
	UpdateTortoise_Normal,	//正常状态
	UpdateTortoise_Defend,	//保护状态
	UpdateTortoise_TorKillL,//被乌龟在左面杀掉
	UpdateTortoise_TorKillR,//被乌龟在右面杀掉
};

class MonsterTortoise : public MonsterBase, public b2RayCastCallback
{
public:
	MonsterTortoise(b2World * pWorld);
	~MonsterTortoise();

	static MonsterTortoise * create(const char *pszFileName, b2World * pWorld);
	static MonsterTortoise * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	virtual void update(float dt);

public:
	//光线投射回调，用于空中位置校验
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction);
	//设置状态为无效
	virtual void setInvalidStatus();
	//设置状态为正常
	virtual void setNormalStatus();
	//马里奥碰触
	virtual void marioContact(MarioSprite * pMario);
	//防守龟滚动碰撞
	virtual void defendTorRollContact(MonsterTortoise * pTortoise);
	//土块弹起时处理,重写
	virtual void tileJumpHandle(TileSprite * pTile);
	//改变速度方向
	virtual void changeVelocityDirection();

public:
	//位置校验定时器
	void checkPosSchedule();
	//是否为防守滚动状态
	bool isDefendRoll();
	//是否防守
	bool isDefend()
	{
		return m_bIsDefend;
	}
	//是否滚动
	bool isRoll()
	{
		return m_bIsRoll;
	}

private:
	//移除自己
	void removeSelf();
	//复位X轴速度
	void resetVelocityX();
	//改变框架
	void changeFixture();

private:
	Vec2 m_lastPos;	//上一时刻位置
	bool m_bLeft;	//是否向左走
	bool m_bIsDefend;	//是否防守
	bool m_bIsRoll;	//是否滚动状态
	bool m_bStepObject;	//是否踩着物体
	bool m_bIsDie;	//是否已死
	int m_ntimes;	//校验频率
	enUpdateTorFixStatus updateFixStatus;	//待更新框架状态
};

#endif // !_MONSTER_TORTOISE_H_


