#ifndef _MONSTER_TORTOISE_H_
#define _MONSTER_TORTOISE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MonsterBase.h"

class Box2dSprite;
class MarioSprite;

//�����ڹ���״̬
enum enUpdateTorFixStatus
{
	UpdateTortoise_Invalid = 0,	//���ø���
	UpdateTortoise_Normal,	//����״̬
	UpdateTortoise_Defend,	//����״̬
	UpdateTortoise_TorKillL,//���ڹ�������ɱ��
	UpdateTortoise_TorKillR,//���ڹ�������ɱ��
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
	//����Ͷ��ص������ڿ���λ��У��
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction);
	//����״̬Ϊ��Ч
	virtual void setInvalidStatus();
	//����״̬Ϊ����
	virtual void setNormalStatus();
	//���������
	virtual void marioContact(MarioSprite * pMario);
	//���ع������ײ
	virtual void defendTorRollContact(MonsterTortoise * pTortoise);
	//���鵯��ʱ����,��д
	virtual void tileJumpHandle(TileSprite * pTile);
	//�ı��ٶȷ���
	virtual void changeVelocityDirection();

public:
	//λ��У�鶨ʱ��
	void checkPosSchedule();
	//�Ƿ�Ϊ���ع���״̬
	bool isDefendRoll();
	//�Ƿ����
	bool isDefend()
	{
		return m_bIsDefend;
	}
	//�Ƿ����
	bool isRoll()
	{
		return m_bIsRoll;
	}

private:
	//�Ƴ��Լ�
	void removeSelf();
	//��λX���ٶ�
	void resetVelocityX();
	//�ı���
	void changeFixture();

private:
	Vec2 m_lastPos;	//��һʱ��λ��
	bool m_bLeft;	//�Ƿ�������
	bool m_bIsDefend;	//�Ƿ����
	bool m_bIsRoll;	//�Ƿ����״̬
	bool m_bStepObject;	//�Ƿ��������
	bool m_bIsDie;	//�Ƿ�����
	int m_ntimes;	//У��Ƶ��
	enUpdateTorFixStatus updateFixStatus;	//�����¿��״̬
};

#endif // !_MONSTER_TORTOISE_H_


