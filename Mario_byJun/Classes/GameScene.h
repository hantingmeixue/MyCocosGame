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
	//ȡ�������
	MarioSprite * getMario()
	{
		return m_pMario;
	}
	//ȡ����������
	b2World * getWorld()
	{
		return m_pWorld;
	}
	Box2dGameWorld * getGameWorld()
	{
		return m_pGameWorld;
	}
	//ȡ�õ�ͼͼ��
	TileMapLayer * getMapLayer()
	{
		return m_pMapLayer;
	}

	//���������
	void updateMap(float dt);

	//������ײ����
	void addContactTile(TileSprite * tile);
	//�����Ƴ�����
	void addRemoveSprite(Box2dSprite * pSprite, enGameSpriteTag tag);
	//����1�����Ŀ
	void addCoinCount(int nCount = 1)
	{
		m_pUiLayer->addCoinCount(nCount);
	}
	//���ӹ���
	void addMonster(MonsterBase * pMonster);
	//��λ����
	void resetMonster(MonsterBase * pMonster);
	//�õ�����
	void getBigCoin(Box2dSprite * pBigCoin);
	//ʱ�����
	void timeLack();

public:
	//����Ʋ�ͼ��
	void fillCloundLayer(Layer * pCloundLayer);
	//�������������ֵ
	void addLife();
	//�������������ֵ
	void reduceLife();
	//���������
	void marioDie();
	//�����ʤ��
	void marioWin();
	//�����Ѿ������ײ�
	void flagHaveAtBottom();
	//�������ף�ɹ����
	void finishCelebrate();
	//������ܲ�����
	void marioRunOver();
	//��ʾʤ����Ϣ
	void showWinInfo(float dt);

private:
	//���Ž�������
	void playLackMusic(float dt);
	//�Ƴ�������
	void removeBodyFixture(b2Body * pBody);
	//��Ϸ����
	void gameOver();
	//������Ϸ��������
	void LoadGameAnimationCatch();

private:
	//�����������
	Box2dGameWorld * m_pGameWorld;
	b2World * m_pWorld;
	GameContactListener * m_pGameContactListener;

	TileMapLayer * m_pMapLayer;	//��ͼ��
	GameUiLayer * m_pUiLayer;	//UIͼ��
	MarioSprite * m_pMario;
	Vector<TileSprite *> m_vContactTile;
	Vector<Box2dSprite *> m_vRemoveFixture;	//�Ƴ�����б�
	Vector<Box2dSprite *> m_vRemoveSprite;	//�Ƴ������б�
	Vector<MonsterBase *> m_vMonsterSprite;	//�����б�
	Vector<MonsterBase *> m_vResetMonster;	//��λ�����б�
	Layer * m_pCloundLayer1;	//�Ʋ�ͼ��1
	Layer * m_pCloundLayer2;	//�Ʋ�ͼ��2
	Sprite * m_pWinInfo;	//ʤ����Ϣ

	//��Ϸ����
	int m_nBgMusicId;	//��������ID
	float m_fCameraMoveDistance;	//������ƶ�����
	float m_fMapWidth;	//��ͼ���
	bool m_bGameOver;	//��Ϸ�Ƿ����
	bool m_bMarioAddLife;	//������Ƿ���������
	bool m_bMarioReduceLife;	//������Ƿ��������
	bool m_bflagAtBottom;	//�������ڵײ�
	bool m_bCelebrateFinish;	//��ף��� 

	static GameScene * m_shareGameScene;
};

#endif // __GAMES_SCENE_H__
