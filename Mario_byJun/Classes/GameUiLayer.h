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

	//�Ϸ����
	void menuTopCallback(Ref * sender);
	//�·����
	bool onBottomTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onBottomTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onBottomTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//���ҷ����
	bool onLeftTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onLeftTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onLeftTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	bool onRightTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onRightTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onRightTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//������ť��Ӧ
	bool onJumpTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event);
	void onJumpTouchMoved(cocos2d::Touch * touch, cocos2d::Event * event);
	void onJumpTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event);

	//������Ч�ж�
	bool isTouchUseful(cocos2d::Touch * touch, cocos2d::Event * event);

	//��Ӽ�����Ӧ�¼�
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	//��������¶���
	void SetMario(MarioSprite * mario);

	//���ӽ����Ŀ
	void addCoinCount(int nCount);
	//��Ϸʱ�䶨ʱ��
	void updateGameTime(float dt);
	//���ô�����Ч���
	void setTouchUseful(bool bUseful);

private:
	cocos2d::Sprite * m_pjumpNormalImage;	//��������ť����
	cocos2d::Sprite * m_pjumpSelectedImage;	//����ʱ����ť����
	LabelAtlas * m_pFontCoinCount;	//�����Ŀ
	LabelAtlas * m_pFontRemainTime;	//ʣ��ʱ��

	MarioSprite * m_pMario;
	bool m_bTouchUseful;	//������Ч����
	bool m_bKeyAPressed;
	bool m_bKeySPressed;
	bool m_bKeyDPressed;
	bool m_bKeyWPressed;
	bool m_bKeyKPressed;

	//��Ϸ����
	int m_nCoinCount;	//�����Ŀ
	int m_nRemainTime;	//ʣ��ʱ��
};

#endif // !_GAME_UI_LAYER_H_
