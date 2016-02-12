#include "UsuallyFunctions.h"

USING_NS_CC;

CCAnimation * CreateAnimationByName(const char * szAnimationName, int nAnimationCount, float delay)
{
	CCArray * pSpriteFrameNames = new CCArray;
	pSpriteFrameNames->autorelease();
	CCSpriteFrameCache * pCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	for (int i = 1; i <= nAnimationCount; ++i)
	{
		char szTempName[260] = {0};
		int nLen = snprintf(szTempName, sizeof(szTempName), "%s%d.png", szAnimationName, i);
		if (nLen >= CountArray(szTempName))
			CC_ASSERT(false);
		CCSpriteFrame * pFrame = pCache->spriteFrameByName(szTempName);
		pSpriteFrameNames->addObject(pFrame);
	}

	CCAnimation * pAnimation = CCAnimation::createWithSpriteFrames(pSpriteFrameNames, delay);
	return pAnimation;
}

void VerticesByScaleFactor(b2Vec2 * vertices, int nCount, float fScaleFactor)
{
	CC_ASSERT(vertices != NULL && nCount > 0 && fScaleFactor > 0);

	for (int i = 0; i < nCount; ++i)
	{
		vertices[i].x /= fScaleFactor;
		vertices[i].y /= fScaleFactor;
	}
}
