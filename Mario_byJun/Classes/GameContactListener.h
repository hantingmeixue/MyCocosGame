#ifndef _GAME_CONTACT_LISTENER_H_
#define _GAME_CONTACT_LISTENER_H_

#include "Box2D/Box2D.h"

class GameScene;

class GameContactListener : public b2ContactListener
{
public:
	GameContactListener(GameScene * pGame)
		:m_pGame(pGame)
	{
	}
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){}

private:
	GameScene * m_pGame;
};


#endif	//_GAME_CONTACT_LISTENER_H_
