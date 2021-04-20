#ifndef DataManager_h__
#define DataManager_h__

#include "cocos2d.h"
#include "DataTable.h"
#include "Utils/Tea/TEACrypt.h"
#include <string>
#include <vector>
#include <map>

USING_NS_CC;
using namespace std;

class DataManager
{

public:

	DataManager();
	~DataManager();

public:

	//singleton
	static DataManager* getInstance();
	static void destroyInstance();

	//查询角色数据
	RowData* searchCharacterById(int id);

	//获取角色数据表
	DataTable* getDTCharacter();

	//查询角色星级数据
	RowData* searchStarUpById(int id, int curStars);
	
	RowData* searchStarUpById(int id);
	//查询技能数据
	RowData* searchSkillById(int id);

	//查询buff数据
	RowData* searchBuffById(int id);

	//查询MONSTER数据
	RowData* searchMonsterById(int id);

	//查询技能特效表
	RowData* searchSkillEffectById(int id);

	//查询关卡表
	RowData* searchChapterById(int id);

	//查询道具表
	RowData* searchToolById(int id);

	//查询装备表
	RowData* searchEquipmentReflById(int id);

	//查询道具系统数据表
	RowData* searchToolSystemById(int id);

	//查询炼化炉神秘商人数据表
	RowData* searchRefineFurnaceById(int id);

	//查询炼化炉炼化配置数据;
	RowData* searchRefineFurnaceSellById(int id);

	//查询关卡id
	RowData* searchBarrierById(int id);

	// 获取章节表的整表数据;
	DataTable* getDTChapter() { return &mChapterCache; };

	// 获取关卡表的整表数据;
	DataTable* getDTBarrier() { return &mBarrierCache; };

	// 获取道具表的整表数据;
	DataTable* getDTTool() { return &mToolCache; };

	// 获取装备洗练属性数据表的整表数据;
	DataTable* getDTEquipmentRef() { return &mEquipmentRefCache; };

	// 获取道具系统数据表的整表数据;
	DataTable* getDTToolSystem() { return &mToolSystemCache; };

	// 获取炼化炉数据表的整表数据;
	DataTable* getDTRefineFurnace() { return &mRefineFurnaceCache; };
	//获取炼化炉炼化配置表整表数据;
	DataTable* getDTRefineFurnaceSell() { return &mRefineFurnaceSellCache; };
	//阅历配置表
	DataTable* getDTExperience() { return &mExperienceCache; };
	//马书经验表
	DataTable* getDTHorseBookExp() { return &mHorseBookExpCache; };
	// 任务新服奖励数据
	DataTable* getDTTaskNewServer() { return &mTaskNewServerCache; };
	// 竞技场积分商城表数据
	DataTable* getDTArenaStore() { return &mArenaStoreCache; };

	//情缘对白表
	DataTable* getDTLoveTalk() { return &mLoveTalkCache; };
	//情缘游玩表
	DataTable* getDTLovePlay() { return &mLovePlayCache; };
	//情缘道具掉落表
	DataTable* getDTLoveLevel() { return &mLoveLevelCache; };
	//情缘道具掉落表
	DataTable* getDTLoveFinger() { return &mLoveFingerCache; };
	//情缘邮件表
	DataTable* getDTLoveHero() { return &mLoveHeroCache; };
	//情缘问答表
	DataTable* getDTLoveQue() { return &mLoveQueCache; };
	//情缘寻宝表
	DataTable* getDYLoveTreasure() {return &mLoveTreasure;};
	//情缘属性
	DataTable* getDTLoveAtrribute() {return &mLoveAttribute;};
	//查询迷宫基础表数据
	DataTable* getDTMazeBase() { return &mMazeBaseCache; };
	//查询迷宫宝图表数据
	DataTable* getDTMazeBaotu() { return &mMazeBaotuCache; };
	//查询迷宫事件表数据
	DataTable* getDTMazeThing() { return &mMazeThingCache; };
	//查询迷宫Box数据
	DataTable* getDTMazeBox() {return &mMazeBoxCache; };
	//查询vip商城表数据
	DataTable* getDTShopVip(){ return &mShopVipCache; };
	// 查询训练位解锁配置表;
	DataTable* getDTTraining() { return &mTrainingCache; };

	// 查询功能开放等级表整表数据;
	DataTable* getDTFunctionLevel() { return &mFunctionLevelCache; };

	// 查询等级经验配置表;
	DataTable* getDTHeroExp() { return &mHeroExpCache; };

