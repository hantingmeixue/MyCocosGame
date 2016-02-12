#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include "cocos2d.h"

class MenuLayer : public cocos2d::CCLayer
{
public:
	static cocos2d::CCScene* scene();
	CREATE_FUNC(MenuLayer);

	virtual bool init();

	void menuBegin(CCObject * pSender);
	void menuExit(CCObject * pSender);
	void menuCheat(CCObject * pSender);

private:
	bool m_bCheat;
};

#endif	//_MENU_SCENE_H_
