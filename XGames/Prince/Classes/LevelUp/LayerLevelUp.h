/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerLevelUp
 *  Description:	角色升级的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/31/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerLevelUp_h__
#define LayerLevelUp_h__

#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "Utils/ResourceUtils.h"
#include "FightModule/UIFighter.h"
#include "HeroModule/HeroModel.h"
#include "GuideCenter/GuideDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace std;

typedef  std::function<void()>  LevelUpCallback;

// 升级功能新开启类型;
enum LVUP_OPEN_TYPE
{
	OPEN_TYPE_NEW		= 1,		// 新开启;
	OPEN_TYPE_OPENED,				// 可进行;
	OPEN_TYPE_INVALID
};

// 升级功能跳转类型;
enum LVUP_JUMP_TYPE
{
	JUMP_TYPE_MAIN		= 10001,	// 主界面;
	JUMP_TYPE_INVALID	
};

// 升级页面后续跳转类型;
enum LVUP_UI_JUMP_TYPE
{
	UI_JUMP_TYPE_NONE,				// 空类型(无后续跳转);
	UI_JUMP_TYPE_FIGHT_SETTLEMENT	// 战斗结算;
};

// UI组件集;
typedef struct tagUI_Level_Up
{
	Layout*  pRoot;

	// 新开启;
	ImageView*  imgNew;
	ImageView*	imgNewTitle;
	ImageView*	imgCurTitle;
	Button*		btnNew;

	// 未开启;
	ImageView*  imgNext;
	ImageView*	imgNextTitle;
	Text*		txtNextLv;
	ImageView*	imgNextFunc;

	// 精力和体力上限;
	ImageView*  imgJingli;
	ImageView*  imgJiantou1;
	ImageView*  imgTili;
	ImageView*  imgJiantou2;
	Text*		txtJingli1;
	Text*		txtJingli2;
	Text*		txtTili1;
	Text*		txtTili2;

	tagUI_Level_Up()
	{
		memset(this, 0, sizeof(tagUI_Level_Up));
	}
}UI_Level_Up;

// UI参数;
typedef struct tagUI_LEVEL_UP_PARAM
{
	// 新等级;
	int  nNewLv;

	// 体力和精力值;
	int  nPreEnergy;
	int  nPrePower;
	int  nEnergy;
	int  nPower;

	void clear()
	{
		nNewLv = 1;
		nPreEnergy = 0;
		nPrePower = 0;
		nEnergy = 0;
		nPower = 0;
	}

	bool empty()
	{
		return (nNewLv == 1);
	}

	tagUI_LEVEL_UP_PARAM()
	{
		clear();
	};

}UI_LEVEL_UP_PARAM;

class SceneFight;
class LayerLevelUp : public Layer
{
public:
	~LayerLevelUp(void);

	static LayerLevelUp* create(UI_LEVEL_UP_PARAM* param, LevelUpCallback callback = nullptr);

	virtual bool  init(UI_LEVEL_UP_PARAM* param, LevelUpCallback callback = nullptr);

	virtual void onEnter();

	//void  setLogicParent(SceneFight* parent) { m_layerLogicParent = parent; };

private:
	LayerLevelUp(void);

	void  initUI(UI_LEVEL_UP_PARAM* param);

	void  update(float delta);

	void  showTouchContinue();

	void  onLayerTouched(Ref* ref, Widget::TouchEventType type);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, LVUP_JUMP_TYPE nJumpType);

	void  movementEventCallFunc(Armature *armature, MovementEventType movementType, const std::string& movementID);

	void  openFunc(int nCurLv);

	void  openNextFunc(int nNextLv, int nResId);

	// 判定跳转;
	void  jumpScene();

private:
	UI_Level_Up  m_ui;

	//SceneFight*  m_layerLogicParent;

	LevelUpCallback  _callback;

};

#endif // LayerLevelUp_h__
