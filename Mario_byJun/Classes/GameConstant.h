#ifndef _GAME_CONSTANT_H_
#define _GAME_CONSTANT_H_

#include "cocos2d.h"

#define FZero		0.00001f

//��Ϸ����tag
enum enGameSpriteTag
{
	GSTag_Ground = 100,		//����
	GSTag_Bamboo,			//����
	GSTag_LimitLine,		//������
	GSTag_Coin,				//���
	GSTag_BigCoin,			//����
	GSTag_GeneralTile,		//ͨ������
	GSTag_SpecialTile,		//��������
	GSTag_TransparentTile,	//͸������
	GSTag_StepTile,			//��������
	GSTag_FrictionEdge,		//����߽�
	GSTag_Mario,			//�����
	GSTag_Swamm,			//Ģ��
	GSTag_OverLine,			//������
	GSTag_FlagPot,			//��˵ײ�
	GSTag_Flagpole,			//���
	GSTag_Robbit = 0x1001,	//����
	GSTag_Tortoise = 0x1002,//�ڹ�
};

//�����ƶ��ٶ�
enum enSpriteMoveSpeed
{
	MMSpeed_Robbit = 3,		//����
	MMSpeed_Tortoise = 3,	//�ڹ�
	MMSpeed_TortoiseDefense = 30, //�ڹ����
	MMSpeed_SwammMove = 6,	//Ģ���ƶ��ٶ�
};

//��ЧID
enum enEffectId
{
	Effect_AddLife = 0,	//��������
	Effect_DestroyTile,	//�ƻ�ש��
	Effect_Die,	//����
	Effect_EatSwamm,	//��Ģ��
	Effect_GetCoin,	//�õ����
	Effect_JackSolid,	//��ש��
	Effect_Jump,	//��Ծ
	Effect_KillMonster,	//ɱ������
	Effect_ReduceLife,	//��������ֵ
	Effect_SwammShow,	//����Ģ��
	Effect_TimeLack,	//ʱ�����
	Effect_Tumble,	//��ͷ����
	Effect_DropFlag,	//����
	Effect_Win,	//ʤ��
};

extern const char * g_szEffects[14];

const float g_fMarioLevel1Scale = 0.6f;
const float g_fMarioJumpVelocity = 23.0f;
const cocos2d::Size g_robbitSize(50.0f / CC_CONTENT_SCALE_FACTOR(), 70.0f / CC_CONTENT_SCALE_FACTOR());	//���ӳߴ�
const cocos2d::Size g_tortoiseNormalSize(60.0f / CC_CONTENT_SCALE_FACTOR(), 90.0f / CC_CONTENT_SCALE_FACTOR());	//�ڹ������ߴ�
const cocos2d::Size g_tortoiseDefendSize(60.0f / CC_CONTENT_SCALE_FACTOR(), 60.0f / CC_CONTENT_SCALE_FACTOR());	//�ڹ���سߴ�

int playEffect(enEffectId enId, bool bLoop = false);

#endif // !_GAME_CONSTANT_H_
