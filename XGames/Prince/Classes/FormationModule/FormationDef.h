/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		FormationDef
 *  Description:	阵型相关数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/10/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef FormationDef_h__
#define FormationDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "ResourceManager/ResourceLoader.h"
#include "CustomScrollBar.h"
#include "FightModule/Skill/AbstractSkill.h"
#include "HeroModule/HeroModel.h"
#include "Utils/SystemUtils.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

// 常量定义;
#define FORMATION_EMPTY_POS				-1		// 阵型空位;
#define FORMATION_EMPTY_EQUIPMENT		-1		// 阵型空位空装备;
#define FORMATION_INVALID_SKILL_ID		-1		// 阵型无效技能Id;
#define FORMATION_MAX_POS_COUNT			9		// 阵型站位上限;
#define FORMATION_MAX_HERO_COUNT		6		// 阵型内英雄数量上限;
#define FORMATION_MAX_SKILL_TYPE		5		// 展示技能种类;
#define FORMATION_MAX_COMBO				7		// 最大追击数(加上先手的总数量);

#define HERO_ICON_WIDTH					136		// 英雄列表节点宽;
#define HERO_ICON_HEIGHT				180		// 英雄列表节点高;
#define HERO_LIST_SPACE					5		// HeroList头像间距;
#define HERO_MOVE_DURATION				0.1f	// Hero换位动作持续时长;
#define HERO_LONG_PRESS_INTERVAL		0.3f	// Hero长按判定时长;
//#define HERO_SCALE_DURATION			0.1f	// Hero缩放动作持续时长;
//#define HERO_SCALE_MIN_RATE			0.7f	// Hero缩放比例最小值;
//#define HERO_SCALE_NORMAL_RATE		0.9f	// Hero缩放比例正常值;
//#define HERO_SCALE_MAX_RATE			1.0f	// Hero缩放比例最大值;

#define COMBO_HERO_SCALE_RATE			0.6f	// Combo Hero缩放比例;
#define COMBO_HERO_SPACE				90.0f	// Combo Hero头像间距;
#define COMBO_HERO_MAX_COUNT			8		// Combo Hero数量上限;


// 文本定义;
#define STR_FORMATION_JSON_PATH	   "Image/UIFormation/UIFormation.ExportJson"
#define FONT_NAME_FOUNDER_BOLD	 SystemUtils::getFontFileName()

// 阵型显示模式;
enum Formation_Mode
{
	F_MODE_FORMATION,			// 主城阵型模块;
	F_MODE_TOWER,				// 爬塔进入阵型;
	F_MODE_TRAIL,				// 试炼进入阵型;
	F_MODE_BARRIER,				// 关卡战前进入阵型;
	F_MODE_ACTIVITY,			// 活动关卡进入阵型;
	F_MODE_PVP					// Pvp竞技场进入阵型;
};

// 阵型中的Hero站位;
enum HERO_POS
{
	Hero_Pos_0 = 0,
	Hero_Pos_1 = 1,
	Hero_Pos_2,
	Hero_Pos_3,
	Hero_Pos_4,
	Hero_Pos_5,
	Hero_Pos_6,
	Hero_Pos_7,
	Hero_Pos_8,
	Hero_Pos_Invalid
};


// UI消息类型
enum OBS_PARAM_TYPE_FM
{
	OBS_PARAM_TYPE_FORMATION,			// 阵型;
	OBS_PARAM_TYPE_HERO_LIST,			// 英雄列表;
	OBS_PARAM_TYPE_HERO_ON_FIELD,		// 英雄上阵;
	OBS_PARAM_TYPE_HERO_OFF_FIELD,		// 英雄下阵;
	OBS_PARAM_TYPE_HERO_SWAP,			// 英雄换位;
	OBS_PARAM_TYPE_SELECTED,			// 更新选中;
	OBS_PARAM_TYPE_SKILL_INFO,			// 技能;
	OBS_PARAM_TYPE_COMBO,				// 最大追击;
	OBS_PARAM_TYPE_FRIENDS,				// 小伙伴;
	OBS_PARAM_TYPE_UPLOAD				// 上传阵型信息的结果;
};


// 技能UI组件集;
typedef struct tagUI_FormationSkill
{
	// Button;
	Button*		btnSkill;

	// Skill Img;
	ImageView*	imgSkill;

	tagUI_FormationSkill()
	{
		btnSkill = nullptr;
		imgSkill = nullptr;
	}
}UI_FormationSkill;


