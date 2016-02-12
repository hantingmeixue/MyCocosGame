#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GameContactListener.h"

class BgLayer;
class TileMapLayer;
class GameScene;
class Box2dGameWorld;
class CCBox2dSprite;
class Hero;

enum eObjectTag
{
	TagHeart = 10,
	TagBird,
	TagAirShip,
	TagHero,
	TagGround,
	TagSky,
};

class GameScene : public cocos2d::CCLayer
{
public:
	GameScene(bool bCheat);
	~GameScene();

	static cocos2d::CCScene * scene(bool bCheat);
	static GameScene * create(bool bCheat);

	virtual bool init();
	virtual void onEnter();
	virtual void draw();
	virtual void update(float fDelta);

	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	void ChangeBlood(float fValue);
	void AddRemoveSprite(cocos2d::CCNode * pNode);
	void CanScrollView(float dt);
	void UpdateRollLength(float dt);
	void GameEnd();

	void DelayEnd();

	//按钮
	void menuBack(CCObject * pSender);
	void menuPause(CCObject * pSender);

private:
	//加载游戏动画缓存
	void LoadGameAnimationCatch();

private:
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWorld;
	GameContactListener * m_pGameContactListener;

	BgLayer * m_pBgLayer;
	TileMapLayer * m_pTileMapLayer;
	Hero * m_pHero;
	cocos2d::CCProgressTimer * m_pFill;
	cocos2d::CCLabelAtlas * m_pDistance;

	std::vector<cocos2d::CCNode *> m_vRemoveNode;
	bool m_bScrollView;
	float m_fFlood;
	int m_nDistance;
	float m_fRollScale;
	float m_fRollLength;

	cocos2d::CCSize m_Size;
	bool m_bEnd;
	bool m_bPause;
	bool m_bCheat;
};

#endif	//_GAME_SCENE_H_
