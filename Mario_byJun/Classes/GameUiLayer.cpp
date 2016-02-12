#include "GameUiLayer.h"
#include "Box2D/Box2D.h"
#include "GameScene.h"

USING_NS_CC;

GameUiLayer::GameUiLayer()
: m_pjumpNormalImage(nullptr)
, m_pjumpSelectedImage(nullptr)
, m_pFontCoinCount(nullptr)
, m_pFontRemainTime(nullptr)
, m_pMario(nullptr)
, m_bTouchUseful(true)
, m_bKeyAPressed(false)
, m_bKeySPressed(false)
, m_bKeyDPressed(false)
, m_bKeyWPressed(false)
, m_bKeyKPressed(false)
, m_nCoinCount(0)
, m_nRemainTime(400)
{

}

bool GameUiLayer::init()
{
	if (!Layer::init())
		return false;

	Size contentSize = Director::getInstance()->getWinSize();
	//创建分数相关(cocos编辑器实现)

	//添加方向按钮背景
	Sprite * directionBg = Sprite::createWithSpriteFrameName("ui/action_bg.png");
	directionBg->setAnchorPoint(Vec2::ZERO);
	directionBg->setPosition(Vec2(10, 10));
	this->addChild(directionBg);

	//创建方向按钮
	const Vec2 & direcBgPos = directionBg->getPosition();
	Sprite * direcSpriteN = Sprite::createWithSpriteFrameName("ui/action_button.png");
	Sprite * direcSpriteS = Sprite::createWithSpriteFrameName("ui/action_button.png");
	auto topItem = MenuItemSprite::create(direcSpriteN, direcSpriteS, CC_CALLBACK_1(GameUiLayer::menuTopCallback, this));
	topItem->setPosition(Vec2(direcBgPos.x + 175 / CC_CONTENT_SCALE_FACTOR(), direcBgPos.y + 290 / CC_CONTENT_SCALE_FACTOR()));

	auto direcMenu = Menu::create(topItem, nullptr);
	direcMenu->setPosition(Vec2::ZERO);
	direcMenu->setAnchorPoint(Vec2::ZERO);
	this->addChild(direcMenu, 1);

	//下方向键
	auto bottomItem = Sprite::createWithSpriteFrameName("ui/action_button.png");
	bottomItem->setPosition(Vec2(direcBgPos.x + 175 / CC_CONTENT_SCALE_FACTOR(), direcBgPos.y + 58 / CC_CONTENT_SCALE_FACTOR()));
	this->addChild(bottomItem, 1);

	auto bottomListener = EventListenerTouchOneByOne::create();
	bottomListener->setSwallowTouches(true);

	bottomListener->onTouchBegan = CC_CALLBACK_2(GameUiLayer::onBottomTouchBegan, this);
	bottomListener->onTouchMoved = CC_CALLBACK_2(GameUiLayer::onBottomTouchMoved, this);
	bottomListener->onTouchEnded = CC_CALLBACK_2(GameUiLayer::onBottomTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(bottomListener, bottomItem);

	//左右方向键
	auto leftItem = Sprite::createWithSpriteFrameName("ui/action_button.png");
	leftItem->setPosition(Vec2(direcBgPos.x + 58 / CC_CONTENT_SCALE_FACTOR(), direcBgPos.y + 175 / CC_CONTENT_SCALE_FACTOR()));
	this->addChild(leftItem, 1);

	auto leftListener = EventListenerTouchOneByOne::create();
	leftListener->setSwallowTouches(true);

	leftListener->onTouchBegan = CC_CALLBACK_2(GameUiLayer::onLeftTouchBegan, this);
	leftListener->onTouchMoved = CC_CALLBACK_2(GameUiLayer::onLeftTouchMoved, this);
	leftListener->onTouchEnded = CC_CALLBACK_2(GameUiLayer::onLeftTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(leftListener, leftItem);

	auto rightItem = Sprite::createWithSpriteFrameName("ui/action_button.png");
	rightItem->setPosition(Vec2(direcBgPos.x + 290 / CC_CONTENT_SCALE_FACTOR(), direcBgPos.y + 175 / CC_CONTENT_SCALE_FACTOR()));
	this->addChild(rightItem, 1);

	auto rightListener = EventListenerTouchOneByOne::create();
	rightListener->setSwallowTouches(true);

	rightListener->onTouchBegan = CC_CALLBACK_2(GameUiLayer::onRightTouchBegan, this);
	rightListener->onTouchMoved = CC_CALLBACK_2(GameUiLayer::onRightTouchMoved, this);
	rightListener->onTouchEnded = CC_CALLBACK_2(GameUiLayer::onRightTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(rightListener, rightItem);

	//创建跳跃按钮背景
	m_pjumpNormalImage = Sprite::createWithSpriteFrameName("ui/jump_bg_n.png");
	m_pjumpNormalImage->setPosition(Vec2(contentSize.width - m_pjumpNormalImage->getContentSize().width, 
		m_pjumpNormalImage->getContentSize().height / 2 + 20));
	m_pjumpNormalImage->setVisible(true);
	this->addChild(m_pjumpNormalImage, 1);

	m_pjumpSelectedImage = Sprite::createWithSpriteFrameName("ui/jump_bg_s.png");
	m_pjumpSelectedImage->setPosition(m_pjumpNormalImage->getPosition());
	m_pjumpSelectedImage->setVisible(false);
	this->addChild(m_pjumpSelectedImage, 1);

	//创建跳跃按钮
	auto jumpItem = Sprite::createWithSpriteFrameName("ui/jump_button.png");
	jumpItem->setPosition(m_pjumpNormalImage->getPosition());
	this->addChild(jumpItem, 1);

	auto jumpListener = EventListenerTouchOneByOne::create();
	jumpListener->setSwallowTouches(true);

	jumpListener->onTouchBegan = CC_CALLBACK_2(GameUiLayer::onJumpTouchBegan, this);
	jumpListener->onTouchMoved = CC_CALLBACK_2(GameUiLayer::onJumpTouchMoved, this);
	jumpListener->onTouchEnded = CC_CALLBACK_2(GameUiLayer::onJumpTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(jumpListener, jumpItem);

	//创建键盘事件
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(GameUiLayer::onKeyPressed, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(GameUiLayer::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	//创建信息界面
	Sprite * pCoinSprite = Sprite::createWithSpriteFrameName("object/coin.png");
	pCoinSprite->setPosition(Vec2(100, 670));
	addChild(pCoinSprite);

	Sprite * pXSprite = Sprite::createWithSpriteFrameName("object/X.png");
	pXSprite->setPosition(Vec2(150, 670));
	addChild(pXSprite);

	Sprite * pClockSprite = Sprite::createWithSpriteFrameName("object/clock.png");
	pClockSprite->setPosition(Vec2(contentSize.width - 150, 670));
	addChild(pClockSprite);

	m_pFontCoinCount = LabelAtlas::create("00", "fonts/score_font.png",
		26 / CC_CONTENT_SCALE_FACTOR(), 60 / CC_CONTENT_SCALE_FACTOR(), 48);
	m_pFontCoinCount->setAnchorPoint(Vec2(0, 0.5f));
	m_pFontCoinCount->setPosition(Vec2(170, 670));
	addChild(m_pFontCoinCount);

	m_pFontRemainTime = LabelAtlas::create("400", "fonts/score_font.png",
		26 / CC_CONTENT_SCALE_FACTOR(), 60 / CC_CONTENT_SCALE_FACTOR(), 48);
	m_pFontRemainTime->setAnchorPoint(Vec2(0, 0.5f));
	m_pFontRemainTime->setPosition(Vec2(contentSize.width - 120, 670));
	addChild(m_pFontRemainTime);

	//设置游戏变量
	m_nCoinCount = 0;
	m_nRemainTime = 400;

	//启动游戏时间定时器
	schedule(schedule_selector(GameUiLayer::updateGameTime), 1.0f);

	//创建UI用摄像机
	Camera * cameraUI = Camera::create();
	cameraUI->setCameraFlag(CameraFlag::USER2);
	cameraUI->setDepth(20);
	this->addChild(cameraUI);
	this->setCameraMask((unsigned short)CameraFlag::USER2);

	return true;
}

void GameUiLayer::menuTopCallback(Ref * sender)
{

}

bool GameUiLayer::onBottomTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (!m_bTouchUseful || (!isTouchUseful(touch, event) && !m_bKeySPressed))
		return false;

	m_pMario->setBottomPress(true);
	return true;
}

void GameUiLayer::onBottomTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event)
{

}

void GameUiLayer::onBottomTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	m_pMario->setBottomPress(false);
}

//左右方向键
bool GameUiLayer::onLeftTouchBegan(Touch * touch, Event * event)
{
	if (!m_bTouchUseful || (!isTouchUseful(touch, event) && !m_bKeyAPressed))
		return false;

	m_pMario->SetLeftPress(true);

	return true;
}

void GameUiLayer::onLeftTouchMoved(Touch * touch, Event * event)
{

}

void GameUiLayer::onLeftTouchEnded(Touch * touch, Event * event)
{
	m_pMario->SetLeftPress(false);
}

bool GameUiLayer::onRightTouchBegan(Touch * touch, Event * event)
{
	if (!m_bTouchUseful || (!isTouchUseful(touch, event) && !m_bKeyDPressed))
		return false;

	m_pMario->SetRightPress(true);

	return true;
}

void GameUiLayer::onRightTouchMoved(Touch * touch, Event * event)
{

}

void GameUiLayer::onRightTouchEnded(Touch * touch, Event * event)
{
	m_pMario->SetRightPress(false);
}

bool GameUiLayer::onJumpTouchBegan(Touch * touch, Event * event)
{
	if (!m_bTouchUseful || (!isTouchUseful(touch, event) && !m_bKeyKPressed))
		return false;

	//更新背景
	m_pjumpNormalImage->setVisible(false);
	m_pjumpSelectedImage->setVisible(true);

	m_pMario->SetJumpPress(true);

	return true;
}
void GameUiLayer::onJumpTouchMoved(Touch * touch, Event * event)
{

}

void GameUiLayer::onJumpTouchEnded(Touch * touch, Event * event)
{
	//更新背景
	m_pjumpNormalImage->setVisible(true);
	m_pjumpSelectedImage->setVisible(false);

	m_pMario->SetJumpPress(false);
}

bool GameUiLayer::isTouchUseful(Touch * touch, Event * event)
{
	//触摸有效判断
	if (touch == nullptr || event == nullptr)
		return false;

	auto target = static_cast<Sprite *>(event->getCurrentTarget());
	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height);
	Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
	if (!rect.containsPoint(locationInNode))
		return false;
	else
		return true;
}

void GameUiLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_S:
		m_bKeySPressed = true;
		onBottomTouchBegan(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_A :
		m_bKeyAPressed = true;
		onLeftTouchBegan(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		m_bKeyDPressed = true;
		onRightTouchBegan(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_K:
		m_bKeyKPressed = true;
		onJumpTouchBegan(nullptr, event);
		break;
	default:
		break;
	}
}

void GameUiLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_S:
		m_bKeySPressed = false;
		onBottomTouchEnded(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_A:
		m_bKeyAPressed = false;
		onLeftTouchEnded(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		m_bKeyDPressed = false;
		onRightTouchEnded(nullptr, event);
		break;
	case EventKeyboard::KeyCode::KEY_K:
		m_bKeyKPressed = false;
		onJumpTouchEnded(nullptr, event);
		break;
	default:
		break;
	}
}

void GameUiLayer::SetMario(MarioSprite * mario)
{
	CC_ASSERT(mario != nullptr);
	m_pMario = mario;
}

void GameUiLayer::addCoinCount(int nCount)
{
	CC_ASSERT(nCount >= 0);
	m_nCoinCount += nCount;

	//当为100时，增加生命值

	char szCoin[64] = { 0 };
	snprintf(szCoin, sizeof(szCoin), "%02d", m_nCoinCount);
	m_pFontCoinCount->setString(szCoin);
}

void GameUiLayer::updateGameTime(float dt)
{
	--m_nRemainTime;
	if (m_nRemainTime <= 0)
	{
		unschedule(schedule_selector(GameUiLayer::updateGameTime));
		//结束游戏
		m_pMario->setMarioDie();

		return;
	}
	else if (m_nRemainTime == 100)
	{
		GameScene::getInstance()->timeLack();
	}

	char szTime[64] = { 0 };
	snprintf(szTime, sizeof(szTime), "%d", m_nRemainTime);
	m_pFontRemainTime->setString(szTime);
}

void GameUiLayer::setTouchUseful(bool bUseful)
{
	m_bTouchUseful = bUseful;
	onLeftTouchEnded(nullptr, nullptr);
	onRightTouchEnded(nullptr, nullptr);
}
