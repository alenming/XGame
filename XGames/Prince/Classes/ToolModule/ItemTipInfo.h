#ifndef ItemTipInfo_h__
#define ItemTipInfo_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "FormationModule/CustomScrollBar.h"
#include "Common/Common.h"
#include "ui/UIDef.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "MainModule/MainModel.h"
#include "ui/ModalLayer.h"
#include "Widget/ActionCreator.h"
#include "Utils/SoundUtils.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class ItemTipInfo:public ModalLayer 
{
public:
	~ItemTipInfo(void);
	static ItemTipInfo* create(int toolId);

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi(int toolId);

	string  getItemTypeStr(int type);
	Color3B getItemQuaColor(int nQua);
	string  getPartTypeStr(int partType);

private:
	ItemTipInfo(void);

	int m_ntoolId;
};

#endif
