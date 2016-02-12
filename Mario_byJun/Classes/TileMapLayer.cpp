#include "TileMapLayer.h"
#include "CCBox2dSprite.h"
#include "TileSprite.h"
#include "UsuallyFunctions.h"
#include "GameConstant.h"
#include "GameScene.h"
#include "MonsterBase.h"
#include "MonsterRobbit.h"
#include "MonsterTortoise.h"

USING_NS_CC;

const std::string TileMapLayer::m_szEdgeObjectName[7] =
{
	"ground", "bamboo", "step_tile", "friction_edge",
	"flag_pot", "flagpole", "limit_line"
};

const std::string TileMapLayer::m_szDynamicObjectName[7] =
{
	"coin", "big_coin", "general_tile", "special_tile", "transparent_tile",
	"monster_robbit", "monster_tortoise",
};

//边界对象类型
enum enEdgeObjectType
{
	EOType_Ground = 0,			//地面
	EOType_Bamboo = 1,			//竹子
	EOType_StepTile = 2,		//台阶
	EOType_FrictionEdge = 3,	//摩擦边界
	EOType_FlagPot = 4,	//旗杆底部
	EOType_Flagpole = 5,	//旗杆
	EOType_LimitLine = 6,	//限制线
};

//动态对象类型
enum enDynamicObjectType
{
	DOType_Coin = 0,			//金币
	DOType_BigCoin = 1,			//大金币
	DOType_GeneralTile = 2,		//通常土块
	DOType_SpecialTile = 3,		//特殊土块
	DOType_TransparentTile = 4,	//透明土块
	DOType_MonsterRobbit = 5,	//怪兽兔子
	DOType_MonsterTortoise = 6,	//怪兽乌龟
};

TileMapLayer::TileMapLayer()
:m_pGameWorld(nullptr)
, m_pWorld(nullptr)
, m_pGameScene(nullptr)
, m_pMap1(nullptr)
, m_pBgLyaer1(nullptr)
, m_pBgLyaer2(nullptr)
, m_pFlag(nullptr)
, m_fFlagHeight(0.0f)
, m_bSliding(false)
{

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
	if (!Layer::init())
		return false;

	m_pGameWorld = pGameWorld;
	m_pWorld = m_pGameWorld->GetWorld();
	m_pGameScene = GameScene::getInstance();

	m_pMap1 = TMXTiledMap::create("map_1.tmx");
	addChild(m_pMap1, 20);

	//隐藏图层
	{
		m_pMap1->getLayer("solid")->setVisible(false);
		m_pMap1->getLayer("coin")->setVisible(false);
	}

	//加载地图元素
	addTileMapObjects(m_pMap1);

	//添加塔
	Sprite * pTower = Sprite::createWithSpriteFrameName("object/tower.png");
	pTower->setAnchorPoint(Vec2(1.0f, 0.0f));
	pTower->setPosition(Vec2(m_pMap1->getContentSize().width, m_pMap1->getTileSize().height * 2 / CC_CONTENT_SCALE_FACTOR()));
	m_pMap1->addChild(pTower, 20);

	//添加结束线边界
	{
		Box2dSprite * pSprite = Box2dSprite::create(m_pWorld);

		b2Fixture * fixture = m_pGameWorld->CreateEdgeBody(pSprite, Vec2(-300.0f, -300.0f), m_pMap1->getContentSize().width + 300);

		b2Filter filter;
		filter.categoryBits = 0x0080;
		filter.maskBits = 0x0061;
		fixture->SetFilterData(filter);
		fixture->SetFriction(0.0f);
		fixture->SetSensor(true);

		this->addChild(pSprite, 10, GSTag_OverLine);
	}

	//构造背景图层1
	m_pBgLyaer1 = Layer::create();
	float fBgLength = 0.0f;

	Sprite * pBgSprite = Sprite::create("Bg/bg1_1.png");
	pBgSprite->setAnchorPoint(Vec2::ZERO);
	pBgSprite->setPosition(Vec2(fBgLength, 0));
	m_pBgLyaer1->addChild(pBgSprite);
	fBgLength += pBgSprite->getContentSize().width - 20;

	for (int i = 0; i < 6; ++i)
	{
		pBgSprite = Sprite::create("Bg/bg1_2.png");
		pBgSprite->setAnchorPoint(Vec2::ZERO);
		pBgSprite->setPosition(Vec2(fBgLength, 0));
		m_pBgLyaer1->addChild(pBgSprite);
		fBgLength += pBgSprite->getContentSize().width;
	}

	//构造背景图层2
	m_pBgLyaer2 = Layer::create();
	fBgLength = 0.0f;

	for (int i = 0; i < 3; ++i)
	{
		pBgSprite = Sprite::create("Bg/bg2_1.png");
		pBgSprite->setAnchorPoint(Vec2::ZERO);
		pBgSprite->setPosition(Vec2(fBgLength, 0));
		m_pBgLyaer2->addChild(pBgSprite);
		fBgLength += pBgSprite->getContentSize().width;
	}

	this->addChild(m_pBgLyaer1, 5);
	this->addChild(m_pBgLyaer2, 1);

	return true;
}

void TileMapLayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);

#ifdef _DEBUG
	m_customCmd.init(_globalZOrder + 1, transform, flags);
	m_customCmd.func = CC_CALLBACK_0(Box2dGameWorld::BeginDraw, m_pGameWorld, transform, flags);
	renderer->addCommand(&m_customCmd);
#endif
}

void TileMapLayer::updateBgLayerPos(MarioSprite * pMario)
{
	//更新背景图层
	Size size = Director::getInstance()->getWinSize();
	Vec2 marioPos = pMario->getPosition();
	if (marioPos.x <= size.width / 2)
		return;

	float fDistence = marioPos.x - size.width / 2;
	float fMove1 = fDistence * 0.7;
	float fMove2 = fDistence * 0.85;
	m_pBgLyaer1->setPositionX(fMove1);
	m_pBgLyaer2->setPositionX(fMove2);
}

void TileMapLayer::addTileMapObjects(cocos2d::TMXTiledMap * pMap)
{
	auto groups = pMap->getObjectGroups();

	if (groups.size() > 0)
	{
		for (auto iter = groups.begin(); iter != groups.end(); ++iter)
		{
			TMXObjectGroup * group = (*iter);

			//检测是否边界对象
			bool bIsEdgeObject = false;

			for (int i = 0; i < CountArray(m_szEdgeObjectName); ++i)
			{
				if (group->getGroupName() == m_szEdgeObjectName[i])
				{
					bIsEdgeObject = true;
					break;
				}
			}
			if (bIsEdgeObject)
			{
				addEdgeObjects(group);
				continue;
			}

			//检测是否动态对象
			bool bIsDynamicObject = false;

			for (int i = 0; i < CountArray(m_szDynamicObjectName); ++i)
			{
				if (group->getGroupName() == m_szDynamicObjectName[i])
				{
					bIsDynamicObject = true;
					break;
				}
			}
			if (bIsDynamicObject)
			{
				addDynamicObjects(group);
				continue;
			}

			//对象遗漏
			if (!bIsEdgeObject && !bIsDynamicObject)
			{
				CCASSERT(false, "tilemap object is missed!");
				continue;
			}
		}
	}
}

/*
游戏碰撞筛选掩码分配:
马里奥:0x0001
地面、竹子:0x0002
土块:0x0004
金币:0x0008
土块边界:0x0010
怪兽:0x0020
蘑菇:0x0040
结束边界:0x0080
旗杆:0x0100
限制线:0x0200

游戏碰撞筛选分组分配:
环境之间:-1
怪兽之间:1
*/

