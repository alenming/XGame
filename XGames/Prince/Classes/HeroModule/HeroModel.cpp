#include "HeroModel.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "../ToolModule/ToolModel.h"
#include "Communication/Command.h"
#include "MainModule/MainModel.h"
#include "SmithyModule/SmithyModel.h"
#include "LevelUp/LevelUpController.h"
#include "StarsSuModule/StarsSuModel.h"
#include "SDK_TalkingData/TalkingData.h"
#include "TalentModule/TalentModel.h"
#include "Utils/SystemUtils.h"
#include "LoginModule/LoginController.h"

using namespace std;

HeroModel* HeroModel::mInstance = nullptr;

HeroModel::HeroModel()
{
	mRage = 0;
	mIsInit = false;
	clearBaseAttriParam();
	clearAdvAttriParam();
}

HeroModel::~HeroModel()
{
	mVecHero.clear();
}

HeroModel* HeroModel::getInstance()
{
	if (!mInstance)
	{
		mInstance = new HeroModel();
	}
	return mInstance;
}

void HeroModel::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

//该函数只有初始化时才能调用
void HeroModel::initData(const Json::Value& data)
{
	cleaHero();

	for (size_t i = 0; i < data.size(); i++)
	{
		HeroParam* heroParam = HeroParam::create();
		heroParam->initFromServerData(data[i]);
		mVecHero.pushBack(heroParam);

		//TalkingData等级数据上报，记录登录时的主角等级到天赋数据模块中
		if (heroParam->isMainHero())
		{
			TalkingData::setLevel(heroParam->mLevel);
			TalentModel::getInstance()->setRecordMainHeroLv(heroParam->mLevel);
		}
	}
	notifyObservers();
	mIsInit = true;
}

void HeroModel::updateData(const Json::Value& data)
{
	for (size_t i = 0; i < data.size(); i++)
	{
		int heroId = data[i]["id"].asInt();
		HeroParam* heroParam = nullptr;

		if (!data[i]["stat"].isNull() && data[i]["stat"] == HERO_DELETE_FLAG)
		{
			//删除数据
			heroParam = searchHeroById(heroId);
			if (heroParam)
			{
				mVecHero.eraseObject(heroParam);
			}
		}
		else
		{
			heroParam = searchHeroById(heroId);
			if (heroParam)
			{
				//更新数据
				heroParam->initFromServerData(data[i]);
			}
			else
			{				
				//新增数据
				heroParam = HeroParam::create();
				heroParam->initFromServerData(data[i]);
				mVecHero.pushBack(heroParam);
			}
		}
	}
}

void HeroModel::cleaHero()
{
	mVecHero.clear();
}

int HeroModel::getHeroCount()
{
	return mVecHero.size();
}

HeroParam* HeroModel::searchHeroById( int heroId )
{
	for (auto heroParam : mVecHero)
	{
		if (heroParam->mHeroId == heroId)
		{
			return heroParam;
		}
	}
	return nullptr;
}

//查询英雄信息
HeroParam* HeroModel::searchHeroByTemplateId(int templateId)
{
	for (auto heroParam : mVecHero)
	{
		if (heroParam->mTemplateId == templateId)
		{
			return heroParam;
		}
	}
	return nullptr;
}

Vector<HeroParam*>* HeroModel::getHeroList()
{
	std::sort(mVecHero.begin(), mVecHero.end(), [](HeroParam* p1, HeroParam* p2)->bool
	{
		if (p1->isMainHero() && !p2->isMainHero())
		{
			return true;
		}
		else if(!p1->isMainHero() && p2->isMainHero())
		{
			return false;
		}
		else
		{
			//在阵位上的排在前面
			if (p1->isOnBattle() && !p2->isOnBattle())
			{
				return true;
			}
			else if (!p1->isOnBattle() && p2->isOnBattle())
			{
				return false;
			}
			else
			{
				//都在阵位上或者都不在阵位上，按照战斗力排序
				return p1->mFighting > p2->mFighting;
			}
		}
	});

	return &mVecHero;
}

HeroParam* HeroModel::getMainHero()
{
	for (auto iter = mVecHero.begin(); iter != mVecHero.end(); iter++)
	{
		if ((*iter)->isMainHero())
		{
			return (*iter);
		}
	}
	return nullptr;
}

