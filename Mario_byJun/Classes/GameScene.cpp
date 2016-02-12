#include "GameScene.h"
#include "CCBox2dSprite.h"
#include "UsuallyFunctions.h"
#include "GameConstant.h"
#include "GameUiLayer.h"
#include "TileMapLayer.h"
#include "GameContactListener.h"
#include "TileSprite.h"
#include "MonsterBase.h"
#include "MonsterTortoise.h"
#include "AudioEngine.h"

USING_NS_CC;
using cocos2d::experimental::AudioEngine;

GameScene * GameScene::m_shareGameScene = nullptr;
static float g_fCloundLayerWidth;	//云彩图层宽度

GameScene::GameScene()
:m_pGameWorld(nullptr)
, m_pWorld(nullptr)
, m_pMapLayer(nullptr)
, m_pUiLayer(nullptr)
, m_pMario(nullptr)
, m_pCloundLayer1(nullptr)
, m_pCloundLayer2(nullptr)
, m_pGameContactListener(nullptr)
, m_nBgMusicId(0)
, m_bMarioAddLife(false)
, m_bMarioReduceLife(false)
, m_bflagAtBottom(false)
, m_bCelebrateFinish(false)
, m_bGameOver(false)
{

}

GameScene::~GameScene()
{
	//清空数组
	m_vContactTile.clear();
	m_vRemoveFixture.clear();
	m_vRemoveSprite.clear();
	m_vMonsterSprite.clear();
	m_vResetMonster.clear();
	//预先删除物理世界精灵
	b2Body * node = m_pWorld->GetBodyList();
	while (node)
	{
		b2Body * b = node;
		node = node->GetNext();
		Box2dSprite * pSprite = static_cast<Box2dSprite *>(b->GetUserData());
		pSprite->removeFromParent();
		//assert(false);	类型需进一步确定
	}

	delete m_pGameWorld;
	delete m_pGameContactListener;
	m_shareGameScene = nullptr;
}

GameScene * GameScene::getInstance()
{
	if (!m_shareGameScene)
	{
		m_shareGameScene = new GameScene();
		m_shareGameScene->init();
		m_shareGameScene->autorelease();
	}

	return m_shareGameScene;
}

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::getInstance();
    scene->addChild(layer);

    return scene;
}