	// 查询升级引导配置表;
	DataTable* getDTLevelUpGuide() { return &mTableLevelUpGuide; };

	// 查询章节坐标数据表;
	DataTable* getDTChapterPos() { return &mTableChapterPos; };

	// 查询英雄进阶数据
	RowData* searchHeroAdvanceById(int heroTempletId, int advLvl);

	// 查询英雄进阶数据
	RowData* searchHeroAdvanceById(int AdvanceById);

	// 查询英雄缘分数据
	RowData* searchHeroFateById(int id);
	DataTable* getDTHeroFate(){ return &mHeroFateCache; };
	// 查询阅历数据
	RowData* searchExperienceById(int id);
	// 查询马书经验数据
	RowData* searchHorseBookExpById(int id);
	// 查询任务新服奖励数据
	RowData* searchTaskNewServerById(int id);

	// 查询竞技场积分商城表数据
	RowData* searchArenaStoreById(int id);

	//查询情缘对白表数据
	RowData* searchLoveTalkById(int id);
	//查询情缘游玩表数据
	RowData* searchLovePlayById(int id);
	//查询情缘道具掉落表数据
	RowData* searchLoveLevelById(int id);
	//查询情缘喝酒数据
	RowData* searchLoveFingerById(int id);
	//查询情缘邮件数据
	RowData* searchLoveHeroById(int id);
	//查询情缘问答数据
	RowData* searchLoveQueById(int id);
	//查询情缘寻宝数据
	RowData* searchLoveTreasureByid(int id);
	//查询情缘属性数据
	RowData* searchLoveAtrributeById(int id);
	//查询星灵属性
	RowData* searchStarSpriteAttriById(int id);
	//查询星灵技能表
	RowData* searchStarSpriteSkillById(int id);
	
	//查询迷宫基础表数据
	RowData* searchMazeBaseById(int id);
	//查询迷宫宝图表数据
	RowData* searchMazeBaotuById(int id);
	//查询迷宫事件表数据
	RowData* searchMazeThingById(int id);
	//查询迷宫Box数据表数据
	RowData* searchMazeBoxById(int id);

	//查询vip商城表数据
	RowData* searchShopVipById(int id);


	// 查询训练位解锁配置数据;
	RowData* searchTrainingById(int id);

	// 查询等级经验配置表;
	RowData* searchHeroExp(int id);

	//查询新手引导表
	RowData* searchGuideById(int id);
	//获取新手引导表
	DataTable* getDTGuide();
	//查询对话表
	RowData* searchGuideChatById(int id);

	//查询等级开放表
	RowData* searchFunctionLevelById(int id);

	//查询等级表
	RowData* searchTalentSkillById(int id);

	// 查询升级引导配置表;
	RowData* searchLevelUpGuide(int id);

	DataTable* getDTTrail() { return &mTableTrail; };

	// 查询试炼配置表;
	RowData* searchTrailById(int id);

	// 查询试炼宝藏配置表;
	RowData* searchTrailTreasureById(int id);

	//查询任务表
	RowData* searchTaskById(int id);

	//查询精彩活动
	RowData* searchBestActById(int id);

	//获取任务表
	DataTable* getDTTask(){ return &mTableTask; };

	//获取精彩活动
	DataTable* getDTBestAct() {return &mTableBestAct; };

	//查询战斗背景表
	RowData* searchFightBg(int id);

	// 查询章节坐标数据表;
	RowData* searchChapterPosById(int id);

	//活动签到奖励表
	DataTable* getDTSign(){ return &mTableSign; };

	//活动星宿base表
	DataTable* getDTStarBase(){ return &mTableStarBase; };

	//星宿占星表
	DataTable* getDTStar(){ return &mTableStar; };

	//竞技数据包
	DataTable* getArenaReword() {return &mTableArenaReword; };

	//获得炼化炉数据包
	DataTable* getSmelter() {return &mTableSmelter;};

	// 查询星宿base表数据表;
	RowData* searchStarBaseById(int id);

	// 查询占星数据表;
	RowData* searchStarById(int id);

	// 查询竞技数据表;
	RowData* searchArenaRewordById(int id);

	//获取VIP整表数据
	DataTable* getDTVip();

	//查询VIP特权描述;
	RowData* searchVipDescriptionById(int id);
	
	//查询玩法规则标签id和规则描述id
	RowData* searchRulesLabelById(int id);

	//查询标签文本和玩法规则描述文本
	RowData* searchRulesTextById(int id);

	//获取阵法整表数据
	DataTable* getDTTactics();

	//查询阵法数据表
	RowData* searchTacticsById(int id);

