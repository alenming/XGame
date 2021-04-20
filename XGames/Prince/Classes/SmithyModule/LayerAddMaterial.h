/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerAddMaterial
 *  Description:	添加材料页面封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			03/02/2016
 *  Others: 
 *
 ******************************************************************/

#ifndef LayerAddMaterial_h__
#define LayerAddMaterial_h__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include <string>
#include "MaterialNode.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::ui;
using namespace cocostudio;

typedef struct tagUI_Add_Material
{
	Layout*		pRoot;
	ImageView*	imgBg;
	Text*		txtGetExp;
	Text*		txtNeedExp;
	Button*		btnClose;
	Button*		btnOK;
	extension::TableView*	tableView;

	tagUI_Add_Material()
	{
		memset(this, 0, sizeof(tagUI_Add_Material));
	}
}UI_Add_Material;

typedef std::function<void(vector<SelectedMaterialParam>, bool)> AddMaterialCallback;

class LayerAddMaterial : public Layer, public extension::TableViewDelegate, public extension::TableViewDataSource
{
public:
	LayerAddMaterial(void);
	~LayerAddMaterial(void);

	static LayerAddMaterial* create(int nToolId, AddMaterialCallback callback, vector<SelectedMaterialParam> vcSelectedMaterials);

	virtual bool  init(int nToolId, AddMaterialCallback callback, vector<SelectedMaterialParam> vcSelectedMaterials);

	bool onMaterialSelected(SelectedMaterialParam smp, MaterialNodeSelectEvent event);

	virtual void  tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual extension::TableViewCell*  tableCellAtIndex(extension::TableView *table, ssize_t idx);
    virtual ssize_t  numberOfCellsInTableView(extension::TableView *table);
	virtual Size cellSizeForTable(extension::TableView *table);

private:

	void  initUI();

	void  updateGetExp();

private:

	AddMaterialCallback  _callback;

	UI_Add_Material m_ui;

	int m_nEquToolId;
	vector<SelectedMaterialParam> m_vcSelectedMaterials;
	vector<SelectedMaterialParam> m_vcAllMaterials;

	int m_nSelectedCount;

	Size m_MaterialCellSize;
	Node* m_nodeParent;
};
#endif // LayerAddMaterial_h__