bool GameScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//加载精灵帧图片
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("common.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("mario.plist");
	//加载游戏动画缓存
	LoadGameAnimationCatch();
	//加载星星纹理
	Director::getInstance()->getTextureCache()->addImage("particle/star.png");

	//创建物理世界
	b2Vec2 gravity(0.0f, -80.0f);
	m_pGameWorld = new Box2dGameWorld(gravity);
	m_pWorld = m_pGameWorld->GetWorld();
	m_pGameWorld->SetDebugDraw(true);

	//设置接触监听器
	m_pGameContactListener = new GameContactListener(this);
	m_pWorld->SetContactListener(m_pGameContactListener);

	//创建背景、云彩层,使用默认摄像机
	{
		Sprite * pBgSprite = Sprite::create("Bg/bg3_1.jpg");
		pBgSprite->setAnchorPoint(Vec2::ZERO);
		pBgSprite->setPosition(Vec2(0.0f, 0.0f));
		this->addChild(pBgSprite);

		//构造云彩图层
		g_fCloundLayerWidth = Director::getInstance()->getWinSize().width * 1.5f;
		m_pCloundLayer1 = Layer::create();
		m_pCloundLayer1->setPosition(Vec2(0.0f, 0.0f));
		fillCloundLayer(m_pCloundLayer1);
		this->addChild(m_pCloundLayer1);

		m_pCloundLayer2 = Layer::create();
		m_pCloundLayer2->setPosition(Vec2(g_fCloundLayerWidth, 0.0f));
		fillCloundLayer(m_pCloundLayer2);
		this->addChild(m_pCloundLayer2);
	}

	//创建地图层
	m_pMapLayer = TileMapLayer::create(m_pGameWorld);
	this->addChild(m_pMapLayer, 10);
	m_fMapWidth = m_pMapLayer->getTileMap()->getContentSize().width;

	//创建马里奥主角
	{
		m_pMario = MarioSprite::createWithSpriteFrameName("mario/r_walk/r_walk1.png", m_pWorld);
		m_pMario->setAnchorPoint(Vec2(0.5f, 0.1f));
		m_pMario->setMarioLevel(1);
		TMXTiledMap * tileMap = m_pMapLayer->getTileMap();
		Vec2 posMario(250, 2 * tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR());
		m_pMario->updateFixture(posMario);
		m_pMario->setCameraMask(2);
		
		this->addChild(m_pMario, 15, GSTag_Mario);
		Animation * pAnimation = AnimationCache::getInstance()->getAnimation("rightWalk");
		m_pMario->runAction(Animate::create(pAnimation));
	}

	//创建UI图层
	m_pUiLayer  = GameUiLayer::create();
	this->addChild(m_pUiLayer, 20);
	m_pUiLayer->SetMario(m_pMario);

	m_pWinInfo = Sprite::create("Bg/win.jpg");
	Size winSize = Director::getInstance()->getWinSize();
	m_pWinInfo->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	m_pWinInfo->setCameraMask(m_pUiLayer->getCameraMask());
	m_pWinInfo->setVisible(false);
	m_pUiLayer->addChild(m_pWinInfo, 10);

	//创建地图用相机
	Camera * cameraMap = Camera::create();
	cameraMap->setCameraFlag(CameraFlag::USER1);
	cameraMap->setDepth(10);
	this->addChild(cameraMap, -1);
	m_pMapLayer->setCameraMask(2);

	//更新摄像机
	schedule(schedule_selector(GameScene::updateMap));

	return true;
}

void GameScene::onEnter()
{
	Layer::onEnter();
	srand((unsigned int)time(0));
	m_nBgMusicId = AudioEngine::play2d("sound/music/game_map1.mp3", true);

	this->scheduleUpdate();
}

void GameScene::onExit()
{
	Layer::onExit();

	AudioEngine::stop(m_nBgMusicId);
	this->unscheduleUpdate();
}

void GameScene::updateMap(float dt)
{
	auto cameras = this->getScene()->getCameras();
	for (const auto & cameraMap : cameras)
	{
		if (cameraMap->getCameraFlag() == CameraFlag::USER1)
		{
			Vec2 marioPos = m_pMario->getPosition();
			Size size = Director::getInstance()->getWinSize();
			float cameraX = (marioPos.x < size.width / 2) ? size.width / 2 : marioPos.x;
			cameraX = (cameraX > m_fMapWidth - size.width / 2) ? (m_fMapWidth - size.width / 2) : cameraX;

			Vec3 eyePosOld = cameraMap->getPosition3D();
			Vec3 eyePosNew = Vec3(cameraX, eyePosOld.y, eyePosOld.z);
			cameraMap->setPosition3D(eyePosNew);
			cameraMap->lookAt(Vec3(eyePosNew.x, eyePosNew.y, 0));
			m_fCameraMoveDistance = cameraX - eyePosOld.x;

			//更新背景位置
			if (cameraX != (m_fMapWidth - size.width / 2) && cameraX != (size.width / 2))
				m_pMapLayer->updateBgLayerPos(m_pMario);

			break;
		}
	}
}

