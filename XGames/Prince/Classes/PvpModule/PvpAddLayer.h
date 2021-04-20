#ifndef LoveProAdd_h__
#define LoveProAdd_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Common/Common.h"
#include "ui/UIDef.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "MainModule/MainModel.h"
#include "ui/ModalLayer.h"
#include "PvpHeader.h"
#include "Temp/CustomGoldTips.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int ADDITION_COUNT = 4;

struct  UI_PVP_ADD_ENTER
{
	Layout *pRoot;
	ImageView *Img_di;
	Text      *Label_wenben1;
	Text      *Label_wenben2;
	Text      *Label_wenben3;
	Text      *Label_jinbi;
	Text      *addCount;

	Button    *Button_jianyi;
	Button    *Button_jiayi;
	Button    *Button_quxiao;
	Button    *Button_queren;
};

class PvpAddLayer:public ModalLayer
{
public:
	~PvpAddLayer(void);
	static PvpAddLayer* create();

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();  //初始化主界面

	void onBtnNotify(Ref* sender, Widget::TouchEventType type); //按钮

	void updataSpend();
private:
	PvpAddLayer(void);

	UI_PVP_ADD_ENTER m_ui;

	int  m_count; //购买次数
	int  m_spend; //购买花费

	vector<int> m_VecSpend; //购买花费价格列表
};

#endif
