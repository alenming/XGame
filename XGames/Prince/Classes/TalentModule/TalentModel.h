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

	//���������Ա��ȡ��������id
	int getSkillTypeId(TALENT_SKILL_TYPE tag);

	//��ȡ��������
	vector<TalentSkillData>& getSkillData();

	//��¼��¼ʱ���ǵȼ�����layer�кͼ���Ҫ��ȼ����Ա�
	void setRecordMainHeroLv(int lv);
	int getRecordMainHeroLv();

	//�����ǵ�ǰ���������Ƿ���Ҫչʾ�¿���״̬
	void setSkillNewOpenState(int index);

private:
	TalentModel();
	virtual ~TalentModel();

	//�����츳�������
	void parseTalentData(TALENT_SKILL_TYPE skillType);

private:

	static TalentModel* ms_pInstance;

	//�����Ա�
	int mSex;

	//���ǵȼ�
	int mainHeroLevel;			

	//����
	vector<TalentSkillData> mVecSkillData;
};


#endif