#include "DataManager.h"
#include "Utils/Tea/TEACrypt.h"
#include <fstream>
#include <cmath>

#include "Common/Common.h"

const char* DATA_CHARACTER = "PET_BASE.d";
const char* DATA_SKILL = "SKILL.d";
const char* DATA_BUFF = "BUFF_INFO.d";
const char* DATA_MONSTER = "MONSTER.d";
const char* DATA_SKILL_EFFECT = "SKILL_EFFECT.d";
const char* DATA_CHAPTER = "CHAPTER.d";
const char* DATA_CHAPTER_POS = "CHAPTERSHOW.d";
const char* DATA_BARRIER = "BARRIER.d";
const char* DATA_TOOL = "ALLITEM.d";
const char* DATA_EQUIPMENT_TREF = "EQUIPMENT_TREF.d";
const char* DATA_HERO_ADVANCED = "HERO_ADVANCED.d";
const char* DATA_HERO_FATE = "HERO_FATE.d";

const char* DATA_EQUIPMEN_ADVANCED = "EQUIP_ADVANCED.d";
const char* DATA_EQUIPMENT = "EQUIPMENT.d";
const char* DATA_TOOL_SYSTEM = "SYSTEM.d";
const char* DATA_REFINE_FURNACE = "REFINEFURNACE.d";
const char* DATA_REFINE_FURNACE_SELL = "REFINEFURNACE_SELL.d";
const char* DATA_ARENA_STORE = "ARENA_STORE.d";
const char* DATA_EXPERIENCE = "EXPERIENCE.d";
const char* DATA_HORSEBOOKEXP = "HORSEBOOKEXP.d";
	
const char* DATA_TASKNEWSERVER = "TASKNEWSERVER.d";

const char* DATA_LOVE_TALK = "LOVE_TALK.d";
const char* DATA_LOVE_PLAY = "LOVE_PLAY.d";
const char* DATA_LOVE_LEVEL = "LOVE_LEVEL.d";
const char* DATA_LOVE_FINGER = "LOVE_FINGER.d";
const char* DATA_LOVE_HERO = "LOVE_HERO.d";
const char* DATA_LOVE_QUE = "LOVE_QUE.d";
const char* DATA_LOVE_TREASURE = "LOVE_TREASURE.d";
const char* DATA_LOVE_ATTRIBUTE = "LOVE_ATTRIBUTE.d";

const char* DATA_STAR_ADV = "STAR_ADV.d";
const char* DATA_STAR_ATTRI = "STAR_ATTRI.d";
const char* DATA_STAR_SKILL = "STAR_SKILL.d";

const char* DATA_MAZE_MAZE = "MAZ_STEP.d";
const char* DATA_MAZ_BAOTU = "MAZ_MAP.d";
const char* DATA_MAZ_BOX = "MAZ_BOX.d";
const char* DATA_MAZ_THING = "MAZ_THING.d";

const char* DATA_SHOP_VIP = "SHOPVIP.d";

const char* DATA_TRAINING = "TRAINING.d";
const char* DATA_HERO_EXP = "HERO_EXP.d";

const char* DATA_GUIDE = "NEWBIEGUIDE.d";
const char* DATA_GUIDE_CHAT = "CHAT.d";
const char* DATA_FUNCTION_LEVEL = "FUNCTION_LEVEL.d";

const char* DATA_SKILL_LEAD = "SKILL_LEAD.d";

const char* DATA_LEVELUP_GUIDE = "UPGRADE_GUIDE.d";

const char* DATA_TRAIL = "TRAIL.d";
const char* DATA_TRAIL_TREASURE = "TREASURE.d";

const char* DATA_TASK = "TASK.d";
const char* DATA_BEST_ACT = "BEST_ACT.d";

const char* DATA_BG_COLOR = "BG_COLORVALUE.d";

const char* DATA_SIGN = "Sign.d";

const char* DATA_STAR_BASE = "STAR_BASE.d";
const char* DATA_STAR = "STAR.d";
const char* DATA_ARENA_REWARD = "ARENA_REWARD.d";

const char* DATA_VIP = "VIP.d";

const char* DATA_RULES = "RULE.d";
const char* DATA_RULES_DES = "RULE_DES.d";

const char* DATA_TACTICS = "MATRIX.d";

const char* DATA_PAY = "RECHARGE.d";

const char* DATA_BOSS_REWARD = "BOSS_REWARD.d";

const char* DATA_SMELTER = "SMELTER.d";