void GameScene::update(float dt)
{
	//更新云彩图层位置
	float fChangeLength = MAX(0.1f, 2.0f + m_fCameraMoveDistance * 0.5);
	m_pCloundLayer1->setPositionX(m_pCloundLayer1->getPositionX() - fChangeLength);
	m_pCloundLayer2->setPositionX(m_pCloundLayer2->getPositionX() - fChangeLength);

	if (m_pCloundLayer1->getPositionX() < -g_fCloundLayerWidth - 10.0f)
	{
		m_pCloundLayer1->setPositionX(m_pCloundLayer2->getPositionX() + g_fCloundLayerWidth);
	}
	if (m_pCloundLayer2->getPositionX() < -g_fCloundLayerWidth - 10.0f)
	{
		m_pCloundLayer2->setPositionX(m_pCloundLayer1->getPositionX() + g_fCloundLayerWidth);
	}

	//执行物理引擎
	m_pGameWorld->Step();

	//处理碰撞土块
	for (auto iter = m_vContactTile.begin(); iter != m_vContactTile.end(); ++iter)
	{
		(*iter)->handleContact(m_pMario);
	}
	m_vContactTile.clear();

	//复位怪兽
	for (auto pMonster : m_vResetMonster)
	{
		pMonster->setInvalidStatus();
	}
	m_vResetMonster.clear();

	//移除框架
	for (auto pSprite : m_vRemoveFixture)
	{
		removeBodyFixture(pSprite->getBody());
	}
	m_vRemoveFixture.clear();

	//移除精灵
	for (auto iter = m_vRemoveSprite.begin(); iter != m_vRemoveSprite.end(); ++iter)
	{
		if (((*iter)->getTag() & 0x1000) != 0)	//怪兽移除
		{
			for (auto iterMonster = m_vMonsterSprite.begin(); iterMonster != m_vMonsterSprite.end(); ++iterMonster)
			{
				if ((*iterMonster) == (*iter))
				{
					m_vMonsterSprite.erase(iterMonster);
					break;
				}
			}
		}
		(*iter)->removeFromParent();
	}
	m_vRemoveSprite.clear();

	//处理怪兽
	for (auto pMonster : m_vMonsterSprite)
	{
		enPosStatus posStatus = pMonster->getPosStatus(m_pMario);
		enMonsterStatus monsterStatus = pMonster->getMonsterStatus();

		if (posStatus == PosStatus_Invlid)
		{
			if (monsterStatus != MonsterStatus_Invalid)
			{
				pMonster->setInvalidStatus();
			}
		}
		else if (posStatus == PosStatus_NoShow)
		{
			if (monsterStatus != MonsterStatus_Invalid)
			{
				pMonster->setInvalidStatus();
			}
		}
		else if (posStatus == PosStatus_Move)
		{
			if (monsterStatus != MonsterStatus_Normal)
			{
				pMonster->setNormalStatus();
			}
		}
		else
		{
			CC_ASSERT(false);
		}
	}

	//马里奥增减生命值
	if (m_bMarioAddLife)
	{
		m_pMario->addLife();
		m_bMarioAddLife = false;
	}

	if (m_bMarioReduceLife)
	{
		m_pMario->reduceLife();
		m_bMarioReduceLife = false;
	}
}

void GameScene::addContactTile(TileSprite * tile)
{
	CC_ASSERT(tile != nullptr);
	if (tile == nullptr)
		return;

	//判断土块是否已存在
	bool bHaved = false;
	for (auto iter = m_vContactTile.begin(); iter != m_vContactTile.end(); ++iter)
	{
		if ((*iter) == tile)
		{
			bHaved = true;
			break;
		}
	}
	if (bHaved)
		return;

	m_vContactTile.pushBack(tile);
}

void GameScene::addRemoveSprite(Box2dSprite * pSprite, enGameSpriteTag tag)
{
	CC_ASSERT(pSprite != nullptr);
	if (pSprite == nullptr)
		return;

	//判断精灵是否已存在
	bool bHaved = false;
	for (auto iter = m_vRemoveSprite.begin(); iter != m_vRemoveSprite.end(); ++iter)
	{
		if ((*iter) == pSprite)
		{
			bHaved = true;
			break;
		}
	}
	if (bHaved)
		return;

	if (tag == GSTag_Coin)
	{
		addCoinCount();
		auto coinParticle = ParticleSystemQuad::create("particle/star1.plist");
		coinParticle->setPosition(pSprite->getPosition());
		coinParticle->setCameraMask(pSprite->getCameraMask());
		this->addChild(coinParticle, 11);
	}

	m_vRemoveSprite.pushBack(pSprite);
}

