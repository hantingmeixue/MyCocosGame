#include "MarioSprite.h"
#include "GameConstant.h"
#include "GameScene.h"
#include "box2dGameWorld.h"

USING_NS_CC;

#define Mario_Max_Level	2	//最高等级

MarioSprite::MarioSprite(b2World * pWorld)
: Box2dSprite(pWorld)
, m_eFaceDirection(MarioFD_Right)
, m_eMarioStatus(MarioSt_RWalk)
, m_bBottomPress(false)
, m_bRightPress(false)
, m_bLeftPress(false)
, m_bJumpPress(false)
, m_fJumpBeginPosY(0.0f)
, m_fOldVelocityX(0.0f)
, m_fOldVelocityY(0.0f)
, m_nMarioLevel(1)
, m_bUnmatchedStatus(false)
, m_bTouchUseful(true)
, m_bTumbleFinish(false)
, m_bStandingUseful(true)

{

}

MarioSprite * MarioSprite::create(const char *pszFileName, b2World * pWorld)
{
	MarioSprite * pRet = new MarioSprite(pWorld);
	if (pRet && pRet->initWithFile(pszFileName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

MarioSprite * MarioSprite::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	MarioSprite * pRet = new MarioSprite(pWorld);
	if (pRet && pRet->initWithSpriteFrameName(pszFrameName))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

void MarioSprite::onEnter()
{
	Box2dSprite::onEnter();

	this->scheduleUpdate();
}

void MarioSprite::onExit()
{
	Box2dSprite::onExit();

	this->unscheduleUpdate();
}

void MarioSprite::update(float dt)
{
	b2Vec2 newVelocity = getBody()->GetLinearVelocity();
	enMarioStatus newMarioStatus = MarioSt_RWalk;

	if (newVelocity.y >= -FZero && newVelocity.y <= FZero)
	{
		if (m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
		{
			//播放音效
			if (m_bStandingUseful && m_fOldVelocityY < -FZero)
				playEffect(Effect_Tumble);

			m_fOldVelocityY = newVelocity.y;
			m_fOldVelocityX = newVelocity.x;
			if (m_bBottomPress == false && m_bStandingUseful)
			{
				//启动站起
				m_bStandingUseful = false;
				Animation * standingAni = nullptr;
				if (m_eFaceDirection == MarioFD_Right)
				{
					m_eMarioStatus = MarioSt_RTumble;
					standingAni = AnimationCache::getInstance()->getAnimation("rightStanding");
				}
				else
				{
					m_eMarioStatus = MarioSt_LTumble;
					standingAni = AnimationCache::getInstance()->getAnimation("leftStanding");
				}
				standingAni->setRestoreOriginalFrame(false);
				stopAllActions();
				runAction(Sequence::create(
					Animate::create(standingAni),
					CallFunc::create(CC_CALLBACK_0(MarioSprite::finishStanding, this)),
					nullptr
					));
				return;
			}
			else if (m_bBottomPress)
			{
				if (m_eFaceDirection == MarioFD_Right && m_eMarioStatus != MarioSt_RTumble)
				{
					//转向右面
					SpriteFrame * pRightFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("mario/r_tumble/r_tumble11.png");
					setSpriteFrame(pRightFrame);
					m_eMarioStatus = MarioSt_RTumble;
				}
				else if (m_eFaceDirection == MarioFD_Left && m_eMarioStatus != MarioSt_LTumble)
				{
					//转向左面
					SpriteFrame * pLeftFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("mario/l_tumble/l_tumble11.png");
					setSpriteFrame(pLeftFrame);
					m_eMarioStatus = MarioSt_LTumble;
				}
				getBody()->SetLinearVelocity(b2Vec2(0.0f, newVelocity.y));
				return;
			}
			else
			{
				return;
			}
		}
		else if (m_fOldVelocityY >= -FZero && m_fOldVelocityY <= FZero)
		{
			if (newVelocity.x >= -FZero && newVelocity.x <= FZero)
			{
				if (m_eFaceDirection == MarioFD_Right)
					newMarioStatus = MarioSt_RWalk;
				else
					newMarioStatus = MarioSt_LWalk;
			}
			else
			{
				if (m_eFaceDirection == MarioFD_Right)
					newMarioStatus = MarioSt_RRun;
				else
					newMarioStatus = MarioSt_LRun;
			}
		}
		else if (m_fOldVelocityY > FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RDown;
			else
				newMarioStatus = MarioSt_LDown;
		}
		else if (m_fOldVelocityY < -FZero)
		{
			if (newVelocity.x >= -FZero && newVelocity.x <= FZero)
			{
				if (m_eFaceDirection == MarioFD_Right)
					newMarioStatus = MarioSt_RWalk;
				else
					newMarioStatus = MarioSt_LWalk;
			}
			else
			{
				if (m_eFaceDirection == MarioFD_Right)
					newMarioStatus = MarioSt_RRun;
				else
					newMarioStatus = MarioSt_LRun;
			}
		}
	}
	else if (newVelocity.y > FZero)
	{
		if (m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
		{
			m_fOldVelocityY = newVelocity.y;
			m_fOldVelocityX = newVelocity.x;
			return;
		}
		else if (m_fOldVelocityY >= -FZero && m_fOldVelocityY <= FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RJump;
			else
				newMarioStatus = MarioSt_LJump;
		}
		else if (m_fOldVelocityY < -FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RJump;
			else
				newMarioStatus = MarioSt_LJump;
		}
		else if (m_fOldVelocityY > FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RJump;
			else
				newMarioStatus = MarioSt_LJump;
		}
	}
	else if (newVelocity.y < -FZero)
	{
		if (m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
		{
			m_fOldVelocityY = newVelocity.y;
			m_fOldVelocityX = newVelocity.x;
			return;
		}
		else if (m_fOldVelocityY >= -FZero && m_fOldVelocityY <= FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RDown;
			else
				newMarioStatus = MarioSt_LDown;
		}
		else if (m_fOldVelocityY < -FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RDown;
			else
				newMarioStatus = MarioSt_LDown;
		}
		else if (m_fOldVelocityY > FZero)
		{
			if (m_eFaceDirection == MarioFD_Right)
				newMarioStatus = MarioSt_RDown;
			else
				newMarioStatus = MarioSt_LDown;
		}
	}
	else
	{
		CC_ASSERT(false);
	}
	m_fOldVelocityY = newVelocity.y;
	m_fOldVelocityX = newVelocity.x;

	SetMarioStatus(newMarioStatus);
}

void MarioSprite::SetMarioStatus(enMarioStatus eNewStatus)
{
	if (eNewStatus == m_eMarioStatus)
		return;
	m_eMarioStatus = eNewStatus;

	//更新静止时速度
	if (eNewStatus == MarioSt_LWalk || eNewStatus == MarioSt_RWalk)
	{
		if (m_bLeftPress && !m_bRightPress)
		{
			if (!isScheduled(schedule_selector(MarioSprite::leftVelocityContinue)))
				schedule(schedule_selector(MarioSprite::leftVelocityContinue));
		}
		else if (!m_bLeftPress && m_bRightPress)
		{
			if (!isScheduled(schedule_selector(MarioSprite::rightVelocityContinue)))
				schedule(schedule_selector(MarioSprite::rightVelocityContinue));
		}
		else if (m_bLeftPress && m_bRightPress)
		{
			if (m_eFaceDirection == MarioFD_Left)
			{
				if (!isScheduled(schedule_selector(MarioSprite::leftVelocityContinue)))
					schedule(schedule_selector(MarioSprite::leftVelocityContinue));
			}
			else if (m_eFaceDirection == MarioFD_Right)
			{
				if (!isScheduled(schedule_selector(MarioSprite::rightVelocityContinue)))
					schedule(schedule_selector(MarioSprite::rightVelocityContinue));
			}
		}
	}

	//判断动画名称
	char szAniName[256] = { 0 };	//动画名称
	bool bRestore = true;
	switch (eNewStatus)
	{
	case MarioSt_LRun:
		snprintf(szAniName, sizeof(szAniName), "leftRun");
		break;
	case MarioSt_LWalk:
		snprintf(szAniName, sizeof(szAniName), "leftWalk");
		break;
	case MarioSt_LJump:
		snprintf(szAniName, sizeof(szAniName), "leftJump");
		bRestore = false;
		break;
	case MarioSt_LDown:
		snprintf(szAniName, sizeof(szAniName), "leftDown");
		bRestore = false;
		break;
	case MarioSt_RRun:
		snprintf(szAniName, sizeof(szAniName), "rightRun");
		break;
	case MarioSt_RWalk:
		snprintf(szAniName, sizeof(szAniName), "rightWalk");
		break;
	case MarioSt_RJump:
		snprintf(szAniName, sizeof(szAniName), "rightJump");
		bRestore = false;
		break;
	case MarioSt_RDown:
		snprintf(szAniName, sizeof(szAniName), "rightDown");
		bRestore = false;
		break;
	default:
		CC_ASSERT(false);
		return;
	}

	Animation * pAnimation = AnimationCache::getInstance()->getAnimation(szAniName);
	pAnimation->setRestoreOriginalFrame(bRestore);
	this->stopAllActions();
	this->runAction(Animate::create(pAnimation));
}

void MarioSprite::setBottomPress(bool bBottomPress)
{
	if (bBottomPress == m_bBottomPress)
		return;

	m_bBottomPress = bBottomPress;

	if (!m_bTouchUseful)
		return;

	if (m_bBottomPress)
	{
		if (m_eMarioStatus == MarioSt_LJump || m_eMarioStatus == MarioSt_LDown || 
			m_eMarioStatus == MarioSt_RJump || m_eMarioStatus == MarioSt_RDown)
		{
			if (m_eMarioStatus != MarioSt_LTumble && m_eMarioStatus != MarioSt_RTumble)
			{
				//判定马里奥高度
				float fPtm = getPTMRatio();
				Vec2 pos = getPosition();
				b2Vec2 beginPoint(pos.x / fPtm, pos.y / fPtm);	//投射起点
				b2Vec2 endPoint(beginPoint.x, beginPoint.y - (100 / fPtm));	//投射终点
				b2World * pWorld = GameScene::getInstance()->getWorld();
				MarioRayCastCallBack marioRayCast(this);
				pWorld->RayCast(&marioRayCast, beginPoint, endPoint);
				if (marioRayCast.getHeight() >= 0)	//下面指定位置以内有物体，高度太低，无法翻跟斗
					return;

				//启动筋斗功能
				Animation * tumbleAni = nullptr;
				if (m_eMarioStatus & 0x1000)
				{
					m_eMarioStatus = MarioSt_RTumble;
					tumbleAni = AnimationCache::getInstance()->getAnimation("rightTumble");
				}
				else
				{
					m_eMarioStatus = MarioSt_LTumble;
					tumbleAni = AnimationCache::getInstance()->getAnimation("leftTumble");
				}
				tumbleAni->setRestoreOriginalFrame(false);
				stopAllActions();
				runAction(Sequence::create(
					Animate::create(tumbleAni),
					CallFunc::create(CC_CALLBACK_0(MarioSprite::finishTumble, this)),
					nullptr
					));
				getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				EnableGravity(false);
				m_bTumbleFinish = false;
				m_bStandingUseful = false;
			}
		}
	}
}

void MarioSprite::SetRightPress(bool bRightPress)
{
	if (bRightPress == m_bRightPress)
		return;

	m_bRightPress = bRightPress;

	if (bRightPress)
	{
		m_eFaceDirection = MarioFD_Right;

		if (m_bLeftPress)
			unschedule(schedule_selector(MarioSprite::leftVelocityContinue));

		if (!m_bTouchUseful || m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
			return;

		//更新速度
		b2Body * pBody = getBody();
		b2Vec2 oldVelocity = pBody->GetLinearVelocity();
		if (oldVelocity.y >= -1.0f)
		{
			pBody->SetLinearVelocity(b2Vec2(8.0f, oldVelocity.y));
			//设置加速定时器
			schedule(schedule_selector(MarioSprite::rightVelocityContinue));
		}
	}
	else
	{
		unschedule(schedule_selector(MarioSprite::rightVelocityContinue));

		if (m_bLeftPress && m_bTouchUseful)
		{
			m_eFaceDirection = MarioFD_Left;
			if (m_eMarioStatus != MarioSt_LTumble && m_eMarioStatus != MarioSt_RTumble &&
				getBody()->GetLinearVelocity().y >= -1.0f)
			{
				schedule(schedule_selector(MarioSprite::leftVelocityContinue));
			}
		}
	}
}

void MarioSprite::SetLeftPress(bool bLeftPress)
{
	if (bLeftPress == m_bLeftPress)
		return;

	m_bLeftPress = bLeftPress;

	if (bLeftPress)
	{
		m_eFaceDirection = MarioFD_Left;

		if (m_bRightPress)
			unschedule(schedule_selector(MarioSprite::rightVelocityContinue));

		if (!m_bTouchUseful || m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
			return;

		//更新速度
		b2Body * pBody = getBody();
		b2Vec2 oldVelocity = pBody->GetLinearVelocity();
		if (oldVelocity.y >= -1.0f)
		{
			pBody->SetLinearVelocity(b2Vec2(-8.0f, oldVelocity.y));
			//设置加速定时器
			schedule(schedule_selector(MarioSprite::leftVelocityContinue));
		}
	}
	else
	{
		unschedule(schedule_selector(MarioSprite::leftVelocityContinue));

		if (m_bRightPress && m_bTouchUseful)
		{
			m_eFaceDirection = MarioFD_Right;
			if (m_eMarioStatus != MarioSt_LTumble && m_eMarioStatus != MarioSt_RTumble && 
				getBody()->GetLinearVelocity().y >= -1.0f)
			{
				schedule(schedule_selector(MarioSprite::rightVelocityContinue));
			}
		}
	}
}

void MarioSprite::SetJumpPress(bool bJumpPress)
{
	if (bJumpPress == m_bJumpPress)
		return;

	m_bJumpPress = bJumpPress;

	if (bJumpPress)
	{
		if (!m_bTouchUseful)
			return;
		if (m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
		{
			b2Vec2 velocity = getBody()->GetLinearVelocity();
			if (m_bTumbleFinish && velocity.y > -FZero && velocity.y < FZero)
			{
				if (m_eFaceDirection == MarioFD_Left)
					m_eMarioStatus = MarioSt_LWalk;
				else
					m_eMarioStatus = MarioSt_RWalk;

				//启动横向速度
				if (m_bLeftPress && m_bTouchUseful)
				{
					m_eFaceDirection = MarioFD_Left;
					schedule(schedule_selector(MarioSprite::leftVelocityContinue));
				}
				else if (m_bRightPress && m_bTouchUseful)
				{
					m_eFaceDirection = MarioFD_Right;
					schedule(schedule_selector(MarioSprite::rightVelocityContinue));
				}
			}
			else
			{
				return;
			}
		}

		//更新跳跃状态
		b2Body * pBody = getBody();
		b2Vec2 oldVelocity = pBody->GetLinearVelocity();
		if (oldVelocity.y - 0.0f > -0.00001f && oldVelocity.y - 0.0f < 0.00001f)
		{
			pBody->SetLinearVelocity(b2Vec2(oldVelocity.x, g_fMarioJumpVelocity));
			//设置加速定时器
			m_fJumpBeginPosY = getPositionY();
			schedule(schedule_selector(MarioSprite::jumpVelocityContinue));
			playEffect(Effect_Jump);
		}
	}
	else
	{
		//删除定时器
		unschedule(schedule_selector(MarioSprite::jumpVelocityContinue));
	}
}

void MarioSprite::rightVelocityContinue(float dt)
{
	if (!m_bTouchUseful)
		return;

	b2Body * pBody = getBody();
	b2Vec2 oldVelocity = pBody->GetLinearVelocity();
	float fNewx = oldVelocity.x;
	//if (fNewx < 8.0f)
	//	fNewx += 0.4f;
	//if (fNewx < 2.0f)
	//	fNewx = 2.0f;
	//else if (fNewx > 8.0f)
		fNewx = 8.0f;

	if (oldVelocity.y >= -1.0f)
	{
		pBody->SetLinearVelocity(b2Vec2(fNewx, oldVelocity.y));
	}
}

void MarioSprite::leftVelocityContinue(float dt)
{
	if (!m_bTouchUseful)
		return;

	b2Body * pBody = getBody();
	b2Vec2 oldVelocity = pBody->GetLinearVelocity();
	float fNewx = oldVelocity.x;
	//if (fNewx > -8.0f)
	//	fNewx -= 0.4f;
	//if (fNewx > -2.0f)
	//	fNewx = -2.0f;
	//else if (fNewx < -8.0f)
		fNewx = -8.0f;

	if (oldVelocity.y >= -1.0f)
	{
		pBody->SetLinearVelocity(b2Vec2(fNewx, oldVelocity.y));
	}
}

void MarioSprite::jumpVelocityContinue(float dt)
{
	if (!m_bTouchUseful)
		return;

	b2Body * pBody = getBody();
	b2Vec2 oldVelocity = pBody->GetLinearVelocity();
	if (oldVelocity.y > 0.0f)
		pBody->SetLinearVelocity(b2Vec2(oldVelocity.x, g_fMarioJumpVelocity));

	float fHeightTimes = (m_nMarioLevel == 1) ? 2.2f : 2.5f;
	if ((getPositionY() > m_fJumpBeginPosY + 64.0f * fHeightTimes) || oldVelocity.y <= 0.0f)
	{
		unschedule(schedule_selector(MarioSprite::jumpVelocityContinue));
	}
}

void MarioSprite::setMarioLevel(int nLevel)
{
	//参数校验
	CC_ASSERT(nLevel > 0);
	if (nLevel <= 0)
		return;

	if (nLevel > Mario_Max_Level)
		return;

	m_nMarioLevel = nLevel;
}

void MarioSprite::reduceLife()
{
	if (m_bUnmatchedStatus)
		return;

	if (m_nMarioLevel <= 1)
	{
		setMarioDie();
		playEffect(Effect_Die);
	}
	else
	{
		playEffect(Effect_ReduceLife);
		if (m_nMarioLevel == 2)
		{
			//停止各处理
			setMarioLevel(1);
			stopHandle();

			//设置动作
			float fMoveLength = (1 - g_fMarioLevel1Scale) * getMarioSize().height * 0.5f;

			Action * smallAction = Sequence::create(
				ScaleTo::create(1.0f, g_fMarioLevel1Scale),
				CallFunc::create(CC_CALLBACK_0(MarioSprite::beginBlink, this)),
				CallFunc::create(CC_CALLBACK_0(MarioSprite::recoverHandle, this)),
				nullptr);
			runAction(smallAction);

			//进入无敌状态
			m_bUnmatchedStatus = true;
		}
		else
		{
			CCASSERT(false, "now only support 2 level");
		}
	}
}

void MarioSprite::addLife()
{
	if (m_nMarioLevel >= Mario_Max_Level)
		return;

	playEffect(Effect_AddLife);
	switch (m_nMarioLevel)
	{
	case 1:
	{
		//停止各处理
		setMarioLevel(2);
		stopHandle();

		//设置动作
		float fMoveLength = (1 - g_fMarioLevel1Scale) * getMarioSize().height * 0.5f;
		Action * bigAction = Sequence::create(
			ScaleTo::create(1.0f, 1.0f),
			CallFunc::create(CC_CALLBACK_0(MarioSprite::recoverHandle, this)),
			nullptr);
		runAction(bigAction);

		break;
	}
	default:
	{
		CCASSERT(false, "now only support 2 level");
		break;
	}
	}
}

void MarioSprite::setMarioDie()
{
	stopHandle();
	Animation * dieAnimation = AnimationCache::getInstance()->getAnimation("marioDie");
	dieAnimation->setLoops(2);
	Action * action = Sequence::create(
		Animate::create(dieAnimation),
		Spawn::create(
			JumpBy::create(0.5f, Vec2(0.0f, 200.0f), 200.0f, 1),
			RotateBy::create(0.5f, 180.0f),
			nullptr
		),
		MoveBy::create(0.5f, Vec2(0.0f, -500.0f)),
		CallFunc::create(CC_CALLBACK_0(GameScene::marioDie, GameScene::getInstance())),
		nullptr
		);
	runAction(action);
}

void MarioSprite::updateFixture()
{
	b2Body * body = getBody();
	CCASSERT(body != nullptr, "body can't be null");
	updateFixture(getPosition());
}

void MarioSprite::updateFixture(const Vec2 & pos)
{
	//先删除当前框架
	removeFixtrue();

	//构建框架
	float fScale = (m_nMarioLevel == 1) ? g_fMarioLevel1Scale : 1.0f;	//缩放等级
	setScale(fScale);
	Box2dGameWorld * gameWorld = GameScene::getInstance()->getGameWorld();

	Size marioSize = MarioSprite::getMarioSize();
	marioSize.setSize(marioSize.width * fScale, marioSize.height * fScale);
	b2Vec2 vertices[6];
	vertices[0].Set(-marioSize.width / 2, 2);
	vertices[1].Set(marioSize.width / 2, 2);
	vertices[2].Set(marioSize.width / 2, marioSize.height - 2);
	vertices[3].Set(marioSize.width / 2 - 2, marioSize.height);
	vertices[4].Set(-marioSize.width / 2 + 2, marioSize.height);
	vertices[5].Set(-marioSize.width / 2, marioSize.height - 2);

	b2Fixture * fixture = nullptr;
	b2Body * body = getBody();
	if (!body)
	{
		fixture = gameWorld->CreatePolygonBody(this, pos, b2_dynamicBody, vertices, 6);
		body = getBody();
	}
	else
	{
		setPosition(pos);
		fixture = gameWorld->AddPolygonFixture(body, vertices, 6);
	}

	b2Filter filter;
	filter.categoryBits = 0x0001;
	filter.maskBits = 0x03ee;
	fixture->SetFilterData(filter);
	fixture->SetFriction(0.0f);
	body->SetFixedRotation(true);	//禁用旋转

	//添加下边缘框架
	vertices[0].Set(-marioSize.width / 2 + 3, 0);
	vertices[1].Set(marioSize.width / 2 - 3, 0);
	vertices[2].Set(marioSize.width / 2 - 1, 2);
	vertices[3].Set(-marioSize.width / 2 + 1, 2);

	fixture = gameWorld->AddPolygonFixture(body, vertices, 4);
	fixture->SetFilterData(filter);
	fixture->SetFriction(0.2f);


	//Vec2 center1( - marioSize.width / 2 + 10, 0.0f);
	//float fRadius = 10.0f;
	//fixture = gameWorld->AddCircleFixture(body, center1, fRadius);
	//fixture->SetFilterData(filter);
	//fixture->SetFriction(0.2f);

	//Vec2 center2(+ marioSize.width / 2 - 10, 0.0f);
	//fixture = gameWorld->AddCircleFixture(body, center2, fRadius);
	//fixture->SetFilterData(filter);
	//fixture->SetFriction(0.2f);
}

void MarioSprite::finishTumble()
{
	getBody()->SetLinearVelocity(b2Vec2(0.0f, -g_fMarioJumpVelocity));
	EnableGravity(true);
	m_bTumbleFinish = true;
	m_bStandingUseful = true;
}

void MarioSprite::finishStanding()
{
	m_bStandingUseful = true;
	if (m_eFaceDirection == MarioFD_Right)
	{
		m_eMarioStatus = MarioSt_RRun;
	}
	else
	{
		m_eMarioStatus = MarioSt_LRun;
	}

	if (m_bLeftPress && m_bTouchUseful)
	{
		m_eFaceDirection = MarioFD_Left;
		schedule(schedule_selector(MarioSprite::leftVelocityContinue));
	}
	else if (m_bRightPress && m_bTouchUseful)
	{
		m_eFaceDirection = MarioFD_Right;
		schedule(schedule_selector(MarioSprite::rightVelocityContinue));
	}
}

void MarioSprite::beginBlink()
{
	m_bUnmatchedStatus = true;
	schedule(schedule_selector(MarioSprite::showBlink), 0.1f);
	scheduleOnce(schedule_selector(MarioSprite::endBlink), 2.0f);
}

void MarioSprite::endBlink(float dt)
{
	m_bUnmatchedStatus = false;
	setVisible(true);
	unschedule(schedule_selector(MarioSprite::showBlink));
	unschedule(schedule_selector(MarioSprite::endBlink));
}

void MarioSprite::showBlink(float dt)
{
	setVisible(!isVisible());
}

void MarioSprite::removeFixtrue()
{
	b2Body * body = getBody();
	if (body == nullptr)
		return;

	for (b2Fixture * fixture = body->GetFixtureList(); fixture;)
	{
		b2Fixture * next = fixture->GetNext();
		body->DestroyFixture(fixture);
		fixture = next;
	}

	return;
}

void MarioSprite::stopHandle()
{
	stopAllActions();	//停止动画
	b2Body * body = getBody();
	body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));	//速度置0
	unscheduleUpdate();	//停止更新状态
	removeFixtrue();	//移除框架
	EnableGravity(false);	//使重力无效
	m_bTouchUseful = false;	//使按键无效
}

void MarioSprite::recoverHandle()
{
	scheduleUpdate();	//启动更新定时器
	updateFixture();	//更新框架
	EnableGravity(true);	//使重力有效
	m_bTouchUseful = true;	//使按键有效
	if (m_eMarioStatus == MarioSt_LJump || m_eMarioStatus == MarioSt_RJump)
	{
		getBody()->SetLinearVelocity(b2Vec2(0.0f, 10.0f));	//为跳跃状态设置一向上速度
	}

	if (m_eMarioStatus == MarioSt_LTumble || m_eMarioStatus == MarioSt_RTumble)
	{
		if (m_bBottomPress == false && m_bStandingUseful == false)
		{
			finishStanding();
		}
	}
}

float32 MarioRayCastCallBack::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
	const b2Vec2& normal, float32 fraction)
{
	b2Body * body = fixture->GetBody();
	Box2dSprite * sprite = static_cast<Box2dSprite *>(body->GetUserData());
	int tagA = sprite->getTag();

	if (tagA == GSTag_GeneralTile || tagA == GSTag_SpecialTile ||
		tagA == GSTag_Ground || tagA == GSTag_Bamboo)
	{
		float fPtm = m_pMario->getPTMRatio();
		float fDistance = m_marioPos.y - point.y * fPtm;	//距离马里奥高度
		if (fDistance > m_fHeight)
			m_fHeight = fDistance;

		return 0;
	}
	else
		return -1;
}
