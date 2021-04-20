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

	// ����Ŀ��װ��templateIdId;
	void showSrc(const int& nToolTemplateId);

protected:
	void	initUI();
	
	// ��ť�¼�;
	void  onBtnClose(Ref* sender, Widget::TouchEventType type);

	//
	void  onBtnSrcItem(Ref* sender, Widget::TouchEventType type, const int& method, const int& chapterId=0, const int& barrierId=0 );	
	void  onScrollViewEvent(Ref* sender, ScrollView::EventType type);

	void clearAllItems();

	string getSrcImgName(const int dropMethodId);

private:
	// UI�ؼ�����
	UI_Smithy_Equip_Source  m_ui;

};

#endif // 
