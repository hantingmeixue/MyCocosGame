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

	//ɾ������
	void DestroyPhysics(float dt);

	//�������
	void AddPhysics(float dt);

	//�����ա�����
	void AddGroundSky(CCTMXTiledMap * pMap, CCNode * pPhysicsNode);

private:
	//���ص�ͼ�ж���
	bool AddTileMapObjects(CCTMXTiledMap * pMap, CCNode * pPhysicsNode, bool bSchedule);
	//������һ����ͼ
	void AddNextMap(cocos2d::CCTMXTiledMap * pMap, cocos2d::CCNode * pPhysicsNode);

private:
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWord;

	cocos2d::CCTMXTiledMap * m_pMap1;
	cocos2d::CCTMXTiledMap * m_pMap2;
	cocos2d::CCSpriteBatchNode * m_pPhysicsNode1;	//�������������������
	cocos2d::CCSpriteBatchNode * m_pPhysicsNode2;

	std::list<CCBox2dSprite *> m_liAllBirds;

	int m_nDestoryId;
	bool m_bNeedAdd;
	int m_nMapLevel;
};


#endif	//_TILE_MAP_LAYER_H_
