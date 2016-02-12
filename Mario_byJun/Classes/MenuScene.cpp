#include "MenuScene.h"
#include "LoadingScene.h"
#include "GameConstant.h"
#include "UsuallyFunctions.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;
using cocos2d::experimental::AudioEngine;

static float g_fCloundLayerWidth;	//ÔÆ²ÊÍ¼²ã¿í¶È

MenuScene::MenuScene()
: m_pCloundLayer1(nullptr)
, m_pCloundLayer2(nullptr)
{

}

MenuScene::~MenuScene()
{

}

Scene* MenuScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MenuScene::create();
	scene->addChild(layer);

	return scene;
}

bool MenuScene::init()
{
	if (!Layer::init())
		return false;

	Size winSize = Director::getInstance()->getWinSize();
	//¼ÓÔØ¾«ÁéÖ¡Í¼Æ¬
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("common.plist");
	//¼ÓÔØÓÎÏ·¶¯»­»º´æ
	LoadGameAnimationCatch();

	//µ×Í¼±³¾°
	Sprite * pBg1 = Sprite::create("Bg/bg3_1.jpg");
	pBg1->setAnchorPoint(Vec2::ZERO);
	pBg1->setPosition(Vec2::ZERO);
	this->addChild(pBg1, 1);

	//ÔÆ²ÊÍ¼²ã
	g_fCloundLayerWidth = winSize.width * 1.5f;
	m_pCloundLayer1 = Layer::create();
	m_pCloundLayer1->setPosition(Vec2(0.0f, 0.0f));
	fillCloundLayer(m_pCloundLayer1);
	this->addChild(m_pCloundLayer1, 2);

	m_pCloundLayer2 = Layer::create();
	m_pCloundLayer2->setPosition(Vec2(g_fCloundLayerWidth, 0.0f));
	fillCloundLayer(m_pCloundLayer2);
	this->addChild(m_pCloundLayer2, 2);

	//Ô¶¾°Í¼²ã
	pBg1 = Sprite::create("Bg/bg2_1.png");
	pBg1->setAnchorPoint(Vec2::ZERO);
	pBg1->setPosition(Vec2::ZERO);
	this->addChild(pBg1, 3);

	//½ü¾°Í¼²ã
	Sprite * pBg2 = Sprite::create("Bg/bg1_1.png");
	pBg2->setAnchorPoint(Vec2::ZERO);
	pBg2->setPosition(Vec2(120.0f, 0.0f));
	this->addChild(pBg2, 4);

	Sprite * pBg3 = Sprite::create("Bg/bg1_2.png");
	pBg3->setAnchorPoint(Vec2::ZERO);
	pBg3->setPosition(Vec2(pBg2->getPositionX() + pBg2->getContentSize().width - 20, 0.0f));
	this->addChild(pBg3, 4);

	//µØÍ¼Í¼²ã
	TMXTiledMap * m_pMap1 = TMXTiledMap::create("map_menu.tmx");
	this->addChild(m_pMap1, 10);
	float fTileWidth = m_pMap1->getTileSize().width / CC_CONTENT_SCALE_FACTOR();	//Í¼¿é¿í¶È
	float fTileHeight = m_pMap1->getTileSize().height / CC_CONTENT_SCALE_FACTOR();	//Í¼¿é¸ß¶È

	Sprite * pFlower = Sprite::create();
	pFlower->setAnchorPoint(Vec2::ZERO);
	pFlower->setPosition(Vec2(fTileWidth * 6, 2 * fTileHeight));
	Animation * animation = AnimationCache::getInstance()->getAnimation("blueFlower");
	pFlower->runAction(Animate::create(animation));
	this->addChild(pFlower, 10);

	pFlower = Sprite::create();
	pFlower->setAnchorPoint(Vec2::ZERO);
	pFlower->setPosition(Vec2(fTileWidth * 16, 2 * fTileHeight));
	animation = AnimationCache::getInstance()->getAnimation("redFlower");
	pFlower->runAction(Animate::create(animation));
	this->addChild(pFlower, 10);

	//logo±êÖ¾
	Sprite * pLogo = Sprite::createWithSpriteFrameName("object/logo_mario.png");
	pLogo->setAnchorPoint(Vec2(0.0f, 1.0f));
	pLogo->setPosition(Vec2(200.0f, winSize.height - 50.0f));
	this->addChild(pLogo, 11);

	//²Ëµ¥
	Sprite * beginN = Sprite::createWithSpriteFrameName("ui/begin_game_n.png");
	Sprite * beginS = Sprite::createWithSpriteFrameName("ui/begin_game_s.png");
	auto beginItem = MenuItemSprite::create(beginN, beginS, CC_CALLBACK_1(MenuScene::menuBeginCallback, this));
	beginItem->setAnchorPoint(Vec2::ZERO);
	beginItem->setPosition(Vec2(fTileWidth * 11, fTileHeight * 3));

	Sprite * exitN = Sprite::createWithSpriteFrameName("ui/exit_n.png");
	Sprite * exitS = Sprite::createWithSpriteFrameName("ui/exit_s.png");
	auto exitItem = MenuItemSprite::create(exitN, exitS, CC_CALLBACK_1(MenuScene::menuExitCallback, this));
	exitItem->setAnchorPoint(Vec2(0.0f, 0.5f));
	exitItem->setPosition(Vec2(fTileWidth * 13, beginItem->getPositionY() + beginItem->getContentSize().height / 2));

	auto direcMenu = Menu::create(beginItem, exitItem, nullptr);
	direcMenu->setPosition(Vec2::ZERO);
	//direcMenu->setAnchorPoint(Vec2::ZERO);
	this->addChild(direcMenu, 12);

	return true;
}

