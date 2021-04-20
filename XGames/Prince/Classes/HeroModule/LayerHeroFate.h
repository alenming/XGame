#ifndef LayerHeroFate_h__
#define LayerHeroFate_h__

#include "UI/ModalLayer.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "HeroModel.h"
#include "HeroFateController.h"
#include "Utils/Observer.h"

//
const int MAX_HERO_FATE_COUNT = 8;
struct  UI_HeroFate
{
	// UI���ڵ������
	cocos2d::ui::Layout		*pRoot;

	cocos2d::ui::Layout		*pLayHeros;
	cocos2d::ui::Layout		*pLayTiedFates;

	//Ӣ��
	cocos2d::ui::ScrollView		*pSvHeros;
	//
	cocos2d::ui::Button			*pBtnLeft;
	cocos2d::ui::Button			*pBtnRight;
	//��Ե
	//Ե��
	cocos2d::ui::Layout			*pLayFates[MAX_HERO_FATE_COUNT];
	UIToolHeader				*pHeroFates[MAX_HERO_FATE_COUNT];
	//Ե��λ���Ƿ�Ϊ��
	bool						posFatesIsSpace[MAX_HERO_FATE_COUNT];
	//Ե��˵��
	cocos2d::ui::ImageView		*pImgDes[MAX_HERO_FATE_COUNT];
	//����
	cocos2d::ui::ImageView		*pImgMainRole;
	//��ʾ
	cocos2d::ui::Text			*pTextTips;
	//ˢ��
	cocos2d::ui::Text			*pTextRefresh;
	cocos2d::ui::Text			*pTextRefreshCost;

	//��ʶ��ˢ�¡��滻������ť��λ������(��ť�Ǿ��޶������Ұ�~)
	cocos2d::ui::Layout			*pBtnPnl1;
	cocos2d::ui::Layout			*pBtnPnl2;
	cocos2d::ui::Layout			*pBtnPnl3;

	//��ʶԵ�ְ�ť
	cocos2d::ui::Button			*pBtnTiedFates;
	//ˢ��Ե�ְ�ť
	cocos2d::ui::Button			*pBtnRefreshFates;
	//�滻Ե�ְ�ť
	cocos2d::ui::Button			*pBtnReplaceFates;
};

class LayerHeroFate : public ModalLayer, public Observer
{

public:

	LayerHeroFate();
	virtual ~LayerHeroFate();

	CREATE_FUNC(LayerHeroFate);

public:

	virtual void updateSelf(void* data);

	//��ʼ��
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	//���õ�ǰ��ʾӢ�۵�id
	void setCurrentHeroId(int heroId);

private:

	//��ʼ��UI
	void initUI();

	//��հ�ť����
	void clearBtnPanel();

	void onBtnBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onBtnLeft(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onBtnRight(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//
	void	onBtnCancel(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnTiedFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRemoveFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index);
	void	onBtnRefreshFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//�Ϸ�Ӣ��ͼ��
	void	onBtnHeroClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int heroId);
	//Ե��Ӣ��ͼ��
	void	onBtnFateClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index, int heroId);

	//������ť�¼�
	void	onBtnReplaceBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshRefresh(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshReplace(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onScrollViewEvent(Ref* sender, ScrollviewEventType type);

	void  onCustomRetRemoveFates(Ref* pSender, CustomRetType type);
	//��ʼ��Ӣ���б�
	void initHeroList();	
	//��ʾԵ�ֽ���
	void showFates();

	//�黹���ϼܵ�δ��Ե��Ӣ��ͼ�������Ź����б�
	void cancelSelectedHeroIcon();

	//��ʶԵ�ֽ������
	void tiedFatesResult(ObserverParam* obparam);
	//ˢ��Ե�ֽ������
	void refreshFatesResult(ObserverParam* obparam);
	//�滻Ե�ֽ������
	void replaceFatesResult(ObserverParam* obparam);
	//ȡ��Ե�ֽ������
	void cancelFatesResult();

	//��Ե�ɹ������ζ���
	void playTiedFatesHeartAni();

	//�ѽ�Ե�����ζ���
	void playTiedFatesLineAni(int aniTag);

	//�Ƴ��ѽ�Ե�����ζ���
	void removeTiedFatesLineAni(int index);

	//�����������ͻ�ȡ��������
	string getFateAttriName(const string& attriType);

	//��ȡ�ɽ�ʶԵ����:lvl ���ǵȼ�����VIP�ȼ�
	int getCanTiedFatesCount(int lvl, int sysId);

	//��ȡԵ�ֵ�һ����λ
	int getFatesSpacepos(int nomalCount, int vipCount);
	//Ӣ������ΪԵ��
	void startHeroToFates(int heroId);
	void callBackHeroToFates(float dt);

	//�жϵ�ǰѡ�е�Ե���Ƿ�ˢ�µ��˸��߼�����ͬʱ��û�滻�����
	bool checkNeedReplaceFate();

private:

	HeroFateController mHeroFateController;

	//��ɫ����UI��Ա
	UI_HeroFate m_ui;

	//�·�Ե��ѡ�е�Ӣ��ID
	int m_nCurHeroId;
	//��ǰѡ��λ��
	int m_nCurIndex;

	//
	int m_nHeroToFatesHeroId;

	//��ʾӢ�ۣ��м�3���Ŵ���ʾ
	int m_nDisHeroTotleCount;
	//��ʼ��ʾλ��
	int m_nDisHeroBeginIndex;
	//
	int m_nDisHeroBigBeginIndex;

	//��������
	int m_nMoveCount;
};


#endif