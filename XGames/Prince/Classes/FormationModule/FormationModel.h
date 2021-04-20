/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		FormationModel
 *  Description:	阵型数据模型;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/09/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef FormationModel_h__
#define FormationModel_h__

#include "FormationDef.h"
#include "Utils/Observer.h"
#include "HeroModule/HeroModel.h"

// 试炼相关的阵型属性定义;
typedef struct tagTrail_Formation_Attr
{
	int  nHeroId;

	// 战斗力;
	float  fFc;

	tagTrail_Formation_Attr()
		: nHeroId(-1)
		, fFc(0.0f)
	{
	}
}Trail_Formation_Attr;

class FormationModel : public Observable
{
public:
	~FormationModel(void);

	static FormationModel* getInstance();
	static void destroyInstance();

	// 更新空闲装备列表;
	void  updateFreeEquipment(vector<int>  vcEquipment);

	//设置阵型，角色登录时获取到的阵型数据;
	void setFormation(vector<int> vcFormation);

	// 设置试炼属性;
	void  setTrailAttr(map<int, Trail_Formation_Attr>  mapTrailAttr);

	// 更新阵型;
	void  updateFormation(vector<int> vcFormation);
	void  updateFormation();

	// 设置/查询上阵英雄最大数量;
	void  setMaxHeroCount(int nCount) { m_nMaxHeroCount = nCount; };
	int   getMaxHeroCount() { return m_nMaxHeroCount; };

	// 更新装备;
	void  updateHeroEquipment(map<int, int> mapHeroEquipment);

	// 更新战斗力;
	void  updateFcTeam(int nFcTeam);
	void  updateFcCoe(float fAtk, float fMdd, float fPdd, float fHp);
	void  updateFcSingleA(map<int, float> mapFcSingleA);
	void  updateFcSingleB(map<int, float> mapFcSingleB);

	// 查询战斗力;
	int   getFcTeam();

	// 展示技能描述;
	void  updateSkillInfo(int nHeroId);

	// Hero上阵;
	void  setHeroOnField(int nHeroId, HERO_POS nPos = Hero_Pos_Invalid);

	// Hero下阵;
	void  setHeroOffField(int nHeroId);

	// 交换站位;
	void  swapFormation(HERO_POS nPos1, HERO_POS nPos2);

	// 获取当前空闲装备列表;
	void  getFreeEquipmentList(vector<int>&  vcEquipmentList);

	// 获取当前阵型;
	void  getFormation(vector<int>& vcFormation, bool bNeedRefresh = false);
	void  getFormationWithoutEmptyPos(vector<int>& vcFormation);

	// 获取当前英雄装备列表;
	void  getHeroEquipment(map<int, int>& mapHeroEquipment);

	// 获取当前上阵人数;
	int   getHeroOnFieldCount();

	// 查询Hero是否上阵;
	bool  isHeroOnField(int nHeroId);

	// 获取某个站位的Hero;
	int   getHeroAtPos(HERO_POS nPos);

	// 获取某个Hero的站位;
	HERO_POS   getPosOfHero(int nHeroId);

	// 设置当前选中;
	void  setCurSelected(int nHeroInList, HERO_POS nPos);

	// 获取当前选中的阵型位置;
	int   getCurSelectedPos() { return m_nCurSelectedPos; };

	// 获取当前选中的Hero;
	int   getCurSelectedHero() { return m_nCurSelectedHero; };

	// 上传阵型信息的结果;
	void  procUploadFormation(bool bSucceed);

	// 获取最大追击数;
	int   getMaxCombo() { return m_nMaxCombo; };

	// 设置当前模式;
	void  setFormationMode(Formation_Mode mode);
	Formation_Mode  getFormationMode() { return m_nMode; };

	// 恢复阵型(从试炼模式的数据恢复到普通模式);
	void  resetFormation();

	// 普通阵型数据是否有改变(是否调整了阵型);
	bool  isFormationChanged();

	// 计算最大追击数;
	int   calcMaxCombo(Combo_Skill_Param& comboParam);

	// 更新最大追击;
	void  updateCombo();

	// 构造UI英雄参数;
	void  constructUIHeroParam(int nHeroId, UI_HERO_LIST_PARAM& param);


private:
	FormationModel(void);

	// 更新Hero列表(根据阵型数据对Hero分类);
	void  updateHeroList();

	// 随机获取1件装备;
	int   getEquipment();

	// 获取当前阵型中第一个空位;
	HERO_POS   getFirstEmptyPos();

	// 更新选中;
	void  updateSelected();

	// 更新小伙伴;
	void  updateFriends();

	// 获得技能前置/后置类型;
	//AbstractSkill::RESULT_STATE  getSkillState(int nSkillType);