const char* DATA_NEWAREA_ACTIVITY = "NEWAREA_ACTIVITY.d";

const char* DATA_LOADING_TIPS = "Tips.d";

const char* CARD_ACPOINT = "CARD_ACPOINT.d";

const char* DATA_STAR_UP = "HERO_STAR.d";
const char* DATA_ACTIVITY_SHOW = "ACTIVITY_SHOW.d";

const char* DATA_ACTIVITY = "ACTIVITY.d";

const char* DATA_ACTIVITY_SORT = "ACTIVITY_SORT.d";

const char* DATA_EMBLEM = "Emblem.d";

const char* DATA_BUILDING = "Building_Level.d";

const char* DATA_DONATE = "Donate.d";

const char* DATA_EVENT = "Description.d";

const char* DATA_STAR_STONE = "STAR_STONE.d";

const char* DATA_STONE_BUFF = "STONE_BUFF.d";

const char* DATA_LINEUP = "STAR_HERO.d";

const char* DATA_ARTIFACT = "ARTIFACT.d";

const char* DATA_QUENCH = "EQUIP_QUENCH.d";

const char* DATA_EQUIPMENT_EXP = "EQUIP_EXP.d";

const char* DATA_SHOP = "SHOP.d";

const char* DATA_GROUP_ACTIVITY = "GROUP_ACTIVITY.d";

const char* DATA_GROUP_ACTSHOW = "GROUP_ACTSHOW.d";

const char* DATA_OPEN_FUNC = "OPEN_FUNCTION.d";

const char* DATA_GROUP_BOSS = "GROUP_BOSS.d";

const char* DATA_SHOP_CARD_VIEW = "SHOP_PREVIEW.d";
const char* DATA_PVP_ALL_SHOW = "SHOW.d";

const char* DATA_HERO_RELATION = "HERO_RELATION.d";

const char* DATA_COMMON_TEXT = "COMMON_TEXT.d";

DataManager* DataManager::mInstance = nullptr;

//tea密钥
uint32_t DataManager::mKey[] = {0x01, 0x02, 0x03, 0x04};


DataManager::DataManager()
{
	mTea.setKey(mKey);
	initData();
}

DataManager::~DataManager()
{

}

DataManager* DataManager::getInstance()
{
	if(!mInstance)
	{
		mInstance = new DataManager();
	}
	return mInstance;
}

