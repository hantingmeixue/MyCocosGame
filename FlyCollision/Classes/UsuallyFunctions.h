#ifndef _USUALLY_FUNCTIONS_H_
#define _USUALLY_FUNCTIONS_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#define CountArray(array) (sizeof(array) / sizeof(array[0]))

cocos2d::CCAnimation * CreateAnimationByName(const char * szAnimationName, int nAnimationCount, float delay);

void VerticesByScaleFactor(b2Vec2 * vertices, int nCount, float fScaleFactor);

#endif	//_USUALLY_FUNCTIONS_H_