void GameScene::addMonster(MonsterBase * pMonster)
{
	CC_ASSERT(pMonster != false);
	if (pMonster == nullptr)
		return;

	//判断怪兽是否已存在
	bool bHaved = false;
	for (auto pSprite : m_vMonsterSprite)
	{
		if (pSprite == pMonster)
		{
			bHaved = true;
			break;
		}
	}
	if (bHaved)
		return;

	m_vMonsterSprite.pushBack(pMonster);
}

void GameScene::resetMonster(MonsterBase * pMonster)
{
	CC_ASSERT(pMonster != false);
	if (pMonster == nullptr)
		return;

	//判断怪兽是否已存在
	bool bHaved = false;
	for (auto pSprite : m_vResetMonster)
	{
		if (pSprite == pMonster)
		{
			bHaved = true;
			break;
		}
	}
	if (bHaved)
		return;

	m_vResetMonster.pushBack(pMonster);
}

void GameScene::getBigCoin(Box2dSprite * pBigCoin)
{
	CC_ASSERT(pBigCoin != nullptr);
	pBigCoin->stopAllActions();
	Animation * pAnimation = AnimationCache::getInstance()->getAnimation("fastBigCoin");
	pBigCoin->runAction(Animate::create(pAnimation));
	int nHeightLevel = pBigCoin->getPositionY() / 100;
	Sequence * pSequence = Sequence::create(
		DelayTime::create(1.0f),
		MoveTo::create(nHeightLevel * 0.15f, Vec2(pBigCoin->getPositionX(), -200.0f)),
		CallFunc::create(CC_CALLBACK_0(GameScene::addCoinCount, this, 10)),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, pBigCoin)),
		nullptr
		);
	pBigCoin->runAction(pSequence);

	//添加粒子特效
	auto particle = ParticleSystemQuad::create("particle/star2.plist");
	particle->setPosition(pBigCoin->getPosition());
	particle->setCameraMask(pBigCoin->getCameraMask());
	this->addChild(particle, 11);

	m_vRemoveFixture.pushBack(pBigCoin);
}

void GameScene::timeLack()
{
	AudioEngine::stop(m_nBgMusicId);
	int nLackId = playEffect(Effect_TimeLack);
	scheduleOnce(schedule_selector(GameScene::playLackMusic), AudioEngine::getDuration(nLackId));
}

void GameScene::playLackMusic(float dt)
{
	m_nBgMusicId = AudioEngine::play2d("sound/music/game_map1_allegro.mp3");
}

void GameScene::fillCloundLayer(Layer * pCloundLayer)
{
	SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("object/clound1.png");
	int nFillCount = static_cast<int>(g_fCloundLayerWidth / frame->getRect().size.width - 1);
	CCASSERT(nFillCount >= 1, "the cloundlayer width is small , can't fill one clound");
	float fDistance = g_fCloundLayerWidth / nFillCount;	//云彩间距
	float fHeightMin = 500.0f;	//最小云彩高度

	Sprite * pClound = nullptr;
	for (int i = 0; i < nFillCount; ++i)
	{
		pClound = Sprite::createWithSpriteFrame(frame);
		pClound->setAnchorPoint(Vec2::ZERO);
		pClound->setPosition(Vec2(fDistance * i, fHeightMin + rand() % 100));
		pCloundLayer->addChild(pClound);
	}
}

void GameScene::addLife()
{
	m_bMarioAddLife = true;
}

void GameScene::reduceLife()
{
	m_bMarioReduceLife = true;
}

