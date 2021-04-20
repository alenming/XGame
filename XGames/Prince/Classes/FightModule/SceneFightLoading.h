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

	//初始化
	virtual bool init(FIGHT_TYPE fightType, int barrierId);

	//通知更新
	virtual void updateSelf(void* data) override;

	virtual void onEnter() override;

private:

	//初始化战斗类型
	void initFightType(FIGHT_TYPE fightType, int barrierId);

	//启动异步加载
	void startAsyncLoading();

	//添加角色资源
	void addFighterResPath(int resId);

	//添加技能资源
	void addSkillResPath(int resId);

	//添加背景资源
	void addBgResPath(int resId);

	//添加全部技能资源
	void addFighterAndSkillRes(Vector<Fighter*>* vecFighter, std::set<int>& fighterSet, std::set<int>& skillSet);

	//异步加载完毕回调
	void onAsyncLoadedCallBack();

private:

	//异步加载模块
	ResourceLoader* mResourceLoader;

	//战斗类型
	FIGHT_TYPE mFightType;

	//关卡id
	int mBarrierId;
};


#endif //SceneFight_h__