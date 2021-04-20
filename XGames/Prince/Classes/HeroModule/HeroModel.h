#ifndef HeroModel_h__
#define HeroModel_h__

#include <map>
#include <vector>
#include <string>
#include "base/CCVector.h"
#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "../ToolModule/ToolDef.h"

USING_NS_CC;
using namespace std;

//万能碎片的ID
#define  ID_ALMIGHTY_CHIPS		10051

//角色类型定义
enum HERO_TYPE
{
	NORMAL_HERO = 0,
	MAIN_HERO
};

enum BATTLE_FLAG_TYPE
{
	NO_BATTLE = 0,
	ON_BATTLE_POS_1,
	ON_BATTLE_POS_2,
	ON_BATTLE_POS_3,
	ON_BATTLE_POS_4,
	ON_BATTLE_POS_5,
	ON_BATTLE_POS_6,
	ON_BATTLE_POS_7,
	ON_BATTLE_POS_8,
	ON_BATTLE_POS_9,
	ON_EX
};

enum HERO_QUALITY_TYPE
{
	HERO_QUALITY_WHITE = 1,
	HERO_QUALITY_GREEN,
	HERO_QUALITY_BLUE,
	HERO_QUALITY_PURPLE,
	HERO_QUALITY_ORANGE,
};

enum HERO_PROPERTY_TYPE
{
	HERO_STR = 1,	//力量
	HERO_INT,		//智力
	HERO_AGI,		//敏捷
	HERO_NONE		//无属性
};

enum HERO_JOB_TYPE
{
	HERO_DEFENSE = 1,		//防御性
	HERO_ATK,				//攻击性
	HERO_HELP,				//辅助性
	HERO_FIRST				//先手性
};

enum HERO_SEX_TYPE
{
	HERO_SEX_MALE = 0, //男性
	HERO_SEX_FEMALE	   //女性
};

enum HERO_ATK_TYPE
{
	HERO_ATK_PHY = 1,	//物理攻击
	HERO_ATK_MAG		//法术攻击
};

const int HERO_MALE_TEMPLET_ID = 41099;
const int HERO_FEMALE_TEMPLET_ID = 41098;

const int HERO_MALE_RES_ID = 41099;
const int HERO_FEMALE_RES_ID = 41098;

const string HERO_UPDATE_FLAG = "U";
const string HERO_ADD_FLAG = "I";
const string HERO_DELETE_FLAG = "D";


//英雄基础属性参数
typedef struct HeroBaseAttriParam
{
	//进阶hp
	float mHp;
	//进阶攻击
	float mAtk;
	//进阶物防
	float mPdd;
	//进阶魔防
	float mMdd;
	//战斗力
	int mFightPower;
}HERO_BASE_ATTRI_PARAM;

class HeroParam;

class HeroModel : public Observable
{

public:

	HeroModel();
	virtual ~HeroModel();

	static HeroModel* getInstance();
	static void destroyInstance();

public:

	//英雄数据初始化
	void initData(const Json::Value& data);

	//英雄数据更新
	void updateData(const Json::Value& data);
	/*
	//英雄数据更新(购买镶嵌星石是使用)
	void updateDataForStarCamp(const Json::Value& data);
	*/
	//查询英雄信息
	HeroParam* searchHeroById(int heroId);

	//查询英雄信息
	HeroParam* searchHeroByTemplateId(int templateId);

	//获取英雄列表
	Vector<HeroParam*>* getHeroList();

	//获取主角信息
	HeroParam* getMainHero();

	//获取主角等级
	int getMainHeroLevel();

	//获取英雄数量
	int	getHeroCount();

	//清空英雄列表
	void cleaHero();

	//获取指定英雄的数量
	int getHeroCountByTempletId(int templateId);


	//获取已经上阵的英雄
	vector<HeroParam*> getOnBattleHero();
	//获取未上阵的英雄
	vector<HeroParam*> getUnBattleHero();

