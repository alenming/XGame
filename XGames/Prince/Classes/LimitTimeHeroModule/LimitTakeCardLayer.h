#ifndef LayerTakeCard_h__
#define LayerTakeCard_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"
#include "CCArmature.h"
#include "ToolModule/ToolModel.h"


#include "LimitTimeHeroDataDef.h"
#include "LimitTimeHeroModel.h"
#include "LimitTimeHeroController.h"

USING_NS_CC;
using namespace cocostudio;
class EditBoxEx;
class LayerLoading;

const int GOLG_SYSTEM_ID = 10062;
const int COIN_SYSTEM_ID = 10063;

const int ONE_SPEND = 1;
const int TEN_SPEND = 10;

struct LimitTakeCard_UI
{
	Layout* ui_Root;
	//抽卡结果
	Layout		*Pnl_Card;//抽卡显示界面

	Button		*btn_kaiyici; //抽一次
	Button		*btn_kaishici; //抽十次

	ImageView	*imgNowFree;
	ImageView	*img_jinbi1; //抽一次的金币图标
	Text	*jinbiyici;  //抽一次价格
	ImageView	*img_jinbi2; //抽十次的金币图标
	Text	*jinbishici;// 抽十次价格
	ImageView	*Img_Tips; //提示

	ImageView	*imgCard; //单张
	ImageView	*VecimgCard[10]; //十张


};

class LimitTakeCardLayer: public Layer, public Observer
{

public:
	LimitTakeCardLayer();
	~LimitTakeCardLayer();

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	static LimitTakeCardLayer* create(eChouKaType type);

	bool init(eChouKaType type);
	virtual void onExit();
	virtual void onEnter();
	virtual void update(float dt);

protected:
	void initUI();

	void updatePublicInfo();
	void updateUI();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type,eChouKaType taketype);

	void startTakeCardResultAnimation(const int& loopTimes);
	Armature* creatTakeCardResultAnimation(const int& loopTimes);
	//动画播放完回调
	void onResultAnimationCallFunc(Armature *armature, MovementEventType eventType, const std::string& strName);
	void showResultCards();
	
	void showResultCardsItem(cocos2d::ui::ImageView* pItem, const sGetItemParam& info,bool isChip=false);
	void animationShowCard(cocos2d::ui::ImageView *pLayHeroCard, float fScale, int show);//翻卡动画

	void animitionShowCardCallFunND(cocos2d::Node* pNode, int show);
	void animitionShowCardEndCallFunND(cocos2d::Node* pNode, int show);
	void animationShowCardContinue();
private:

	bool _isFirstRun;
	bool firstChou;
	eChouKaType m_type;

	LimitTakeCard_UI	m_ui;
private:

	int			m_nTakeCardNum;
	bool        m_isChangeChip; //是否转变成碎片
	int         m_ChangChipCount; //转成碎片个数
	int         m_CurShowHeroId;  //当前需要显示英雄简介的英雄id

	bool		m_bTakeCardResultAnimationIsRun;
	Armature*	m_pTakeCardResultArmature;
	Animation*	m_pAnimation;
private:
	int		m_nTakeCardResultAnimationIndex;
	float	m_fTakeCardResultAnimationScale;
	//记录抽卡结果翻卡的时间戳
	int		m_nTakeCardResultAnimationTime;

	//LimitTimeHeroController m_Controller;
};



#endif // 
