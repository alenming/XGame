#ifndef ArtifactLayer_h__
#define ArtifactLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "ArtifactController.h"
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ArtifactController.h"
#include "ArtifactDataDef.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


//成员列表UI
typedef struct tag_UIArtifact
{
	Layout* pRoot;						//根节点

	ImageView*	imgNameBg;				
	Layout*	PnlBorn;
	vector<Layout*> vecPnlJewel;

	Layout* PnlArtifactList;
	vector<Layout*> vecPnlArtifact;

	ImageView* imgConditionBg;
	Layout*	PnlDetails;
	Button* btnActive;
	vector<Layout*> vecPnlCondition;

	ImageView* imgFightAttriBg;
	Button*  btnCommon;
	Button*  btnHigh;
	ImageView* imgCoin;
	Text*  txtCostStore;
	Text*  txtCostMoney;
	Layout* pnlFightAttri;
	vector<Layout*> vecPnlAttri;

	Button*	btnRefineOne;
	Button*	btnRefineTen;
	Button* btnCancel;
	Button* btnSave;

	ImageView* imgRefineAll;

}sTag_UIArtifact;

typedef enum ArtifactOrder
{
	eORDER_DEFAULT,
	eORDER_FIRST,
	eORDER_SECOND,
	eORDER_THIRD,
	eORDER_FOURTH,
	eORDER_FIFTH

}eArtifactOrder;

//按钮标记
typedef enum UI_BtnFlag
{
	BTN_ACTIVE,
	BTN_REFINEONE,
	BTN_REFINETEN,
	BTN_CANCEL,
	BTN_SAVE,
	BTN_NORMAL,
	BTN_ADVANCE,
	BTN_FIRST_ARTIFACT,
	BTN_SECOND_ARTIFACT,
	BTN_THIRD_ARTIFACT,
	BTN_FOURTH_ARTIFACT,
	BTN_FIFTH_ARTIFACT
	
}eUI_BtnFlag;

class ArtifactLayer : public Layer, public Observer
{
public:
	ArtifactLayer();
	~ArtifactLayer(void);

	CREATE_FUNC(ArtifactLayer);

public:
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

private:
	void initUI();
	void updateUI();

	//面板切入动画
	void playEnterAni();

	//刷新激活状态
	void updateActiveStates();
	//刷新洗练状态
	void updateRefineStates();

	//刷新洗练数据
	void updateRefineData();

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//统一处理神器动画
	void doArtifactAni();

	//播放神器公用动画底图
	void playCommonBGAni();

	//播放神器各自的动画特效图
	void playArtifactAni();

	void updateListState();

	//检测当前神器是否可以激活
	bool isCanActive();

	//检测是否满足洗练条件
	bool isCanRefine();

	//设置洗练和保存的按钮显示
	void setBtnShowState();

	//洗练消耗
	void setRefineCost();

	virtual void updateSelf(void* data);

private:
	sTag_UIArtifact m_ui;

	//当前神器的序列号
	eArtifactOrder mCurArtifact;

	//当前洗练的类型 普通、高级
	eRefineMethod mCurRefineMethod;

	ArtifactController m_pArtifactController;

	//是否需要取消洗练的临时值
	bool m_bIsNeedResetRefine;

	//是否是保存洗练属性值
	bool m_bIsSaveData;
};


#endif