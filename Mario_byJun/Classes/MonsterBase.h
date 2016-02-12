#ifndef _MONSTER_BASE_H_
#define _MONSTER_BASE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "CCBox2dSprite.h"

//����״̬
enum enMonsterStatus
{
	MonsterStatus_Invalid = 0,	//��Ч״̬
	MonsterStatus_Normal,	//��������״̬
};

//λ��״̬
enum enPosStatus
{
	PosStatus_Invlid = 0,	//��Ч�����������1��ʱ
	PosStatus_NoShow,	//������ʾ��������Ļ�ɼ���Χ��ʱ
	PosStatus_Move,	//�ƶ���������С��1����������Ļ���ɼ���Χ��
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
	//���ó�ʼλ��
	inline void setBeginPos(const Vec2 & pos);
	inline const Vec2 & getBeginPos();
	//ȡ��λ��״̬
	enPosStatus getPosStatus(MarioSprite * pMario);
	//���óߴ�
	inline void setMonsterSize(const Size & size);
	inline const Size & getMonsterSize();
	//����״̬
	inline void setMonsterStatus(enMonsterStatus status);
	inline enMonsterStatus getMonsterStatus();
	//�������ײ��Ч���ûֻ֡����1���������ײ
	bool isMarioContactUseful();
	void updateMarioContact(float dt);

public:
	//����״̬Ϊ��Ч
	virtual void setInvalidStatus() = 0;
	//����״̬Ϊ����
	virtual void setNormalStatus() = 0;
	//���������
	virtual void marioContact(MarioSprite * pMario) = 0;
	//���ع������ײ
	virtual void defendTorRollContact(MonsterTortoise * pTortoise) = 0;
	//���鵯��ʱ����,Ĭ�ϲ�����
	virtual void tileJumpHandle(TileSprite * pTile){}
	//�ı��ٶȷ���
	virtual void changeVelocityDirection() = 0;

private:
	Vec2 m_beginPos;
	Size m_monsterSize;
	enMonsterStatus m_eMonsterStatus;
	bool m_bMarioContactUseful;	//��ײ��Ч��־
};

#include "MonsterBase.inl"

#endif // !_MONSTER_BASE_H_
