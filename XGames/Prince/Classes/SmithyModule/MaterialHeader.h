/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		MaterialHeader
 *  Description:	可添加/删除材料封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/02/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef MaterialHeader_h__
#define MaterialHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include <string>
#include "LayerAddMaterial.h"

using namespace std;
using namespace cocos2d::ui;

enum MaterialClickEvent
{
	MCE_ADD,
	MCE_DEL
};

typedef std::function<void(MaterialClickEvent)> MaterialClickCallback;

class MaterialHeader : public Node
{
public:
	MaterialHeader(void);
	~MaterialHeader(void);

	static MaterialHeader* create(MaterialClickCallback callback, SelectedMaterialParam param);

	virtual bool init(MaterialClickCallback callback, SelectedMaterialParam param);

	void  updateMaterial(SelectedMaterialParam param);
	void  removeMaterial();

	SelectedMaterialParam* getParam() { return &m_param; };
	bool  isEmpty() { return m_param.empty(); };

	// 锁定;
	void  lock(bool block = true);

	// 淡入淡出;
	void  customFadeIn();
	void  customFadeOut();

private:

	void  initUI();

private:

	SelectedMaterialParam m_param;
	MaterialClickCallback _callback;

	ImageView*		m_imgFrame;		// 框;
	Button*			m_btnAdd;		// 添加符号;
	UIToolHeader*	m_Material;		// 材料图标;
	Button*			m_btnDelete;	// 删除;
};

#endif // MaterialHeader_h__
