#ifndef _MARIO_SPRITE_H_
#define _MARIO_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

enum enFaceDirection
{
	MarioFD_Right = 0,	//面向右面
	MarioFD_Left,	//面向左面
};

enum enMarioStatus
{
	MarioSt_LRun = 0x0000,	//左奔跑
	MarioSt_LWalk = 0x0001,		//左走
	MarioSt_LJump = 0x0002,		//左跳
	MarioSt_LDown = 0x0003,		//左落下
	MarioSt_LTumble = 0x0004,	//左筋斗
	MarioSt_RRun = 0x1000,		//右奔跑
	MarioSt_RWalk = 0x1001,		//右走
	MarioSt_RJump = 0x1002,		//右跳
	MarioSt_RDown = 0x1003,		//右落下
	MarioSt_RTumble = 0x1004,	//右筋斗
};

class MarioSprite : public Box2dSprite
{
public:
	static const Size & getMarioSize()
	{
		static Size marioSize(64.0f / CC_CONTENT_SCALE_FACTOR(), 120.0f / CC_CONTENT_SCALE_FACTOR());
		return marioSize;
	}

	MarioSprite(b2World * pWorld);
	static MarioSprite * create(const char *pszFileName, b2World * pWorld);
	static MarioSprite * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	virtual void onEnter();
	virtual void onExit();
	virtual void update(float dt);

public:
	void setBottomPress(bool bBottomPress);
	void SetRightPress(bool bRightPress);
	void SetLeftPress(bool bLeftPress);
	void SetJumpPress(bool bJumpPress);

	//持续运动定时器
	void rightVelocityContinue(float dt);
	void leftVelocityContinue(float dt);
	void jumpVelocityContinue(float dt);

	//更新马里奥状态
	void updateMarioStatus()
	{
		update(0.0f);
	}
	//取得马里奥当前状态
	enMarioStatus getMarioStatus()
	{
		return m_eMarioStatus;
	}
	//取得马里奥等级
	int getMarioLevel()
	{
		return m_nMarioLevel;
	}
	//设置马里奥等级
	void setMarioLevel(int nLevel);
	//减少生命
	void reduceLife();
	//增加生命
	void addLife();
	//设置马里奥死亡
	void setMarioDie();
	//更新马里奥框架
	void updateFixture();
	void updateFixture(const Vec2 & pos);
	//筋斗动作完成
	void finishTumble();
	//站立完成
	void finishStanding();

	//控制马里奥闪烁
private:
	//开始闪烁
	void beginBlink();
	//结束闪烁
	void endBlink(float dt);
	//显示闪烁
	void showBlink(float dt);

private:
	//设置马里奥状态
	void SetMarioStatus(enMarioStatus eNewStatus);
	//删除框架
	void removeFixtrue();
	//停止各处理
	void stopHandle();
	//恢复各处理
	void recoverHandle();

private:
	enFaceDirection m_eFaceDirection;	//朝向
	enMarioStatus m_eMarioStatus;	//状态
	bool m_bBottomPress;	//下键是否被按下
	bool m_bRightPress;	//右键是否按下
	bool m_bLeftPress;	//左键是否按下
	bool m_bJumpPress;	//jump键是否按下
	float m_fJumpBeginPosY;	//起跳时y位置
	float m_fOldVelocityX;	//老x速度
	float m_fOldVelocityY;	//老y速度
	int m_nMarioLevel;	//马里奥当前等级
	bool m_bUnmatchedStatus;	//是否无敌状态
	bool m_bTouchUseful;	//按键是否有效
	bool m_bTumbleFinish;	//筋斗是否完成
	bool m_bStandingUseful;	//站立是否有效
};

//光线投射，判断马里奥距离物体高度
class MarioRayCastCallBack : public b2RayCastCallback
{
public:
	MarioRayCastCallBack(MarioSprite * pMario)
		:m_pMario(pMario)
		, m_fHeight(-1.0f)
	{
		m_marioPos = pMario->getPosition();
	}

	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction);

	float getHeight()
	{
		return m_fHeight;
	}

private:
	MarioSprite * m_pMario;
	Vec2 m_marioPos;
	float m_fHeight;	//距地面高度
};


#endif // !_MARIO_SPRITE_H_
