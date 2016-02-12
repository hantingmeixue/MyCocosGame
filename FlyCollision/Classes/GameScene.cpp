#include "MenuScene.h"
#include "GameScene.h"
#include "UsuallyFunctions.h"
#include "BgLayer.h"
#include "TileMapLayer.h"
#include "CCBox2dSprite.h"
#include "box2dGameWorld.h"
#include "GameContactListener.h"
#include "Hero.h"
#include "SimpleAudioEngine.h"
#include <algorithm>


USING_NS_CC;
using namespace CocosDenshion;

extern const float g_fBgRollLength1;
extern const float g_fBgRollLength2;

GameScene::GameScene(bool bCheat)
	:m_bCheat(bCheat)
	,m_pGameWorld(NULL)
	,m_pWorld(NULL)
	,m_pBgLayer(NULL)
	,m_pTileMapLayer(NULL)
	,m_bScrollView(false)
	,m_fFlood(100.0f)
	,m_nDistance(0)
	,m_fRollScale(1.0f)
	,m_fRollLength(g_fBgRollLength2)
	,m_bEnd(false)
	,m_bPause(false)
{

}

GameScene::~GameScene()
{
	//预先删除物理世界精灵
	b2Body * node = m_pWorld->GetBodyList();
	while (node)
	{
		b2Body * b = node;
		node = node->GetNext();
		CCBox2dSprite * pSprite = static_cast<CCBox2dSprite *>(b->GetUserData());
		pSprite->removeFromParent();
	}
	delete m_pGameWorld;
	delete m_pGameContactListener;
}

CCScene * GameScene::scene(bool bCheat)
{
	CCScene * scene = CCScene::create();
	GameScene * layer = GameScene::create(bCheat);
	scene->addChild(layer);

	return scene;
}

GameScene * GameScene::create(bool bCheat)
{
	GameScene * pRet = new GameScene(bCheat);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return 0;
	}
}

