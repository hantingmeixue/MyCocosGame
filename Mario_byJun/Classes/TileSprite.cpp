#include "TileSprite.h"
#include "MarioSprite.h"
#include "GameScene.h"
#include "SwammSprite.h"
#include "GameConstant.h"
#include "TileMapLayer.h"

USING_NS_CC;

TileSprite::TileSprite(b2World * pWorld)
:Box2dSprite(pWorld)
, m_eTileType(TSType_Null)
,m_nCoinCount(0)
, m_bFinishHandleContact(true)
{

}

TileSprite * TileSprite::create(const char *pszFileName, b2World * pWorld)
{
	TileSprite * pRet = new TileSprite(pWorld);
	if (pRet && pRet->initWithFile(pszFileName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

TileSprite * TileSprite::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	TileSprite * pRet = new TileSprite(pWorld);
	if (pRet && pRet->initWithSpriteFrameName(pszFrameName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

void TileSprite::setTileType(enTileSpriteType tileType, int nCoinCount/* = 0*/)
{
	m_eTileType = tileType;
	if (nCoinCount > 0 && tileType != TSType_Coin)
	{
		CCASSERT(false, "tile is't coin type, but coin's count is more than 0!");
		return;
	}
	else if (nCoinCount >= 0)
	{
		m_nCoinCount = nCoinCount;
	}
}

enTileSpriteType TileSprite::getTileType()
{
	return m_eTileType;
}

int TileSprite::getCoinCount()
{
	return m_nCoinCount;
}

//马里奥顶撞土块
void TileSprite::marioContact(MarioSprite * pMario)
{
	if (m_eTileType == TSType_Invalid || m_bFinishHandleContact == false)
		return;
	//顶撞有效判断
	static int n1 = 0;
	Vec2 myPos = getPosition();
	if (isMarioJackUseful(pMario, myPos) == false)
		return;

	GameScene::getInstance()->addContactTile(this);
}

//处理碰撞土块
void TileSprite::handleContact(MarioSprite * pMario)
{
	//无效土块判断
	if (m_eTileType == TSType_Invalid)
		return;

	Vec2 pos = getPosition();
	bool bMarioTumble = (pMario->getMarioStatus() == MarioSt_LTumble || pMario->getMarioStatus() == MarioSt_RTumble);	//是否翻跟头
	m_bFinishHandleContact = false;

	//播放碰撞音效
	if (!bMarioTumble)
	{
		if (m_eTileType == TSType_Null && pMario->getMarioLevel() > 1)
			playEffect(Effect_DestroyTile);
		else
			playEffect(Effect_JackSolid);
	}

	if (m_eTileType == TSType_Null)	//空土块
	{
		if (pMario->getMarioLevel() > 1)	//正常马里奥的碰撞
		{
			GameScene * gameScene = GameScene::getInstance();
			Sprite * broken1 = Sprite::createWithSpriteFrameName("tile/broken_tile1.png");
			broken1->setPosition(Vec2(pos.x - 10, pos.y + 10));
			broken1->setCameraMask(2);
			gameScene->addChild(broken1, 10);

			Sprite * broken2 = Sprite::createWithSpriteFrameName("tile/broken_tile2.png");
			broken2->setPosition(pos);
			broken2->setCameraMask(2);
			gameScene->addChild(broken2, 10);

			Sprite * broken3 = Sprite::createWithSpriteFrameName("tile/broken_tile3.png");
			broken3->setPosition(Vec2(pos.x + 10, pos.y - 20));
			broken3->setCameraMask(2);
			gameScene->addChild(broken3, 10);

			broken1->runAction(Sequence::create(JumpTo::create(bMarioTumble ? 0.5f : 1.0f, Vec2(pos.x - 350, -50.0f), bMarioTumble ? 150 : 300.0f, 1),
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, broken1)), nullptr));

			broken2->runAction(Sequence::create(JumpTo::create(bMarioTumble ? 0.5f : 1.0f, Vec2(pos.x - 50.0f, -50.0f), bMarioTumble ? 150 : 300.0f, 1),
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, broken2)), nullptr));

			broken3->runAction(Sequence::create(JumpTo::create(bMarioTumble ? 0.5f : 1.0f, Vec2(pos.x + 390.0f, -50.0f), bMarioTumble ? 150 : 300.0f, 1),
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, broken3)), nullptr));

			this->removeFromParent();
		}
		else	//缩小版马里奥的碰撞
		{
			//设置基础动作
			ActionInterval * baseAction = Spawn::create(
				MoveBy::create(0.1f, Vec2(0.0f, bMarioTumble ? -20.0f : 20.0f)),
				CCScaleBy::create(0.1f, 1.3f),
				nullptr);

			this->runAction(Sequence::create(
				baseAction, baseAction->reverse(),
				CallFunc::create(CC_CALLBACK_0(TileSprite::finishHandleContact, this)),
				nullptr));
		}
	}
	else if (m_eTileType == TSType_Coin)	//金币土块
	{
		if (m_nCoinCount <= 0)
		{
			setInvalid();
			return;
		}

		//设置基础动作
		ActionInterval * baseAction = Spawn::create(MoveBy::create(0.1f, Vec2(0.0f, bMarioTumble ? -20.0f : 20.0f)),
			CCScaleBy::create(0.1f, 1.3f), nullptr);

		//根据金币数目设置土块动作
		if (m_nCoinCount == 1)
		{
			this->runAction(Sequence::create(
				baseAction, baseAction->reverse(),
				CallFunc::create(CC_CALLBACK_0(TileSprite::setInvalid, this)),
				CallFunc::create(CC_CALLBACK_0(TileSprite::finishHandleContact, this)),
				nullptr));
		}
		else if (m_nCoinCount > 1)
		{
			this->runAction(Sequence::create(
				baseAction, baseAction->reverse(),
				CallFunc::create(CC_CALLBACK_0(TileSprite::finishHandleContact, this)),
				nullptr));
		}
		--m_nCoinCount;
		//设置金币动作
		Sprite * pCoin = Sprite::createWithSpriteFrameName("coin/coin1.png");
		Size size = getContentSize();
		pCoin->setPosition(Vec2(size.width / 2, size.height / 2));
		pCoin->setCameraMask(2);
		this->addChild(pCoin, -1);

		Animation * pAnimation = AnimationCache::getInstance()->getAnimation("fastCoin");
		pCoin->runAction(Animate::create(pAnimation));
		ActionInterval * moveAction = nullptr;
		if (bMarioTumble)
			moveAction = MoveBy::create(0.3f, Vec2(0.0f, -100.0f));
		else
			moveAction = JumpBy::create(0.7f, Vec2(0.0f, 50.0f), 100.0f, 1);
		pCoin->runAction(Sequence::create(moveAction,
			CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, pCoin)),
			CallFunc::create(CC_CALLBACK_0(TileSprite::addCoinCount, this)), nullptr));
		playEffect(Effect_GetCoin);
	}
	else if (m_eTileType == TSType_Swamm)
	{
		//设置基础动作
		ActionInterval * baseAction = Spawn::create(MoveBy::create(0.1f, Vec2(0.0f, bMarioTumble ? -20.0f : 20.0f)),
			CCScaleBy::create(0.1f, 1.3f), nullptr);

		this->runAction(Sequence::create(baseAction, baseAction->reverse(),
			CallFunc::create(CC_CALLBACK_0(TileSprite::setInvalid, this)),
			CallFunc::create(CC_CALLBACK_0(TileSprite::finishHandleContact, this)),
			CallFunc::create(CC_CALLBACK_0(TileSprite::addSwammSprite, this, bMarioTumble)), nullptr));
		playEffect(Effect_SwammShow);
	}

	if (!bMarioTumble)
	{
		//执行AABB检验，判断是否在土块上面有怪兽、是否新一轮跟头碰撞
		Size size = getContentSize();
		float fPtm = getPTMRatio();
		b2AABB aabb;
		aabb.lowerBound.Set((pos.x - size.width / 2) / fPtm, (pos.y - size.height / 2) / fPtm);
		aabb.upperBound.Set((pos.x + size.width / 2) / fPtm, (pos.y + (size.height) / 2 + 5) / fPtm);
		TileQueryCallBack tileCallBack(this);
		GameScene::getInstance()->getWorld()->QueryAABB(&tileCallBack, aabb);
	}
}

