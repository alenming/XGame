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
#include "LoveHeader.h"
#include "LoveModule/LoveModel.h"
#include "LoveModule/LoveController.h"
#include "Temp/CustomGoldTips.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int ADDITION_COUNT = 4;

struct  UI_LOVE_ADD_ENTER
{
	Layout *pRoot;
	ImageView *Img_di;
	Button    *btnClose;
	Button    *btnReset;

	Text      *Lab_FriendShip;
	ImageView *Img_Position;
	Text      *Lab_Name;
	Text      *Lab_Tips;

	ImageView  *Img_Addition;
	Text       *Lab_ATK;
	Text       *Lab_ATKAdd;
	Text       *Lab_HPAdd;
	Text       *Lab_DEF1Add;
	Text       *Lab_DEF2Add;

	ImageView  *AdditionImg_Icon[ADDITION_COUNT];
	Text       *AdditionLab_AdditionName[ADDITION_COUNT];
	Text       *AdditionLab_Level[ADDITION_COUNT];
	Button     *AdditionBtn_Add[ADDITION_COUNT];
	Text       *AdditionLab_Cost[ADDITION_COUNT];
	ImageView  *AdditionImg_Max[ADDITION_COUNT];
};

enum ATTRIBUTE_TYPE
{
	ATTRIBUTE_TYPE_NONE,
	ATTRIBUTE_TYPE_ATK,
	ATTRIBUTE_TYPE_HP,
	ATTRIBUTE_TYPE_PDD,
	ATTRIBUTE_TYPE_MDD,
};

class LoveProAdd:public ModalLayer,public Observer
{
public:
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);


	~LoveProAdd(void);
	static LoveProAdd* create(int petId);

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi(int petId);  //初始化主界面
	void updataPanelInfo();
	void setFixedInfo();
	void setActiveInfo();
	void attributePiao();

	void onBtnNotify(Ref* sender, Widget::TouchEventType type); //按钮

private:
	LoveProAdd(void);

	UI_LOVE_ADD_ENTER m_ui;

	int m_petId;

	ATTRIBUTE_TYPE m_type; //哪个属性
};

#endif
