#include "MenuScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
USING_NS_CC;

CCScene* MenuLayer::scene()
{
	CCScene * scene = CCScene::create();
	MenuLayer * layer = MenuLayer::create();
	scene->addChild(layer);

	return scene;
}

bool MenuLayer::init()
{
	if (!CCLayer::init())
		return false;

	m_bCheat = false;
	CCSpriteFrameCache * pFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	pFrameCache->addSpriteFramesWithFile("common1.plist");


	CCSize size = CCDirector::sharedDirector()->getWinSize();
	//创建背景
	CCSprite * pBg = CCSprite::create("menu.jpg");
	pBg->setPosition(ccp(size.width / 2, size.height / 2));
	addChild(pBg);

	CCMenuItemImage * pBeginItem = CCMenuItemImage::create(
		"start1.png", "start2.png", this, menu_selector(MenuLayer::menuBegin));
	pBeginItem->setPosition(ccp(size.width / 4, size.height / 2));

	CCMenuItemSprite * pPauseItem= CCMenuItemSprite::create(
		CCSprite::createWithSpriteFrameName("exit.png"), CCSprite::createWithSpriteFrameName("exit.png"),
		this, menu_selector(MenuLayer::menuExit));
	pPauseItem->setAnchorPoint(ccp(1, 1));
	pPauseItem->setPosition(ccp(size.width - 10, size.height -10));

	CCMenuItemSprite * pCheatItem = CCMenuItemSprite::create(
		CCSprite::createWithSpriteFrameName("cheat.png"), CCSprite::createWithSpriteFrameName("cheat.png"),
		this, menu_selector(MenuLayer::menuCheat));
	pCheatItem->setAnchorPoint(ccp(1, 0));
	pCheatItem->setPosition(ccp(size.width, 0));

	CCMenu * pMenu = CCMenu::create(pBeginItem, pPauseItem, pCheatItem, NULL);
	pMenu->ignoreAnchorPointForPosition(true);
	pMenu->setPosition(CCPointZero);
	addChild(pMenu);

	//上下摇动动画
	CCSprite * pGameLogo = CCSprite::create("title.png");
	pGameLogo->setPosition(ccp(size.width * 3 / 4, size.height / 2 - 15));
	addChild(pGameLogo);

	CCSequence * pSwingAction = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 30)),
		CCMoveBy::create(0.5f, ccp(0, -30)), NULL);
	pGameLogo->runAction(CCRepeatForever::create(pSwingAction));

	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sound/bkm_jy.mp3");

	return true;
}

void MenuLayer::menuBegin(CCObject * pSender)
{
	CCScene * pScene = GameScene::scene(m_bCheat);

	CCDirector::sharedDirector()->replaceScene(pScene);
}

void MenuLayer::menuExit(CCObject * pSender)
{
	CCDirector::sharedDirector()->end();
}

void MenuLayer::menuCheat(CCObject * pSender)
{
	m_bCheat = true;
}