	// 获取支付表整表数据;
	DataTable* getDTPay();

	// 查询支付表;
	RowData* searchPayById(int id);

	//查询boss奖励数据表
	RowData* searchBossRewardById(int id);

	//获取开服狂欢整表数据
	DataTable* getDTNewAreaActivity();

	//根据ID查询开服狂欢数据
	RowData* searchNewAreaActivityById(int id);

	//查询Loading提示语
	RowData* searchLoadingTipsById(int id);

	//查询卡牌锚点数据
	RowData* searchCardAcPointById(int id);

	// 查询活动关卡卡牌配置;
	RowData* searchActivityShowById(int id);

	//获取运营活动整表数据
	DataTable* getDTOperateActivity();

	//查询获取运营活动某个数据
	RowData* searchOperateActivityById(int id);

	//运营活动配置排序整表数据
	DataTable* getDTOperateActivitySort();

	//帮派旗帜数据
	DataTable* getTableEmblem();
	RowData* searchEmblemById(int id);

	//查找帮派建筑数据
	DataTable* getTableBuilding();
	RowData* searchBuildingById(int id);

	//查找帮派捐献数据
	DataTable* getTableDonate();
	RowData* searchDonateById(int id);

	//查找帮派事件
	DataTable* getTableEvent();
	RowData* searchEventById(int id);

	//查找帮派活动
	DataTable* getGroupActivityInfo();
	RowData* searchGroupActivityInfoById(int id);

	//帮派活动展示物品数据
	DataTable* getGroupActivityShowInfo();
	RowData* searchGroupActivityshowInfoById( int id );

	//查找英雄石头
	DataTable* getTableHeroStone();
	RowData* searchHeroStoneById(int id);

	//查找属性介绍
	DataTable* getTableAttbitureIntroduce();
	RowData* searchAttbitureIntroduceById(int id);

	//查找阵形
	DataTable* getTableStarCampLinup();
	RowData* searchStarCampLinupById(int id);

	//神器表格
	DataTable* getDTArtifact();
	RowData* searchArtifactAttriById(int id);

	// 获取装备数据表数据;
	DataTable* getDTEquipment() { return &mEquipmentCache; };
	RowData* searchEquipmentById(int id);

	// 获取装备进阶数据表数据;
	DataTable* getDTEquipmentAdvanced() { return &mEquipmentAdvancedCache; };
	RowData* searchEquipmentAdvancedById(int id);

	// 获取淬炼数据表数据;
	RowData* searchEquipmentQuenchById(int id);

	// 获取装备经验表数据;
	RowData* searchEquipmentExp(int id);

	//获取商店整表数据
	DataTable* getDTShop();
	RowData* searchShopById( int id );

	//主界面功能开放提示表
	DataTable* getDTOpenFunction();
	RowData* searchOpenFunctionById(int id);

	//帮派boss表
	DataTable* getDTGroupBoss();
	RowData* searchGroupBossById(int id);

	//抽卡预览
	DataTable* getDTShopCardView();

	//pvpshow
	DataTable* getTablePvpAllShow();

	//英雄关系表
	DataTable* getDTHeroRelation();
	RowData* searchHeroRelationById(int id);

	//中文提示
	DataTable* getDTCommonTexdt();
	//获取提示文本内容
	string searchCommonTexdtById(int id);

private:

	//初始化数据
	void initData();

	//绑定数据表
	void bindTable(DataTable& dataTable, const char* filepath);
	
private:

	static DataManager* mInstance;

	//Tea加解密密钥
	static uint32_t mKey[];

	//Tea解密算法类
	TEACrypt mTea;

	//角色表缓存数据
	DataTable mCharacterCache;
	//角色升星表
	DataTable mHeroStarUpCache;

	//技能表
	DataTable mSkillCache;

	//技能特效表
	DataTable mSkillEffectCache;

	//BUFF表
	DataTable mBuffCache;

	//MONSTER表
	DataTable mMonsterCache;


	//章节数据表;
	DataTable mChapterCache;

	//关卡数据表;
	DataTable mBarrierCache;

	//道具数据表;
	DataTable mToolCache;

	//装备洗练属性数据表;
	DataTable mEquipmentRefCache;

	//装备数据表;
	DataTable mEquipmentCache;
	//装备进阶数据表;
	DataTable mEquipmentAdvancedCache;
	//道具系统表
	DataTable mToolSystemCache;
	//英雄进阶表
	DataTable mHeroAdvanceCache;

	//英雄缘分表
	DataTable mHeroFateCache;