// 阵型界面UI组件集;
class SkillTips;
typedef struct tagUI_Formation
{
	Layout*			pRoot;
	Layout*			pnlFormation;

	// Hero;
	ImageView*				imgHeroListBg;
	ScrollView*				listHero;
	CustomScrollBar*		listHeroBar;
	ImageView*				imgHeroSelected;

	// Skill(5*skill + 1*tips);
	Layout*					pnlSkill;
	UI_FormationSkill		btnSkill[5];
	Button*					btnEditSkill;

	// Formation;
	Text*			txtFc;
	Layout*			pnlField;
	ImageView*		imgPos[FORMATION_MAX_POS_COUNT];
	ImageView*		imgPosIndex[FORMATION_MAX_POS_COUNT];
	Button*			btnShowCombo;
	Button*			btnCloseCombo;
	ImageView*		imgHeroCountTitle;
	Text*			txtHeroCount;
	Text*			txtFrontLevel;
	Text*			txtMiddleLevel;
	Text*			txtBackLevel;

	// 阵法;
	Button*			btnZhenfa;
	Text*			txtAddition;

	// Combo;
	ImageView*		imgRushBg;
	ImageView*		imgRushFirstHeroSkill_Manual;		// 主动技能，奥义;
	ImageView*		imgRushFirstHeroSkill_Atk;			// 普攻;
	ImageView*		imgRush[FORMATION_MAX_COMBO];
	ImageView*		imgRushX[FORMATION_MAX_COMBO];
	UIToolHeader*	rushHero[FORMATION_MAX_COMBO];
	ImageView*		imgRushSkillPre[FORMATION_MAX_COMBO];
	ImageView*		imgRushSkillDst[FORMATION_MAX_COMBO];
	ImageView*		imgRushArrow[FORMATION_MAX_COMBO];
	ImageView*		imgRushEmpty;

	// 活动关卡上阵英雄比对;
	Layout*			pnlActivityHeroList;
	Text*			txtActivityHeroes[FORMATION_MAX_HERO_COUNT];

	tagUI_Formation()
	{
		memset(this, 0, sizeof(tagUI_Formation));
	}
}UI_Formation;


// Hero列表UI参数;
typedef struct tagUI_HERO_LIST_PARAM
{
	// 显示模式;
	Formation_Mode nMode;

	// HeroId;
	int  nHeroId;
	int  nResId;

	// 星级;
	int  nStar;

	// 等级;
	int  nLv;

	// 血量;
	float  fHpPercent;

	// 是否上阵;
	bool bIsOnField;

	// 阵位;
	HERO_POS  nPos;

	// 技能前置/后置;
	AbstractSkill::RESULT_STATE  nPreState;
	AbstractSkill::RESULT_STATE  nNextState;

	tagUI_HERO_LIST_PARAM()
		: nMode(F_MODE_FORMATION)
		, nHeroId(-1)
		, nResId(-1)
		, nStar(0)
		, nLv(0)
		, fHpPercent(0.0f)
		, bIsOnField(false)
		, nPos(Hero_Pos_Invalid)
		, nPreState(AbstractSkill::RESULT_STATE::NONE_STATE)
		, nNextState(AbstractSkill::RESULT_STATE::NONE_STATE)
	{
	}
}UI_HERO_LIST_PARAM;


// 阵型UI参数;
typedef struct tagUI_FORMATION_PARAM
{
	// 战斗力;
	int			 nFc;

	// 阵型站位;
	vector<int>  vcFormation;

	// Hero数据;
	vector<UI_HERO_LIST_PARAM> vcHeroData;

	tagUI_FORMATION_PARAM()
		: nFc(0)
	{
		vcFormation.clear();
		vcHeroData.clear();
	}
}UI_FORMATION_PARAM;


// Combo UI参数;
typedef struct tagUI_COMBO_PARAM
{
	vector<int>  vcComboList;
	vector<int>  vcIcon;

	tagUI_COMBO_PARAM()
	{
		vcComboList.clear();
		vcIcon.clear();
	}
}UI_COMBO_PARAM;


// 技能展示UI参数;
typedef struct tagFORMATION_SKILL_PARAM
{
	// 技能Id;
	int  nSkillId;

	// 技能Icon;
	int  nResId;

	// 技能类型;
	int  nSkillType;

	// 技能名称;
	string  strName;

	// 技能描述;
	string  strDesc;

	// 是否激活;
	bool  bEnabled;

	tagFORMATION_SKILL_PARAM()
		: nSkillId(-1)
		, nSkillType(0)
		, nResId(-1)
		, bEnabled(false)
	{
		strName.clear();
		strDesc.clear();
	}

}FORMATION_SKILL_PARAM;


// UI交换阵位参数;
typedef struct tagUI_SWAP_FORMATION_PARAM
{
	// 战斗力;
	int  nFc;

	// 交换的Hero数据;
	UI_HERO_LIST_PARAM  uiOriHero;
	UI_HERO_LIST_PARAM  uiDstHero;

	tagUI_SWAP_FORMATION_PARAM()
		: nFc(0)
	{

	}
}UI_SWAP_FORMATION_PARAM;