void TileMapLayer::addEdgeObjects(TMXObjectGroup * edgeGroup)
{
	ValueVector & objects = edgeGroup->getObjects();

	//判断对象组类型
	int nGroupType = -1;

	for (int i = 0; i < CountArray(m_szEdgeObjectName); ++i)
	{
		if (edgeGroup->getGroupName() == m_szEdgeObjectName[i])
		{
			nGroupType = i;
			break;
		}
	}
	if (nGroupType == -1)
	{
		CCASSERT(false, "this group is not Edge!");
		return;
	}

	//遍历组内对象
	for (auto & obj : objects)
	{
		ValueMap & dict = obj.asValueMap();

		if (dict["polylinePoints"].getType() == Value::Type::VECTOR)
		{
			float fStartX = dict["x"].asFloat();
			float fStartY = dict["y"].asFloat();

			ValueVector & linePoints = dict["polylinePoints"].asValueVector();
			std::vector<b2Vec2> allPoints;
			allPoints.reserve(linePoints.size());

			for (auto iterPoints = linePoints.begin(); iterPoints != linePoints.end(); ++iterPoints)
			{
				ValueMap & point = (*iterPoints).asValueMap();
				float x = point["x"].asInt() / CC_CONTENT_SCALE_FACTOR();
				float y = -point["y"].asInt() / CC_CONTENT_SCALE_FACTOR();
				allPoints.push_back(b2Vec2(x, y));
			}

			Box2dSprite * pSprite = Box2dSprite::create(m_pWorld);
			pSprite->setAnchorPoint(Vec2::ONE);

			//判断精灵类型
			switch (nGroupType)
			{
			case EOType_Ground:
				{
					b2Fixture * fixture = m_pGameWorld->CreateChainBody(pSprite, Vec2(fStartX, fStartY), allPoints.data(), allPoints.size());

					b2Filter filter;
					filter.categoryBits = 0x0002;
					filter.maskBits = 0x0061;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);
					fixture->SetFriction(0.2f);

					m_pMap1->addChild(pSprite, 10, GSTag_Ground);
				}
				break;
			case EOType_Bamboo:
				{
					b2Fixture * fixture = m_pGameWorld->CreateChainBody(pSprite, Vec2(fStartX, fStartY), allPoints.data(), allPoints.size());

					b2Filter filter;
					filter.categoryBits = 0x0002;
					filter.maskBits = 0x0061;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);
					fixture->SetFriction(0.0f);

					m_pMap1->addChild(pSprite, 10, GSTag_Bamboo);
				}
				break;

			case EOType_FrictionEdge:
			{
				b2Fixture * fixture = m_pGameWorld->CreateChainBody(pSprite, Vec2(fStartX, fStartY), allPoints.data(), allPoints.size());

				b2Filter filter;
				filter.categoryBits = 0x0002;
				filter.maskBits = 0x0021;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.2f);

				m_pMap1->addChild(pSprite, 10, GSTag_FrictionEdge);
			}
				break;

			case EOType_LimitLine:
			{
				b2Fixture * fixture = m_pGameWorld->CreateChainBody(pSprite, Vec2(fStartX, fStartY), allPoints.data(), allPoints.size());

				b2Filter filter;
				filter.categoryBits = 0x0200;
				filter.maskBits = 0x0001;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);

				m_pMap1->addChild(pSprite, 10, GSTag_LimitLine);
			}
				break;
			default:
				CCASSERT(false, "this object is'nt polyline type.");
				break;
			}
		}
		else
		{
			ValueMap & dict = obj.asValueMap();

			float x = dict["x"].asFloat();
			float y = dict["y"].asFloat();
			float width = dict["width"].asFloat();
			float height = dict["height"].asFloat();

			//类型判断
			switch (nGroupType)
			{
			case EOType_StepTile:
				{
					Box2dSprite * pSprite = Box2dSprite::create(m_pWorld);
					b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y + height / 2),
						b2_staticBody, width, height);

					b2Filter filter;
					filter.categoryBits = 0x0004;
					filter.maskBits = 0x0061;
					filter.groupIndex = -1;
					fixture->SetFilterData(filter);
					fixture->SetFriction(0.0f);

					m_pMap1->addChild(pSprite, 10, GSTag_StepTile);
				}
				break;
			case EOType_FlagPot:
			{
				Box2dSprite * pSprite = Box2dSprite::createWithSpriteFrameName("object/flagpole.png", m_pWorld);
				pSprite->setAnchorPoint(Vec2(0.5f, 0.0f));
				b2Vec2 vertices[4];
				vertices[0] = b2Vec2(-width / 2, 0);
				vertices[1] = b2Vec2(width / 2, 0);
				vertices[2] = b2Vec2(width / 2, height);
				vertices[3] = b2Vec2(-width / 2, height);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y),
					b2_staticBody, vertices, 4);

				b2Filter filter;
				filter.categoryBits = 0x0100;
				filter.maskBits = 0x0061;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);

				m_pMap1->addChild(pSprite, 10, GSTag_FlagPot);
				break;
			}
			case EOType_Flagpole:
			{
				Box2dSprite * pSprite = Box2dSprite::create(m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y + height / 2),
					b2_staticBody, width, height);

				b2Filter filter;
				filter.categoryBits = 0x0100;
				filter.maskBits = 0x0061;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetSensor(true);

				m_pMap1->addChild(pSprite, 10, GSTag_Flagpole);

				//添加旗帜
				m_pFlag = Sprite::createWithSpriteFrameName("flag/flag1.png");
				m_pFlag->setAnchorPoint(Vec2(1.0f, 1.0f));
				m_pFlag->setPosition(x + 2, y + height - 40.0f);
				m_pMap1->addChild(m_pFlag, 10);
				m_fFlagHeight = height - 40.0f - m_pFlag->getContentSize().height;	//旗帜有效高度
				Animation * animation = AnimationCache::getInstance()->getAnimation("winFlag");
				m_pFlag->runAction(Animate::create(animation));
				m_bSliding = false;

				break;
			}
			default:
				CCASSERT(false, "this object is'nt edge type.");
				return ;
			}
		}
	}
}

