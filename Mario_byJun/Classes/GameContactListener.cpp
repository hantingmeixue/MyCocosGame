#include "GameContactListener.h"
#include "CCBox2dSprite.h"
#include "GameScene.h"
#include "MarioSprite.h"
#include "GameConstant.h"
#include "TileSprite.h"
#include "MonsterBase.h"
#include "MonsterTortoise.h"
#include <cmath>
//#include "SimpleAudioEngine.h"
//#include <algorithm>
////#include <UsuallyFunctions.h>
//
//using namespace CocosDenshion;
//
void GameContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture * fixtureA = contact->GetFixtureA();
	b2Fixture * fixtureB = contact->GetFixtureB();
	b2Body * bodyA = fixtureA->GetBody();
	b2Body * bodyB = fixtureB->GetBody();
	Box2dSprite * spriteA = static_cast<Box2dSprite *>(bodyA->GetUserData());
	Box2dSprite * spriteB = static_cast<Box2dSprite *>(bodyB->GetUserData());
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA == GSTag_Mario || tagB == GSTag_Mario)	//��ײ�����
	{
		//ʼ��ʹAΪӢ��
		if (tagB == GSTag_Mario)
		{
			std::swap(tagA, tagB);
			std::swap(bodyA, bodyB);
			std::swap(spriteA, spriteB);
			std::swap(fixtureA, fixtureB);
		}

		if (tagB == GSTag_GeneralTile || tagB == GSTag_SpecialTile)	//����������ײ
		{
			static_cast<TileSprite *>(spriteB)->marioContact(static_cast<MarioSprite *>(spriteA));
		}
		else if (tagB == GSTag_TransparentTile && !spriteB->isVisible())
		{
			enMarioStatus status = static_cast<MarioSprite *>(spriteA)->getMarioStatus();
			if (status == MarioSt_LJump || status == MarioSt_RJump)
			{
				spriteB->setVisible(true);

				//���û�������
				ActionInterval * baseAction = Spawn::create(
					MoveBy::create(0.1f, Vec2(0.0f, 20.0f)),
					CCScaleBy::create(0.1f, 1.3f),
					nullptr);

				spriteB->runAction(Sequence::create(
					baseAction, baseAction->reverse(),
					nullptr));
			}
		}
		else if (tagB == GSTag_Coin)	//������ײ���
		{
			playEffect(Effect_GetCoin);
			m_pGame->addRemoveSprite(spriteB, GSTag_Coin);
		}
		else if (tagB == GSTag_BigCoin)
		{
			playEffect(Effect_GetCoin);
			m_pGame->getBigCoin(spriteB);
		}
		else if ((tagB & 0x1000) != 0)	//������ײ
		{
			MonsterBase * monsterB = static_cast<MonsterBase *>(spriteB);
			if (monsterB->isMarioContactUseful())
				monsterB->marioContact(static_cast<MarioSprite *>(spriteA));
		}
		else if (tagB == GSTag_Swamm)	//��ײĢ��
		{
			m_pGame->addLife();
			playEffect(Effect_EatSwamm);
			m_pGame->addRemoveSprite(spriteB, GSTag_Swamm);
		}
		else if (tagB == GSTag_OverLine)
		{
			m_pGame->marioDie();
		}
		else if (tagB == GSTag_Flagpole)	//������ˣ�ʤ��
		{
			m_pGame->marioWin();
		}
	}
	else if (tagA == GSTag_OverLine || tagB == GSTag_OverLine)	//
	{
		//ʼ��ʹAΪ�����߽�
		if (tagB == GSTag_OverLine)
		{
			std::swap(tagA, tagB);
			std::swap(bodyA, bodyB);
			std::swap(spriteA, spriteB);
			std::swap(fixtureA, fixtureB);
		}

		if ((tagB & 0x1000) != 0)	//������ײ
		{
			m_pGame->resetMonster(static_cast<MonsterBase *>(spriteB));
		}
		else if (tagB == GSTag_Swamm)
		{
			m_pGame->addRemoveSprite(spriteB, GSTag_Swamm);
		}
	}
	else if ((tagA & 0x1000) != 0 && (tagB & 0x1000) != 0)	//���޻�����ײ
	{
		//�ж��Ƿ�����Ч״̬
		MonsterBase * monsterA = static_cast<MonsterBase *>(spriteA);
		MonsterBase * monsterB = static_cast<MonsterBase *>(spriteB);

		if ((monsterA->getMonsterStatus() == MonsterStatus_Invalid) || (monsterB->getMonsterStatus() == MonsterStatus_Invalid))
			return;

		bool bDefendTorRoll = false;	//�Ƿ�Ϊ���ع��������
		if (tagA == GSTag_Tortoise || tagB == GSTag_Tortoise)
		{
			//����������ڹ���ײ
			if (tagA == GSTag_Tortoise)
			{
				MonsterTortoise * tortoiseA = static_cast<MonsterTortoise *>(spriteA);
				if (tortoiseA->isDefendRoll())
				{
					static_cast<MonsterBase *>(spriteB)->defendTorRollContact(tortoiseA);
					bDefendTorRoll = true;
				}
			}
			if (!bDefendTorRoll && tagB == GSTag_Tortoise)
			{
				MonsterTortoise * tortoiseB = static_cast<MonsterTortoise *>(spriteB);
				if (tortoiseB->isDefendRoll())
				{
					static_cast<MonsterBase *>(spriteA)->defendTorRollContact(tortoiseB);
					bDefendTorRoll = true;
				}
			}
		}

		if (bDefendTorRoll == false)
		{
			//�ı���ײ���޵��ٶȷ���
			b2Vec2 velocityA = bodyA->GetLinearVelocity();
			b2Vec2 velocityB = bodyB->GetLinearVelocity();

			if ((velocityA.x < -0.00001f && velocityB.x > 0.00001f) ||
				(velocityB.x < -0.00001f && velocityA.x > 0.00001f))
			{
				//�ٶ��෴ʱ,�ı������ٶȷ���
				monsterA->changeVelocityDirection();
				monsterB->changeVelocityDirection();
			}
			else if ((velocityA.x < -0.00001f && velocityB.x < -0.00001f) ||
				(velocityA.x > 0.00001f && velocityB.x > 0.00001f))
			{
				//�ٶ���ͬʱ,�ı��ٶȴ��߷���
				if (std::abs(velocityA.x) > std::abs(velocityB.x))
				{
					monsterA->changeVelocityDirection();
				}
				else/* if (std::abs(velocityA.x) < std::abs(velocityB.x))*/
				{
					monsterB->changeVelocityDirection();
				}
			}
			else
			{
				if ((velocityA.x >= -0.00001f && velocityA.x <= 0.00001f) &&
					(velocityB.x >= -0.00001f && velocityB.x <= 0.00001f))
				{

				}
				else if (velocityB.x >= -0.00001f && velocityB.x <= 0.00001f)
				{
					monsterA->changeVelocityDirection();
				}
				else if (velocityA.x >= -0.00001f && velocityA.x <= 0.00001f)
				{
					monsterB->changeVelocityDirection();
				}
			}
		}
	}
	else if ((tagA == GSTag_Ground && tagB == GSTag_Tortoise) ||
		(tagB == GSTag_Ground && tagA == GSTag_Tortoise))
	{
		if (tagA == GSTag_Tortoise)
		{
			MonsterTortoise * tortoiseA = static_cast<MonsterTortoise *>(spriteA);
			if (tortoiseA->isDefend() && !tortoiseA->isRoll())
			{
				bodyA->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			}
		}
		else
		{
			MonsterTortoise * tortoiseB = static_cast<MonsterTortoise *>(spriteB);
			if (tortoiseB->isDefend() && !tortoiseB->isRoll())
			{
				bodyB->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			}
		}
	}
}