void DataManager::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void DataManager::initData()
{
	bindTable(mCharacterCache, DATA_CHARACTER);
	bindTable(mSkillCache, DATA_SKILL);
	bindTable(mBuffCache, DATA_BUFF);
	bindTable(mMonsterCache, DATA_MONSTER);
	bindTable(mSkillEffectCache, DATA_SKILL_EFFECT);
	bindTable(mChapterCache, DATA_CHAPTER);
	bindTable(mBarrierCache, DATA_BARRIER);
	bindTable(mToolCache, DATA_TOOL);
	bindTable(mEquipmentRefCache, DATA_EQUIPMENT_TREF);
	bindTable(mEquipmentAdvancedCache, DATA_EQUIPMEN_ADVANCED);
	bindTable(mEquipmentCache, DATA_EQUIPMENT);
	bindTable(mHeroAdvanceCache, DATA_HERO_ADVANCED);
	bindTable(mHeroFateCache, DATA_HERO_FATE);
	bindTable(mToolSystemCache, DATA_TOOL_SYSTEM);
	bindTable(mRefineFurnaceCache, DATA_REFINE_FURNACE);
	bindTable(mRefineFurnaceSellCache, DATA_REFINE_FURNACE_SELL);
	bindTable(mArenaStoreCache, DATA_ARENA_STORE);
	bindTable(mExperienceCache, DATA_EXPERIENCE);
	bindTable(mHorseBookExpCache, DATA_HORSEBOOKEXP);
	bindTable(mTaskNewServerCache, DATA_TASKNEWSERVER);
	bindTable(mLoveTalkCache, DATA_LOVE_TALK);
	bindTable(mLovePlayCache, DATA_LOVE_PLAY);
	bindTable(mLoveLevelCache, DATA_LOVE_LEVEL);
	bindTable(mLoveFingerCache, DATA_LOVE_FINGER);
	bindTable(mLoveHeroCache, DATA_LOVE_HERO);
	bindTable(mLoveQueCache, DATA_LOVE_QUE);
	bindTable(mLoveTreasure,DATA_LOVE_TREASURE);
	bindTable(mLoveAttribute,DATA_LOVE_ATTRIBUTE);
	bindTable(mStarAdvCache, DATA_STAR_ADV);
	bindTable(mStarAttriCache, DATA_STAR_ATTRI);
	bindTable(mStarSkillCache, DATA_STAR_SKILL);
	bindTable(mMazeBaseCache, DATA_MAZE_MAZE);
	bindTable(mMazeBaotuCache, DATA_MAZ_BAOTU);
	bindTable(mMazeBoxCache,DATA_MAZ_BOX);
	bindTable(mMazeThingCache, DATA_MAZ_THING);
	bindTable(mShopVipCache, DATA_SHOP_VIP);
	bindTable(mTrainingCache, DATA_TRAINING);
	bindTable(mGuideCache, DATA_GUIDE);
	bindTable(mGuideChatCache, DATA_GUIDE_CHAT);
	bindTable(mHeroExpCache, DATA_HERO_EXP);
	bindTable(mFunctionLevelCache, DATA_FUNCTION_LEVEL);
	bindTable(mTalentSkillCache, DATA_SKILL_LEAD);
	bindTable(mTableLevelUpGuide, DATA_LEVELUP_GUIDE);
	bindTable(mTableTrail, DATA_TRAIL);
	bindTable(mTableTrailTreasure, DATA_TRAIL_TREASURE);
	bindTable(mTableTask, DATA_TASK);
	bindTable(mTableBestAct, DATA_BEST_ACT);
	bindTable(mTableBgColor, DATA_BG_COLOR);
	bindTable(mTableSign,DATA_SIGN);
	bindTable(mTableChapterPos, DATA_CHAPTER_POS);
	bindTable(mTableStarBase,DATA_STAR_BASE);
	bindTable(mTableStar,DATA_STAR);
	bindTable(mTableArenaReword,DATA_ARENA_REWARD);
	bindTable(mTableVip, DATA_VIP);
	bindTable(mTableRulesId, DATA_RULES);
	bindTable(mTableRulesDes, DATA_RULES_DES);
	bindTable(mTableTactics, DATA_TACTICS);
	bindTable(mTablePay, DATA_PAY);
	bindTable(mTableBossReward, DATA_BOSS_REWARD);
	bindTable(mTableSmelter,DATA_SMELTER);
	bindTable(mTableNewAreaActivity, DATA_NEWAREA_ACTIVITY);
	bindTable(mTableLoadingTips, DATA_LOADING_TIPS);
	bindTable(mTableCardAcPoint,CARD_ACPOINT);
	bindTable(mHeroStarUpCache, DATA_STAR_UP);
	bindTable(mTableActivityShow, DATA_ACTIVITY_SHOW);
	bindTable(mTableOperateActivity,DATA_ACTIVITY);
	bindTable(mTableOperateActivitySort,DATA_ACTIVITY_SORT);
	bindTable(mTableEmblem,DATA_EMBLEM);
	bindTable(mTableBuilding, DATA_BUILDING);
	bindTable(mTableDonate, DATA_DONATE);
	bindTable(mTableEvent, DATA_EVENT);
	bindTable(mTableGroupActivity,DATA_GROUP_ACTIVITY);
	bindTable(mTableGroupActShow,DATA_GROUP_ACTSHOW);
	bindTable(mTableAttributeIntruduce,DATA_STAR_STONE);
	bindTable(mTableHeroStone,DATA_STONE_BUFF);
	bindTable(mTableStarsCampLinup,DATA_LINEUP);
	bindTable(mTableArtifact, DATA_ARTIFACT);
	bindTable(mTableQuench, DATA_QUENCH);
	bindTable(mTableEquipmentExp, DATA_EQUIPMENT_EXP);
	bindTable(mTableShop,DATA_SHOP);
	bindTable(mTableOpenFunc, DATA_OPEN_FUNC);
	bindTable(mTableGroupBoss, DATA_GROUP_BOSS);
	bindTable(mTableShopCardView,DATA_SHOP_CARD_VIEW);
	bindTable(nTablePvpAllShow,DATA_PVP_ALL_SHOW);
	bindTable(mTableHeroRelation, DATA_HERO_RELATION);
	bindTable(mTableCommonText,DATA_COMMON_TEXT);
}

void DataManager::bindTable(DataTable& dataTable, const char* filename)
{
	dataTable.init(filename, &mTea);
}

RowData* DataManager::searchCharacterById(int id)
{
	return mCharacterCache.searchDataById(id);
}

DataTable* DataManager::getDTCharacter()
{
	return &mCharacterCache;
}