int HeroModel::getMainHeroLevel()
{
	HeroParam* mainHero = getMainHero();
	if (mainHero)
	{
		return mainHero->mLevel;
	}
	return -1;
}

int HeroModel::getHeroCountByTempletId(int templateId)
{
	int count = 0;
	for (size_t i = 0; i < mVecHero.size(); i++)
	{
		HeroParam* heroParam = mVecHero.at(i);
		if (heroParam->mTemplateId == templateId)// && heroParam->mBattleFlag == NO_BATTLE
		{
			count++;
		}
	}
	return count;
}

vector<HeroParam*> HeroModel::getOnBattleHero()
{
	vector<HeroParam*> vecHero;
	for (size_t i = 0; i < mVecHero.size(); i++)
	{
		HeroParam* heroParam = mVecHero.at(i);
		if (heroParam->isOnBattle())
		{
			vecHero.push_back(heroParam);
		}
	}
	return vecHero;
}

vector<HeroParam*> HeroModel::getUnBattleHero()
{
	vector<HeroParam*> vecHero;
	for (size_t i = 0; i < mVecHero.size(); i++)
	{
		HeroParam* heroParam = mVecHero.at(i);
		if (!heroParam->isOnBattle())
		{
			vecHero.push_back(heroParam);
		}
	}
	return vecHero;
}


bool HeroModel::isHeroAdvValid(int heroId)
{
	HeroParam* pHeroParam = searchHeroById(heroId);
	if (!pHeroParam)
	{
		return false;
	}
	return pHeroParam->isHeroAdvValid();
	
}

bool HeroModel::isHeroStarUpValid(int heroId)
{
	HeroParam* pHeroParam = searchHeroById(heroId);
	if (!pHeroParam || pHeroParam->isMainHero())
	{
		return false;
	}
	return pHeroParam->isHeroStarUpValid();
}

bool HeroModel::isHeroEquipAdvValid(int heroId)
{
	HeroParam* pHeroParam = searchHeroById(heroId);
	if (!pHeroParam)
	{
		return false;
	}
	return pHeroParam->isHeroEquipAdvValid();
}

bool HeroModel::isHeroEquipPutOn(int heroId)
{
	HeroParam* pHeroParam = searchHeroById(heroId);
	if (!pHeroParam)
	{
		return false;
	}
	return pHeroParam->isHeroEquipPutOn();
}

//基础属性变化参数，用于角色数字界面显示动画
void HeroModel::clearBaseAttriParam()
{
	memset(&m_oldBaseAttriParam, 0, sizeof(HERO_BASE_ATTRI_PARAM));
}

void HeroModel::setBaseAttriParam(const HERO_BASE_ATTRI_PARAM& changeParam)
{
	m_oldBaseAttriParam = changeParam;
}

HERO_BASE_ATTRI_PARAM& HeroModel::getBaseAttriParam()
{
	return m_oldBaseAttriParam;
}

//进阶基础属性变化参数，用于角色数字界面显示动画
void HeroModel::clearAdvAttriParam()
{
	memset(&m_oldAdvAttriParam, 0, sizeof(HERO_BASE_ATTRI_PARAM));
}

void HeroModel::setAdvAttriParam(const HERO_BASE_ATTRI_PARAM& changeParam)
{
	m_oldAdvAttriParam = changeParam;
}

HERO_BASE_ATTRI_PARAM& HeroModel::getAdvAttriParam()
{
	return m_oldAdvAttriParam;
}

void HeroModel::setHp( int nHeroId, int nHp )
{
	HeroParam* pHeroParam = searchHeroById(nHeroId);
	assert(pHeroParam != nullptr);
	pHeroParam->mHpCur = nHp;
}

float HeroModel::getHpPercent( int nHeroId )
{
	HeroParam* pHeroParam = searchHeroById(nHeroId);
	assert(pHeroParam != nullptr);
	float fPercent = ((float)pHeroParam->mHpCur) / ((float)pHeroParam->mHp);
	return fPercent;
}

