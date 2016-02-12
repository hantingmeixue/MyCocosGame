#include "Hero.h"

USING_NS_CC;

Hero * Hero::create(const char *pszFileName, b2World * pWorld)
{
	Hero* pRet = new Hero(pWorld);
	if (pRet && pRet->initWithFile(pszFileName))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

Hero * Hero::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	Hero* pRet = new Hero(pWorld);
	if (pRet && pRet->initWithSpriteFrameName(pszFrameName))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

void Hero::ReadyFlying()
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	float fGoLength = size.width / 2 - getPosition().x;

	this->runAction(CCMoveBy::create(2.0f, CCPoint(fGoLength, 0)));
	scheduleOnce(schedule_selector(Hero::WorldScroll), 2.0f);
}

void Hero::WorldScroll(float dt)
{
	flying();
	this->EnbaleGravity(true);
}

void Hero::hit()
{
	CCSprite * pHit = CCSprite::createWithSpriteFrameName("attack1.png");
	pHit->setPosition(this->getPosition());
	CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("attack");
	pAnimation->setRestoreOriginalFrame(false);
	CCAnimate * pAction = CCAnimate::create(pAnimation);
	CCSequence * sequence = CCSequence::create(pAction, CCCallFunc::create(pHit, callfunc_selector(CCSprite::removeFromParent)), NULL);

	pHit->runAction(sequence);
	pHit->setScale(0.5f);
	this->getParent()->addChild(pHit);
}

void Hero::flying()
{
	CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("flying");

	pAnimation->setRestoreOriginalFrame(false);
	CCAnimate * pAction = CCAnimate::create(pAnimation);
	this->runAction(pAction);
}

void Hero::jump()
{
	CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("drop");

	pAnimation->setRestoreOriginalFrame(false);
	CCAnimate * pAction = CCAnimate::create(pAnimation);
	this->runAction(pAction->reverse());
}

void Hero::drop()
{
	CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("drop");
	pAnimation->setRestoreOriginalFrame(false);
	CCAnimate * pAction = CCAnimate::create(pAnimation);
	this->runAction(pAction);
}

void Hero::die()
{
	CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("die");
	pAnimation->setRestoreOriginalFrame(false);
	CCAnimate * pAction = CCAnimate::create(pAnimation);
	this->runAction(pAction);
}
