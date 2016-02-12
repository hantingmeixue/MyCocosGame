#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include "cocos2d.h"

class MenuScene : public cocos2d::Layer
{
public:
	MenuScene();
	~MenuScene();

	static cocos2d::Scene* createScene();
	CREATE_FUNC(MenuScene);

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float dt);

public:
	//开始按钮回调
	void menuBeginCallback(Ref * sender);
	//退出按钮回调
	void menuExitCallback(Ref * sender);

private:
	//填充云彩图层
	void fillCloundLayer(Layer * pCloundLayer);
	//加载游戏动画缓存
	void LoadGameAnimationCatch();

private:
	Layer * m_pCloundLayer1;	//云彩图层1
	Layer * m_pCloundLayer2;	//云彩图层2
};

#endif // !_MENU_SCENE_H_
