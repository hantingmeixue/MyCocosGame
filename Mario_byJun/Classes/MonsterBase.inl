#include "MonsterBase.h"

void MonsterBase::setBeginPos(const Vec2 & pos)
{
	m_beginPos.setPoint(pos.x, pos.y);
}

const Vec2 & MonsterBase::getBeginPos()
{
	return m_beginPos;
}

void MonsterBase::setMonsterSize(const Size & size)
{
	m_monsterSize.setSize(size.width, size.height);
}

const Size & MonsterBase::getMonsterSize()
{
	return m_monsterSize;
}

void MonsterBase::setMonsterStatus(enMonsterStatus status)
{
	m_eMonsterStatus = status;
}

enMonsterStatus MonsterBase::getMonsterStatus()
{
	return m_eMonsterStatus;
}
