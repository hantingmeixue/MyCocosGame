#ifndef _MONSTER_BASE_H_
#define _MONSTER_BASE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "CCBox2dSprite.h"

//怪兽状态
enum enMonsterStatus
{
	MonsterStatus_Invalid = 0,	//无效状态
	MonsterStatus_Normal,	//正常行走状态
};

//位置状态
enum enPosStatus
{
	PosStatus_Invlid = 0,	//无效，当距离大于1屏时
	PosStatus_NoShow,	//不可显示，当在屏幕可见范围内时
	PosStatus_Move,	//移动，当距离小于1屏，且在屏幕不可见范围内
};

class Box2dSprite;
class MarioSprite;
class MonsterTortoise;
class TileSprite;

class MonsterBase : public Box2dSprite
{
public:
	MonsterBase(b2World * pWorld);
	~MonsterBase();

public:
	//设置初始位置
	inline void setBeginPos(const Vec2 & pos);
	inline const Vec2 & getBeginPos();
	//取得位置状态
	enPosStatus getPosStatus(MarioSprite * pMario);
	//设置尺寸
	inline void setMonsterSize(const Size & size);
	inline const Size & getMonsterSize();
	//设置状态
	inline void setMonsterStatus(enMonsterStatus status);
	inline enMonsterStatus getMonsterStatus();
	//马里奥碰撞有效与否，没帧只接受1次马里奥碰撞
	bool isMarioContactUseful();
	void updateMarioContact(float dt);

public:
	//设置状态为无效
	virtual void setInvalidStatus() = 0;
	//设置状态为正常
	virtual void setNormalStatus() = 0;
	//马里奥碰触
	virtual void marioContact(MarioSprite * pMario) = 0;
	//防守龟滚动碰撞
	virtual void defendTorRollContact(MonsterTortoise * pTortoise) = 0;
	//土块弹起时处理,默认不处理
	virtual void tileJumpHandle(TileSprite * pTile){}
	//改变速度方向
	virtual void changeVelocityDirection() = 0;

private:
	Vec2 m_beginPos;
	Size m_monsterSize;
	enMonsterStatus m_eMonsterStatus;
	bool m_bMarioContactUseful;	//碰撞有效标志
};

#include "MonsterBase.inl"

#endif // !_MONSTER_BASE_H_
