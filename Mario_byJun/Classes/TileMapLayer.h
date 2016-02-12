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
	//ͨ�ù�����״����
	static void commonMonsterShape(const Size & size, b2Vec2 vertices[6], int nCount);
	//���������λ�ã����±���λ��
	void updateBgLayerPos(MarioSprite * pMario);
	//ȡ�õ�ͼ�ļ���
	TMXTiledMap * getTileMap()
	{
		return m_pMap1;
	}
	//��������
	void slideFlag();

private:
	//��ӵ�ͼ����
	void addTileMapObjects(cocos2d::TMXTiledMap * pMap);
	//��ӱ߽����
	void addEdgeObjects(cocos2d::TMXObjectGroup * edgeGroup);
	//��Ӷ�̬����
	void addDynamicObjects(cocos2d::TMXObjectGroup * dynamicGroup);
	//�����������͡������Ϣ
	void setTileSpriteInfo(TileSprite * pSprite, ValueMap & dict);

private:
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWorld;

	GameScene * m_pGameScene;
	TMXTiledMap * m_pMap1;
	static const std::string m_szEdgeObjectName[7];	//�߽������
	static const std::string m_szDynamicObjectName[7];	//��̬����
	Layer * m_pBgLyaer1;	//����ͼ��1
	Layer * m_pBgLyaer2;	//����ͼ��2
	Sprite * m_pFlag;	//����
	bool m_bSliding;	//�Ƿ��ڻ���
	float m_fFlagHeight;	//���ĸ߶�

	CustomCommand m_customCmd;
};

#endif // !_TILE_MAP_LAYER_H_
