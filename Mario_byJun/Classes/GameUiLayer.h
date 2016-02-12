#ifndef _GAME_UI_LAYER_H_
#define _GAME_UI_LAYER_H_

#include "cocos2d.h"
#include "MarioSprite.h"

class GameUiLayer : public cocos2d::Layer
{
public:
	GameUiLayer();

	virtual bool init();

	CREATE_FUNC(GameUiLayer);

	//上方向键
	void menuTopCallback(Ref * sender);
	//下方向键
	bool onBottomTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onBottomTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onBottomTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//左右方向键
	bool onLeftTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onLeftTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onLeftTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	bool onRightTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onRightTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onRightTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//起跳按钮响应
	bool onJumpTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onJumpTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onJumpTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//触摸有效判断
	bool isTouchUseful(cocos2d::Touch * touch, cocos2d::Event * event);

	//添加键盘响应事件
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	//设置马里奥对象
	void SetMario(MarioSprite * mario);

	//增加金币数目
	void addCoinCount(int nCount);
	//游戏时间定时器
	void updateGameTime(float dt);
	//设置触摸有效与否
	void setTouchUseful(bool bUseful);

private:
	cocos2d::Sprite * m_pjumpNormalImage;	//正常跳起按钮背景
	cocos2d::Sprite * m_pjumpSelectedImage;	//按下时跳起按钮背景
	LabelAtlas * m_pFontCoinCount;	//金币数目
	LabelAtlas * m_pFontRemainTime;	//剩余时间

	MarioSprite * m_pMario;
	bool m_bTouchUseful;	//触摸有效控制
	bool m_bKeyAPressed;
	bool m_bKeySPressed;
	bool m_bKeyDPressed;
	bool m_bKeyWPressed;
	bool m_bKeyKPressed;

	//游戏变量
	int m_nCoinCount;	//金币数目
	int m_nRemainTime;	//剩余时间
};

#endif // !_GAME_UI_LAYER_H_
