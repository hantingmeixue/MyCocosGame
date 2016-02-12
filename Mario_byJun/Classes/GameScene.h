#ifndef _GAMES_SCENE_H_
#define _GAMES_SCENE_H_

#include "cocos2d.h"
#include "box2dGameWorld.h"
#include "Box2D/Box2D.h"
#include "GameContactListener.h"
#include "MarioSprite.h"
#include "GameUiLayer.h"
#include "GameConstant.h"

class TileMapLayer;
class TileSprite;
class MonsterBase;
class MonsterTortoise;

class GameScene : public cocos2d::Layer
{
public:
	GameScene();
	~GameScene();

	static GameScene * getInstance();

    static cocos2d::Scene* createScene();
	CREATE_FUNC(GameScene);

    virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float dt);

public:
	//取得马里奥
	MarioSprite * getMario()
	{
		return m_pMario;
	}
	//取得物理世界
	b2World * getWorld()
	{
		return m_pWorld;
	}
	Box2dGameWorld * getGameWorld()
	{
		return m_pGameWorld;
	}
	//取得地图图层
	TileMapLayer * getMapLayer()
	{
		return m_pMapLayer;
	}

	//更新摄像机
	void updateMap(float dt);

	//增加碰撞土块
	void addContactTile(TileSprite * tile);
	//增加移除精灵
	void addRemoveSprite(Box2dSprite * pSprite, enGameSpriteTag tag);
	//增加1金币数目
	void addCoinCount(int nCount = 1)
	{
		m_pUiLayer->addCoinCount(nCount);
	}
	//增加怪兽
	void addMonster(MonsterBase * pMonster);
	//复位怪兽
	void resetMonster(MonsterBase * pMonster);
	//得到大金币
	void getBigCoin(Box2dSprite * pBigCoin);
	//时间紧张
	void timeLack();

public:
	//填充云彩图层
	void fillCloundLayer(Layer * pCloundLayer);
	//增加马里奥生命值
	void addLife();
	//减少马里奥生命值
	void reduceLife();
	//马里奥死亡
	void marioDie();
	//马里奥胜利
	void marioWin();
	//旗帜已经移至底部
	void flagHaveAtBottom();
	//马里奥庆祝成功完成
	void finishCelebrate();
	//马里奥跑步结束
	void marioRunOver();
	//显示胜利信息
	void showWinInfo(float dt);

private:
	//播放紧张音乐
	void playLackMusic(float dt);
	//移除物体框架
	void removeBodyFixture(b2Body * pBody);
	//游戏结束
	void gameOver();
	//加载游戏动画缓存
	void LoadGameAnimationCatch();

private:
	//物理引擎参数
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWorld;
	GameContactListener * m_pGameContactListener;

	TileMapLayer * m_pMapLayer;	//地图层
	GameUiLayer * m_pUiLayer;	//UI图层
	MarioSprite * m_pMario;
	Vector<TileSprite *> m_vContactTile;
	Vector<Box2dSprite *> m_vRemoveFixture;	//移除框架列表
	Vector<Box2dSprite *> m_vRemoveSprite;	//移除精灵列表
	Vector<MonsterBase *> m_vMonsterSprite;	//怪兽列表
	Vector<MonsterBase *> m_vResetMonster;	//复位怪兽列表
	Layer * m_pCloundLayer1;	//云彩图层1
	Layer * m_pCloundLayer2;	//云彩图层2
	Sprite * m_pWinInfo;	//胜利信息

	//游戏变量
	int m_nBgMusicId;	//背景音乐ID
	float m_fCameraMoveDistance;	//摄像机移动距离
	float m_fMapWidth;	//地图宽度
	bool m_bGameOver;	//游戏是否结束
	bool m_bMarioAddLife;	//马里奥是否增加生命
	bool m_bMarioReduceLife;	//马里奥是否减少生命
	bool m_bflagAtBottom;	//旗帜已在底部
	bool m_bCelebrateFinish;	//庆祝完成 

	static GameScene * m_shareGameScene;
};

#endif // __GAMES_SCENE_H__
