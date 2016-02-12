#include "CCBox2dSprite.h"
#include "MonsterBase.h"
#include "MarioSprite.h"
#include "GameScene.h"

MonsterBase::MonsterBase(b2World * pWorld)
:Box2dSprite(pWorld)
, m_beginPos(0.0f, 0.0f)
, m_monsterSize(0.0f, 0.0f)
, m_eMonsterStatus(MonsterStatus_Invalid)
, m_bMarioContactUseful(true)
{

}

MonsterBase::~MonsterBase()
{

}

enPosStatus MonsterBase::getPosStatus(MarioSprite * pMario)
{
	enPosStatus  posStatus = PosStatus_Invlid;

	Size contentSize = Director::getInstance()->getWinSize();

	float fDistance = getPositionX() - pMario->getPosition().x;
	fDistance = (fDistance < 0) ? -fDistance : fDistance;

	if (fDistance >= contentSize.width)
		posStatus = PosStatus_Invlid;
	else if ((fDistance <= contentSize.width / 2 + m_monsterSize.width) && m_eMonsterStatus == MonsterStatus_Invalid)
		posStatus = PosStatus_NoShow;
	else
		posStatus = PosStatus_Move;

	return posStatus;
}

bool MonsterBase::isMarioContactUseful()
{
	if (m_bMarioContactUseful == true)
	{
		m_bMarioContactUseful = false;
		schedule(schedule_selector(MonsterBase::updateMarioContact));
		return true;
	}
	else
	{
		return false;
	}
}

void MonsterBase::updateMarioContact(float dt)
{
	m_bMarioContactUseful = true;
	unschedule(schedule_selector(MonsterBase::updateMarioContact));
}

