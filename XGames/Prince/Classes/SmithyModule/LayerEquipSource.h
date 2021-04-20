#ifndef LayerEquipSource_h__
#define LayerEquipSource_h__

#include "SmithyDef.h"
#include "UI/ModalLayer.h"

class LayerEquipSource: public ModalLayer
{

public:
	LayerEquipSource();
	~LayerEquipSource();
	CREATE_FUNC(LayerEquipSource);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

public:

	// 设置目标装备templateIdId;
	void showSrc(const int& nToolTemplateId);

protected:
	void	initUI();
	
	// 按钮事件;
	void  onBtnClose(Ref* sender, Widget::TouchEventType type);

	//
	void  onBtnSrcItem(Ref* sender, Widget::TouchEventType type, const int& method, const int& chapterId=0, const int& barrierId=0 );	
	void  onScrollViewEvent(Ref* sender, ScrollView::EventType type);

	void clearAllItems();

	string getSrcImgName(const int dropMethodId);

private:
	// UI控件集合
	UI_Smithy_Equip_Source  m_ui;

};

#endif // 
