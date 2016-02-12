#include "MonsterTortoise.h"
#include "MarioSprite.h"
#include "GameScene.h"
#include "GameConstant.h"
#include "TileMapLayer.h"

MonsterTortoise::MonsterTortoise(b2World * pWorld)
:MonsterBase(pWorld)
, m_lastPos(0.0f, 0.0f)
, m_bLeft(true)
, m_bIsDefend(false)
, m_bStepObject(false)
, m_bIsDie(false)
, m_bIsRoll(false)
, m_ntimes(0)
, updateFixStatus(UpdateTortoise_Invalid)
{

}

MonsterTortoise::~MonsterTortoise()
{

}

MonsterTortoise * MonsterTortoise::create(const char *pszFileName, b2World * pWorld)
{
	MonsterTortoise * pRet = new MonsterTortoise(pWorld);
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

MonsterTortoise * MonsterTortoise::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	MonsterTortoise * pRet = new MonsterTortoise(pWorld);
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

void MonsterTortoise::update(float dt)
{
	//处理当前框架状态
	if (updateFixStatus != UpdateTortoise_Invalid)
	{
		if (updateFixStatus == UpdateTortoise_TorKillL || updateFixStatus == UpdateTortoise_TorKillR)
		{
			//执行杀死动作
			Vec2 pos = getPosition();
			Spawn * action = nullptr;
			float fPosLevel = (pos.y + 100) / 100.0f;	//高度等级

			//设置动作、显示
			if (updateFixStatus == UpdateTortoise_TorKillL)
			{
				action = Spawn::create(JumpTo::create(fPosLevel * 1.0f, Vec2(pos.x + 400 * fPosLevel, -100.0f), 800.0f, 1),
					RotateBy::create(fPosLevel * 1.0f, 720 * fPosLevel), nullptr);
			}
			else if (updateFixStatus == UpdateTortoise_TorKillR)
			{
				action = Spawn::create(JumpTo::create(fPosLevel * 1.0f, Vec2(pos.x - 400 * fPosLevel, -100.0f), 800.0f, 1),
					RotateBy::create(fPosLevel * 1.0f, -720 * fPosLevel), nullptr);
			}

			SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
				"monster/tortoise_defend/tortoise_defend1.png");
			setSpriteFrame(frame);

			//复位各信息
			stopAllActions();
			getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			runAction(Sequence::create(action, CallFunc::create(CC_CALLBACK_0(MonsterTortoise::removeSelf, this)), nullptr));
		}

		changeFixture();
		updateFixStatus = UpdateTortoise_Invalid;
	}

	if (++m_ntimes >= 2)
	{
		checkPosSchedule();
		m_ntimes = 0;
	}
}

float32 MonsterTortoise::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	b2Body * body = fixture->GetBody();
	Box2dSprite * sprite = static_cast<Box2dSprite *>(body->GetUserData());
	int tagA = sprite->getTag();

	if (tagA == GSTag_GeneralTile || tagA == GSTag_SpecialTile || tagA ==  GSTag_Ground ||
		(tagA == GSTag_TransparentTile && sprite->isVisible()))
	{
		m_bStepObject = true;
		return 0;
	}
	else
		return -1;
}

void MonsterTortoise::setInvalidStatus()
{
	if (m_bIsDie)
		return;

	setMonsterStatus(MonsterStatus_Invalid);
	//复位怪兽信息
	setPosition(getBeginPos());
	stopAllActions();
	m_lastPos = getBeginPos();
	setVisible(false);
	getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	m_bLeft = true;

	if (m_bIsDefend)
	{
		//改变防守状态框架为正常框架
		updateFixStatus = UpdateTortoise_Normal;
		changeFixture();
	}
	updateFixStatus = UpdateTortoise_Invalid;
	m_bIsDefend = false;
	m_bIsRoll = false;
	unscheduleUpdate();
}

void MonsterTortoise::setNormalStatus()
{
	if (m_bIsDie)
		return;

	setMonsterStatus(MonsterStatus_Normal);
	//设置初始信息
	setPosition(getBeginPos());
	m_lastPos = getBeginPos();
	setVisible(true);
	MarioSprite * pMario = GameScene::getInstance()->getMario();
	m_bLeft = (m_lastPos.x >= pMario->getPositionX()) ? true : false;
	if (m_bLeft)
	{
		getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_Tortoise, 0.0f));
		Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseLeft");
		runAction(Animate::create(animation));
	}
	else
	{
		getBody()->SetLinearVelocity(b2Vec2(MMSpeed_Tortoise, 0.0f));
		Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseRight");
		runAction(Animate::create(animation));
	}
	scheduleUpdate();
}