bool GameScene::init()
{
	if (!CCLayer::init())
		return false;

	LoadGameAnimationCatch();

	m_Size = CCDirector::sharedDirector()->getWinSize();

	float fScaleFactor = CCDirector::sharedDirector()->getContentScaleFactor();
	m_pBgLayer = BgLayer::create();
	addChild(m_pBgLayer, -1);

	//创建血条
	CCSprite * pBloodBag = CCSprite::createWithSpriteFrameName("progress1.png");
	pBloodBag->setAnchorPoint(ccp(0, 1));
	pBloodBag->setPosition(ccp(10, m_Size.height - 10));
	addChild(pBloodBag);

	m_pFill = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("progress2.png"));
	m_pFill->setType(kCCProgressTimerTypeBar);
	m_pFill->setMidpoint(ccp(0, 0.5f));
	m_pFill->setBarChangeRate(ccp(1, 0));
	m_pFill->setPercentage(m_fFlood);
	m_pFill->setAnchorPoint(ccp(0.5f, 0.5f));
	m_pFill->setPosition(ccp(pBloodBag->getContentSize().width / 2, pBloodBag->getContentSize().height / 2));
	pBloodBag->addChild(m_pFill);

	//创建距离
	CCSprite * pDistance = CCSprite::createWithSpriteFrameName("distance.png");
	pDistance->setAnchorPoint(ccp(0, 0.5));
	pDistance->setPosition(ccp(pBloodBag->getPositionX() + pBloodBag->getContentSize().width + 10,
		pBloodBag->getPositionY() - pBloodBag->getContentSize().height / 2));
	addChild(pDistance);

	m_pDistance = CCLabelAtlas::create("0", "fonts/distance_font.png", 25.0f / fScaleFactor, 29.0f / fScaleFactor, 48);
	m_pDistance->setAnchorPoint(ccp(0, 0.5));
	m_pDistance->setPosition(ccp(pDistance->getPositionX() + pDistance->getContentSize().width + 10,
		pDistance->getPositionY()));
	addChild(m_pDistance);

	//按钮
	CCMenuItemSprite * pBackItem = CCMenuItemSprite::create(
		CCSprite::createWithSpriteFrameName("back.png"), CCSprite::createWithSpriteFrameName("back1.png")
		, this, menu_selector(GameScene::menuBack));
	pBackItem->setAnchorPoint(ccp(0, 0));
	pBackItem->setPosition(ccp(15, 15));

	CCMenuItemSprite * pPauseItem = CCMenuItemSprite::create(
		CCSprite::createWithSpriteFrameName("pause.png"), CCSprite::createWithSpriteFrameName("pause.png"),
		this, menu_selector(GameScene::menuPause));
	pPauseItem->setAnchorPoint(ccp(1, 1));
	pPauseItem->setPosition(ccp(m_Size.width - 50, m_Size.height -10));

	CCMenu * pMenu = CCMenu::create(pBackItem, pPauseItem, NULL);
	pMenu->ignoreAnchorPointForPosition(true);
	pMenu->setAnchorPoint(ccp(0, 0));
	pMenu->setPosition(CCPointZero);
	addChild(pMenu);

	//创建物理世界
	b2Vec2 gravity(0.0f, -9.8f);
	m_pGameWorld = new Box2dGameWorld(gravity);
	m_pWorld = m_pGameWorld->GetWorld();
	m_pGameWorld->SetDebugDraw(true);

	m_pTileMapLayer = TileMapLayer::create(m_pGameWorld);
	addChild(m_pTileMapLayer, -1);

	{
		m_pHero = Hero::createWithSpriteFrameName("flying1.png", m_pWorld);

		b2Fixture * fixture = m_pGameWorld->CreateCircleBody(m_pHero, ccp(100, m_Size.height - 100),
			b2_dynamicBody, ccp(-8 / fScaleFactor, 38 / fScaleFactor), 25.0f / fScaleFactor);
		b2Body * pBody = m_pHero->getBody();
		pBody->SetFixedRotation(true);
		m_pHero->EnbaleGravity(false);

		b2Filter filter;
		filter.categoryBits = 0x0004;
		filter.maskBits = 0xffff;

		fixture->SetFilterData(filter);

		b2Vec2 vertices[4];
		vertices[0].Set(4.0f, 15.0f);
		vertices[1].Set(4.0f, -78.0f);
		vertices[2].Set(30.0f, -78.0f);
		vertices[3].Set(30.0f, 15.0f);
		m_pGameWorld->AddPolygonFixture(pBody, vertices, 4)->SetFilterData(filter);

		addChild(m_pHero, 10, TagHero);
	}


	//设置游戏监听器
	m_pGameContactListener = new GameContactListener(this, m_bCheat);
	m_pWorld->SetContactListener(m_pGameContactListener);

	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/bkm_jy.mp3", true);

	return true;
}

void GameScene::onEnter()
{
	CCLayer::onEnter();
	srand((unsigned int)time(NULL));

	this->scheduleUpdate();

	m_pHero->ReadyFlying();
	scheduleOnce(schedule_selector(GameScene::CanScrollView), 2.0f);
	schedule(schedule_selector(GameScene::UpdateRollLength), 5.0f);
}

void GameScene::draw()
{
	CCLayer::draw();

	//m_pGameWorld->BeginDraw();
}

void GameScene::update(float fDelta)
{
	if (m_bScrollView)
	{
		m_pTileMapLayer->UpdateMap(m_fRollLength);
	}

	//更新距离
	m_nDistance += m_fRollLength;
	char szLength[12];
	memset(szLength, 0, sizeof(szLength));
	snprintf(szLength, sizeof(szLength), "%d", m_nDistance);
	m_pDistance->setString(szLength);

	m_pGameWorld->Step();

	for (std::vector<cocos2d::CCNode *>::iterator iter = m_vRemoveNode.begin(); iter != m_vRemoveNode.end(); ++iter)
	{
		CCBox2dSprite * b1 = dynamic_cast<CCBox2dSprite *>(*iter);
		(*iter)->removeFromParent();
	}
	m_vRemoveNode.clear();
}

