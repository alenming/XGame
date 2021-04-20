#ifndef LimitTimeHeroLayer_h__
#define LimitTimeHeroLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "LimitTimeHeroController.h"
#include "LimitTimeHeroDataDef.h"
#include "LimitTimeHeroModel.h"
#include "LimitTimeHeroController.h"
#include "HeroModule/HeroModel.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//��ʱ��UI�ṹ
typedef struct tag_UILimitTimeHero
{
	Layout* pRoot;						//���ڵ�

	//Ӣ��չʾ�ڵ�
	ImageView*	imgHeroNick;
	Text*		txtHeroName;
	Text*		txtTenReward;
	Text*		txtOneBuy;
	Text*		txtTenBuy;
	//TextAtlas*	txtHeroTalent;
	ImageView*	imgHeroTalent;
	Layout*		pnlHeroAni;
	Button*		btnHelp;
	Button*		btnOneBuy;
	Button*		btnTenBuy;

	//��������չʾ�ڵ�
	ui::ScrollView*	scrollScoreRank;
	ui::ScrollView*	scrollRewardRank;
	Layout*		pnlScoreItemTemp;
	Layout*		pnlRewardItemTemp;

	//���ֽ��Ƚڵ�
	LoadingBar*	barScore;
	vector<Button*> vecScoreBox;

	//����
	Text*		txtMyScore;
	Text*		txtMyRank;
	Text*		txtGold;
	Text*		txtResDays;		//�ʣ������
	Text*		txtResTime;		//�ʣ��ʱ����
	Button*		btnClose;

}sTag_UILimitTimeHero;

//��ť���
typedef enum UI_Btn
{
	BTN_HELP,
	BTN_BUYONE,
	BTN_BUYTEN,
	BTN_SCOREBOX,
	BTN_CLOSE

}eUI_Btn;

class LimitTimeHeroLayer : public Layer, public Observer, public TimeLocalListener
{
public:
	LimitTimeHeroLayer();
	virtual ~LimitTimeHeroLayer();

	CREATE_FUNC(LimitTimeHeroLayer);

public:
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

private:
	void initUI();
	void updateUI();

	//����ʱ
	void showDownTime();

	//Ӣ��չʾ
	void showHeroAni();

	//����������Ϣ
	void upateRankRewardView();

	//���»����������ҵĻ�����Ϣ
	void updateScoreRankState();

	//ˢ�»��ֱ���״̬
	void updateRewardBoxState();

	void onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//չʾ����ȡ���ֱ���
	void onBoxClicked(Button* btn); 

	//���佱����ȡ��չʾ
	void showBoxRewardGetted();

	//�鿨����ϻ��Ϣ��Ӧ
	void onTakeCardBoxEvent();

	//��ӳ���߲�
	void addBuyLayerByType(eChouKaType type);

	//�ʱ���ʽ�����졢ʱ���֡���
	void formatDetailsTime(int secs, sFormatTime* strTime);

	//�������¼�����
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//�������ȡ״̬����
	void showCanGetAni(Node* parentNode);
	//�Ƴ�����ȡ״̬����
	void removeCanGetAni(Node* parentNode);

	//��ʱ��.
	void openUITimer();
	void timeTick(int dt);

	virtual void updateSelf(void* data);

private:
	sTag_UILimitTimeHero m_ui;

	//��������������
	CustomScrollBar* m_scoreBar;

	//��������������
	CustomScrollBar* m_rewardBar;

	//����Ӣ��
	HeroParam* m_heroParam;

	LimitTimeHeroController m_Controller;

	//��ǰ��ȡ�ı���ID
	int m_iBoxId;
};


#endif