RowData* DataManager::searchStarUpById(int heroTempletId, int curStars)
{
	int id = heroTempletId * 100 + curStars;
	return mHeroStarUpCache.searchDataById(id);
}

RowData* DataManager::searchStarUpById(int heroId)
{
	return mHeroStarUpCache.searchDataById(heroId);
}

RowData* DataManager::searchSkillById(int id)
{
	return mSkillCache.searchDataById(id);
}

//查询buff数据
RowData* DataManager::searchBuffById(int id)
{
	return mBuffCache.searchDataById(id);
}

//查询MONSTER数据
RowData* DataManager::searchMonsterById(int id)
{
	return mMonsterCache.searchDataById(id);
}

//查询MONSTER数据
RowData* DataManager::searchSkillEffectById(int id)
{
	return mSkillEffectCache.searchDataById(id);
}

RowData* DataManager::searchChapterById(int id)
{
	return mChapterCache.searchDataById(id);
}

//查询道具表
RowData* DataManager::searchToolById(int id)
{
	return mToolCache.searchDataById(id);
}

//查询洗练属性装备表
RowData* DataManager::searchEquipmentReflById(int id)
{
	return mEquipmentRefCache.searchDataById(id);
}


//查询装备数据表
RowData* DataManager::searchEquipmentById(int id)
{
	return mEquipmentCache.searchDataById(id);
}

//查询进阶数据表
RowData* DataManager::searchEquipmentAdvancedById(int id)
{
	return mEquipmentAdvancedCache.searchDataById(id);
}

//查询道具系统数据表
RowData* DataManager::searchToolSystemById(int id)
{
	return mToolSystemCache.searchDataById(id);
}

//查询炼化炉神秘商人数据表
RowData* DataManager::searchRefineFurnaceById(int id)
{
	return mRefineFurnaceCache.searchDataById(id);
}
//查询炼化炉炼化配置数据
RowData* DataManager::searchRefineFurnaceSellById(int id)
{
	return mRefineFurnaceSellCache.searchDataById(id);
}

RowData* DataManager::searchHeroAdvanceById(int heroTempletId, int advLvl)
{
	int id = heroTempletId * 100 + advLvl;
	return mHeroAdvanceCache.searchDataById(id);
}

RowData* DataManager::searchHeroAdvanceById(int AdvanceById)
{
	return mHeroAdvanceCache.searchDataById(AdvanceById);
}

RowData* DataManager::searchBarrierById(int id)
{
	return mBarrierCache.searchDataById(id);
}

RowData* DataManager::searchHeroFateById(int id)
{
	return mHeroFateCache.searchDataById(id);
}

// 查询阅历数据
RowData* DataManager::searchExperienceById(int id)
{
	return mExperienceCache.searchDataById(id);
}

// 查询马书经验数据
RowData* DataManager::searchHorseBookExpById(int id)
{
	return mHorseBookExpCache.searchDataById(id);
}

// 查询任务新服奖励数据
RowData* DataManager::searchTaskNewServerById(int id)
{
	return mTaskNewServerCache.searchDataById(id);
}

// 查询竞技场积分商城表数据
RowData* DataManager::searchArenaStoreById(int id)
{
	return mArenaStoreCache.searchDataById(id);
}

//查询情缘对白表数据
RowData* DataManager::searchLoveTalkById(int id)
{
	return mLoveTalkCache.searchDataById(id);
}
//查询情缘游玩表数据
RowData* DataManager::searchLovePlayById(int id)
{
	return mLovePlayCache.searchDataById(id);
}
//查询情缘道具掉落表数据
RowData* DataManager::searchLoveLevelById(int id)
{
	return mLoveLevelCache.searchDataById(id);
}
//
RowData* DataManager::searchLoveFingerById(int id)
{
	return mLoveFingerCache.searchDataById(id);
}
RowData* DataManager::searchLoveHeroById(int id)
{
	return mLoveHeroCache.searchDataById(id);
}

RowData* DataManager::searchLoveQueById(int id)
{
	return mLoveQueCache.searchDataById(id);
}

RowData* DataManager::searchLoveTreasureByid(int id)
{
	return mLoveTreasure.searchDataById(id);
}

RowData* DataManager::searchLoveAtrributeById(int id)
{
	return mLoveAttribute.searchDataById(id);
}

RowData* DataManager::searchStarSpriteAttriById(int id)
{
	return mStarAttriCache.searchDataById(id);
}