	//判断英雄数据是否已经初始化完毕
	bool isInit() { return mIsInit; };

	//角色是否可以进阶
	bool isHeroAdvValid(int heroId);

	//角色是否可以升星
	bool isHeroStarUpValid(int heroId);

	//角色是否有装备可以进阶
	bool isHeroEquipAdvValid(int heroId);

	//角色是否有装备可穿戴
	bool isHeroEquipPutOn(int heroId);

	//基础属性变化参数，用于角色数字界面显示动画
	void clearBaseAttriParam();
	void setBaseAttriParam(const HERO_BASE_ATTRI_PARAM& changeParam);
	HERO_BASE_ATTRI_PARAM& getBaseAttriParam();

	//进阶基础属性变化参数，用于角色数字界面显示动画
	void clearAdvAttriParam();
	void setAdvAttriParam(const HERO_BASE_ATTRI_PARAM& changeParam);
	HERO_BASE_ATTRI_PARAM& getAdvAttriParam();

	// 存取英雄血量(只获取百分比);
	void  setHp(int nHeroId, int nHp);
	float getHpPercent(int nHeroId);

	// 存取公共怒气;
	void  setRage(int nRage) { mRage = nRage; };
	int   getRage()  { return mRage; };

	//判断某一进阶等级是否是突破
	bool isBreak(int advLvl);

	//获取某一进阶等级的星星数量
	int getStarCount(int advLvl);

	//获取总体战斗力
	int getTotalFighting();

	//是否有可招募的英雄
	bool hasRecruitableHero();

	//获取万能碎片的数量
	int getAlmightyChipsNum();

private:

	static HeroModel* mInstance;

	//英雄数据
	Vector<HeroParam*> mVecHero;

	// 公共怒气值(0-100);
	int mRage;

	//是否已经初始化
	bool mIsInit;

	//垃圾
	HERO_BASE_ATTRI_PARAM m_oldBaseAttriParam;
	HERO_BASE_ATTRI_PARAM m_oldAdvAttriParam;
};

struct AdvMatParam
{
	//道具id
	int itemId;
	//资源id
	int resId;
	//道具所需数量
	int needNum;
};

struct AdvancePreviewData
{
	AdvancePreviewData()
		: isInit(false)
		, mAdvLvl(0)
		, mHp(0)
		, mAtk(0)
		, mPdd(0)
		, mMdd(0)
		, mNextHp(0)
		, mNextAtk(0)
		, mNextPdd(0)
		, mNextMdd(0)
	{}

	//是否初始化
	bool isInit;
	//进阶等级
	int mAdvLvl;
	//进阶hp
	float mHp;
	//进阶攻击
	float mAtk;
	//进阶物防
	float mPdd;
	//进阶魔防
	float mMdd;

	//进阶hp
	float mNextHp;
	//进阶攻击
	float mNextAtk;
	//进阶物防
	float mNextPdd;
	//进阶魔防
	float mNextMdd;
};

//英雄数据
class HeroParam : public Ref
{

public:

	HeroParam()
		: mHeroId(0)
		, mTemplateId(0)
		, mResId(-1)
		, mBattleFlag(0)
		, mLevel(0)
		, mQuality(0)
		, mHeroStars(0)
		, mExp(0)
		, mExpMax(0)
		, mRoleType(0)
		, mSex(0)
		, mAtk(0)
		, mPDef(0)
		, mMDef(0)
		, mHp(0)
		, mHpCur(0)
		, mAdvLvl(0)
		, mNormalSkillId(-1)
		, mActiveSkillId(-1)
		, mRushSkillId(-1)
		, mBuffSkillId(-1)
		, mTriggerSkillId(-1)
		, mWeaponId(-1)
		, mHeaderId(-1)
		, mBodyId(-1)
		, mEarringId(-1)
		, mBookId(-1)
		, mHorseId(-1)
		, mCritP(0)
		, mCritCoe(0)
		, mCriResist(0)
		, mComboP(0)
		, mCriDamRed(0)
		, mMissP(0)
		, mAtkGrow(0)
		, mHpGrow(0)
		, mPddGrow(0)
		, mMddGrow(0)
		, mFighting(0)
		, mAdvNeedLevel(0)
		, mTalent(0)
		, mJobTyp(1)
	{};
	virtual ~HeroParam(){};

