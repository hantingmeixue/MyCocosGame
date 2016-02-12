#include "GameContactListener.h"
#include "CCBox2dSprite.h"
#include "GameScene.h"
#include "Hero.h"
#include "SimpleAudioEngine.h"
#include <algorithm>
#include <UsuallyFunctions.h>

using namespace CocosDenshion;

void GameContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture * fixtureA = contact->GetFixtureA();
	b2Fixture * fixtureB = contact->GetFixtureB();
	b2Body * bodyA = fixtureA->GetBody();
	b2Body * bodyB = fixtureB->GetBody();
	CCBox2dSprite * spriteA = static_cast<CCBox2dSprite *>(bodyA->GetUserData());
	CCBox2dSprite * spriteB = static_cast<CCBox2dSprite *>(bodyB->GetUserData());
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA == TagHero || tagB == TagHero)
	{
		//始终使A为英雄
		if (tagB == TagHero)
		{
			std::swap(tagA, tagB);
			std::swap(bodyA, bodyB);
			std::swap(spriteA, spriteB);
			std::swap(fixtureA, fixtureB);
		}

		//防止同一物体多个框架均接触检测
		if (tagB == TagAirShip || tagB == TagBird || tagB == TagHeart)
		{
			for (int i = 0; i < CountArray(m_pPrevBodys); ++i)
			{
				if (m_pPrevBodys[i] == bodyB)
				{
					return ;
				}
			}
			for (int i = 0; i < CountArray(m_pPrevBodys); ++i)
			{
				if (m_pPrevBodys[i] == 0)
				{
					m_pPrevBodys[i] = bodyB;
					break;
				}
			}
		}

		SimpleAudioEngine * pAEngine = SimpleAudioEngine::sharedEngine();
		switch(tagB)
		{
		case TagGround:
			{
				m_pGame->ChangeBlood(m_bCheat ? GroudFlood1 :GroudFlood);
				pAEngine->playEffect("sound/ground.mp3");
			}
			break;
		case TagAirShip:
			{
				m_pGame->ChangeBlood(m_bCheat ? AirShipFlood1 :AirShipFlood);
				pAEngine->playEffect("sound/hit.mp3");
			}
			break;
		case TagBird:
			{
				m_pGame->ChangeBlood(m_bCheat ? BirdFlood1 :BirdFlood);
				pAEngine->playEffect("sound/hit.mp3");
			}
			break;
		case TagHeart:
			{
				m_pGame->ChangeBlood(m_bCheat ? HeartFlood1 :HeartFlood);
				m_pGame->AddRemoveSprite(spriteB);
				pAEngine->playEffect("sound/heart.mp3");
			}
			break;
		default:
			break;
		}
	}

}

void GameContactListener::EndContact(b2Contact* contact)
{
	b2Fixture * fixtureA = contact->GetFixtureA();
	b2Fixture * fixtureB = contact->GetFixtureB();
	b2Body * bodyA = fixtureA->GetBody();
	b2Body * bodyB = fixtureB->GetBody();
	CCBox2dSprite * spriteA = static_cast<CCBox2dSprite *>(bodyA->GetUserData());
	CCBox2dSprite * spriteB = static_cast<CCBox2dSprite *>(bodyB->GetUserData());
	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA == TagHero)
	{
		for (int i = 0; i < CountArray(m_pPrevBodys); ++i)
		{
			if (m_pPrevBodys[i] == bodyB)
			{
				m_pPrevBodys[i] = 0;
				break;
			}
		}
	}
	else if (tagB == TagHero)
	{
		for (int i = 0; i < CountArray(m_pPrevBodys); ++i)
		{
			if (m_pPrevBodys[i] == bodyA)
			{
				m_pPrevBodys[i] = 0;
				break;
			}
		}
	}
}

void GameContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Fixture * fixtureA = contact->GetFixtureA();
	b2Fixture * fixtureB = contact->GetFixtureB();

	int tagA = static_cast<CCBox2dSprite *>(fixtureA->GetBody()->GetUserData())->getTag();
	int tagB = static_cast<CCBox2dSprite *>(fixtureB->GetBody()->GetUserData())->getTag();

	if ((tagA == TagHero && (tagB == TagBird || tagB == TagAirShip)) ||
		(tagB == TagHero && (tagA == TagBird || tagA == TagAirShip)))
	{
		contact->SetEnabled(false);
	}
}

void GameContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}