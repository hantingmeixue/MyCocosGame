#include "BgLayer.h"

USING_NS_CC;

extern const float g_fBgRollLength1 = 3.0f;
extern const float g_fBgRollLength2 = 6.0f;

bool BgLayer::init()
{
	if (!CCLayer::init())
		return false;

	CCSize size = CCDirector::sharedDirector()->getWinSize();
	//Ìí¼Ó±³¾°²ã
	pBg0 = CCSprite::create("bj0.jpg");
	pBg0->setPosition(ccp(size.width / 2, size.height / 2));
	addChild(pBg0, 1);

	pBg1_1 = CCSprite::create("bj1.png");
	pBg1_1->setAnchorPoint(ccp(0, 0));
	pBg1_1->setPosition(ccp(0, 0));
	addChild(pBg1_1, 2);

	pBg1_2 = CCSprite::create("bj1.png");
	pBg1_2->setAnchorPoint(ccp(0, 0));
	pBg1_2->setPosition(ccp(pBg1_1->getContentSize().width, 0));
	addChild(pBg1_2, 2);

	pBg2_1 = CCSprite::create("bj2.png");
	pBg2_1->setAnchorPoint(ccp(0, 0));
	pBg2_1->setPosition(ccp(0, 0));
	addChild(pBg2_1, 3);

	pBg2_2 = CCSprite::create("bj2.png");
	pBg2_2->setAnchorPoint(ccp(0, 0));
	pBg2_2->setPosition(ccp(pBg2_1->getContentSize().width, 0));
	addChild(pBg2_2, 3);

	CC_ASSERT(pBg1_1->getContentSize().width - g_fBgRollLength1 > size.width);
	CC_ASSERT(pBg2_1->getContentSize().width - g_fBgRollLength2 > size.width);

	return true;
}

void BgLayer::scrollBackGround()
{
	m_fRoll1 = g_fBgRollLength1;
	m_fRoll2 = g_fBgRollLength2;
	schedule(schedule_selector(BgLayer::scrollUpdate));
}

void BgLayer::stopScroll()
{
	unschedule(schedule_selector(BgLayer::scrollUpdate));
}

void BgLayer::setRollScale(float fScale)
{
	m_fRoll1 = g_fBgRollLength1 * fScale;
	m_fRoll2 = g_fBgRollLength2 * fScale;
}

void BgLayer::scrollUpdate(float dt)
{
	scrollTwoNode(pBg1_1, pBg1_2, m_fRoll1);
	scrollTwoNode(pBg2_1, pBg2_2, m_fRoll2);
}

void BgLayer::scrollTwoNode(cocos2d::CCNode * pNode1, cocos2d::CCNode * pNode2, float fScrollLength)
{
	float oldPosX = pNode1->getPositionX();
	pNode1->setPositionX(oldPosX - fScrollLength);

	oldPosX = pNode2->getPositionX();
	pNode2->setPositionX(oldPosX - fScrollLength);

	//·ÀÖ¹ÉÁË¸£¬¼ÆËã¿ÉÓÃ¿ÕÓà¼ä¸ôÖµ
	float fSpace = (pNode2->getContentSize().width - CCDirector::sharedDirector()->getWinSize().width) / 2 ;

	if (pNode1->getPositionX() + pNode1->getContentSize().width + fSpace< 0)
	{
		pNode1->setPositionX(pNode2->getPositionX() + pNode2->getContentSize().width);
	}

	//fSpace = (pNode1->getContentSize().width - CCDirector::sharedDirector()->getWinSize().width) / 2 ;
	else if (pNode2->getPositionX() + pNode2->getContentSize().width + fSpace < 0)
	{
		pNode2->setPositionX(pNode1->getPositionX() + pNode1->getContentSize().width);
	}
}
