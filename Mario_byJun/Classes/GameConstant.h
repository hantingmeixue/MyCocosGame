#ifndef _GAME_CONSTANT_H_
#define _GAME_CONSTANT_H_

#include "cocos2d.h"

#define FZero		0.00001f

//游戏精灵tag
enum enGameSpriteTag
{
	GSTag_Ground = 100,		//地面
	GSTag_Bamboo,			//竹子
	GSTag_LimitLine,		//限制线
	GSTag_Coin,				//金币
	GSTag_BigCoin,			//大金币
	GSTag_GeneralTile,		//通常土块
	GSTag_SpecialTile,		//特殊土块
	GSTag_TransparentTile,	//透明土块
	GSTag_StepTile,			//阶梯土块
	GSTag_FrictionEdge,		//土块边界
	GSTag_Mario,			//马里奥
	GSTag_Swamm,			//蘑菇
	GSTag_OverLine,			//结束线
	GSTag_FlagPot,			//旗杆底部
	GSTag_Flagpole,			//旗杆
	GSTag_Robbit = 0x1001,	//兔子
	GSTag_Tortoise = 0x1002,//乌龟
};

//精灵移动速度
enum enSpriteMoveSpeed
{
	MMSpeed_Robbit = 3,		//兔子
	MMSpeed_Tortoise = 3,	//乌龟
	MMSpeed_TortoiseDefense = 30, //乌龟防御
	MMSpeed_SwammMove = 6,	//蘑菇移动速度
};

//音效ID
enum enEffectId
{
	Effect_AddLife = 0,	//增加生命
	Effect_DestroyTile,	//破坏砖块
	Effect_Die,	//死掉
	Effect_EatSwamm,	//吃蘑菇
	Effect_GetCoin,	//得到金币
	Effect_JackSolid,	//顶砖块
	Effect_Jump,	//跳跃
	Effect_KillMonster,	//杀死怪兽
	Effect_ReduceLife,	//减少生命值
	Effect_SwammShow,	//出现蘑菇
	Effect_TimeLack,	//时间紧张
	Effect_Tumble,	//跟头坐下
	Effect_DropFlag,	//拉旗
	Effect_Win,	//胜利
};

extern const char * g_szEffects[14];

const float g_fMarioLevel1Scale = 0.6f;
const float g_fMarioJumpVelocity = 23.0f;
const cocos2d::Size g_robbitSize(50.0f / CC_CONTENT_SCALE_FACTOR(), 70.0f / CC_CONTENT_SCALE_FACTOR());	//兔子尺寸
const cocos2d::Size g_tortoiseNormalSize(60.0f / CC_CONTENT_SCALE_FACTOR(), 90.0f / CC_CONTENT_SCALE_FACTOR());	//乌龟正常尺寸
const cocos2d::Size g_tortoiseDefendSize(60.0f / CC_CONTENT_SCALE_FACTOR(), 60.0f / CC_CONTENT_SCALE_FACTOR());	//乌龟防守尺寸

int playEffect(enEffectId enId, bool bLoop = false);

#endif // !_GAME_CONSTANT_H_
