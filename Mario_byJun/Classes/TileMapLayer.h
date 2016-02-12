#ifndef _TILE_MAP_LAYER_H_
#define _TILE_MAP_LAYER_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "box2dGameWorld.h"
#include <string>
#include "TileSprite.h"

class GameScene;
class MarioSprite;

class TileMapLayer : public cocos2d::Layer
{
public:
	TileMapLayer();
	static TileMapLayer * create(Box2dGameWorld * pGameWorld);

	virtual bool init(Box2dGameWorld * pGameWorld);
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);

public:
	//通用怪兽形状生成
	static void commonMonsterShape(const Size & size, b2Vec2 vertices[6], int nCount);
	//根据马里奥位置，更新背景位置
	void updateBgLayerPos(MarioSprite * pMario);
	//取得地图文件层
	TMXTiledMap * getTileMap()
	{
		return m_pMap1;
	}
	//滑落旗帜
	void slideFlag();

private:
	//添加地图对象
	void addTileMapObjects(cocos2d::TMXTiledMap * pMap);
	//添加边界对象
	void addEdgeObjects(cocos2d::TMXObjectGroup * edgeGroup);
	//添加动态对象
	void addDynamicObjects(cocos2d::TMXObjectGroup * dynamicGroup);
	//设置土块类型、金币信息
	void setTileSpriteInfo(TileSprite * pSprite, ValueMap & dict);

private:
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWorld;

	GameScene * m_pGameScene;
	TMXTiledMap * m_pMap1;
	static const std::string m_szEdgeObjectName[7];	//边界类对象
	static const std::string m_szDynamicObjectName[7];	//动态对象
	Layer * m_pBgLyaer1;	//背景图层1
	Layer * m_pBgLyaer2;	//背景图层2
	Sprite * m_pFlag;	//旗帜
	bool m_bSliding;	//是否在滑落
	float m_fFlagHeight;	//旗帜高度

	CustomCommand m_customCmd;
};

#endif // !_TILE_MAP_LAYER_H_