void MonsterTortoise::marioContact(MarioSprite * pMario)
{
	if (pMario == nullptr)
	{
		CC_ASSERT(false);
		return;
	}

	if (getMonsterStatus() == MonsterStatus_Invalid)
	{
		return;
	}
	else if (m_bIsDefend)
	{
		//重置弹力系数
		b2Fixture * fixture = getBody()->GetFixtureList();

		b2Vec2 oldVelocity = getBody()->GetLinearVelocity();
		enMarioStatus marioStatus = pMario->getMarioStatus();
		if (m_bIsRoll)
		{
			if (marioStatus == MarioSt_LJump || marioStatus == MarioSt_LDown ||
				marioStatus == MarioSt_RJump || marioStatus == MarioSt_RDown)
			{
				//马里奥空中状态，停止防守龟滚动
				getBody()->SetLinearVelocity(b2Vec2(0.0f, oldVelocity.y));
				stopAllActions();
				m_bIsRoll = false;

				if (marioStatus == MarioSt_LDown || marioStatus == MarioSt_RDown)
				{
					//回弹马里奥
					b2Vec2 marioVelocity = pMario->getBody()->GetLinearVelocity();
					pMario->getBody()->SetLinearVelocity(b2Vec2(marioVelocity.x, -marioVelocity.y));
				}
			}
			else
			{
				//马里奥地上状态，杀死马里奥
				GameScene::getInstance()->reduceLife();
			}
		}
		else
		{
			if ((pMario->getMarioStatus() & 0x1000) == 0)
			{
				//左向滚动
				m_bLeft = true;
				getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_TortoiseDefense, oldVelocity.y));
			}
			else
			{
				//右向滚动
				m_bLeft = false;
				getBody()->SetLinearVelocity(b2Vec2(MMSpeed_TortoiseDefense, oldVelocity.y));
			}
			m_bIsRoll = true;
			stopAllActions();
			Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseDefend");
			runAction(Animate::create(animation));

			if (marioStatus == MarioSt_LDown || marioStatus == MarioSt_RDown)
			{
				//回弹马里奥
				b2Vec2 marioVelocity = pMario->getBody()->GetLinearVelocity();
				pMario->getBody()->SetLinearVelocity(b2Vec2(marioVelocity.x, -marioVelocity.y));
			}
		}
	}
	else
	{
		if (pMario->getMarioStatus() == MarioSt_LDown || pMario->getMarioStatus() == MarioSt_RDown ||
			pMario->getMarioStatus() == MarioSt_LTumble || pMario->getMarioStatus() == MarioSt_RTumble)
		{
			const Size & monsterSize = getMonsterSize();
			const Size & marioSize = pMario->getMarioSize();
			Vec2 pos = getPosition();
			Vec2 posM = pMario->getPosition();
			bool bMarioTumble = (pMario->getMarioStatus() == MarioSt_LTumble || pMario->getMarioStatus() == MarioSt_RTumble);

			if ((pos.y <= posM.y) &&
				(pos.x - posM.x > -monsterSize.width / 2 - marioSize.width / 2) &&
				(pos.x - posM.x < monsterSize.width / 2 + marioSize.width / 2))
			{
				//乌龟变为防守状态
				if (!bMarioTumble)
				{
					stopAllActions();
					getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
					m_bIsDefend = true;
					m_bIsRoll = false;

					SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
						"monster/tortoise_defend/tortoise_defend1.png");
					setSpriteFrame(frame);
					updateFixStatus = UpdateTortoise_Defend;
					playEffect(Effect_KillMonster);

					//回弹马里奥
					b2Vec2 marioVelocity = pMario->getBody()->GetLinearVelocity();
					pMario->getBody()->SetLinearVelocity(b2Vec2(marioVelocity.x, -marioVelocity.y));
				}
				else
				{
					if (posM.x >= pos.x)
					{
						//左向滚动
						m_bLeft = true;
						getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_TortoiseDefense, 0.0f));
					}
					else
					{
						//右向滚动
						m_bLeft = false;
						getBody()->SetLinearVelocity(b2Vec2(MMSpeed_TortoiseDefense, 0.0f));
					}
					m_bIsDefend = true;
					m_bIsRoll = true;
					m_lastPos.set(getPosition());
					updateFixStatus = UpdateTortoise_Defend;
					stopAllActions();
					Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseDefend");
					runAction(Animate::create(animation));
				}
			}
			else
			{
				GameScene::getInstance()->reduceLife();
			}
		}
		else
		{
			GameScene::getInstance()->reduceLife();
		}
	}

}

