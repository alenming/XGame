/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		MaterialNode
 *  Description:	材料节点封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/03/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef MaterialNode_h__
#define MaterialNode_h__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include <string>
#include "SmithyDef.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::ui;
using namespace cocostudio;

typedef struct tagUI_Material_Node
{
	Layout*			pRoot;
	Button*			btnNode;
	UIToolHeader*	item;
	Text*			txtName;
	Text*			txtExp;
	CheckBox*		cbState;

	tagUI_Material_Node()
	{
		memset(this, 0, sizeof(tagUI_Material_Node));
	}
}UI_Material_Node;

typedef struct tagMaterialNodeParam
{
	Node*	widgetLogicParent;
	string  strName;
	int		nExp;
	bool	bSelected;
	SelectedMaterialParam smParam;

	void clear()
	{
		widgetLogicParent = nullptr;
		strName = "";
		nExp = 0;
		bSelected = false;
		smParam.clear();
	}

	tagMaterialNodeParam()
	{
		clear();
	}
}MaterialNodeParam;

enum MaterialNodeSelectEvent
{
	MNSE_SELECTED,
	MNSE_UNSELECTED,
};

typedef std::function<bool(SelectedMaterialParam, MaterialNodeSelectEvent)>  MaterialNodeSelectCallback;

class MaterialNode : public Node
{
public:
	MaterialNode(void);
	~MaterialNode(void);

	static MaterialNode* create(MaterialNodeParam param, MaterialNodeSelectCallback callback);

	virtual bool init(MaterialNodeParam param, MaterialNodeSelectCallback callback);

	void  updateParam(MaterialNodeParam param);

private:

	void  initUI();

private:

	UI_Material_Node	m_ui;
	MaterialNodeParam	m_param;
	MaterialNodeSelectCallback _callback;
	bool  m_bCancelSelected;
};

#endif // MaterialNode_h__