//查询星灵技能表
RowData* DataManager::searchStarSpriteSkillById(int id)
{
	return mStarSkillCache.searchDataById(id);
}

//查询迷宫基础表数据
RowData* DataManager::searchMazeBaseById(int id)
{
	return mMazeBaseCache.searchDataById(id);
}

//查询迷宫宝图表数据
RowData* DataManager::searchMazeBaotuById(int id)
{
	return mMazeBaotuCache.searchDataById(id);
}

//查询迷宫事件表数据
RowData* DataManager::searchMazeThingById(int id)
{
	return mMazeThingCache.searchDataById(id);
}

//查询迷宫Box数据表数据
RowData* DataManager::searchMazeBoxById(int id)
{
	return mMazeBoxCache.searchDataById(id);
}

//查询vip商城表数据
RowData* DataManager::searchShopVipById(int id)
{
	return mShopVipCache.searchDataById(id);

}

// 查询训练位解锁配置数据;
RowData* DataManager::searchTrainingById( int id )
{
	return mTrainingCache.searchDataById(id);
}

// 查询英雄等级配置表数据;
RowData* DataManager::searchHeroExp(int id)
{
	return mHeroExpCache.searchDataById(id);
}

//查询新手引导表
RowData* DataManager::searchGuideById(int id)
{
	return mGuideCache.searchDataById(id);
}

//获取新手引导表
DataTable* DataManager::getDTGuide()
{
	return &mGuideCache;
}

//获取VIP数据表
DataTable* DataManager::getDTVip()
{
	return &mTableVip;
}

//获取阵法数据表
DataTable* DataManager::getDTTactics()
{
	return &mTableTactics;
}

//查询对话表
RowData* DataManager::searchGuideChatById(int id)
{
	return mGuideChatCache.searchDataById(id);
}

RowData* DataManager::searchFunctionLevelById(int id)
{
	return mFunctionLevelCache.searchDataById(id);
}

RowData* DataManager::searchTalentSkillById(int id)
{
	return mTalentSkillCache.searchDataById(id);
}

RowData* DataManager::searchLevelUpGuide( int id )
{
	return mTableLevelUpGuide.searchDataById(id);
}

RowData* DataManager::searchTrailById( int id )
{
	return mTableTrail.searchDataById(id);
}

RowData* DataManager::searchTrailTreasureById( int id )
{
	return mTableTrailTreasure.searchDataById(id);
}

RowData* DataManager::searchTaskById(int id)
{
	return mTableTask.searchDataById(id);
}

RowData* DataManager::searchBestActById(int id)
{
	return mTableBestAct.searchDataById(id);
}

RowData* DataManager::searchFightBg(int id)
{
	return mTableBgColor.searchDataById(id);
}

RowData* DataManager::searchChapterPosById(int id)
{
	return mTableChapterPos.searchDataById(id);
}

RowData* DataManager::searchStarBaseById(int id)
{
	return mTableStarBase.searchDataById(id);
}

RowData* DataManager::searchStarById(int id)
{
	return mTableStar.searchDataById(id);
}

RowData* DataManager::searchArenaRewordById(int id)
{
	return mTableArenaReword.searchDataById(id);
}

RowData* DataManager::searchVipDescriptionById(int id)
{
	return mTableVip.searchDataById(id);
}

//查询玩法规则标签id和玩法描述id
RowData* DataManager::searchRulesLabelById(int id)
{
	return mTableRulesId.searchDataById(id);
}

//查询玩法规则标签描述和玩法描述表
RowData* DataManager::searchRulesTextById(int id)
{
	return mTableRulesDes.searchDataById(id);
}

//查询阵法数据
RowData* DataManager::searchTacticsById(int id)
{
	return mTableTactics.searchDataById(id);
}

DataTable* DataManager::getDTPay()
{
	return &mTablePay;
}

RowData* DataManager::searchPayById( int id )
{
	return mTablePay.searchDataById(id);
}

//查询boss奖励数据
RowData* DataManager::searchBossRewardById(int id)
{
	return mTableBossReward.searchDataById(id);
}

//获得开服活动整表数据
DataTable* DataManager::getDTNewAreaActivity()
{
	return &mTableNewAreaActivity;
}

//根据ID查询开服活动表
RowData* DataManager::searchNewAreaActivityById(int id)
{
	return mTableNewAreaActivity.searchDataById(id);
}

//查询loading Tips
RowData* DataManager::searchLoadingTipsById(int id)
{
	return mTableLoadingTips.searchDataById(id);
}