void GameScene::marioDie()
{
	Size winSize = Director::getInstance()->getWinSize();
	LayerColor * colorLayer = LayerColor::create(Color4B(0, 0, 0, 255), winSize.width, winSize.height);
	colorLayer->ignoreAnchorPointForPosition(false);
	colorLayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	Sprite * stencil = Sprite::createWithSpriteFrameName("object/shade.png");
	stencil->setScale(4.0f);
	ClippingNode * clipping = ClippingNode::create(stencil);
	clipping->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	clipping->setInverted(true);
	clipping->setAlphaThreshold(0.0f);
	clipping->addChild(colorLayer);
	clipping->setCameraMask((unsigned short)CameraFlag::USER2);
	this->addChild(clipping, 100);

	//模板执行缩放动作
	Action * scaleAction = Sequence::create(
		ScaleTo::create(0.5f, 0.5f),
		Hide::create(),
		DelayTime::create(0.5f),
		CallFunc::create(CC_CALLBACK_0(GameScene::gameOver, this)),
		nullptr
		);
	stencil->runAction(scaleAction);

	AudioEngine::stop(m_nBgMusicId);
}

void GameScene::marioWin()
{
	//初始化
	m_pMario->stopAllActions();
	m_pMario->unscheduleUpdate();
	m_pMario->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	m_pUiLayer->setTouchUseful(false);
	m_bflagAtBottom = false;
	m_bCelebrateFinish = false;

	m_pMapLayer->slideFlag();	//滑落旗杆

	//马里奥执行动画
	Animation * leftTumble = AnimationCache::getInstance()->getAnimation("leftTumble");
	Animation * leftStanding = AnimationCache::getInstance()->getAnimation("leftStanding");
	Animation * leftWalk = AnimationCache::getInstance()->getAnimation("leftWalk");
	leftStanding->setRestoreOriginalFrame(false);
	Sequence * sequence = Sequence::create(
		Spawn::create(
		JumpBy::create(0.5f, Vec2(200.0f, 100.0f), 50.0f, 1),
		Animate::create(leftTumble),
		nullptr),
		Animate::create(leftStanding),
		CallFunc::create(CC_CALLBACK_0(GameScene::finishCelebrate, this)),
		Animate::create(leftWalk),
		nullptr
		);

	m_pMario->runAction(sequence);

	AudioEngine::stop(m_nBgMusicId);
	playEffect(Effect_DropFlag);
	playEffect(Effect_Win);
}

void GameScene::flagHaveAtBottom()
{
	m_bflagAtBottom = true;
	if (m_bCelebrateFinish)
	{
		marioRunOver();
	}
}

void GameScene::finishCelebrate()
{
	m_bCelebrateFinish = true;
	b2Vec2 oldVelocity = m_pMario->getBody()->GetLinearVelocity();
	m_pMario->getBody()->SetLinearVelocity(b2Vec2(0.0f, oldVelocity.y));
	if (m_bflagAtBottom)
	{
		marioRunOver();
	}
}

void GameScene::marioRunOver()
{
	Animation * rightRun = AnimationCache::getInstance()->getAnimation("rightRun");
	m_pMario->stopAllActions();
	m_pMario->runAction(Animate::create(rightRun));
	m_pMario->runAction(MoveBy::create(2.5f, Vec2(600.0f, 0.0f)));
	scheduleOnce(schedule_selector(GameScene::showWinInfo), 1.0f);
}

void GameScene::showWinInfo(float dt)
{
	m_pWinInfo->setVisible(true);
	m_pWinInfo->setOpacity(0);

	Sequence * sequence = Sequence::create(
		FadeIn::create(1.0f),
		DelayTime::create(3.0f),
		CallFunc::create(CC_CALLBACK_0(GameScene::gameOver, this)),
		nullptr
		);
	m_pWinInfo->runAction(sequence);
}

void GameScene::gameOver()
{
	if (m_bGameOver == false)
	{
		Director::getInstance()->popScene();
		m_bGameOver = true;
	}
}

void GameScene::removeBodyFixture(b2Body * pBody)
{
	CC_ASSERT(pBody);
	if (pBody == nullptr)
		return;

	for (b2Fixture * fixture = pBody->GetFixtureList(); fixture;)
	{
		b2Fixture * next = fixture->GetNext();
		pBody->DestroyFixture(fixture);
		fixture = next;
	}
}