void TileMapLayer::addDynamicObjects(TMXObjectGroup * dynamicGroup)
{
	ValueVector & objects = dynamicGroup->getObjects();

	//判断对象组类型
	int nGroupType = -1;

	for (int i = 0; i < CountArray(m_szDynamicObjectName); ++i)
	{
		if (dynamicGroup->getGroupName() == m_szDynamicObjectName[i])
		{
			nGroupType = i;
			break;
		}
	}
	if (nGroupType == -1)
	{
		CCASSERT(false, "this group is not Dynamic!");
		return;
	}

	//遍历组内对象
	for (auto & obj : objects)
	{
		ValueMap & dict = obj.asValueMap();

		float x = dict["x"].asFloat();
		float y = dict["y"].asFloat();
		float width = dict["width"].asFloat();
		float height = dict["height"].asFloat();

		//类型判断
		switch (nGroupType)
		{
			case DOType_Coin:
			{
				Box2dSprite * pSprite = Box2dSprite::createWithSpriteFrameName("coin/coin1.png", m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreateCircleBody(pSprite, Vec2(x + width / 2, y + height / 2),
					b2_staticBody/*, Vec2::ZERO, width / 2*/);

				b2Filter filter;
				filter.categoryBits = 0x0008;
				filter.maskBits = 0x0001;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetSensor(true);

				m_pMap1->addChild(pSprite, 10, GSTag_Coin);

				Animation * pAnimation = AnimationCache::getInstance()->getAnimation("coin");
				pSprite->runAction(Animate::create(pAnimation));
				break;
			}
			case DOType_BigCoin:
			{
				Box2dSprite * pSprite = Box2dSprite::createWithSpriteFrameName("coin/big_coin1.png", m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreateCircleBody(pSprite, Vec2(x + width / 2, y + height / 2),
					b2_staticBody);

				b2Filter filter;
				filter.categoryBits = 0x0008;
				filter.maskBits = 0x0001;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetSensor(true);

				m_pMap1->addChild(pSprite, 10, GSTag_BigCoin);

				Animation * pAnimation = AnimationCache::getInstance()->getAnimation("bigCoin");
				Action * pAction = Animate::create(pAnimation);
				pSprite->runAction(pAction);
				break;
			}
			case DOType_GeneralTile:
			{
				TileSprite * pSprite = TileSprite::createWithSpriteFrameName("tile/general_tile1.png", m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y + height / 2),
					b2_staticBody, width, height);

				b2Filter filter;
				filter.categoryBits = 0x0004;
				filter.maskBits = 0x0061;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);
				pSprite->getBody()->SetFixedRotation(true);	//禁用旋转

				//添加上边缘框架
				b2Vec2 vertices[2];
				vertices[0].Set(-width / 2, height / 2);
				vertices[1].Set(width / 2, height / 2);
				fixture = m_pGameWorld->AddChainFixture(pSprite->getBody(), vertices, 2);
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.2f);

				setTileSpriteInfo(pSprite, dict);
				m_pMap1->addChild(pSprite, 10, GSTag_GeneralTile);

				Animation * pAnimation = AnimationCache::getInstance()->getAnimation("generalTile");
				pSprite->runAction(Animate::create(pAnimation));

				break;
			}
			case DOType_SpecialTile:
			{
				TileSprite * pSprite = TileSprite::createWithSpriteFrameName("tile/special_tile1.png", m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y + height / 2),
					b2_staticBody, width, height);

				b2Filter filter;
				filter.categoryBits = 0x0004;
				filter.maskBits = 0x0061;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);
				pSprite->getBody()->SetFixedRotation(true);	//禁用旋转

				//添加上边缘框架
				b2Vec2 vertices[2];
				vertices[0].Set(-width / 2, height / 2);
				vertices[1].Set(width / 2, height / 2);
				fixture = m_pGameWorld->AddChainFixture(pSprite->getBody(), vertices, 2);
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.2f);

				setTileSpriteInfo(pSprite, dict);
				m_pMap1->addChild(pSprite, 10, GSTag_SpecialTile);

				Animation * pAnimation = AnimationCache::getInstance()->getAnimation("specialTile");
				pSprite->runAction(Animate::create(pAnimation));

				break;
			}
			case DOType_TransparentTile:
			{
				TileSprite * pSprite = TileSprite::createWithSpriteFrameName("tile/static_tile.png", m_pWorld);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, Vec2(x + width / 2, y + height / 2),
				   b2_staticBody, width, height);

				b2Filter filter;
				filter.categoryBits = 0x0004;
				filter.maskBits = 0x0061;
				filter.groupIndex = -1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);
				pSprite->getBody()->SetFixedRotation(true);	//禁用旋转

				//添加上边缘框架
				b2Vec2 vertices[2];
				vertices[0].Set(-width / 2, height / 2);
				vertices[1].Set(width / 2, height / 2);
				fixture = m_pGameWorld->AddChainFixture(pSprite->getBody(), vertices, 2);
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.2f);

				setTileSpriteInfo(pSprite, dict);
				m_pMap1->addChild(pSprite, 10, GSTag_TransparentTile);
				pSprite->setVisible(false);

				break;
			}
			case DOType_MonsterRobbit:	//兔子
			{
				Vec2 center(x + width / 2, y + height / 2 + g_robbitSize.height / 2);	//中心位置

				b2Vec2 vertices[6];	//兔子形状
				commonMonsterShape(g_robbitSize, vertices, 6);

				MonsterRobbit * pSprite = MonsterRobbit::createWithSpriteFrameName("monster/robbit_left/robbit_left1.png", m_pWorld);
				pSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
				pSprite->setBeginPos(center);
				pSprite->setMonsterSize(g_robbitSize);
				pSprite->setMonsterStatus(MonsterStatus_Invalid);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, center, b2_dynamicBody, vertices, CountArray(vertices));

				b2Filter filter;
				filter.categoryBits = 0x0020;
				filter.maskBits = 0x0187;
				filter.groupIndex = 1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);
				pSprite->getBody()->SetFixedRotation(true);

				m_pMap1->addChild(pSprite, 10, GSTag_Robbit);
				m_pGameScene->addMonster(pSprite);

				break;
			}
			case DOType_MonsterTortoise:	//乌龟
			{
				Vec2 center(x + width / 2, y + height / 2 + g_tortoiseNormalSize.height / 2);	//中心位置

				b2Vec2 vertices[6];	//乌龟形状
				commonMonsterShape(g_tortoiseNormalSize, vertices, 6);

				MonsterTortoise * pSprite = MonsterTortoise::createWithSpriteFrameName("monster/tortoise_left/tortoise_left1.png", m_pWorld);
				pSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
				pSprite->setBeginPos(center);
				pSprite->setMonsterSize(g_tortoiseNormalSize);
				pSprite->setMonsterStatus(MonsterStatus_Invalid);
				b2Fixture * fixture = m_pGameWorld->CreatePolygonBody(pSprite, center, b2_dynamicBody, vertices, CountArray(vertices));

				b2Filter filter;
				filter.categoryBits = 0x0020;
				filter.maskBits = 0x0187;
				filter.groupIndex = 1;
				fixture->SetFilterData(filter);
				fixture->SetFriction(0.0f);
				pSprite->getBody()->SetFixedRotation(true);

				m_pMap1->addChild(pSprite, 10, GSTag_Tortoise);
				m_pGameScene->addMonster(pSprite);

				break;
			}
			default:
				CCASSERT(false, "this object is'nt dynamic type.");
				break;
		}

	}
}

