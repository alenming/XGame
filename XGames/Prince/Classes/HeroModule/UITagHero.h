#ifndef UITagHero_h__
#define UITagHero_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/UICardHead.h"
#include "CCArmature.h"

USING_NS_CC;
using namespace ui;
using namespace cocostudio;

//const int SKILL_ITEM_COUNT = 5;
const int MAIN_HERO_FATE_COUNT = 5;
const int HERO_FATE_COUNT = 6;
const int STAR_COUNT = 6;
const int EXP_ITEM_COUNT = 4;

enum class HERO_BTN_ID
{
	BACK = 0,
	TAB_ALL_HERO,
	TAB_ATK_HERO,
	TAB_DEF_HERO,
	TAB_HELP_HERO,
	TAB_FIRST_HERO,

	BTN_TIANFU,
	BTN_ATTR,
	BTN_SKILL,
	BTN_LEVELUP,
	BTN_ADVANCE,
	BTN_WEAPON,
	BTN_CLOTHES,
	BTN_HORSE,
	BTN_HAT,
	BTN_EARRING,
	BTN_BOOK,
	BTN_START_ADV,
	BTN_START_BREAK,
	BTN_STARUP,
	BTN_START_STARUP,
	BTN_EXCHANGE_ONE,
	BTN_EXCHANGE_TEN,
	BTN_ALMIGHTY,			//点击弹出万能碎片弹窗
	BTN_CLOSE,

	BTN_WEAR,
	BTN_GETWAY
};

enum nameColor
{
	DEFAULT_COLOR,
	WHITE_COLOR,
	GREEN_COLOR,
	BLUE_COLOR,
	PURPLE_COLOR,
	ORANGE_COLOR,
	RED_COLOR
};

//当前显示的面板，包括1.卡牌面板，2.觉醒面板，3.属性面板，4.升星面板
enum curPanel
{
	CARD_PANEL,
	ADV_PANEL,
	ATTRI_PANEL,
	STARSUP_PANEL
};

