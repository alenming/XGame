#ifndef LayerManualSkill_h__
#define LayerManualSkill_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "../UI/UIDef.h"
#include "Widget/SkillTips.h"
#include "CCArmature.h"
#include <functional>

#define MAX_HERO_COUNT 5

class SceneFight;

USING_NS_CC;
using namespace ui;
using namespace cocostudio;

typedef function<void(Armature *armature, MovementEventType movementType, const std::string& movementID)> AniCallBack;

class UIManualSkillItem : public Node
{

public:

	UIManualSkillItem();
	~UIManualSkillItem();

	//创建技能组件
	static UIManualSkillItem* create(Layout* root);

public:

	//初始化
	virtual bool init();

	//设置模版
	void setRoot(Layout* root);

	//设置技能图标灰态
	void setSkillIconGray(bool gray);

	//是否可触发技能
	void setSkillEnable(bool enable);

	//播放CD动画
	void playCDAni(const AniCallBack& callback);

public:

	//根结点
	Layout* mRoot;
	//角色头像
	Layout* mSkillPanel;
	//所需SP
	Text* mTxtSP;
	//SP不足时显示红色字体
	Text* mTxtRedSp;
	//技能对应的角色名称
	Text* mTxtHeroName;
	//蒙板
	ImageView* mImgMask;
	//遮盖
	ImageView* mImgDisable;
	//响应按钮
	Button* mBtnSkill;
	//技能图标
	Sprite* mSkillIcon;
	//高亮边框
	ImageView* mBrightBorder;
	//普通边框
	ImageView* mBorder;
	//CD动画容器
	Layout* mCDPanel;
	//锁定标记
	ImageView* mLockImg;
	//主动技能动画
	Armature* mLightAni;

	//主动技能回合数动画
	Armature* mSkillCDAni;

	//技能的佩带者index
	int mFighterIndex;

	//是否可释放技能
	bool mSkillEnable;

	//技能ID
	int mSkillId;

	//上一次的CD
	int mLastCD;

	//本次CD
	int mCurrentCD;

};


class LayerManualSkill: public cocos2d::Layer
{
public:

	LayerManualSkill();
	~LayerManualSkill();

	CREATE_FUNC(LayerManualSkill);

public:

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	virtual void updateUI(bool playTouchAni = false);

	virtual void initSkill();

protected:

	virtual bool initUI();


	virtual void onBtnSkill(Ref* ref, cocos2d::ui::Widget::TouchEventType type, UIManualSkillItem* skillItem);

private:

	//父结点
	SceneFight*	m_pParent;

	//技能组件列表
	vector<UIManualSkillItem*> mVecManualSkillItem;

	//容器
	Node* mContainerNode;

};

#endif // LayerManualSkill_h__