void TileMapLayer::setTileSpriteInfo(TileSprite * pSprite, ValueMap & dict)
{
	if (!pSprite)
		return;

	//取得类型、金币数
	enTileSpriteType tileType = TSType_Null;
	int nCoinCount = 0;

	if (dict["type"].getType() == Value::Type::STRING)
	{
		std::string type = dict["type"].asString();

		switch (atoi(type.data()))
		{
		case 0:
			tileType = TSType_Null;
			break;
		case 1:
			tileType = TSType_Coin;
			break;
		case 2:
			tileType = TSType_Swamm;
			break;
		case 3:
			tileType = TSType_Invalid;
			break;
		default:
			CCASSERT(false, "no this tile type");
			break;
		}
	}

	if (dict["coin"].getType() == Value::Type::STRING)
	{
		std::string coin = dict["coin"].asString();
		nCoinCount = atoi(coin.data());
	}

	//设置土块类型
	pSprite->setTileType(tileType, nCoinCount);
}

void TileMapLayer::commonMonsterShape(const Size & size, b2Vec2 vertices[6], int nCount)
{
	//参数校验
	CCASSERT(vertices != nullptr, "shape vertices is null");
	CCASSERT(nCount == 6, "monster shape vertices count must be 6");

	float fWidth = size.width;
	float fHeight = size.height;
	vertices[0].Set(-fWidth / 2, -fHeight / 2 + 2);
	vertices[1].Set(-fWidth / 2 + 2, -fHeight / 2);
	vertices[2].Set(fWidth / 2 - 2, -fHeight / 2);
	vertices[3].Set(fWidth / 2, -fHeight / 2 + 2);
	vertices[4].Set(fWidth / 2, fHeight / 2);
	vertices[5].Set(-fWidth / 2, fHeight / 2);

	return;
}

void TileMapLayer::slideFlag()
{
	if (m_bSliding)
		return;

	Action * action = Sequence::create(
		MoveBy::create(2.0f, Vec2(0.0f, -m_fFlagHeight)),
		CallFunc::create(CC_CALLBACK_0(GameScene::flagHaveAtBottom, m_pGameScene)),
		nullptr
		);
	m_pFlag->runAction(action);
	m_bSliding = true;
}