void GameScene::LoadGameAnimationCatch()
{
	//地图
	{
		char * szAnimationName[7] = { "fastCoin", "coin", "bigCoin", "fastBigCoin",
		"specialTile", "generalTile", "winFlag"};
		char * szAnimationFileName[7] = { "coin/coin", "coin/coin", "coin/big_coin", "coin/big_coin",
			"tile/special_tile", "tile/general_tile", "flag/flag"};
		int AnimationCount[7] = { 4, 4, 7, 7,
			16, 19, 10,};
		float fDelay[7] = { 0.05f, 0.1f, 0.1f, 0.03f,
			1.0f / 16, 1.0f / 19, 0.05f, };
		int nLoops[7] = { -1, -1, -1, -1,
			-1, -1, -1};

		for (int i = 0; i < CountArray(szAnimationName); ++i)
		{
			Animation * pAnimation = CreateAnimationByName(szAnimationFileName[i], AnimationCount[i], fDelay[i], nLoops[i]);
			AnimationCache::getInstance()->addAnimation(pAnimation, szAnimationName[i]);
		}
	}

	//马里奥
	{
		char * szAnimationName[13] = { "leftWalk", "leftRun", "leftJump", "leftDown",
			"rightWalk", "rightRun", "rightJump", "rightDown",
			"marioDie",
			"leftTumble", "leftStanding", "rightTumble", "rightStanding",
		};
		char * szAnimationFileName[13] = { "mario/l_walk/l_walk", "mario/l_run/l_run", "mario/l_jump/l_jump", "mario/l_down/l_down",
			"mario/r_walk/r_walk", "mario/r_run/r_run", "mario/r_jump/r_jump", "mario/r_down/r_down",
		"mario/die/die",
		"mario/l_tumble/l_tumble", "mario/l_standing/l_standing", "mario/r_tumble/r_tumble", "mario/r_standing/r_standing"
		};
		int AnimationCount[13] = { 27, 25, 10, 1,
			27, 25, 10, 1,
			13,
			11, 15, 11, 15,
		};
		float fDelay[13] = { 0.1f, 0.012f, 0.015f, 0.1f,
			0.1f, 0.012f, 0.015f, 0.1f,
			0.03f,
			0.02f, 0.02f, 0.02f, 0.02f,
		};
		int nLoops[13] = { -1, -1, 1, 1,
			-1, -1, 1, 1,
			1,
			1, 1, 1, 1,
		};
	
		for (int i = 0; i < CountArray(szAnimationName); ++i)
		{
			Animation * pAnimation = CreateAnimationByName(szAnimationFileName[i], AnimationCount[i], fDelay[i], nLoops[i]);
			AnimationCache::getInstance()->addAnimation(pAnimation, szAnimationName[i]);
		}
	}

	//怪物
	{
		char * szAnimationName[7] = { "robbitLeft", "robbitLeftDie", "robbitRight", "robbitRightDie",
			"tortoiseLeft", "tortoiseRight", "tortoiseDefend", };
		char * szAnimationFileName[7] = { "monster/robbit_left/robbit_left", "monster/robbit_left_die/robbit_left_die",
			"monster/robbit_right/robbit_right", "monster/robbit_right_die/robbit_right_die",
			"monster/tortoise_left/tortoise_left", "monster/tortoise_right/tortoise_right",
			"monster/tortoise_defend/tortoise_defend", };
		int AnimationCount[7] = { 4, 2, 4, 2, 4, 4, 3, };
		float fDelay[7] = { 0.2f, 0.5f, 0.2f, 0.5f, 0.2f, 0.2f, 0.05f, };
		int nLoops[7] = { -1, 1, -1, 1, -1, -1, -1 };

		for (int i = 0; i < CountArray(szAnimationName); ++i)
		{
			Animation * pAnimation = CreateAnimationByName(szAnimationFileName[i], AnimationCount[i], fDelay[i], nLoops[i]);
			AnimationCache::getInstance()->addAnimation(pAnimation, szAnimationName[i]);
		}
	}
}
