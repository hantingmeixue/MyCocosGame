#ifndef _BG_LAYER_H_
#define _BG_LAYER_H_

#include "cocos2d.h"

class BgLayer : public cocos2d::CCLayer
{
public:
	CREATE_FUNC(BgLayer);
	virtual bool init();

	void scrollBackGround();
	void stopScroll();
	void setRollScale(float fScale);

	void scrollUpdate(float dt);

	//¸¨Öúº¯Êý
private:
	void scrollTwoNode(cocos2d::CCNode * pNode1, cocos2d::CCNode * pNode2, float fScrollLength);

private:
	cocos2d::CCSprite * pBg0;
	cocos2d::CCSprite * pBg1_1;
	cocos2d::CCSprite * pBg1_2;
	cocos2d::CCSprite * pBg2_1;
	cocos2d::CCSprite * pBg2_2;
	float m_fRoll1;
	float m_fRoll2;
};

#endif	//_BG_LAYER_H_
