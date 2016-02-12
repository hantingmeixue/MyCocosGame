#include "MonsterRobbit.h"
#include "MarioSprite.h"
#include "GameScene.h"
#include "GameConstant.h"
#include "MonsterTortoise.h"

MonsterRobbit::MonsterRobbit(b2World * pWorld)
:MonsterBase(pWorld)
, m_lastPos(0.0f, 0.0f)
, m_bLeft(true)
, m_bIsDie(false)
{

}

MonsterRobbit::~MonsterRobbit()
{

}

MonsterRobbit * MonsterRobbit::create(const char *pszFileName, b2World * pWorld)
{
	MonsterRobbit * pRet = new MonsterRobbit(pWorld);
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

MonsterRobbit * MonsterRobbit::createWithSpriteFrameName(const char * pszFrameName, b2World * pWorld)
{
	MonsterRobbit * pRet = new MonsterRobbit(pWorld);
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

void MonsterRobbit::setInvalidStatus()
{
	if (m_bIsDie)
		return;
	setMonsterStatus(MonsterStatus_Invalid);
	//复位怪兽信息
	setPosition(getBeginPos());
	stopAllActions();
	m_lastPos = getBeginPos();
	setVisible(false);
	unschedule(schedule_selector(MonsterRobbit::checkPosSchedule));
	getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	m_bLeft = true;
}

void MonsterRobbit::setNormalStatus()
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
		getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_Robbit, 0.0f));
		Animation * animation = AnimationCache::getInstance()->getAnimation("robbitLeft");
		runAction(Animate::create(animation));
	}
	else
	{
		getBody()->SetLinearVelocity(b2Vec2(MMSpeed_Robbit, 0.0f));
		Animation * animation = AnimationCache::getInstance()->getAnimation("robbitRight");
		runAction(Animate::create(animation));
	}
	schedule(schedule_selector(MonsterRobbit::checkPosSchedule), 0.1f);
}

void MonsterRobbit::marioContact(MarioSprite * pMario)
{
	if (pMario == nullptr)
	{
		CC_ASSERT(false);
		return;
	}

	if (m_bIsDie)
	{
		return;
	}
	else if (getMonsterStatus() == MonsterStatus_Invalid)
	{
		return;
	}
	else if (pMario->getMarioStatus() == MarioSt_LDown || pMario->getMarioStatus() == MarioSt_RDown ||
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
			//杀死兔子
			if (!bMarioTumble)
			{
				Animation * animation = nullptr;
				if (m_bLeft)
				{
					animation = AnimationCache::getInstance()->getAnimation("robbitLeftDie");
				}
				else
				{
					animation = AnimationCache::getInstance()->getAnimation("robbitRightDie");
				}
				animation->setRestoreOriginalFrame(false);
				stopAllActions();
				runAction(Sequence::create(Animate::create(animation), /*CCDelayTime::create(10.0f),*/
					CallFunc::create(CC_CALLBACK_0(MonsterRobbit::removeSelf, this)), NULL));

				//初始化信息
				getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				m_bLeft = false;
				m_bIsDie = true;
				unschedule(schedule_selector(MonsterRobbit::checkPosSchedule));
				scheduleOnce(schedule_selector(MonsterRobbit::removeFixtureSchedule), 0.0f);	//移除框架
				EnableGravity(false);
				setMonsterStatus(MonsterStatus_Invalid);
				playEffect(Effect_KillMonster);

				//回弹马里奥
				b2Vec2 marioVelocity = pMario->getBody()->GetLinearVelocity();
				pMario->getBody()->SetLinearVelocity(b2Vec2(marioVelocity.x, -marioVelocity.y));
			}
			else
			{
				removeSelf();
			}
		}
		else
		{
			GameScene::getInstance()->reduceLife();
			if ((m_bLeft && posM.x > pos.x) || (!m_bLeft && posM.x <= pos.x))
				changeVelocityDirection();
		}
	}
	else
	{
		GameScene::getInstance()->reduceLife();
		if ((m_bLeft && pMario->getPositionX() > getPositionX()) || (!m_bLeft && pMario->getPositionX() <= getPositionX()))
			changeVelocityDirection();
	}
}