void TileSprite::setInvalid()
{
	this->stopAllActions();
	m_eTileType = TSType_Invalid;

	SpriteFrame * frame =  SpriteFrameCache::getInstance()->getSpriteFrameByName("tile/static_tile.png");
	setSpriteFrame(frame);
}

void TileSprite::addCoinCount()
{
	GameScene::getInstance()->addCoinCount();
}

void TileSprite::addSwammSprite(bool bTumble)
{
	GameScene * gameScene = GameScene::getInstance();
	SwammSprite * pSprite = nullptr;
	int nMarioLevel = GameScene::getInstance()->getMario()->getMarioLevel();
	if (nMarioLevel >= 2)
		pSprite = SwammSprite::createWithSpriteFrameName("object/flower.png", gameScene->getWorld());
	else
		pSprite = SwammSprite::createWithSpriteFrameName("object/swamm.png", gameScene->getWorld());

	b2Fixture * fixture = gameScene->getGameWorld()->CreatePolygonBody(pSprite, getPosition(), b2_dynamicBody);

	b2Filter filter;
	filter.categoryBits = 0x0000;
	filter.maskBits = 0x0080;
	fixture->SetFilterData(filter);
	fixture->SetFriction(0.0f);
	pSprite->getBody()->SetFixedRotation(true);	//禁用旋转
	pSprite->EnableGravity(false);

	pSprite->setCameraMask(2);
	GameScene::getInstance()->getMapLayer()->getTileMap()->addChild(pSprite, 5, GSTag_Swamm);

	//执行动作
	pSprite->runAction(Sequence::create(
		MoveBy::create(0.4f, Vec2(0, bTumble ? -70.0f : 70.0f)),
		CCCallFunc::create(CC_CALLBACK_0(SwammSprite::beginAction, pSprite,
		(nMarioLevel >= 2) ? SwammType_Flower : SwammType_Swamm)),
		nullptr));
}

