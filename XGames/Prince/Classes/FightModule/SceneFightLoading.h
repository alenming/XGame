#ifndef SceneFightLoading_h__
#define SceneFightLoading_h__

#include "cocos2d.h"
#include "ChapterModule/ChapterDef.h"
#include "Utils/Observer.h"
#include "FightModel.h"

USING_NS_CC;

enum ASYNC_RES_TYPE
{
	ASYNC_RES_FIGHTER,
	ASYNC_RES_SKILL,
	ASYNC_RES_BACKGROUND,
	ASYNC_RES_UI
};

class SceneFightLoading : public Scene, public Observer
{

public:

	SceneFightLoading();
	virtual ~SceneFightLoading();

	static SceneFightLoading* create(FIGHT_TYPE fightType, int barrierId);

public:

	//��ʼ��
	virtual bool init(FIGHT_TYPE fightType, int barrierId);

	//֪ͨ����
	virtual void updateSelf(void* data) override;

	virtual void onEnter() override;

private:

	//��ʼ��ս������
	void initFightType(FIGHT_TYPE fightType, int barrierId);

	//�����첽����
	void startAsyncLoading();

	//��ӽ�ɫ��Դ
	void addFighterResPath(int resId);

	//��Ӽ�����Դ
	void addSkillResPath(int resId);

	//��ӱ�����Դ
	void addBgResPath(int resId);

	//���ȫ��������Դ
	void addFighterAndSkillRes(Vector<Fighter*>* vecFighter, std::set<int>& fighterSet, std::set<int>& skillSet);

	//�첽������ϻص�
	void onAsyncLoadedCallBack();

private:

	//�첽����ģ��
	ResourceLoader* mResourceLoader;

	//ս������
	FIGHT_TYPE mFightType;

	//�ؿ�id
	int mBarrierId;
};


#endif //SceneFight_h__