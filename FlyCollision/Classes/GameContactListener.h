#ifndef _GAME_CONTACT_LISTENER_H_
#define _GAME_CONTACT_LISTENER_H_

#include "Box2D/Box2D.h"

class GameScene;

enum eFloodChange
{
	GroudFlood = -20,
	AirShipFlood = -10,
	BirdFlood = -5,
	HeartFlood = 5,

	GroudFlood1 = -1,
	AirShipFlood1 = -1,
	BirdFlood1 = -1,
	HeartFlood1 = 50,
};

class GameContactListener : public b2ContactListener
{
public:
	GameContactListener(GameScene * pGame, bool bCheat)
		:m_pGame(pGame)
		,m_bCheat(bCheat)
	{
		memset(m_pPrevBodys, 0, sizeof(m_pPrevBodys));
	}
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

private:
	GameScene * m_pGame;
	bool m_bCheat;
	b2Body * m_pPrevBodys[20];
};


#endif	//_GAME_CONTACT_LISTENER_H_