bool TileSprite::isMarioJackUseful(MarioSprite * pMario, const Vec2 & tilePos)
{
	Size marioSize = MarioSprite::getMarioSize();
	if (pMario->getMarioLevel() == 1)
	{
		marioSize.setSize(marioSize.width * g_fMarioLevel1Scale, marioSize.height * g_fMarioLevel1Scale);
	}
	const Size & tileSize = this->getContentSize();
	//构造碰撞有效矩形区域
	float x = tilePos.x - tileSize.width / 2 - marioSize.width / 2 + 2;
	float y = tilePos.y - tileSize.height / 2 - marioSize.height - 1;
	float width = tileSize.width + marioSize.width - 4;
	float height = 3;
	Rect useRectDown(x, y, width, height);	//底下矩形
	float yUp = tilePos.y + tileSize.height / 2 - 2;
	Rect useRectUp(x, yUp, width, height);	//上面矩形

	Vec2 marioPos = pMario->getPosition();
	pMario->updateMarioStatus();
	if ((pMario->getMarioStatus() == MarioSt_LJump || pMario->getMarioStatus() == MarioSt_RJump) &&
		useRectDown.containsPoint(marioPos))
		return true;
	else if ((pMario->getMarioStatus() == MarioSt_LTumble || pMario->getMarioStatus() == MarioSt_RTumble) &&
		useRectUp.containsPoint(marioPos))
		return true;
	else
		return false;
}