//UI描述类
typedef struct tagHeroUI
{
	//根节点
	Layout* mRoot;	

	//英雄列表面板
	Layout* mHeroListPanel;
	//英雄列表滚动区域
	ScrollView* mHeroListScrollView;
	vector<UIToolHeader*> mVecHeroIcon;
	//向上滚动图标
	ImageView* mScrollUpBtn;
	//向下滚动图标
	ImageView* mScrollDownBtn;
	//关闭面板按钮
	Button* mBackBtn;

	//万能碎片面板
	Layout* mAlmightyPanel;
	Layout* mPnlAlmightyIcon;
	Layout* mPnlHeroIcon;
	Text*   mTextAlmighty;
	Text*   mTextHero;
	LoadingBar* mBarNumProgress;
	Text*	mTextNumProgress;
	Text*   mTextExchangeTips;
	Button* mBtnExChangeOne;
	Button* mBtnExChangeTen;
	Button* mBtnClose;

	//主角按钮面板
	Layout* mMainHeroBtnPanle;
	//主角觉醒按钮标志
	ImageView* mMainHeroAdvFlagImg;
	//星宿红点标识
	ImageView* mStarsSuFlagImg;

	//英雄按钮面板
	Layout* mHeroBtnPanel;
	//英雄觉醒按钮标志
	ImageView* mHeroAdvFlagImg;
	
	//英雄升星材料不足加号
	//ImageView* mHeroChipsAddFlagImg;

	//中间UI面板升星进度
	LoadingBar* mMidStarsUpBar;			//英雄升星进度条
	Text* mMidChipCount;				//英雄升星碎片提示

	//英雄升星UI
	Layout* mPnlStarsUp;
	Layout* mPnlStarsOld;				//升星前星级
	Layout* mPnlStarsNew;				//升星后星级
	Text*   mStasrsUpTxt;
	ImageView* mImgFullStars;
	Layout* mPnlStarsAttri;
	Layout* mPnlAdvChips;				//万能碎片
	Layout* mPnlHeroChips;				//英雄碎片
	LoadingBar* mHeroStarUpBar;			//英雄升星进度条
	Text* mHeroChipCount;				//英雄升星碎片提示
	ImageView* mHeroStarUpFlagImg;		//英雄可升星红点标识
	Button* mBtnStarsUp;
	Point mStarsUpPnlPos;
	//升星属性相关
	Text*  mAtkCurNum;
	Text*  mAtkNextNum;
	Text*  mHpCurNum;
	Text*  mHpNextNum;
	Text*  mPddCurNum;
	Text*  mPddNextNum;
	Text*  mMddCurNum;
	Text*  mMddNextNum;
	ImageView* mAtkArrow;
	ImageView* mHpArrow;
	ImageView* mPddArrow;
	ImageView* mMddArrow;

	ImageView* mOldStarImg[STAR_COUNT];
	ImageView* mNewStarImg[STAR_COUNT];

	//装备
	ImageView* mWeaponImg;
	ImageView* mClothesImg;
	ImageView* mHatImg;
	ImageView* mEarringImg;
	ImageView* mBookImg;
	ImageView* mHorseImg;
	UIToolHeader* mWeapon;
	UIToolHeader* mClothes;
	UIToolHeader* mHorse;
	UIToolHeader* mHat;
	UIToolHeader* mEarring;
	UIToolHeader* mBook;
	
	//英雄信息
	Text* mNameText;
	//ImageView* mImgAdvLvl;
	//TextAtlas * mTalentText;					//资质
	ImageView* mImgTalent;
	ImageView* mTopStarImg[STAR_COUNT];
	ImageView* mNickNameImg;
	ImageView* mImgAttriType;			//英雄类型
	//属性
	ImageView* mHeroAttriImg;
	Point mheroAttrImgOriginPos;
	Text* mAtkTextNum;
	Text* mHpTextNum;
	Text* mPddTextNum;
	Text* mMddTextNum;
	Text* mFightTextNum;
	ImageView* mAtkIcon;
	
	//spine动画
	Layout* mHeroSpinePanel;
	//缘分
	Layout* mHeroFatePanel;
	ImageView* mHeroFateTitle;

	//英雄角色形象面板
	Layout* mHeroPanel;
	Point mHeroPanelPos;
	UICardHead* mHeroCard;
	
	//英雄属性面板
	Layout* mHeroInfoPanel;
	Point mHeroInfoPanelPos;
	ui::ScrollView* mScrollPanel;
	//TextAtlas* mHeroTalent;			//英雄资质
	ImageView* mImgHeroTalent;
	Text* mSpeed;				//速度值
	Text* mAtkAdd;
	Text* mHpAdd;
	Text* mMddAdd;
	Text* mPddAdd;
	Text* mCritTextNum;			//暴击
	Text* mCritCoeTextNum;		//暴击伤害
	Text* mMisPrbTextNum;		//闪避率
	Text* mHitPrbTextNum;		//命中率
	Text* mCritResisTextNum;	//暴击抗性	
	Text* mComboTextNum;		//连击概率
	//需根据英雄攻击类型区分
	Text* mInfoAtkAddText;
	ImageView* mInfoAtkAddIcon;
	//英雄介绍
	Text* mHeroIntroText;
	//Text* mHeroTypeIntroText;
	ImageView* mImgHeroType;	//力智敏
	ImageView* mImgJobType;		//英雄职业
	

	//觉醒、突破面板
	Layout* mAdvInfoPanel;
	Point mAdvInfoPanelPos;
	//觉醒属性面板
	Layout* mAdvPropertyPanel;
	//觉醒标签
	ImageView* mAdvTitle1;
	ImageView* mAdvTitle2;
	//开始觉醒按钮
	Button* mBtnStartAdv;
	//属性
	Text* mAdvanceAtkTextNum;
	Text* mAdvanceHpTextNum;
	Text* mAdvancePddTextNum;
	Text* mAdvanceMddTextNum;
	ImageView* mArrow1;
	ImageView* mArrow2;
	ImageView* mArrow3;
	ImageView* mArrow4;
	//需根据攻击类型区分
	Text* mAdvAtkText;
	ImageView* mAdvAtkIcon;

	//激活技能
	Text* mAdvanceTips1;
	Layout* mAdvanceSkillPanel;
	//品质颜色、进度条
	Text* mQuaName1;
	Text* mQuaName2;
	LoadingBar* mQuaUpBar;
	Text* mQuaUpJinDu;

	//觉醒消耗
	Text* mAdvanceCoin;
	Layout* mAdvanceMatPanel;

	//觉醒到最高等级
	Text* mAdvanceMaxLv;
	
	Text* mNeedLevelTxt;

	//突破标签
	ImageView* mBreakTitle1;
	ImageView* mBreakTitle2;
	//突破按钮
	Button* mBreakBtn;

	//需根据攻击类型区分
	Text* mBreakAtkAddText;
	ImageView* mBreakAtkAddIcon;

	//角色粒子效果
	Armature* mParticleFrontAni;
	Armature* mParticleBackAni;
	

	tagHeroUI()
	{
		memset(this, 0, sizeof(tagHeroUI));
	}

}HeroUI;

#endif //UITagHero_h__