void MonsterTortoise::defendTorRollContact(MonsterTortoise * pTortoise)
{
	//状态校验
	if (getMonsterStatus() == MonsterStatus_Invalid)
		return;
	if (updateFixStatus == UpdateTortoise_TorKillR || updateFixStatus == UpdateTortoise_TorKillL)
		return;

	Vec2 mePos = getPosition();
	Vec2 torPos = pTortoise->getPosition();
	m_bIsDefend = true;
	m_bIsDie = true;

	//更改更新状态
	if (mePos.x < torPos.x)
		updateFixStatus = UpdateTortoise_TorKillR;
	else
		updateFixStatus = UpdateTortoise_TorKillL;
}

void MonsterTortoise::tileJumpHandle(TileSprite * pTile)
{
	//状态校验
	CCASSERT(pTile != nullptr, "jump tile can't be null");
	if (getMonsterStatus() == MonsterStatus_Invalid)
		return;
	if (updateFixStatus == UpdateTortoise_TorKillR || updateFixStatus == UpdateTortoise_TorKillL)
		return;

	Vec2 mePos = getPosition();
	Vec2 tilePos = pTile->getPosition();

	//响应弹起土块碰撞
	stopAllActions();
	Action * action = nullptr;
	if (mePos.x < tilePos.x)
		action = Sequence::create(
		JumpBy::create(1.0f, Vec2(-100.0f, 10.0f), 200.0f, 1),
		//CallFunc::create(CC_CALLBACK_0(MonsterTortoise::resetVelocityX, this)),
		nullptr);
	else
		action = Sequence::create(JumpBy::create(1.0f, Vec2(100.0f, 10.0f), 200.0f, 1),
		//CallFunc::create(CC_CALLBACK_0(MonsterTortoise::resetVelocityX, this)),
		nullptr);
	runAction(action);

	getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	m_bIsDefend = true;
	m_bIsRoll = false;

	SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
		"monster/tortoise_defend/tortoise_defend1.png");
	setSpriteFrame(frame);
	updateFixStatus = UpdateTortoise_Defend;
}

void MonsterTortoise::changeVelocityDirection()
{
	//状态判断
	if (getMonsterStatus() == MonsterStatus_Invalid)
		return;

	b2Vec2 oldVelocity = getBody()->GetLinearVelocity();

	if (m_bLeft)
	{
		m_bLeft = false;
		stopAllActions();

		if (!m_bIsDefend)
		{
			getBody()->SetLinearVelocity(b2Vec2(MMSpeed_Tortoise, oldVelocity.y));
			Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseRight");
			runAction(Animate::create(animation));
		}
		else if (m_bIsRoll)
		{
			getBody()->SetLinearVelocity(b2Vec2(MMSpeed_TortoiseDefense, oldVelocity.y));
			Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseDefend");
			runAction(Animate::create(animation));
		}
	}
	else
	{
		m_bLeft = true;
		stopAllActions();

		if (!m_bIsDefend)
		{
			getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_Tortoise, oldVelocity.y));
			Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseLeft");
			runAction(Animate::create(animation));
		}
		else if (m_bIsRoll)
		{
			getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_TortoiseDefense, oldVelocity.y));
			Animation * animation = AnimationCache::getInstance()->getAnimation("tortoiseDefend");
			runAction(Animate::create(animation));
		}
	}
}

