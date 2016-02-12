#include "SwammSprite.h"
#include "GameConstant.h"
#include "GameScene.h"
USING_NS_CC;

SwammSprite::SwammSprite(b2World * pWorld)
:Box2dSprite(pWorld)
, m_swammType(SwammType_Swamm)
, m_bLeft(false)
{

}

SwammSprite * SwammSprite::create(const char *pszFileName, b2World * pWorld)
{
	SwammSprite * pRet = new SwammSprite(pWorld);
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

SwammSprite * SwammSprite::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	SwammSprite * pRet = new SwammSprite(pWorld);
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

void SwammSprite::update(float dt)
{
	b2Vec2 velocity = getBody()->GetLinearVelocity();
	Vec2 pos = getPosition();

	if (m_swammType == SwammType_Swamm)
	{
		if ((pos.x - m_oldPos.x > -FZero) && (pos.x - m_oldPos.x < FZero))
		{
			getBody()->SetLinearVelocity(b2Vec2(m_bLeft ? MMSpeed_SwammMove : -MMSpeed_SwammMove, velocity.y));
			m_bLeft = !m_bLeft;
		}
	}
	else if (m_swammType == SwammType_Flower)
	{
		if ((rand() % 300 == 0) && (pos.y - m_oldPos.y > -FZero) && (pos.y - m_oldPos.y < FZero))
		{
			//执行跳跃动作
			getBody()->SetLinearVelocity(b2Vec2(0.0f, 20.0f));
		}
	}

	m_oldPos = pos;
	float fDistance = pos.x - GameScene::getInstance()->getMario()->getPosition().x;
	Size winSize = Director::getInstance()->getWinSize();
	if (fDistance < -winSize.width || fDistance > winSize.width)
	{
		removeFromParent();
	}
}

void SwammSprite::beginAction(enSwammType type)
{
	m_swammType = type;
	m_oldPos = getPosition();
	//重置框架属性
	b2Fixture * fixture = getBody()->GetFixtureList();
	b2Filter filter;
	filter.categoryBits = 0x0040;
	filter.maskBits = 0x0007;
	fixture->SetFilterData(filter);
	EnableGravity(true);

	if (type == SwammType_Swamm)
	{
		//设置缩放动作
		this->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.3f, 1.2f), ScaleTo::create(0.3f, 1.0f), nullptr)));
		getBody()->SetLinearVelocity(b2Vec2(MMSpeed_SwammMove, 0.0f));
		m_bLeft = false;
	}

	//更新蘑菇状态
	scheduleUpdate();
}