void MonsterRobbit::defendTorRollContact(MonsterTortoise * pTortoise)
{
	//状态校验
	if (m_bIsDie)
		return;

	//复位各信息
	m_bIsDie = true;
	stopAllActions();
	getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	unschedule(schedule_selector(MonsterRobbit::checkPosSchedule));
	scheduleOnce(schedule_selector(MonsterRobbit::removeFixtureSchedule), 0.0f);	//移除框架
	setMonsterStatus(MonsterStatus_Invalid);

	//设置显示
	if (m_bLeft)
	{
		SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
			"monster/robbit_left_die/robbit_left_die1.png");
		setSpriteFrame(frame);
	}
	else
	{
		SpriteFrame * frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(
			"monster/robbit_right_die/robbit_right_die1.png");
		setSpriteFrame(frame);
	}

	//设置动作
	Spawn * action = nullptr;
	Vec2 mePos = getPosition();
	Vec2 torPos = pTortoise->getPosition();
	float fPosLevel = (mePos.y + 100) / 100.0f;	//高度等级

	if (mePos.x > torPos.x)
	{
		action = Spawn::create(JumpTo::create(fPosLevel * 1.0f, Vec2(mePos.x + 400 * fPosLevel, -100.0f), 800.0f, 1),
			RotateBy::create(fPosLevel * 1.0f, 720 * fPosLevel), nullptr);
	}
	else
	{
		action = Spawn::create(JumpTo::create(fPosLevel * 1.0f, Vec2(mePos.x - 400 * fPosLevel, -100.0f), 800.0f, 1),
			RotateBy::create(fPosLevel * 1.0f, -720 * fPosLevel), nullptr);
	}
	runAction(Sequence::create(action, CallFunc::create(CC_CALLBACK_0(MonsterRobbit::removeSelf, this)), nullptr));
}

void MonsterRobbit::changeVelocityDirection()
{
	//状态判断
	if (getMonsterStatus() == MonsterStatus_Invalid || m_bIsDie)
		return;

	b2Vec2 oldVelocity = getBody()->GetLinearVelocity();

	if (m_bLeft)
	{
		getBody()->SetLinearVelocity(b2Vec2(MMSpeed_Robbit, oldVelocity.y));
		m_bLeft = false;
		stopAllActions();
		Animation * animation = AnimationCache::getInstance()->getAnimation("robbitRight");
		runAction(Animate::create(animation));
	}
	else
	{
		getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_Robbit, oldVelocity.y));
		m_bLeft = true;
		stopAllActions();
		Animation * animation = AnimationCache::getInstance()->getAnimation("robbitLeft");
		runAction(Animate::create(animation));
	}
}

void MonsterRobbit::checkPosSchedule(float dt)
{
	Vec2 newPos = getPosition();
	if (newPos.x - m_lastPos.x >= -0.00001f && newPos.x - m_lastPos.x <= 0.00001f)
	{
		//转向处理
		changeVelocityDirection();
	}
	else
	{
		b2Vec2 oldVelocity = getBody()->GetLinearVelocity();
		if ((oldVelocity.y - 0.0f >= -0.00001f) && (oldVelocity.y - 0.0f <= 0.00001f) && !(rand() % 200))	//偶尔跳起
			oldVelocity.y = 15.0f;
		if (m_bLeft)
			getBody()->SetLinearVelocity(b2Vec2(-MMSpeed_Robbit, oldVelocity.y));
		else
			getBody()->SetLinearVelocity(b2Vec2(MMSpeed_Robbit, oldVelocity.y));
	}
	m_lastPos.set(newPos);
}

void MonsterRobbit::removeFixtureSchedule(float dt)
{
	b2Body * body = getBody();
	for (b2Fixture * fixture = body->GetFixtureList(); fixture;)
	{
		b2Fixture * next = fixture->GetNext();
		body->DestroyFixture(fixture);
		fixture = next;
	}

	unschedule(schedule_selector(MonsterRobbit::removeFixtureSchedule));
}

void MonsterRobbit::removeSelf()
{
	m_bIsDie = true;
	GameScene::getInstance()->addRemoveSprite(this, GSTag_Robbit);
}