//查询loading Tips
RowData* DataManager::searchCardAcPointById(int id)
{
	return mTableCardAcPoint.searchDataById(id);
}

RowData* DataManager::searchActivityShowById( int id )
{
	return mTableActivityShow.searchDataById(id);
}

//获取运营活动整表数据
DataTable* DataManager::getDTOperateActivity()
{
	return &mTableOperateActivity;
}

//查询获取运营活动某个数据
RowData* DataManager::searchOperateActivityById(int id)
{
	return mTableOperateActivity.searchDataById(id);
}

DataTable* DataManager::getDTOperateActivitySort()
{
	return &mTableOperateActivitySort;
}


//帮派相关数据表
DataTable* DataManager::getTableEmblem()
{
	return &mTableEmblem;
}

RowData* DataManager::searchEmblemById(int id)
{
	return mTableEmblem.searchDataById(id);
}

DataTable* DataManager::getTableBuilding()
{
	return &mTableBuilding;
}

RowData* DataManager::searchBuildingById(int id)
{
	return mTableBuilding.searchDataById(id);
}

DataTable* DataManager::getTableDonate()
{
	return &mTableDonate;
}

RowData* DataManager::searchDonateById(int id)
{
	return mTableDonate.searchDataById(id);
}

DataTable* DataManager::getTableEvent()
{
	return &mTableEvent;
}

RowData* DataManager::searchEventById(int id)
{
	return mTableEvent.searchDataById(id);
}

DataTable* DataManager::getGroupActivityInfo()
{
	return &mTableGroupActivity;
}

RowData* DataManager::searchGroupActivityshowInfoById( int id )
{
	return mTableGroupActShow.searchDataById(id);
}

DataTable* DataManager::getGroupActivityShowInfo()
{
	return &mTableGroupActShow;
}

RowData* DataManager::searchGroupActivityInfoById( int id )
{
	return mTableGroupActivity.searchDataById(id);
}

//查找英雄石头
DataTable* DataManager:: getTableHeroStone()
{
		return &mTableHeroStone;
}

RowData* DataManager:: searchHeroStoneById(int id)
{
	return mTableHeroStone.searchDataById(id);
}

//查找属性介绍
DataTable* DataManager:: getTableAttbitureIntroduce()
{
		return &mTableAttributeIntruduce;
}
RowData* DataManager:: searchAttbitureIntroduceById(int id)
{
	return mTableAttributeIntruduce.searchDataById(id);
}

//查找属性介绍
DataTable* DataManager:: getTableStarCampLinup()
{
	return &mTableStarsCampLinup;
}
RowData* DataManager:: searchStarCampLinupById(int id)
{
	return mTableStarsCampLinup.searchDataById(id);
}

DataTable* DataManager::getDTArtifact()
{
	return &mTableArtifact;
}

RowData* DataManager::searchArtifactAttriById( int id )
{
	return mTableArtifact.searchDataById(id);
}

RowData* DataManager::searchEquipmentQuenchById( int id )
{
	return mTableQuench.searchDataById(id);
}

RowData* DataManager::searchEquipmentExp( int id )
{
	return mTableEquipmentExp.searchDataById(id);
}

DataTable* DataManager::getDTShop()
{
	return &mTableShop;
}

RowData* DataManager::searchShopById( int id )
{
	return mTableShop.searchDataById(id);
}

DataTable* DataManager::getDTOpenFunction()
{
	return &mTableOpenFunc;
}

RowData* DataManager::searchOpenFunctionById( int id )
{
	return mTableOpenFunc.searchDataById(id);
}

DataTable* DataManager::getDTGroupBoss()
{
	return &mTableGroupBoss;
}

RowData* DataManager::searchGroupBossById( int id )
{
	return mTableGroupBoss.searchDataById(id);
}

DataTable* DataManager::getDTShopCardView()
{
	return &mTableShopCardView;
}

DataTable* DataManager::getTablePvpAllShow()
{
	return &nTablePvpAllShow;
}

DataTable* DataManager::getDTHeroRelation()
{
	return &mTableHeroRelation;
}

RowData* DataManager::searchHeroRelationById( int id )
{
	return mTableHeroRelation.searchDataById(id);
}

DataTable* DataManager::getDTCommonTexdt()
{
	return &mTableCommonText;
}

string DataManager::searchCommonTexdtById( int id )
{
	RowData* _row = mTableCommonText.searchDataById(id);
	return (_row == nullptr) ? "" : _row->getStringData("context");
}