void MonsterTortoise::checkPosSchedule()
{
	Vec2 newPos = getPosition();
	b2Body * body = getBody();
	b2Vec2 oldVelocity = body->GetLinearVelocity();
	if (newPos.x - m_lastPos.x >= -0.01f && newPos.x - m_lastPos.x <= 0.01f)
	{
		//转向处理
		if (!m_bIsDefend || (m_bIsDefend && m_bIsRoll))
		{
			changeVelocityDirection();
		}
	}
	else
	{
		if (m_bLeft)
		{
			if (!m_bIsDefend)
				body->SetLinearVelocity(b2Vec2(-MMSpeed_Tortoise, oldVelocity.y));
			//else if (oldVelocity.x < -0.00001f)
			//	getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_TortoiseDefense, oldVelocity.y));
		}
		else
		{
			if (!m_bIsDefend)
				body->SetLinearVelocity(b2Vec2(MMSpeed_Tortoise, oldVelocity.y));
			//else if (oldVelocity.x > 0.00001f)
			//	getBody()->SetLinearVelocity(b2Vec2(MMSpeed_TortoiseDefense, oldVelocity.y));
		}
	}

	////防守非滚动状态x轴速度更改
	//if (m_bIsDefend && !m_bIsRoll && (oldVelocity.x < -0.00001f || oldVelocity.x > 0.00001f))
	//{
	//	body->SetAwake(false);
	//	body->SetLinearVelocity(b2Vec2(0.0f, oldVelocity.y));
	//}
	m_lastPos.set(newPos);

	//空中校验,是否踩着物体,光线投射
	if (!m_bIsDefend)
	{
		m_bStepObject = false;
		float fPtm = getPTMRatio();
		b2Vec2 beginPoint1(newPos.x / fPtm, newPos.y / fPtm);	//投射起点
		b2Vec2 endPoint1(beginPoint1.x, beginPoint1.y - getContentSize().height / fPtm);	//投射终点
		b2World * pWorld = GameScene::getInstance()->getWorld();
		pWorld->RayCast(this, beginPoint1, endPoint1);
		if (!m_bStepObject)
		{
			//由于可能透过细小缝隙，故而执行二次投射
			b2Vec2 beginPoint2(beginPoint1.x + (m_bLeft ? -1 : 1) * getContentSize().width / 4 / fPtm, beginPoint1.y);
			b2Vec2 endPoint2(beginPoint2.x, endPoint1.y);
			pWorld->RayCast(this, beginPoint2, endPoint2);
		}

		if (!m_bStepObject)
		{
			//再走将会掉下，掉头
			changeVelocityDirection();
		}
	}
}

bool MonsterTortoise::isDefendRoll()
{
	if (m_bIsDefend && (getBody()->GetLinearVelocity().x < -0.00001f || getBody()->GetLinearVelocity().x > 0.00001f))
		return true;
	else
		return false;
}

void MonsterTortoise::removeSelf()
{
	m_bIsDie = true;
	GameScene::getInstance()->addRemoveSprite(this, GSTag_Robbit);
}

void MonsterTortoise::resetVelocityX()
{
	b2Body * body = getBody();
	body->SetLinearVelocity(b2Vec2(0.0f, body->GetLinearVelocity().y));
}

void MonsterTortoise::changeFixture()
{
	//状态校验
	if (updateFixStatus == UpdateTortoise_Invalid)
		return;

	//预先移除当前框架
	b2Body * body = getBody();
	for (b2Fixture * f = body->GetFixtureList(); f;)
	{
		b2Fixture * next = f->GetNext();
		body->DestroyFixture(f);
		f = next;
	}

	//添加各状态框架
	b2Fixture * fixture = nullptr;
	if (updateFixStatus == UpdateTortoise_TorKillL || updateFixStatus == UpdateTortoise_TorKillR)
	{
		//不处理被杀死时的框架
	}
	else if (updateFixStatus == UpdateTortoise_Normal)
	{
		//添加自然状态框架
		b2Vec2 vertices[6];	//乌龟形状
		TileMapLayer::commonMonsterShape(g_tortoiseNormalSize, vertices, 6);
		fixture = GameScene::getInstance()->getGameWorld()->AddPolygonFixture(body, vertices, 6);
		MonsterBase::setMonsterSize(g_tortoiseNormalSize);
		setAnchorPoint(Vec2(0.5f, 0.45f));
	}
	else if (updateFixStatus == UpdateTortoise_Defend)
	{
		//添加防御状态框架
		fixture = GameScene::getInstance()->getGameWorld()->AddCircleFixture(body, Vec2::ZERO, g_tortoiseDefendSize.width / 2);
		MonsterBase::setMonsterSize(g_tortoiseDefendSize);
		setAnchorPoint(Vec2(0.5f, 0.5f));
	}

	if (fixture)
	{
		b2Filter filter;
		filter.categoryBits = 0x0020;
		filter.maskBits = 0x0087;
		filter.groupIndex = 1;
		fixture->SetFilterData(filter);
		fixture->SetFriction(0.0f);
	}

	return;
}

