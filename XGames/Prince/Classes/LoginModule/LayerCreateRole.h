#ifndef LayerCreateRole_h__
#define LayerCreateRole_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Widget/SpineAnimation.h"
#include "Widget/EditBoxEx.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UICreateRole
{
	Layout* root;
	Layout* parent;

	Button* heroMaleBtn;
	Button* heroFemaleBtn;
	ImageView* heroImg;
	ImageView* tvImg;
	Layout* rolePanel;

	vector<ImageView*> vecHeroStar;
	vector<ImageView*> vecHeroInfo;

	TextField* nameTextField;
	EditBoxEx* nameEditBoxEx;

	Button* changeBtn;
	Button* startBtn;
};

class LayerCreateRole : public Layer, public Observer
{
	
public:

	LayerCreateRole();
	virtual ~LayerCreateRole();

	CREATE_FUNC(LayerCreateRole);

	static Scene* createScene();

public:

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void updateSelf(void* data);

private:

	//初始化UI
	void initUI();

	//初始化骨骼动画
	void initSkeleton();

	//初始化动画
	void initAnimation();

	//初始化动作列表
	void initActionList();

	//选择英雄响应
	void onMaleHeroSelect(Ref* target, Widget::TouchEventType eventType);
	void onFemaleHeroSelect(Ref* target, Widget::TouchEventType eventType);

	//开始按钮响应
	void onBtnStart(Ref* target, Widget::TouchEventType eventType);

	//换名字按钮响应
	void onBtnChangeName(Ref* target, Widget::TouchEventType eventType);

	//更换角色动作响应
	void onChangeAction(Ref* target, Widget::TouchEventType eventType);

	//选择英雄
	void selectMaleHero(bool playAni = true);
	void selectFemaleHero(bool playAni = true);

	//播放选择动画
	void playSelectAni();

	//设置名字
	void setRoleName();

	//设置骨骼动作
	void setSkeletonAction();

private:

	UICreateRole mUI;

	//是否选择男性角色
	bool mIsSelectMale;

	SpineAnimation* mMaleSkeleton;
	SpineAnimation* mFemaleSkeleton;

	Armature* mSelectAni;
	Armature* mTextAni;

	//角色创建男性角色名字列表
	vector<string> mMaleNameList;

	//角色创建女性角色名字列表
	vector<string> mFemaleNameList;

	//当前男性名字索引
	int mMaleNameIndex;

	//当前女性名字索引
	int mFemaleNameIndex;

	//男性角色动作列表
	vector<string> mVecMaleAction;

	//女性角色动作列表
	vector<string> mVecFemaleAction;

	//当前男性动作列表索引
	int mMaleActionIndex;

	//当前女性动作列表索引
	int mFemaleActionIndex;

};




#endif //LayerCreateRole_h__