void MenuScene::onEnter()
{
	Layer::onEnter();
	scheduleUpdate();
	AudioEngine::play2d("sound/music/menu.mp3", true);
}

void MenuScene::onExit()
{
	Layer::onExit();
	unscheduleUpdate();
}

void MenuScene::update(float dt)
{
	//¸üÐÂÔÆ²ÊÍ¼²ã
	m_pCloundLayer1->setPositionX(m_pCloundLayer1->getPositionX() - 2.0f);
	m_pCloundLayer2->setPositionX(m_pCloundLayer2->getPositionX() - 2.0f);

	if (m_pCloundLayer1->getPositionX() < -g_fCloundLayerWidth - 10.0f)
	{
		m_pCloundLayer1->setPositionX(m_pCloundLayer2->getPositionX() + g_fCloundLayerWidth);
	}
	if (m_pCloundLayer2->getPositionX() < -g_fCloundLayerWidth - 10.0f)
	{
		m_pCloundLayer2->setPositionX(m_pCloundLayer1->getPositionX() + g_fCloundLayerWidth);
	}
}

void MenuScene::menuBeginCallback(Ref * sender)
{
	Scene * loadingScene = LoadingScene::createScene();
	Director::getInstance()->pushScene(loadingScene);
	AudioEngine::stopAll();
}

void MenuScene::menuExitCallback(Ref * sender)
{
	Director::getInstance()->end();
}

void MenuScene::fillCloundLayer(Layer * pCloundLayer)
{
	SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("object/clound1.png");
	int nFillCount = static_cast<int>(g_fCloundLayerWidth / frame->getRect().size.width - 1);
	CCASSERT(nFillCount >= 1, "the cloundlayer width is small , can't fill one clound");
	float fDistance = g_fCloundLayerWidth / nFillCount;	//ÔÆ²Ê¼ä¾à
	float fHeightMin = 500.0f;	//×îÐ¡ÔÆ²Ê¸ß¶È

	Sprite * pClound = nullptr;
	for (int i = 0; i < nFillCount; ++i)
	{
		pClound = Sprite::createWithSpriteFrame(frame);
		pClound->setAnchorPoint(Vec2::ZERO);
		pClound->setPosition(Vec2(fDistance * i, fHeightMin + rand() % 100));
		pCloundLayer->addChild(pClound);
	}
}

void MenuScene::LoadGameAnimationCatch()
{
	//µØÍ¼
	{
		char * szAnimationName[2] = { "blueFlower", "redFlower",};
		char * szAnimationFileName[2] = { "environment/blue_flower/blue_flower",
			"environment/red_flower/red_flower",};
		int AnimationCount[2] = { 12, 12,};
		float fDelay[2] = { 0.2f, 0.2f,};
		int nLoops[2] = { -1, -1,};

		for (int i = 0; i < CountArray(szAnimationName); ++i)
		{
			Animation * pAnimation = CreateAnimationByName(szAnimationFileName[i], AnimationCount[i], fDelay[i], nLoops[i]);
			AnimationCache::getInstance()->addAnimation(pAnimation, szAnimationName[i]);
		}
	}
}