// 获得技能前置/后置类型;
static AbstractSkill::RESULT_STATE  getSkillState( int nSkillType )
{
	AbstractSkill::RESULT_STATE nState = AbstractSkill::NONE_STATE;
	switch (nSkillType)
	{
	case 1:		// 雷;
		nState = AbstractSkill::FELL;
		break;

	case 2:		// 火;
		nState = AbstractSkill::BACK;
		break;

	case 3:		// 风;
		nState = AbstractSkill::FLOAT;
		break;
	default:
		break;
	}
	return nState;
}

// 获得技能前置/后置类型图标;
static std::string  getSkillStateIcon( AbstractSkill::RESULT_STATE nState )
{
	string strName = "Image/UIFormation/Icon/";
	switch (nState)
	{
	case AbstractSkill::NONE_STATE:
		{
			strName.clear();
			strName.append("Image/Icon/Small/-1.png");
		}
		break;
	case AbstractSkill::FELL:		// 雷;
		strName.append("img_jineng_lei_2.png");
		break;
	case AbstractSkill::BACK:		// 火;
		strName.append("img_jineng_huo_2.png");
		break;
	case AbstractSkill::FLOAT:		// 风;
		strName.append("img_jineng_feng_2.png");
		break;
	default:
		break;
	}

	return strName;
}

// 获得攻防先辅图标;
static string getJobIcon(HERO_JOB_TYPE jobType)
{
	string strName = "Image/Icon/global/";
	switch (jobType)
	{
	case HERO_DEFENSE:
		strName.append("img_shuxing2_fang.png");
		break;
	case HERO_ATK:
		strName.append("img_shuxing2_gong.png");
		break;
	case HERO_HELP:
		strName.append("img_shuxing2_fu.png");
		break;
	case HERO_FIRST:
		strName.append("img_shuxing2_xian.png");
		break;
	default:
		{
			strName.clear();
			strName.append("Image/Icon/Small/-1.png");
		}
		break;
	}
	return strName;
}

// 最大追击技能记录参数;
typedef struct tagCombo_Skill_Param
{
	// 序列号;
	int  nIndex;

	// 最大连击数;
	int  nMaxCombo;

	// 首发Hero的技能类型;
	AbstractSkill::SKILL_TYPE  nFirstSkillType;

	// 普攻或主动攻击触发<被动1>技能追击, 英雄列表;
	vector<int>  vcComboList;
	vector<int>  vcComboListIcon;

	// 触发<被动3>技能追击, 英雄列表;
	vector<int>  vcTrigerList;
	vector<int>  vcTrigerListIcon;

	// 追击技能列表<HeroId, <preSkillResultState, dstSkillResultState> >;
	vector<pair<int, pair<AbstractSkill::RESULT_STATE, AbstractSkill::RESULT_STATE> > >  vcComboSkillState;

	tagCombo_Skill_Param()
	{
		clear();
	}

	void  clear()
	{
		nIndex = 0;
		nMaxCombo = 0;
		nFirstSkillType = AbstractSkill::NORMAL_SKILL;
		vcComboList.clear();
		vcComboListIcon.clear();
		vcTrigerList.clear();
		vcTrigerListIcon.clear();
		vcComboSkillState.clear();
	}

	// 添加一个英雄到序列中;
	void  addHero(int nHeroId,
		AbstractSkill::SKILL_TYPE nRushSkillType,
		AbstractSkill::RESULT_STATE preSkillResultState = AbstractSkill::RESULT_STATE::NONE_STATE,
		AbstractSkill::RESULT_STATE dstSkillResultState = AbstractSkill::RESULT_STATE::NONE_STATE)
	{
		// 被动1有前后置状态;
		if (nRushSkillType == AbstractSkill::SKILL_TYPE::RUSH_SKILL)
		{
			vcComboList.push_back(nHeroId);
			vcComboSkillState.push_back(pair<int, pair<AbstractSkill::RESULT_STATE, AbstractSkill::RESULT_STATE> >(nHeroId,
				pair<AbstractSkill::RESULT_STATE, AbstractSkill::RESULT_STATE>(preSkillResultState, dstSkillResultState)));
		}
		// 被动3没有前后置状态;
		if (nRushSkillType == AbstractSkill::SKILL_TYPE::TRIGGER_SKILL)
		{
			vcTrigerList.push_back(nHeroId);
		}

		// 最大连击数;
		// 不计算被动3连击数了;
		nMaxCombo = vcComboList.size();
	}

	int  getComboListCount()
	{
		return vcComboList.size();
	}

	int  getTrigerListCount()
	{
		return vcTrigerList.size();
	}

}Combo_Skill_Param;


#endif // FormationDef_h__