void GameContactListener::EndContact(b2Contact* contact)
{

}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Fixture * fixtureA = contact->GetFixtureA();
	b2Fixture * fixtureB = contact->GetFixtureB();
	b2Body * bodyA = fixtureA->GetBody();
	b2Body * bodyB = fixtureB->GetBody();
	Box2dSprite * spriteA = static_cast<Box2dSprite *>(bodyA->GetUserData());
	Box2dSprite * spriteB = static_cast<Box2dSprite *>(bodyB->GetUserData());
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA == GSTag_Mario || tagB == GSTag_Mario)
	{
		//ʼ��ʹAΪӢ��
		if (tagB == GSTag_Mario)
		{
			std::swap(tagA, tagB);
			std::swap(bodyA, bodyB);
			std::swap(spriteA, spriteB);
			std::swap(fixtureA, fixtureB);
		}

		if (tagB == GSTag_Swamm || (tagB & 0x1000) != 0)	//ʹĢ����ײ��Ч
		{
			contact->SetEnabled(false);
			return;
		}
		else if (tagB == GSTag_TransparentTile && !spriteB->isVisible())	//����͸������
		{
			contact->SetEnabled(false);
			return;
		}
	}
	else if ((tagA & 0x1000) != 0 && (tagB & 0x1000) != 0)	//���޻�����ײ
	{
		contact->SetEnabled(false);
		return;
	}
	else if ((tagA == GSTag_TransparentTile && tagB == GSTag_Tortoise) ||
		(tagB == GSTag_TransparentTile && tagA == GSTag_Tortoise))	//����͸������
	{
		if (tagB == GSTag_TransparentTile && !spriteB->isVisible())
		{
			contact->SetEnabled(false);
			return;
		}
		else if (tagA == GSTag_TransparentTile && !spriteA->isVisible())
		{
			contact->SetEnabled(false);
			return;
		}
	}

}