bool GameScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_bScrollView)
	{
		if (m_bEnd)
		{
			CCScene *pScene = MenuLayer::scene();
			CCDirector::sharedDirector()->replaceScene(pScene);
		}
		return false;
	}

	if (! m_bPause)
	{
		b2Body * pBody = m_pHero->getBody();
		pBody->SetLinearVelocity(b2Vec2(0.0f, 4.9f));
		m_pHero->jump();
	}

	return true;
}

void GameScene::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{

}

void GameScene::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	b2Body * pBody = m_pHero->getBody();

	m_pHero->drop();

	return ;
}

void GameScene::ChangeBlood(float fValue)
{
	if (!m_bScrollView)
		return ;

	if (fValue < 0)
	{
		m_pHero->hit();
	}
	m_fFlood += fValue;
	if (m_fFlood > 100.0f)
		m_fFlood = 100.0f;
	m_pFill->setPercentage(m_fFlood);

	if (m_fFlood <= 0.0001f)
		GameEnd();
}

void GameScene::AddRemoveSprite(cocos2d::CCNode * pNode)
{
	if (!m_bScrollView)
		return ;
	std::vector<cocos2d::CCNode *>::iterator iterFind = std::find(m_vRemoveNode.begin(), m_vRemoveNode.end(), pNode);
	if (iterFind != m_vRemoveNode.end())
		return ;

	CCBox2dSprite * b1 = dynamic_cast<CCBox2dSprite *>(pNode);

	m_vRemoveNode.push_back(pNode);
}

void GameScene::CanScrollView(float dt)
{
	m_pBgLayer->scrollBackGround();

	m_bScrollView = true;
}

void GameScene::UpdateRollLength(float dt)
{
	m_fRollScale += 0.01f;
	if (m_fRollScale > 3.0f)
		m_fRollScale = 3.0f;
	m_fRollLength = g_fBgRollLength2 * m_fRollScale;
	m_pBgLayer->setRollScale(m_fRollScale);
}

void GameScene::GameEnd()
{
	m_bScrollView = false;
	m_pBgLayer->stopScroll();

	CCSprite * pEnd = CCSprite::create("over.png");
	pEnd->setPosition(ccp(m_Size.width / 2, m_Size.height / 2));
	pEnd->setScale(0.5f);
	pEnd->runAction(CCSequence::create(CCScaleTo::create(1.0f, 1.0f),
		CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(GameScene::DelayEnd)), NULL));
	this->addChild(pEnd, 20);

	if (m_fFlood <= 0.0001f)
		m_pHero->die();

	unscheduleUpdate();
	unschedule(schedule_selector(GameScene::UpdateRollLength));
}

void GameScene::DelayEnd()
{
	m_bEnd = true;
}

void GameScene::menuBack(CCObject * pSender)
{
	CCScene *pScene = MenuLayer::scene();
	CCDirector::sharedDirector()->replaceScene(pScene);
}

void GameScene::menuPause(CCObject * pSender)
{
	if (!m_bPause)
	{
		unscheduleUpdate();
		m_pBgLayer->stopScroll();
		CCDirector::sharedDirector()->stopAnimation();
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		m_bPause = true;
	}
	else
	{
		scheduleUpdate();
		m_pBgLayer->scrollBackGround();
		CCDirector::sharedDirector()->startAnimation();
		SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		m_bPause = false;
	}
}

//把游戏中要用到的动画加载到动画缓存
void GameScene::LoadGameAnimationCatch()
{
	char * szAnimationName[5] = {"attack", "bird", "flying", "drop", "die",};
	int AnimationCount[5] = {6, 9, 3, 3, 4};
	float fDelay[5] = {0.05f, 0.05f, 0.05f, 0.05f, 0.05f};

	for (int i = 0; i < CountArray(szAnimationName); ++i)
	{
		CCAnimation * pAnimation = CreateAnimationByName(szAnimationName[i], AnimationCount[i], fDelay[i]);
		CCAnimationCache::sharedAnimationCache()->addAnimation(pAnimation, szAnimationName[i]);
	}
}
