#ifndef LayerTakeCard_h__
#define LayerTakeCard_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "TakeCardController.h"
#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"
#include "CCArmature.h"
#include "ToolModule/ToolModel.h"
#include "TakeCardDef.h"

class EditBoxEx;
class LayerLoading;

const int GOLG_SYSTEM_ID = 10062;
const int COIN_SYSTEM_ID = 10063;

const int ONE_SPEND = 1;
const int TEN_SPEND = 10;

using namespace cocostudio;


class LayerTakeCard: public cocos2d::Layer, public Observer
{

public:
	LayerTakeCard();
	~LayerTakeCard();

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	static LayerTakeCard* create(bool pushScene);

	bool init(bool pushScene);
	virtual void onExit();
	virtual void onEnter();
	void updateNextFram(float ft);

	virtual void	onGetTakeCardInfo(const TAKE_CARD_INFO& cardInfos);
	virtual void	onTakeCardResult();
protected:
	void	initUI();
	void	initTakeCardUI();	

	void	onTakeCardClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index); //点击切换地煞正反面
	void	onBtnTakeCard1(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times); //银币抽卡
	void	onBtnTakeCard2(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times); //金币抽卡
	void	onBtnTakeCardView(Ref* sender, cocos2d::ui::Widget::TouchEventType type); //抽卡预览

	void    takeCardCoin(int times);
	void    takeCardGold(int times);

	//
	void	onBtnAgain(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times);
	void    onBtnTakeCardBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//

	void showResultCards(); //抽到卡显示翻卡
	void showResultCardsItem(cocos2d::ui::ImageView* pItem, const NEW_CARD_INFO& info,bool isChip = false);

	void setRewardText(const int& amount, const TakeCardType& index);
	//
	void setTakeCardInfo();
	
	//设置银币倒计时
	void setCoinCountDownTime();
	//设置金币倒计时
	void setGoldCountDownTime();

	Armature* creatTakeCardResultAnimation(const int& loopTimes);
	void startTakeCardResultAnimation(const int& loopTimes);
	void onResultAnimationCallFunc(Armature *armature, MovementEventType eventType, const std::string& strName);
	//
	//抽卡按钮旋转动画
	void animationCardRotation(int index);
	void animitionCardRotationCallFunND(cocos2d::Node* pNode, int pValue);

	//抽卡翻卡动画
	void animationShowCard(cocos2d::ui::ImageView	*pLayHeroCard, float fScale=1.0, int show=0);
	void animitionShowCardCallFunND(cocos2d::Node* pNode, int show);
	void animitionShowCardEndCallFunND(cocos2d::Node* pNode, int show);

	void animationShowCardContinue();
private:
	UI_Root		m_ui;  //商城界面控件集合

	TakeCardType		m_nTakeCardType;
	TakeCardType_What   m_mTakeCardTypeWhat;
	//倒计时
	bool    m_isFreeTime;
	bool    m_isFreeTime2;

	TakeCardState m_state;  //是翻牌主界面还是显示界面
	//
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

	bool    m_isPushScene;

	int		COIN_ONE_SPEND;
	int		COIN_TEN_SPEND;
	int		GOLD_ONE_SPEND;
	int		GOLD_TEN_SPEND;
};

#endif // 
