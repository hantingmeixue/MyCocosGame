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
	//����״̬Ϊ��Ч
	virtual void setInvalidStatus();
	//����״̬Ϊ����
	virtual void setNormalStatus();
	//���������
	virtual void marioContact(MarioSprite * pMario);
	//���ع������ײ
	virtual void defendTorRollContact(MonsterTortoise * pTortoise);
	//�ı��ٶȷ���
	virtual void changeVelocityDirection();

public:
	//λ��У�鶨ʱ��
	void checkPosSchedule(float dt);
	//�Ƴ���ܶ�ʱ��
	void removeFixtureSchedule(float dt);
	//�Ƴ��Լ�
	void removeSelf();

private:
	Vec2 m_lastPos;	//��һʱ��λ��
	bool m_bLeft;	//�Ƿ�������
	bool m_bIsDie;	//�Ƿ�����
};

#endif // !_MONSTER_ROBBIT_H_


