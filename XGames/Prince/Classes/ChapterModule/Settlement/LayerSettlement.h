/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSettlement
 *  Description:	战斗结果展示UI;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/23/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerSettlement_h__
#define LayerSettlement_h__

#include "SettlementDef.h"

enum Btn_Tag
{
	Btn_Tag_Pos_1 = 101,	
	Btn_Tag_Pos_2,
	Btn_Tag_Pos_3,
	Btn_Tag_Pos_4
};

class SceneFight;
class LayerSettlement : public Layer
{
public:
	~LayerSettlement(void);

	static LayerSettlement* create(SettlementInfo info);

	virtual bool init(SettlementInfo info);

	virtual void onEnter();

	virtual void onExit();

	void  setLogicParent(SceneFight* parent) { m_pLogicParent = parent; };


private:
	LayerSettlement(void);

	void  initUI();

	void  onLayerTouched(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 动画回调;
	void  movementEventCallFunc(Armature *armature, MovementEventType movementType, const std::string& movementID);

	// 帧回调;
	void  frameEventCallFunc(Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex);

	// 模拟按钮响应;
	void  enableTouch(Node* node, Btn_Tag nTag);
	void  onBtnCLicked(Btn_Tag nTag);

	// 动态提示;
	void  showTouchContinue(bool bWin);

	// 升级页面回调;
	void  onLevelUpDone();

	// 获得物品动画;
	void  runItemAnimation(Node* pSender, int nIndex);
	void  shiningStar(UIToolHeader* item);

	// 队员经验提升/升级;
	void  showGlobalExp();
	void  showGlobalLevelUp(pair<int, Point> heroPos);

private:

	//UI_Settlement	m_ui;

	SceneFight*  m_pLogicParent;

	SettlementInfo  m_settlementInfo;

	Layout*		m_pRoot;
	Armature*	_winArmature1;		// 胜利标识飞入;
	Armature*	_winArmature2;		// 星星;

	vector<UIToolHeader*>  m_vcItem;


	enum UI_Settlement_Widget
	{
		Pnl_Root,
		Pnl_Win,
		Lab_Exp,
		Lab_Coin,
		Img_Drop_Line,
		Pnl_Lose,
		Lab_Desc,
		Btn_Training,
		Btn_Adv,
		Btn_Equ,
		Btn_Star
	};

};

#endif // LayerSettlement_h__
