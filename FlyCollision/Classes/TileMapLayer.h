#ifndef _TILE_MAP_LAYER_H_
#define _TILE_MAP_LAYER_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include <list>
#include "CCBox2dSprite.h"
#include "box2dGameWorld.h"


class TileMapLayer : public cocos2d::CCLayer
{
public:
	TileMapLayer();
	static TileMapLayer * create(Box2dGameWorld * pGameWorld);

	virtual bool init(Box2dGameWorld * pGameWorld);

	bool UpdateMap(float fRollLength);

	//删除物体
	void DestroyPhysics(float dt);

	//添加物体
	void AddPhysics(float dt);

	//添加天空、地面
	void AddGroundSky(CCTMXTiledMap * pMap, CCNode * pPhysicsNode);

private:
	//加载地图中对象
	bool AddTileMapObjects(CCTMXTiledMap * pMap, CCNode * pPhysicsNode, bool bSchedule);
	//加载新一个地图
	void AddNextMap(cocos2d::CCTMXTiledMap * pMap, cocos2d::CCNode * pPhysicsNode);

private:
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWord;

	cocos2d::CCTMXTiledMap * m_pMap1;
	cocos2d::CCTMXTiledMap * m_pMap2;
	cocos2d::CCSpriteBatchNode * m_pPhysicsNode1;	//包含所有物理世界对象
	cocos2d::CCSpriteBatchNode * m_pPhysicsNode2;

	std::list<CCBox2dSprite *> m_liAllBirds;

	int m_nDestoryId;
	bool m_bNeedAdd;
	int m_nMapLevel;
};


#endif	//_TILE_MAP_LAYER_H_
