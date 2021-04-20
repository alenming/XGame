#ifndef IEffectUI_h__
#define IEffectUI_h__

#include "cocos2d.h"
#include "FightStateMachine.h"

// 接口类，由技能特效的父绘制节点继承和实现，父绘制节点可能是战士UI，也可能是层UI
class IEffectUI
{
public:
	// 获取作用状态时的特效在父节点的绘制位置
	virtual cocos2d::Point getEffectActPosition(FightStateMachine::E_SIDE side) = 0;

	// 获取前置状态时（发射/平移效果）的特效在父节点的起点绘制位置
	virtual cocos2d::Point getEffectStartPostion(cocos2d::Node* pNode) = 0;

};

#endif // IEffectUI_h__
