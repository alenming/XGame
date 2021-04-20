/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerOp
 *  Description:	试炼选择对手/星之佑护界面的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/16/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerOp_h__
#define LayerOp_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "OpNode.h"
#include "BuffNode.h"
#include "LayerTrail.h"
#include "ToolModule/ToolDef.h"

class OpNode;
class BuffNode;
class LayerTrail;
class LayerOp : public Layer, public Observer, public LayerHeroToolObserver
{
public:
	LayerOp(void);
	~LayerOp(void);

	static LayerOp* create(POP_WND_TYPE type);

	virtual bool init(POP_WND_TYPE type);

	virtual void onEnter();

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type);

	virtual void  updateSelf(void* pData);

	virtual void  update(float delta);

	void  onOpChallange(unsigned int nLv);

	void  onBuffClicked(int nBuffId);

	virtual void  onSelectHeroConfirm(const int& nHeroId);

	// 设置父节点;
	void  setParent(LayerTrail* parent) { m_pParent = parent; };

private:

	void  initUI();

	// 更新对手;
	void  updateOp(vector<UI_TOWER_OP_PARAM>* vcParam);
	void  updateOpState(int nValidLv);

	// 更新星星数量;
	void  updateStar(int nCount);

	// 更新buff;
	void  updateBuff(vector<UI_BUFF_PARAM>* vcParam);

	// 更新buff可兑换状态;
	void  updateBuffState(map<int, bool>* mapBuffState);

	// 关闭窗口的回调;
	void  onCustomPopRet(Ref* pSender, CustomRetType nRet);

private:

	UI_Trail_Op  m_ui;

	// 父节点;
	LayerTrail*  m_pParent;

	// 窗体类型;
	POP_WND_TYPE  m_nWndType;

	// 选中的buff;
	int  m_nSelectedBuffId;

	// buff<index, BuffNode>;
	map<int, BuffNode*>  m_mapBuff;

	// 对手节点<index, OpNode>;
	map<int, OpNode*>  m_mapOp;
};

#endif // LayerOp_h__