bool HeroModel::isBreak(int nHeroId)
{
	HeroParam* pHeroParam = searchHeroById(nHeroId);
	RowData* advInfo = DataManager::getInstance()->searchHeroAdvanceById(pHeroParam->mTemplateId, pHeroParam->mAdvLvl);
	if(advInfo)
	{
		if(advInfo->getIntData("value") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else               
	{
		return true;
	}
}

int HeroModel::getStarCount(int advLvl)
{
	return (advLvl - 1) / 10 + 1;
}

int HeroModel::getTotalFighting()
{
	int fighting = 0;
	for (HeroParam* heroParam : mVecHero)
	{
		if (heroParam->isOnBattle())
		{
			fighting += heroParam->mFighting;
		}
	}
	return fighting;
}

bool HeroModel::hasRecruitableHero()
{
	const TOOL_INFO_MAP& mapToolInfo = ToolModel::getInstance()->GetAllToolInfo();
	for (auto iter = mapToolInfo.begin(); iter != mapToolInfo.end(); iter++)
	{
		const TOOL_INFO& toolInfo = iter->second;
		if (toolInfo.nLocat == TOOL_TYPE_HERO_CHIP)
		{
			RowData* spData = DataManager::getInstance()->searchToolById(toolInfo.ntemplateId);
			int heroTempletId = spData->getIntData("transItem");
			int needNum = spData->getIntData("amount");
			//没有英雄，且碎片数量大于等于招募数量，判定有可招募英雄
			if (!searchHeroByTemplateId(heroTempletId) && toolInfo.nstack >= needNum)
			{
				return true;
			}
		}
	}
	return false;
}

int HeroModel::getAlmightyChipsNum()
{
	return ToolModel::getInstance()->searchNumByTemplateId(ID_ALMIGHTY_CHIPS);
}
/*
void HeroModel::updateDataForStarCamp( const Json::Value& data )
{
	//一种类别的英雄数据全更新
	for (int i = 0; i < data.size(); i++)
	{

	}
}
*/
//////////////////////////昏割线//////////////////////////////////////////

HeroParam* HeroParam::create()
{
	HeroParam* heroParam = new HeroParam();
	heroParam->autorelease();
	return heroParam;
}

void HeroParam::initFromServerData(const Json::Value& data)
{
	mRoleType = data["type"].asInt();

	int mainHeroStars = 0;
	if(isMainHero())
	{
		//先记录下主角的星级，主角有可能升星
		mainHeroStars = mHeroStars;
	}

	mHeroId = data["id"].asInt();
	mTemplateId = data["templateId"].asInt();
	mAdvLvl = data["advLvl"].asInt();	
	mHeroStars = data["stars"].asInt();
	mSpeed = data["speed"].asInt();

	//技能数据
	mNormalSkillId = data["atkID"].asInt();
	mActiveSkillId = data["a_sklID"].asInt();
	mRushSkillId = data["p1_sklID"].asInt();
	mBuffSkillId = data["p2_sklID"].asInt();
	mTriggerSkillId = data["p3_sklID"].asInt();

	mName = data["name"].asString();
	mBattleFlag = data["battleflag"].asInt();	

	int newLevel = data["level"].asInt();

	// 插入主角升级检测;
	if (isMainHero() && newLevel > mLevel && mLevel != 0)
	{
		//TalkingData等级改变数据上报
		TalkingData::setLevel(newLevel);
		LevelUpController::getInstance()->confirmLevelUp(newLevel);

		//升级有可能运营活动可以领奖
		HeroModel::getInstance()->notifyObservers();

		//角色升级数据上报
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if (SystemUtils::getChannelId() == CHANNEL_ID_U9)
		{
			LoginController::getInstance()->submitExtraData(U8ExtraGameData::TYPE_LEVEL_UP);
		}
#endif
	}
	mLevel = newLevel;

	mExp = data["exp"].asInt();
	mExpMax = data["maxExp"].asInt();

	if(isMainHero() && mainHeroStars < mHeroStars)
	{
		//主角升星时，记录基础属性升星前的数值
		HERO_BASE_ATTRI_PARAM attriParam;
		attriParam.mAtk = mAtk;
		attriParam.mHp = mHp;
		attriParam.mPdd = mPDef;
		attriParam.mMdd = mMDef;
		attriParam.mFightPower = mFighting;
		HeroModel::getInstance()->setBaseAttriParam(attriParam);
	}

	mAtk = data["atk"].asInt();
	mPDef = data["pdd"].asInt();
	mMDef = data["mdd"].asInt();
	mHp = data["hp"].asInt();

	mHeaderId = data["head"].asInt();
	mBodyId = data["body"].asInt();
	mWeaponId = data["weapon"].asInt();
	mEarringId = data["earring"].asInt();
	mHorseId = data["warhorse"].asInt();
	mBookId = data["warbook"].asInt();
	
	mCritP = data["criPrb"].asDouble() * 100;
	mCritCoe = data["criCoe"].asDouble() * 100;
	mComboP = data["ctrPrb"].asDouble() * 100;
	mMissP = data["miss"].asDouble() * 100;
	mCriDamRed = data["coeRes"].asDouble() * 100;
	mCriResist = data["criRes"].asDouble() * 100;
	mHitPrb = data["hit"].asDouble() * 100;
	mFighting = data["battlePower"].asInt();

	if (isMainHero())
	{
		mVecActivedFate.clear();
		Json::Value fateList = data["fateList"];
		for (size_t i = 0; i < fateList.size(); i++)
		{
			int fateId = fateList[i].asInt();
			mVecActivedFate.push_back(fateId);
		}	
	}
	
	initFromLocalData();

	//资质
	RowData* heroData = DataManager::getInstance()->searchCharacterById(mTemplateId);
	if(heroData)
	{
		mTalent = heroData->getIntData("apt");
	}

	if(isMainHero() && mainHeroStars < mHeroStars)
	{
		//通知星宿，主角升星
		HeroModel::getInstance()->notifyObservers();
	}
}

void HeroParam::initFromLocalData()
{
	//初始化英雄基本数据
	RowData* heroData = DataManager::getInstance()->searchCharacterById(mTemplateId);
	if (heroData)
	{
		mHeroType = heroData->getIntData("tagTyp");
		mAtkTyp =  heroData->getIntData("atkTyp");
		mJobTyp = heroData->getIntData("jobType");
		mSex = heroData->getIntData("sexTyp");
	}

	//初始化英雄进阶、升星属性数据
	RowData* starsData = DataManager::getInstance()->searchStarUpById(mTemplateId, mHeroStars);
	RowData* advanceData = DataManager::getInstance()->searchHeroAdvanceById(mTemplateId, mAdvLvl);
	if(starsData)
	{
		//成长值数据
		mAtkGrow = starsData->getIntData("groAtk");
		mHpGrow = starsData->getIntData("groHp");
		mPddGrow = starsData->getIntData("groPdd");
		mMddGrow = starsData->getIntData("groMdd");
	}
	if (advanceData)
	{		
		//英雄品质
		mQuality = advanceData->getIntData("qua");

		//消耗材料
		auto matFunc = [advanceData, this](const string& matField, const string& amountField)
		{
			int matId = advanceData->getIntData(matField);
			if (matId > 0)
			{
				AdvMatParam advMatParam;
				advMatParam.itemId = matId;
				RowData* toolData = DataManager::getInstance()->searchToolById(matId);
				advMatParam.resId = toolData->getIntData("resId");
				advMatParam.needNum = advanceData->getIntData(amountField);
				mVecAdvMat.push_back(advMatParam);
			}
		};
		mVecAdvMat.clear();
		matFunc("mat1Id", "amount1");
		matFunc("mat2Id", "amount2");
		matFunc("mat3Id", "amount3");

		//消耗银币
		mAdvCoin = advanceData->getIntData("costCoin");

		//进阶所需等级
		mAdvNeedLevel = advanceData->getIntData("level");
	}
	
	RowData* itemData = DataManager::getInstance()->searchToolById(mTemplateId);
	if (itemData)
	{
		mResId = itemData->getIntData("resId");
	}

	//升星需要的碎片
	if(!isMainHero())
	{
		RowData* starUpMatData = DataManager::getInstance()->searchStarUpById(mTemplateId, mHeroStars);
		if(starUpMatData)
		{
			int matId = starUpMatData->getIntData("matId");
			mStarUpMat.itemId = matId;
			if(matId != -1)
			{
				RowData* toolData = DataManager::getInstance()->searchToolById(matId);
				mStarUpMat.resId = toolData->getIntData("resId");
				mStarUpMat.needNum = starUpMatData->getIntData("amount");
			}
			else
			{
				mStarUpMat.resId = -1;
				mStarUpMat.needNum = 0;
			}	
		}
	}

	//初始化英雄缘分数据
	if (mRoleType == HERO_TYPE::MAIN_HERO)
	{

	}
}

bool HeroParam::isFateActived(int fateId)
{
	for (size_t i = 0; i < mVecActivedFate.size(); i++)
	{
		if (mVecActivedFate[i] == fateId)
		{
			return true;
		}
	}
	return false;
}

bool HeroParam::isMainHero()
{
	return mRoleType == HERO_TYPE::MAIN_HERO;
}

//角色是否可以进阶
bool HeroParam::isHeroAdvValid()
{
	if (mVecAdvMat.empty())
	{
		return false;
	}

	//判定等级，银币是否足够
	int curCoin = MainModel::getInstance()->getMainParam()->mCoin;
	int curLvl = mLevel;
	if (curCoin < mAdvCoin || curLvl < mAdvNeedLevel)
	{
		return false;
	}

	//判定材料是否足够
	for (AdvMatParam& advParam : mVecAdvMat)
	{
		const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(advParam.itemId);
		if (!toolInfo || toolInfo->nstack < advParam.needNum)
		{
			return false;
		}
	}
	return true;
}

//角色是否可以升星
bool HeroParam::isHeroStarUpValid()
{
	int curNum = ToolModel::getInstance()->searchNumByTemplateId(mStarUpMat.itemId);
	if(curNum >= mStarUpMat.needNum && mHeroStars < MAX_STARS && !isMainHero())
	{
		return true;
	}
	return false;
}

//角色是否有装备可以进阶
bool HeroParam::isHeroEquipAdvValid()
{
	if (mWeaponId > 0 && 
		SmithyModel::getInstance()->checkEquState(mWeaponId))
	{
		return true;
	}
	if (mBodyId > 0 && 
		SmithyModel::getInstance()->checkEquState(mBodyId))
	{
		return true;
	}
	if (mEarringId > 0 && 
		SmithyModel::getInstance()->checkEquState(mEarringId))
	{
		return true;
	}
	if (mHeaderId > 0 && 
		SmithyModel::getInstance()->checkEquState(mHeaderId))
	{
		return true;
	}
	if (mHorseId > 0 && 
		SmithyModel::getInstance()->checkEquState(mHorseId))
	{
		return true;
	}
	if (mBookId > 0 && 
		SmithyModel::getInstance()->checkEquState(mBookId))
	{
		return true;
	}

	return false;
}

bool HeroParam::isHeroEquipPutOn()
{
	if (mWeaponId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_WEAPON) > 0)
	{
		return true;
	}
	if (mBodyId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_CLOTHES) > 0)
	{
		return true;
	}
	if (mEarringId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_EARRINGS) > 0)
	{
		return true;
	}
	if (mHeaderId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_HAT) > 0)
	{
		return true;
	}
	if (mHorseId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_HORSE) > 0)
	{
		return true;
	}
	if (mBookId < 0 && 
		ToolModel::getInstance()->searchPutOnEquip(EQUIP_ITEM_TYPE_BOOK) > 0)
	{
		return true;
	}

	return false;
}

bool HeroParam::isOnBattle()
{
	return mBattleFlag != NO_BATTLE;
}

//获取角色名称，主角显示为“主角”
string HeroParam::getName()
{
	if (isMainHero())
	{
		//return STR_UTF8("主角");
		return DataManager::getInstance()->searchCommonTexdtById(10064);
	}
	else
	{
		return mName;
	}
}

//获取角色带觉醒等级的名称
string HeroParam::getAdvName()
{
	//查觉醒表
	RowData* advInfo = DataManager::getInstance()->searchHeroAdvanceById(mTemplateId, mAdvLvl);
	if(advInfo)
	{
		return advInfo->getStringData("name");
	}
	return "";
}

//获取角色真实名称，主角显示为主角的名字
string HeroParam::getRealName()
{
	return mName;
}