	// 通知UI刷新显示内容;
	void  updateUI(OBS_PARAM_TYPE_FM type, void* data);

	//////////// Combo相关; /////////////

	///// 1级循环，逻辑首发Hero启动攻击(普攻或主动技能);
	void  startAtk(int nHeroId, AbstractSkill::SKILL_TYPE nSkillType, Combo_Skill_Param& comboSkillParam);

	///// 2级递归，分类查找Rush;
	void  startRush(int nHeroId, int nPstType, Combo_Skill_Param& comboSkillParam);

	// 判断Hero能否追打指定前置类型，并返回追打后置类型;
	bool  isHeroCanRush(int nHeroId, int nPreType, int& nRushResult);

	// 查找下一个符合条件的Hero(1: 本回合未追打过; 2: 可以追打给定的前置);
	int   getNextHeroCanRush(int nPreHeroId, int nPstType, int& nRushResult);

	// 记录/重置Hero追打;
	void  setHeroRushRecord(int nHeroId, bool bRushOver = true);
	void  resetHeroRushRecord(vector<int> vcFormation);

	// 记录/重置Hero被动3追打记录;
	void  setHeroTriggerRecord(int nHeroId, bool bTriggerOver = true);
	void  resetHeroTriggerRecord(vector<int> vcFormation);

	// 去掉重复的Combo记录(此操作单独抽出来是因为可能以后需要不去掉重复的Id);
	void  filterComboList(vector<int>& vcComboList);

	// 追打列表最大值计算函数;
	bool  sortComboList(const Combo_Skill_Param& param1, const Combo_Skill_Param& param2);

	// 计算技能施放后的总击打次数，并判断是否触发了某个Hero的被动3产生连击;
	int   isStartTrigger(int nSkillId);
	int   calcHitCount(int nSkillId);
	int   getTriggerHero();

	// 单个Hero/阵容Fc计算;
	int   calcFcSingle(int nHeroId);
	int   calcFcTeam();

	// 读表;
	float getDataFromDTSystem( int nId );

	// 战斗力计算系数;
	float getBattlePowerPartA(int nHeroId);
	float getBattlePowerPartB(int nHeroId);

	// 属性值计算;(name: "atk", "hp", "mdd", "pdd");
	float getHeroAttriValue( HeroParam* heroParam, string strName );

private:

	static FormationModel* mInstance;

	// 上阵英雄列表<战斗值，Id>;
	multimap<int, int>	m_mapHeroOnField;

	// 未上阵英雄列表<战斗值，Id>;
	multimap<int, int>	m_mapHeroOffField;

	// 空闲装备列表;
	vector<int>  m_vcFreeEquipment;

	// 上阵Hero附带的装备(Id, EquId);
	map<int, int>  m_mapHeroEquipment;

	// 战斗力属性(Id, (A / B));
	int   m_nFcTeam;
	map<int, float>  m_mapFcSingleA;
	map<int, float>  m_mapFcSingleB;

	// 上阵Hero数量上限;
	int   m_nMaxHeroCount;

	// 当前阵型;
	//vector<pair<int, int> >  m_vcFormation;
	vector<int>  m_vcFormation;

	// 试炼相关的阵型属性;
	map<int, Trail_Formation_Attr>   m_mapTrailAttr;

	// 小伙伴列表;
	vector<int>  m_vcFriends;

	// 当前选中的英雄Id;
	int  m_nCurSelectedHero;

	// 当前选中的阵型位置;
	HERO_POS  m_nCurSelectedPos;

	// 本回合计算追打时的逻辑首发Hero;
	int  m_nLogicFirstHeroOnRush;

	// 追打记录(查找追打时使用);
	map<int, bool>  m_mapHeroRushRecord;

	// 被动3触发记录(触发被动3时使用);
	map<int, bool>  m_mapHeroTriggerRecord;

	// 当前最大连击数，用来判断是否触发被动3;
	int  m_nMaxHitCount;

	// 最大追击数;
	int  m_nMaxCombo;

	// 追击技能记录<pre, dst>;
	vector<pair<int, int> >  m_vcComboSkill;

	// 英雄属性值加成(服务器下发，根据英雄上/下阵进行增/减);
	float  m_yueliHp;
	float  m_yueliAtk;
	float  m_yueliMdd;
	float  m_yueliPdd;

	// 属性价值(csv);
	float  m_fAtkCoe;
	float  m_fHpCoe;
	float  m_fMddCoe;
	float  m_fPddCoe;
	float  m_fComboCoe;

	// 数据模式(普通/试炼);
	Formation_Mode  m_nMode;

	// 阵型备份;
	vector<int>  m_vcFormationBackup;

};


#endif // FormationModel_h__

