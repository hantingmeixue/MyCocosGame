#ifndef _SWAMM_SPRITE_H_
#define _SWAMM_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"

enum enSwammType
{
	SwammType_Swamm = 0,	//Ģ��
	SwammType_Flower,		//�ʻ�
};

class SwammSprite : public Box2dSprite
{
public:
	SwammSprite(b2World * pWorld);
	static SwammSprite * create(const char *pszFileName, b2World * pWorld);
	static SwammSprite * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	virtual void update(float dt);

	//��ʼִ�ж���
	void beginAction(enSwammType type);

private:
	enSwammType m_swammType;	//Ģ������
	Vec2 m_oldPos;	//��λ��
	bool m_bLeft;	//�Ƿ�����
};

#endif // !_SWAMM_SPRITE_H_
