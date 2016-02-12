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
	//��ʼ��ť�ص�
	void menuBeginCallback(Ref * sender);
	//�˳���ť�ص�
	void menuExitCallback(Ref * sender);

private:
	//����Ʋ�ͼ��
	void fillCloundLayer(Layer * pCloundLayer);
	//������Ϸ��������
	void LoadGameAnimationCatch();

private:
	Layer * m_pCloundLayer1;	//�Ʋ�ͼ��1
	Layer * m_pCloundLayer2;	//�Ʋ�ͼ��2
};

#endif // !_MENU_SCENE_H_
