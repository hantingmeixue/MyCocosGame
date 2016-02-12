#include "LoadingScene.h"
#include "GameScene.h"
#include "GameConstant.h"
#include "UsuallyFunctions.h"
#include "AudioEngine.h"

USING_NS_CC;
using cocos2d::experimental::AudioEngine;

LoadingScene::LoadingScene()
{

}

LoadingScene::~LoadingScene()
{

}

Scene* LoadingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoadingScene::create();
	scene->addChild(layer);

	return scene;
}

bool LoadingScene::init()
{
	if (!Layer::init())
		return false;

	Size winSize = Director::getInstance()->getWinSize();

	auto colorLayer = CCLayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(colorLayer);

	Sprite * pMario = Sprite::createWithSpriteFrameName("object/mario.png");
	pMario->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(pMario, 1);

	return true;
}

void LoadingScene::onEnter()
{
	Layer::onEnter();
	scheduleOnce(schedule_selector(LoadingScene::addGameScene), 0.1f);
}

void LoadingScene::onExit()
{
	Layer::onExit();
}

void LoadingScene::addGameScene(float dt)
{
	preloadSound();
	Scene * gameScene = GameScene::createScene();
	Director::getInstance()->replaceScene(gameScene);
	unschedule(schedule_selector(LoadingScene::addGameScene));
}

void LoadingScene::preloadSound()
{
	//‘§º”‘ÿ“Ù¿÷
	AudioEngine::preload("sound/music/game_map1.mp3");
	AudioEngine::preload("sound/music/game_map1_allegro.mp3");

	//‘§º”‘ÿ“Ù–ß
	for (int i = 0; i < CountArray(g_szEffects); ++i)
	{
		AudioEngine::preload(g_szEffects[i]);
	}
}
