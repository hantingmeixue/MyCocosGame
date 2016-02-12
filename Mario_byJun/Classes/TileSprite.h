#ifndef _CC_TILE_SPRITE_H_
#define _CC_TILE_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"
#include "MonsterBase.h"
#include "GameConstant.h"

class MonsterBase;
class MarioSprite;

//��������
enum enTileSpriteType
{
	TSType_Null = 0,		//�շ���
	TSType_Coin,			//��ҷ���
	TSType_Swamm,			//Ģ������
	TSType_Invalid,			//��Ч����
};

class TileSprite : public Box2dSprite
{
public:
	TileSprite(b2World * pWorld);
	static TileSprite * create(const char *pszFileName, b2World * pWorld);
	static TileSprite * createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld);

	void setTileType(enTileSpriteType tileType, int nCoinCount = 0);
	enTileSpriteType getTileType();
	int getCoinCount();

	//����¶�ײ����
	void marioContact(MarioSprite * pMario);
	//������ײ����
	void handleContact(MarioSprite * pMario);
	//����Ϊ��Ч����
	void setInvalid();
	//���ӽ����Ŀ
	void addCoinCount();
	//���Ģ��
	void addSwammSprite(bool bTumble);

private:
	//��ײ��Ч�ж�
	bool isMarioJackUseful(MarioSprite * pMario, const Vec2 & tilePos);
	//�����ײ����
	void finishHandleContact()
	{
		m_bFinishHandleContact = true;
	}

private:
	enTileSpriteType m_eTileType;	//��������
	int m_nCoinCount;	//Ǯ����Ŀ
	bool m_bFinishHandleContact;	//��ײ�������
};

//�������ײ����ʱ��ִ��AABB��⣬���������ϵĹ���
class TileQueryCallBack : public b2QueryCallback
{
public:
	TileQueryCallBack(TileSprite * pTile)
		:m_pTile(pTile)
	{
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body * body = fixture->GetBody();
		Box2dSprite * sprite = static_cast<Box2dSprite *>(body->GetUserData());
		int tag = sprite->getTag();

		if ((tag & 0x1000) != 0)
		{
			static_cast<MonsterBase *>(sprite)->tileJumpHandle(m_pTile);
		}

		return true;
	}

private:
	TileSprite * m_pTile;	//��Ӧ����
};

#endif // !_CC_TILE_SPRITE_H_