	//炼化炉商店配置表
	DataTable mRefineFurnaceCache;
	//炼化炉炼化配置表
	DataTable mRefineFurnaceSellCache;
	//阅历配置表
	DataTable mExperienceCache;
	//马书经验表
	DataTable mHorseBookExpCache;
	//新服任务表
	DataTable mTaskNewServerCache;
	//竞技场积分商城表
	DataTable mArenaStoreCache;

	//情缘对白表
	DataTable mLoveTalkCache;
	//情缘游玩表
	DataTable mLovePlayCache;
	//情缘道具掉落表
	DataTable mLoveLevelCache;
	//情缘喝酒表
	DataTable mLoveFingerCache;
	//情缘邮件
	DataTable mLoveHeroCache;
	//情缘问答
	DataTable mLoveQueCache;
	//情缘寻宝
	DataTable mLoveTreasure;
	//情缘属性
	DataTable mLoveAttribute;
	//星灵升级表
	DataTable mStarAdvCache;
	//星灵属性表
	DataTable mStarAttriCache;
	//星灵技能表
	DataTable mStarSkillCache;
	//迷宫基础表数据
	DataTable mMazeBaseCache;
	//迷宫宝图表数据
	DataTable mMazeBaotuCache;
	//迷宫事件
	DataTable mMazeThingCache;
	//迷宫Box数据
	DataTable mMazeBoxCache;

	//vip商城表数据
	DataTable mShopVipCache;

	// 训练位解锁配置表;
	DataTable mTrainingCache;

	// 等级配置表;
	DataTable mHeroExpCache;

	//新手引导表
	DataTable mGuideCache;
	//引导对话以及剧情对话表
	DataTable mGuideChatCache;

	//等级开放表
	DataTable mFunctionLevelCache;

	//主角技能表
	DataTable mTalentSkillCache;

	// 升级引导表;
	DataTable mTableLevelUpGuide;

	// 试炼配置表;
	DataTable mTableTrail;

	// 试炼宝藏配置表;
	DataTable mTableTrailTreasure;

	//任务配置表
	DataTable mTableTask;

	//精彩活动配置表
	DataTable mTableBestAct;

	//战斗背景表
	DataTable mTableBgColor;

	// 章节坐标数据表;
	DataTable mTableChapterPos;

	//签到奖励表
	DataTable mTableSign;

	// 章节坐标数据表;
	DataTable mTableStarBase;

	// 章节坐标数据表;
	DataTable mTableStar;

	// 竞技数据表;
	DataTable mTableArenaReword;

	//Vip数据表
	DataTable mTableVip;

	//玩法规则ID表
	DataTable mTableRulesId;
	//玩法规则文本表
	DataTable mTableRulesDes;

	//阵法数据表
	DataTable mTableTactics;

	// 支付数据表;
	DataTable mTablePay;

	//boss奖励表
	DataTable mTableBossReward;

	//炼化炉表
	DataTable mTableSmelter;

	//开服活动表
	DataTable mTableNewAreaActivity;

	//loading提示语数据表
	DataTable mTableLoadingTips;

	//打卡片锚点配置
	DataTable mTableCardAcPoint;

	// 活动关卡卡牌配置;
	DataTable mTableActivityShow;

	//运营活动配置
	DataTable mTableOperateActivity;

	//运营活动配置排序
	DataTable mTableOperateActivitySort;

	//帮派旗帜
	DataTable mTableEmblem;

	//帮派建筑
	DataTable mTableBuilding;

	//帮派捐献
	DataTable mTableDonate;

	//帮派事件表
	DataTable mTableEvent;

	//帮派活动数据
	DataTable mTableGroupActivity;

	//帮派活动展示数据
	DataTable mTableGroupActShow;

	//英雄石头表
	DataTable mTableHeroStone;

	//属性介绍表
	DataTable mTableAttributeIntruduce;

	//星盘阵形表
	DataTable mTableStarsCampLinup;

	//神器表格
	DataTable mTableArtifact;

	// 淬炼表;
	DataTable mTableQuench;

	// 装备经验表;
	DataTable mTableEquipmentExp;

	//商店表格
	DataTable mTableShop;

	//主界面功能开放提示表
	DataTable mTableOpenFunc;

	//帮派boss表
	DataTable mTableGroupBoss;

	//抽卡预览
	DataTable mTableShopCardView;

	//pvpShow
	DataTable nTablePvpAllShow;

	//英雄关系表
	DataTable mTableHeroRelation;

	//文本提示字符
	DataTable mTableCommonText;
};



#endif