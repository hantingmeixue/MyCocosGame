#ifndef _MARIO_SPRITE_H_
#define _MARIO_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

enum enFaceDirection
{
	MarioFD_Right = 0,	//��������
	MarioFD_Left,	//��������
};

enum enMarioStatus
{
	MarioSt_LRun = 0x0000,	//����
	MarioSt_LWalk = 0x0001,		//����
	MarioSt_LJump = 0x0002,		//����
	MarioSt_LDown = 0x0003,		//������
	MarioSt_LTumble = 0x0004,	//��
	MarioSt_RRun = 0x1000,		//�ұ���
	MarioSt_RWalk = 0x1001,		//����
	MarioSt_RJump = 0x1002,		//����
	MarioSt_RDown = 0x1003,		//������
	MarioSt_RTumble = 0x1004,	//�ҽ
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

	//�����˶���ʱ��
	void rightVelocityContinue(float dt);
	void leftVelocityContinue(float dt);
	void jumpVelocityContinue(float dt);

	//���������״̬
	void updateMarioStatus()
	{
		update(0.0f);
	}
	//ȡ������µ�ǰ״̬
	enMarioStatus getMarioStatus()
	{
		return m_eMarioStatus;
	}
	//ȡ������µȼ�
	int getMarioLevel()
	{
		return m_nMarioLevel;
	}
	//��������µȼ�
	void setMarioLevel(int nLevel);
	//��������
	void reduceLife();
	//��������
	void addLife();
	//�������������
	void setMarioDie();
	//��������¿��
	void updateFixture();
	void updateFixture(const Vec2 & pos);
	//��������
	void finishTumble();
	//վ�����
	void finishStanding();

	//�����������˸
private:
	//��ʼ��˸
	void beginBlink();
	//������˸
	void endBlink(float dt);
	//��ʾ��˸
	void showBlink(float dt);

private:
	//���������״̬
	void SetMarioStatus(enMarioStatus eNewStatus);
	//ɾ�����
	void removeFixtrue();
	//ֹͣ������
	void stopHandle();
	//�ָ�������
	void recoverHandle();

private:
	enFaceDirection m_eFaceDirection;	//����
	enMarioStatus m_eMarioStatus;	//״̬
	bool m_bBottomPress;	//�¼��Ƿ񱻰���
	bool m_bRightPress;	//�Ҽ��Ƿ���
	bool m_bLeftPress;	//����Ƿ���
	bool m_bJumpPress;	//jump���Ƿ���
	float m_fJumpBeginPosY;	//����ʱyλ��
	float m_fOldVelocityX;	//��x�ٶ�
	float m_fOldVelocityY;	//��y�ٶ�
	int m_nMarioLevel;	//����µ�ǰ�ȼ�
	bool m_bUnmatchedStatus;	//�Ƿ��޵�״̬
	bool m_bTouchUseful;	//�����Ƿ���Ч
	bool m_bTumbleFinish;	//��Ƿ����
	bool m_bStandingUseful;	//վ���Ƿ���Ч
};

//����Ͷ�䣬�ж�����¾�������߶�
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
	float m_fHeight;	//�����߶�
};


#endif // !_MARIO_SPRITE_H_
