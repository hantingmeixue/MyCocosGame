#include "TileMapLayer.h"
#include "UsuallyFunctions.h"
#include "GameScene.h"

USING_NS_CC;

extern const float g_fBgRollLength1;
extern const float g_fBgRollLength2;

TileMapLayer::TileMapLayer()
	:m_pGameWorld(NULL)
	,m_pWord(NULL)
	,m_pMap1(NULL)
	,m_pMap2(NULL)
	,m_pPhysicsNode1(NULL)
	,m_pPhysicsNode2(NULL)
	,m_nDestoryId(2)
	,m_bNeedAdd(true)
	,m_nMapLevel(1)
{
	m_liAllBirds.clear();
}

TileMapLayer * TileMapLayer::create(Box2dGameWorld * pGameWorld)
{
	TileMapLayer * pRet = new TileMapLayer();
	if (pRet && pRet->init(pGameWorld))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool TileMapLayer::init(Box2dGameWorld * pGameWorld)
{
	if (!CCLayer::init())
		return false;

	m_pGameWorld = pGameWorld;
	m_pWord = m_pGameWorld->GetWorld();

	m_pMap1 = CCTMXTiledMap::create("map1.tmx");
	m_pMap1->setAnchorPoint(CCPointZero);
	m_pMap1->setPosition(CCPointZero);
	addChild(m_pMap1, - 1);

	m_pPhysicsNode1 = CCSpriteBatchNode::create("common1.png", 120);
	m_pPhysicsNode1->setPosition(CCPointZero);
	addChild(m_pPhysicsNode1);

	m_pMap2 = CCTMXTiledMap::create("map1.tmx");
	m_pMap2->setAnchorPoint(CCPointZero);
	m_pMap2->setPosition(ccp(m_pMap1->getPositionX() + m_pMap1->getContentSize().width, 0.0f));
	addChild(m_pMap2, - 1);

	m_pPhysicsNode2 = CCSpriteBatchNode::create("common1.png", 120);
	m_pPhysicsNode2->setPosition(m_pMap2->getPosition());
	addChild(m_pPhysicsNode2);

	AddTileMapObjects(m_pMap1, m_pPhysicsNode1, false);

	return true;
}

bool TileMapLayer::UpdateMap(float fRollLength)
{
	//滚动地图
	if (m_pMap1->getPositionX() + m_pMap1->getContentSize().width + fRollLength < 0.0f)
	{
		m_pMap1->setPositionX(m_pMap2->getPositionX() + m_pMap2->getContentSize().width);
		m_pPhysicsNode1->setPositionX(m_pMap1->getPositionX());
		m_bNeedAdd = true;
		m_nDestoryId = 1;
		m_pPhysicsNode1->removeAllChildren();
		//schedule(schedule_selector(TileMapLayer::DestroyPhysics), 0.05f);
	}

	if (m_pMap2->getPositionX() + m_pMap2->getContentSize().width + fRollLength < 0.0f)
	{
		m_pMap2->setPositionX(m_pMap1->getPositionX() + m_pMap1->getContentSize().width);
		m_pPhysicsNode2->setPositionX(m_pMap2->getPositionX());
		m_bNeedAdd = true;
		m_nDestoryId = 2;
		m_pPhysicsNode2->removeAllChildren();
		//schedule(schedule_selector(TileMapLayer::DestroyPhysics), 0.05f);
	}

	//加载物体
	if (m_bNeedAdd && (m_pMap1->getPositionX() + m_pMap1->getContentSize().width / 2 < 0.0f))
	{
		m_bNeedAdd = false;
		m_pPhysicsNode2->removeAllChildren();
		AddGroundSky(m_pMap2, m_pPhysicsNode2);
		schedule(schedule_selector(TileMapLayer::AddPhysics), 0.05f);
		if (m_nMapLevel < 10)
			++m_nMapLevel;
	}

	if (m_bNeedAdd && (m_pMap2->getPositionX() + m_pMap2->getContentSize().width / 2 < 0.0f))
	{
		m_bNeedAdd = false;
		m_pPhysicsNode1->removeAllChildren();
		AddGroundSky(m_pMap1, m_pPhysicsNode1);
		schedule(schedule_selector(TileMapLayer::AddPhysics), 0.05f);
		if (m_nMapLevel < 10)
			++m_nMapLevel;
	}

	m_pMap1->setPositionX(m_pMap1->getPositionX() - fRollLength);
	m_pMap2->setPositionX(m_pMap2->getPositionX() - fRollLength);

	CCArray * children = m_pPhysicsNode1->getChildren();
	CCObject * obj = 0;
	std::vector<CCBox2dSprite *> vRemoveSprite;
	vRemoveSprite.reserve(children->count());
	CCARRAY_FOREACH(children, obj)
	{
		CCBox2dSprite * pSprite = dynamic_cast<CCBox2dSprite *>(obj);
		if (pSprite)
		{
			const CCPoint & point = pSprite->getPosition();
			int nTag = pSprite->getTag(); 
			if ((nTag != TagGround && nTag != TagSky) && ((pSprite->getParent()->convertToWorldSpace(point)).x + pSprite->getContentSize().width < -10.0f))
				vRemoveSprite.push_back(pSprite);
			else
				pSprite->setPosition(ccp(point.x - fRollLength, point.y));
		}
	}

	for (int i = 0; i < (int)vRemoveSprite.size(); ++i)
	{
		vRemoveSprite[i]->removeFromParent();
	}

	vRemoveSprite.clear();
	children = m_pPhysicsNode2->getChildren();
	obj = 0;
	CCARRAY_FOREACH(children, obj)
	{
		CCBox2dSprite * pSprite = dynamic_cast<CCBox2dSprite *>(obj);
		if (pSprite)
		{
			const CCPoint & point = pSprite->getPosition();
			int nTag = pSprite->getTag(); 
			if ((nTag != TagGround && nTag != TagSky) && ((pSprite->getParent()->convertToWorldSpace(point)).x + pSprite->getContentSize().width < -10.0f))
				vRemoveSprite.push_back(pSprite);
			else
				pSprite->setPosition(ccp(point.x - fRollLength, point.y));
		}
	}

	for (int i = 0; i < (int)vRemoveSprite.size(); ++i)
	{
		vRemoveSprite[i]->removeFromParent();
	}
	vRemoveSprite.clear();

	for (std::list<CCBox2dSprite *>::iterator iter = m_liAllBirds.begin(); iter != m_liAllBirds.end();)
	{
		if ((*iter) == NULL || (*iter)->getParent() == NULL)
			iter = m_liAllBirds.erase(iter);
		else if ((*iter)->convertToWorldSpace(ccp(0,0)).x <= CCDirector::sharedDirector()->getWinSize().width)
		{
			CCAction * pAction = CCMoveBy::create(2.0, ccp(rand() % 100 -300, (rand() % 2 ? 1 : -1) * 100));
			(*iter)->runAction(pAction);
			iter = m_liAllBirds.erase(iter);
		}
		else
			++iter;
	}

	return true;
}

void TileMapLayer::DestroyPhysics(float dt)
{
	CCNode * pDestroyNode = 0;
	if (m_nDestoryId == 1)
		pDestroyNode = m_pPhysicsNode1;
	else if (m_nDestoryId == 2)
		pDestroyNode = m_pPhysicsNode2;
	else
		CC_ASSERT(false);

	CCArray * children = pDestroyNode->getChildren();
	CCObject * obj;
	int nRemoveCount = 0;

	CCBox2dSprite * pAllSprite[10];
	memset(pAllSprite, 0, sizeof(pAllSprite));

	CCARRAY_FOREACH(children, obj)
	{
		CCBox2dSprite * pSprite = static_cast<CCBox2dSprite *>(obj);
		if (pSprite && pSprite->getParent()!= NULL)
		{
			pAllSprite[nRemoveCount++] = pSprite;
		}
		if (nRemoveCount >= 2)
			break;
	}

	for (int i = 0; i < nRemoveCount; ++i)
		pAllSprite[i]->removeFromParent();

	if (nRemoveCount < 2)
	{
		unschedule(schedule_selector(TileMapLayer::DestroyPhysics));
	}
}

void TileMapLayer::AddPhysics(float dt)
{
	CCTMXTiledMap * pMap = 0;
	CCNode * pAddNode = 0;
	if (m_nDestoryId == 1)
	{
		pMap = m_pMap1;
		pAddNode = m_pPhysicsNode1;
	}
	else if (m_nDestoryId == 2)
	{
		pMap = m_pMap2;
		pAddNode = m_pPhysicsNode2;
	}
	else
		CC_ASSERT(false);

	bool bGoOn = AddTileMapObjects(pMap, pAddNode, true);
	if (!bGoOn)
		unschedule(schedule_selector(TileMapLayer::AddPhysics));
}

//添加天空、地面
void TileMapLayer::AddGroundSky(CCTMXTiledMap * pMap, CCNode * pPhysicsNode)
{
	CCPoint point = (pMap->getParent())->convertToWorldSpace(pMap->getPosition());
	point.y = pMap->getTileSize().height * 2 / CCDirector::sharedDirector()->getContentScaleFactor();

	CCBox2dSprite * pSprite = CCBox2dSprite::createWithSpriteFrameName("cheat.png",m_pWord);
	pSprite->setAnchorPoint(ccp(1,1));
	b2Fixture * pFixture = m_pGameWorld->CreateEdgeBody(pSprite, point, pMap->getContentSize().width);
	b2Filter filter;
	filter.categoryBits = 0x0001;
	filter.maskBits = 0xffff;
	pFixture->SetFilterData(filter);
	pPhysicsNode->addChild(pSprite, 1, TagGround);

	//设置天空边界
	point.y = CCDirector::sharedDirector()->getWinSize().height - 10;

	pSprite = CCBox2dSprite::createWithSpriteFrameName("cheat.png",m_pWord);
	pSprite->setAnchorPoint(ccp(1,0));
	pFixture = m_pGameWorld->CreateEdgeBody(pSprite, point, pMap->getContentSize().width);
	filter.categoryBits = 0x0002;
	filter.maskBits = 0x0004;
	pFixture->SetFilterData(filter);
	pPhysicsNode->addChild(pSprite, 1, TagSky);
}

bool TileMapLayer::AddTileMapObjects(CCTMXTiledMap * pMap, CCNode * pPhysicsNode, bool bSchedule)
{
	static int nAllCount = 0;
	if (!bSchedule)
		AddGroundSky(pMap, pPhysicsNode);

	const char * szObjectName[3] = {"heart", "bird", "airShip",};
	const char * szSpriteFrameName[3] = {"heart.png", "bird1.png", "airship.png",};
	eObjectTag tag[3] = {TagHeart, TagBird, TagAirShip};

	float fScaleFactor = CCDirector::sharedDirector()->getContentScaleFactor();
	int nNowCount = 0;
	CCPoint pointMap = (pMap->getParent())->convertToWorldSpace(pMap->getPosition());
	for (int i = 0; i < CountArray(szObjectName); ++i)
	{
		CCTMXObjectGroup * pGroup = pMap->objectGroupNamed(szObjectName[i]);
		CCArray * pObjects = pGroup->getObjects();
		CCObject * pObj = NULL;
		CCDictionary * dict = NULL;

		CCARRAY_FOREACH(pObjects, pObj)
		{
			//随机限制加载物体
			if (tag[i] == TagHeart)
			{
				if (rand() % 2 >= 1)
					continue;
			}
			else if (tag[i] == TagAirShip)
			{
				if (rand() % 15 > m_nMapLevel)
					continue;
			}
			else if (tag[i] == TagBird)
			{
				if (rand() % 15 > m_nMapLevel)
					continue;
			}

			++nNowCount;
			if (bSchedule)
			{
				if (nNowCount <= nAllCount)
					continue;
				if (nNowCount - nAllCount > 2)
					break;
			}

			dict = dynamic_cast<CCDictionary *>(pObj);
			if (!dict)
				break;

			const char * key = "x";
			float x = dynamic_cast<CCString *>(dict->objectForKey(key))->floatValue();
			key = "y";
			float y = dynamic_cast<CCString *>(dict->objectForKey(key))->floatValue();
			key = "width";
			float width = dynamic_cast<CCString *>(dict->objectForKey(key))->floatValue();
			key = "height";
			float height = dynamic_cast<CCString *>(dict->objectForKey(key))->floatValue();

			CCBox2dSprite * pSprite = CCBox2dSprite::createWithSpriteFrameName(szSpriteFrameName[i], m_pWord);
			CCPoint point((x + width / 2) / fScaleFactor + pointMap.x, (y + height / 2) / fScaleFactor);
			//CCLOG("x %f, y %f, width %f, height %f, point.x %f, point.y %f\n", x,y, width, height, point.x, point.y);

			switch(tag[i])
			{
			case TagHeart:
				{
					b2Fixture * fixture = m_pGameWorld->CreateCircleBody(pSprite, point, b2_staticBody, CCPointZero, pSprite->getContentSize().width / 4);
					//pSprite->getBody()->SetFixedRotation(true);
					b2Filter filter;
					filter.categoryBits = 0x0008;
					filter.maskBits = 0x0004;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);
					fixture->SetSensor(true);
				}
				break;
			case TagBird:
				{
					b2Fixture * fixture = m_pGameWorld->CreateCircleBody(pSprite, point, b2_dynamicBody);
					pSprite->getBody()->SetFixedRotation(true);
					pSprite->EnbaleGravity(false);
					b2Filter filter;
					filter.categoryBits = 0x000f;
					filter.maskBits = 0x0005;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);
					//fixture->SetSensor(true);
				}
				break;
			case TagAirShip:
				{
					b2Vec2 vertices[5];
					vertices[0].Set(-20.0f, 75.0f);
					vertices[1].Set(-91.0f, 20.0f);
					vertices[2].Set(-45.0f, -35.0f);
					vertices[3].Set(40.0f, -35.0f);
					vertices[4].Set(91.0f, 20.0f);
					VerticesByScaleFactor(vertices, 5, fScaleFactor);

					b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, point, b2_dynamicBody, vertices, 5);
					pSprite->getBody()->SetFixedRotation(true);
					pSprite->EnbaleGravity(false);

					b2Filter filter;
					filter.categoryBits = 0x0010;
					filter.maskBits = 0x0005;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);

					vertices[0].Set(-45.0f, -35.0f);
					vertices[1].Set(-30.0f, -75.0f);
					vertices[2].Set(40.0f, -75.0f);
					vertices[3].Set(40.0f, -35.0f);
					VerticesByScaleFactor(vertices, 4, fScaleFactor);
					m_pGameWorld->AddPolygonFixture(pSprite->getBody(), vertices, 4)->SetFilterData(filter);
				}
				break;
			default:
				CC_ASSERT(false);
				break;
			}

			pPhysicsNode->addChild(pSprite, 1, tag[i]);

			if (tag[i] == TagBird)
			{
				m_liAllBirds.push_back(pSprite);
				CCAnimation * pAnimation = CCAnimationCache::sharedAnimationCache()->animationByName("bird");
				pAnimation->setLoops(-1);
				CCAnimate * pAction = CCAnimate::create(pAnimation);
				pSprite->runAction(pAction);
			}
		}
		if (bSchedule)
		{
			if (nNowCount - nAllCount > 2)
				break;
		}
	}

	if (bSchedule)
	{
		if (nNowCount - nAllCount > 2)
		{
			nAllCount = nNowCount - 1;
			return true;
		}
		else
		{
			nAllCount = 0;
			return false;
		}

	}

	return false;
}

//加载新一个地图
void TileMapLayer::AddNextMap(cocos2d::CCTMXTiledMap * pMap, cocos2d::CCNode * pPhysicsNode)
{

}
