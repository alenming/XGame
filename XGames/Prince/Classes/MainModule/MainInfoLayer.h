#ifndef MainInfoLayer_h__
#define MainInfoLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "Utils/Observer.h"
#include "MazeModule/LayerMazeLimitBuyItem.h"
#include "MazeModule/LayerMazeLimitBuy.h"
#include "FriendModule/FriendDef.h"
#include "SignModule/LayerSignController.h"
#include "MailModule/MailModel.h"
#include "LimitTimeHeroModule/LimitTimeHeroController.h"

USING_NS_CC;
using namespace ui;

typedef struct tagUI_MAIN_INFO
{
	//���ڵ�
	Layout* root;
	//���ϽǸ����
	ImageView* roleImg;
	Point roleImgPos;
	//�ȼ�
	TextAtlas* levelAtl;
	//vip�ȼ�
	Button* vipBtn;
	//����
	Text* nameText;
	//����ͷ��
	Button* playerInfoBtn;
	//ս��
	Text* fightingText;
	//������
	ProgressTimer* expBar;

	//��ǰ����/�������
	Text* curPowText;
	//��ǰ����/�����
	Text* curEnergyText;
	//��������
	Text* silverText;
	//�������
	Text* goldText;

	//�Ҳఴť
	ImageView* leftBackImg;
	Point leftBackImgPos;
	Button* formationBtn;
	Button* packBtn;
	Button* heroBtn;
	Button* taskBtn;
	Button* rankTopBtn;
	Button* dequeBtn; //����
	ImageView *jiantou;
	Vec2     jiantouPos;
	Layout* dequeDi; //�����
	Vec2     dequeDiPos;
	Button* zhenFaBtn; //��
	Button* shenQiBtn; //����
	Button* starPanleBtn; //����




	//�Ϸ���ť
	Button* signBtn;		//ǩ��
	Button* mailBtn;		//�ʼ�
	Button* depositBtn;		//��������
	Button* openBtn;		//���쿪������
	Button* firstRechargeBtn;	//�׳�
	Button* chongzhi;
	Button* limitTimeHeroBtn;	//��ʱ��
	Button* tianjiangjinbi; //�콵���
	Button* steryShop;		//�����̵�
	Text*	steryShopTiem;

	//������ť
	Button* friendBtn;
	Button* setBtn;
	Button* chatBtn;

	//Button* addGoldBtn;

	//���½ǹ��ܿ�����ʾ��ť
	ImageView* funcOpenImg;
	Button* funcOpenBtn;
	Text*	openTipsText;
	Layout* tipsDetailsPnl;

}UIMainInfo;

//�ȼ����ű�����
typedef struct openFuncData
{
	int id;
	int level;
	string iconName;
	string resPath;
	string strDes;

	openFuncData()
		: id(0), level(0), iconName(""), resPath(""), strDes("") {}
}sOpenFuncData;

enum class MAIN_BTN_TAG
{
	FORMATION,		//����
	PACK,			//����
	HERO,			//Ӣ��
	STARSU,			//����
	TASK,			//����
	DEQUE,          //����
	ZHENFA,          //��
	SHENQI,          //����
	STARTPAN,        //����

	BOSS,			//����Boss
	REGISTER,		//ǩ��
	VIP,			//��ֵ
	OPERATE,        //��Ӫ�
	FIRSTRECHARGE,	//�׳�
	LIMITTIMEHERO,	//��ʱ��
	TIANJIANGJINBI,	//�콵���
	STERYSHOP,		//�����̵�

	MAIL,			//�ʼ�
	FRIEND,			//����
	SETGAME,        //����
	CHAT,           //����  

	ROB,			//�Ӷ�
	MAZE,			//�þ�
	PLAYERINFO,		//�����Ϣ
	OPEN,			//���쿪������
	RANKTOP,		//���а�
	ADDGOLD,		//�ӽ��
	ADDSILVER,		//������
	ADDPOWER,		//������
	ADDENERGY		//�Ӿ���
};

//��ʱ����ͣ����������½���չʾ��
enum class ACTIVE_TYPE
{
	ACTIVE_DEFAULT,	//û�лʱ��ʾ��һ�����ŵĹ���ͼ��
	ACTIVE_BOSS,	//boss�
};


class MainInfoLayer : public Layer, public Observer
{

public:

	MainInfoLayer();
	virtual ~MainInfoLayer();

	CREATE_FUNC(MainInfoLayer);

public:

	virtual bool init() override;

	virtual void onEnter() override;

	virtual void onExit() override;

	virtual void updateSelf(void* data) override;

	//��������
	void setUserInfo();

	//���ý�ɫ�ɽ��ױ�־
	void setHeroAdvFlag();

	//�����������ȡ���
	void setTaskFlag();

	//����ǩ�����
	void setSignFlag();

	//����ǩ�����
	void setMailFlag();

	//�����콵��ұ��
	void setGoldActiviryFlag();

	//������ʱ��С���
	void setLimitTimeHeroFlag();

	//����������
	void setChatFlag();

	//���ú��ѱ��
	void setFriendFlag(bool first);

	//���ÿ������콱������
	void setActivityFlag();

	//������Ӫ����
	void setOperateFlag();

	//���������¿������
	void setNewStartCampFlage();

	//����vipͼ��С���
	void setVipFlag();

	//��ť��Ӧ
	virtual void onBtnClick(Ref* ref, cocos2d::ui::Widget::TouchEventType type, MAIN_BTN_TAG btnFlag);

	//��ת��ָ������
	void jumpToScene(MAIN_BTN_TAG btnFlag);

	//�������������
	void addChatMsgItem();

	//������һ��Ҫ���ŵĹ��ܺ���ʱ�״̬
	void setFuncState();
	void setNextFuncOpen();
	void showActiveAni(bool isActive);
	void setActiveState(ACTIVE_TYPE type);
	void showFuncOpenTips();

	void allActivityButton(); //�������л��ť
	void setActivityButtonVisible( Button * sender, bool visible); //�������л��ť�Ƿ���ʾ
	void ShowActivityButton(); //���л��ť��ʾ

	Sprite* getBlurScreenShot();
private:

	UIMainInfo mUI;

	bool isFirst;// �Ƿ��һ����ȡ���������б�

	bool isDuiWuOpen; //����ť�Ƿ��
	bool isDuiWuCanRun; //����ť�˶��Ƿ����

	//��¼�Ĺ��ܿ�������
	sOpenFuncData m_sData;

	LimitTimeHeroController m_pController;

	vector<Vec2> m_allActivityPos; //���л��ť��ʼλ��
	struct SHOWE_ACT_NODE
	{
		Button* node;
		bool visible;
	};
	vector<SHOWE_ACT_NODE> m_allActivityShowBtn; //���л��ť��ʼλ��
};


#endif //MainInfoLayer