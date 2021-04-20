#ifndef TalentModel_h__
#define TalentModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "TalentData.h"


class TalentModel : public Observable
{
public:

	static TalentModel* getInstance();
	static void destroyInstance();

	//根据主角性别获取技能类型id
	int getSkillTypeId(TALENT_SKILL_TYPE tag);

	//获取技能数据
	vector<TalentSkillData>& getSkillData();

	//记录登录时主角等级，在layer中和技能要求等级做对比
	void setRecordMainHeroLv(int lv);
	int getRecordMainHeroLv();

	//设置是当前技能数据是否需要展示新开启状态
	void setSkillNewOpenState(int index);

private:
	TalentModel();
	virtual ~TalentModel();

	//解析天赋表格数据
	void parseTalentData(TALENT_SKILL_TYPE skillType);

private:

	static TalentModel* ms_pInstance;

	//主角性别
	int mSex;

	//主角等级
	int mainHeroLevel;			

	//数据
	vector<TalentSkillData> mVecSkillData;
};


#endif