	static HeroParam* create();

public:

	//服务端数据初始化
	void initFromServerData(const Json::Value& data);

	//本地数据初始化
	void initFromLocalData();

	//查询缘分是否被激活
	bool isFateActived(int fateId);

	//是否是主角
	bool isMainHero();

	//角色是否可以进阶
	bool isHeroAdvValid();

	//角色是否可以升星
	bool isHeroStarUpValid();

	//角色是否有装备可以进阶
	bool isHeroEquipAdvValid();

	//角色是否有装备可以穿戴
	bool isHeroEquipPutOn();

	//角色是否在阵位上
	bool isOnBattle();

	//获取角色名称，主角显示为“主角”
	string getName();

	//获取角色带觉醒等级的名称
	string getAdvName();

	//获取角色真实名称，主角显示为主角的名字
	string getRealName();

public:

	//英雄ID，服务端ID
	int mHeroId;
	//模版id，用于查本地数据表获取英雄数据
	int mTemplateId;
	//英雄类型，力、智、敏
	int mHeroType;
	//资源id
	int mResId;
	//是否上阵
	int mBattleFlag;
	//英雄等级
	int mLevel;
	//英雄品质
	int mQuality;
	//英雄星级
	int mHeroStars;
	//英雄当前经验
	int mExp;
	//英雄最大经验
	int mExpMax;
	//角色是主角还是英雄
	int mRoleType;
	//角色性别
	int mSex;
	//英雄名称
	string mName;
	// 攻击类型
	int mAtkTyp;
	//英雄职业 1.防御  2.攻击  3.辅助
	int mJobTyp;

	// 攻击
	int mAtk;
	//物理防御
	int mPDef;
	//法术防御
	int mMDef;
	//血量cc
	int mHp;
	//速度
	int mSpeed;

	// 当前血量;
	int mHpCur;

	//英雄进阶等级
	int mAdvLvl;

	//英雄进阶所需等级
	int mAdvNeedLevel;

	//普通技能
	int mNormalSkillId;
	//主动技能
	int mActiveSkillId;
	//被动1技能
	int mRushSkillId;
	//被动2技能
	int mBuffSkillId;
	//被动3技能
	int mTriggerSkillId;

	//武器id
	int mWeaponId;
	//头盔id
	int mHeaderId;
	//盔甲id
	int mBodyId;
	//首饰id
	int mEarringId;
	//战马id
	int mHorseId;
	//兵书id
	int mBookId;

	//暴击概率
	int mCritP;
	//抗暴率
	int mCriResist;
	//暴击伤害系数
	int mCritCoe;
	//暴击伤害减免系数
	int mCriDamRed;
	//连击概率
	int mComboP;
	//闪避概率
	int mMissP;
	//命中率
	int mHitPrb;

	//攻击力成长值
	int mAtkGrow;
	//生命成长值
	int mHpGrow;
	//物理防御成长值
	int mPddGrow;
	//魔法防御成长值
	int mMddGrow;

	//战斗力
	int mFighting;

	//英雄资质
	int mTalent;

	//已激活的缘分列表
	vector<int> mVecActivedFate;

	//进阶消耗银币
	int mAdvCoin;
	//进阶消耗材料
	vector<AdvMatParam> mVecAdvMat;
	//升星消耗材料
	AdvMatParam mStarUpMat;

	//进阶预览信息
	AdvancePreviewData mAdvancePreviewData;

};


#endif // HeroModel_h__
