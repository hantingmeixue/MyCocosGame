#ifndef _LOADING_SCENE_H_
#define _LOADING_SCENE_H_

#include "cocos2d.h"

class LoadingScene : public cocos2d::Layer
{
public:
	LoadingScene();
	~LoadingScene();

	static cocos2d::Scene* createScene();
	CREATE_FUNC(LoadingScene);

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

private:
	//—”≥Ÿº”‘ÿ”Œœ∑≥°æ∞
	void addGameScene(float dt);
	//‘§º”‘ÿ“Ù¿÷°¢“Ù–ß
	void preloadSound();
};

#endif	//_LOADING_SCENE_H_