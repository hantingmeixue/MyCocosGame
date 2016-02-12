#ifndef _CC_TILE_SPRITE_H_
#define _CC_TILE_SPRITE_H_

#include "cocos2d.h"
#include "CCBox2dSprite.h"
#include "Box2D/Box2D.h"
#include "MonsterBase.h"
#include "GameConstant.h"

class MonsterBase;
class MarioSprite;

//方块类型
enum enTileSpriteType
{
	TSType_Null = 0,		//空方块
	TSType_Coin,			//金币方块
	TSType_Swamm,			//蘑菇方块
	TSType_Invalid,			//无效方块
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

	//马里奥顶撞土块
	void marioContact(MarioSprite * pMario);
	//处理碰撞土块
	void handleContact(MarioSprite * pMario);
	//设置为无效土块
	void setInvalid();
	//增加金币数目
	void addCoinCount();
	//添加蘑菇
	void addSwammSprite(bool bTumble);

private:
	//碰撞有效判断
	bool isMarioJackUseful(MarioSprite * pMario, const Vec2 & tilePos);
	//完成碰撞处理
	void finishHandleContact()
	{
		m_bFinishHandleContact = true;
	}

private:
	enTileSpriteType m_eTileType;	//方块类型
	int m_nCoinCount;	//钱币数目
	bool m_bFinishHandleContact;	//碰撞处理完成
};

//马里奥碰撞土块时，执行AABB检测，处理土块上的怪物
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
	TileSprite * m_pTile;	//对应土块
};

#endif // !_CC_TILE_SPRITE_H_
