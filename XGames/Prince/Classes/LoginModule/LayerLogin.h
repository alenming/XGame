#ifndef LayerLogin_h__
#define LayerLogin_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "LoginController.h"
#include "ILoginView.h"
#include "LayerIntro.h"
#include "Widget/LayerWaiting.h"
#include "UpgradeModule/LayerUpgrade.h"
#include "LoginDef.h"


class EditBoxEx;
class LayerLoading;
using namespace cocos2d;
struct UI_LOGIN
{
	// UI���ڵ������
	Layout*	pRoot;

	/********�������********/
	Layout* pLoginPnl;				//�˺ŵ�¼���
	Layout* pRegisterPnl;			//����ע�����
	Layout* pModifyPwdPnl;			//�޸��������
	Layout* pActivatePnl;			//���������˺����
	Layout* pLoginTipsPnl;			//��¼����ʾ���
	Layout* pLoginTipsTryPnl;		//�����˺ŵ�¼��ʾ���
	Button* BtnLoginSdk;			//���ɰ�׿sdkʱ��¼��ť


	/***********��¼���*************/
	TextField*	pInpLoginName;		//��¼�ӽ����ڣ��˺������
	EditBoxEx*	pEbxLoginName;
	TextField*	pInpLoginPwd;		//��¼�ӽ����ڣ����������
	EditBoxEx*	pEbxLoginPwd;

	Button* pBtnModifyPwd;			//�޸�����
	Button* pBtnRegister;			//����ע��
	Button* pBtnTryPlay;			//����
	Button* pBtnLogin;				//��¼

	/***********����ע�����*************/
	TextField	*pInpRegName;		//ע���ӽ����ڣ��˺������
	EditBoxEx	*pEbxRegName;
	TextField	*pInpRegPwd;		//ע���ӽ����ڣ����������
	EditBoxEx	*pEbxRegPwd;
	TextField	*pInpSurePwd;		//ע���ӽ����ڣ�ȷ�����������
	EditBoxEx	*pEbxSurePwd;

	CheckBox* pBtnShowPwd;			//��ʾ����
	CheckBox* pBtnShowSurePwd;		//��ʾȷ������
	Button* pBtnReg;				//ע�Ტ��¼���˺�
	Button* pBtnRegBack;			//����

	Text* pTextAgreement;			//��ϷЭ����

	/***********�޸��������*************/
	TextField	*pInpOldName;		//�����ӽ����ڣ��˺������
	EditBoxEx	*pEbxOldName;
	TextField	*pInpOldPwd;		//�����ӽ����ڣ�ԭ���������
	EditBoxEx	*pEbxOldPwd;
	TextField	*pInpNewPwd;		//�����ӽ����ڣ������������
	EditBoxEx	*pEbxNewPwd;
	TextField	*pInpSureNewPwd;	//�����ӽ����ڣ�������ȷ�Ͽ�
	EditBoxEx	*pEbxSureNewPwd;

	CheckBox* pBtnShowOldPwd;		//��ʾ������
	CheckBox* pBtnShowNewPwd;		//��ʾ������
	CheckBox* pBtnShowSureNewPwd;	//��ʾȷ������
	Button* pBtnSureModify;			//ȷ���޸�
	Button* pBtnModBack;			//����

	/***********�����˺����*************/
	TextField	*pInpActName;		//�����ӽ����ڣ��˺������
	EditBoxEx	*pEbxActName;
	TextField	*pInpActPwd;		//�����ӽ����ڣ����������
	EditBoxEx	*pEbxActPwd;

	CheckBox* pBtnShowActPwd;		//��ʾ����
	Button* pBtnActAccout;			//�����˺�
	Button* pBtnActBack;			//����

	Text* pTextSwitchAccout;		//�л��˺�

	/***********��¼��ʾ���*************/
	Text* pTextCntDown;				//����ʱ
	Text* pTextClick;				//�л��˺��ı�

	Text* pTextTryCntDown;			//�����¼����ʱ
	Text* pTextTryClick;			//�����˺��ı�


	// 8��8��;
	Text*  txt88;
	

	/*********************************�Ͻ���***************************************/
	/* 
	// UI���ڵ������
	cocos2d::ui::Layout	*pRoot;
	// ��Ϸ����ӽ��������
	cocos2d::ui::Layout	*pPnlEnter;
	cocos2d::ui::Button *BtnLoginSdk;

	// ����ӽ����ڣ�������Ϸ��ť
	cocos2d::ui::Button	*pBtnEnter;

	cocos2d::ui::ImageView	*pImgServer;
	cocos2d::ui::ImageView	*pImgReg;
	cocos2d::ui::ImageView	*pImgMod;


	// ��¼�ӽ����ڣ���¼�˺������
	cocos2d::ui::TextField	*pInpLoginName;
	EditBoxEx	*pEbxLoginName;
	// ��¼�ӽ����ڣ���¼���������
	cocos2d::ui::TextField	*pInpLoginPwd;
	EditBoxEx	*pEbxLoginPwd;

	// ��¼�ӽ����ڣ���¼��ť
	cocos2d::ui::Button	*pBtnLoginOK;
	// ��¼�ӽ����ڣ�ע�����û���ť
	cocos2d::ui::Button	*pBtnReg;
	// ��¼�ӽ����ڣ��޸����밴ť
	cocos2d::ui::Button	*pBtnMod;
	//ѡ��
	cocos2d::ui::TextField*	pInpServerName;
	EditBoxEx*				pEbxServerName;

	// 8��8��;
	Text*  txt88;                                                                     
	*/
	/*********************************�Ͻ���***************************************/

	UI_LOGIN()
	{
		memset(this, 0, sizeof(UI_LOGIN));
	}
};

typedef enum UI_BTN_FALG
{
	Btn_ModifyPwd,			//�޸�����
	Btn_Register,			//����ע��
	Btn_TryPlay,			//����
	Btn_Login,				//��¼
	Btn_Reg,				//ע�Ტ��¼���˺�
	Btn_RegBack,			//����
	Btn_SureModify,			//ȷ���޸�
	Btn_ModBack,			//����
	Btn_ActAccout,			//�����˺�
	Btn_ActBack,			//����
	Text_SwitchAccount,		//��¼ʱ����л��˺�
	Text_ActAccount,		//��¼ʱ��������˺�
	Text_Agreement,			//����Э����

}eUI_BTN_FALG;

class LayerLogin: public cocos2d::Layer, public ILoginView
{

public:
	
	LayerLogin();
	virtual ~LayerLogin();

	CREATE_FUNC(LayerLogin);

public:

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

	virtual void onRecvLoginData(int nResult, const char* szMsg);
	virtual void onRecvRegisterData(int nResult, const char* szMsg);
	virtual void onRecvModifyPwdData(int nResult, const char* szMsg);
	virtual void onRecvActivateData(int nResult, const char* szMsg);
	virtual void onRecvTryGameData(int nResult, const char* szMsg);

	virtual void onRecvCreateRoleData();

	// �������;
	void  onUpgradeDone(string strMsg = "");

	// sdk��½;
	void  loginSdk();

protected:

	void	initUI();

	//��ʼ���˺ŵ�¼���
	void    initLoginUI();
	//��ʼ������ע�����
	void    initRegiterUI();
	//��ʼ���޸��������
	void    initModifyPwdUI();
	//��ʼ�������˺����
	void    initActivateUI();
	//��ʼ����¼����ʾ���
	void    initLoginTipsUI();

	void	onBtnClick( Ref* ref, Widget::TouchEventType type, eUI_BTN_FALG nWidgetName );
	void	onCheckBoxShowPwd(Ref* ref, CheckBoxEventType type);

	//�����¼�¼�
	void    onEventLogin();
	void    onEventModifyPwd();
	void    onEventRegister();
	void    onEventTryGame();
	void    onEventActivate();

	//����ʱ
	void    downTime(float dt);

	//����ʺŸ�ʽ
	bool    checkNameFormat(const char* userName);
	//��������ʽ
	bool    checkPwdFormat(const char* password);

	void	loadLocalAccount();
	void	saveLocalAccount();
	void    saveTryGameLocalAccount();		//��¼�����ʺ�
	void    saveLastAccountIsTryGame(bool isTryGame);		//��¼������ҵ�½�Ƿ��������ʺ�

	// ������;
	void    checkUpdate();
	void	lbCheckUpdate();
	void    showUpgradeTips(bool bShow = true);

private:

	//UI�ؼ�����
	UI_LOGIN m_ui;

	//��ǰ��ʾ�Ľ���
	Layout* m_pnlCurShow;

	//��¼���л�����ʱ
	int m_iDownTime;

	//�û���
	std::string	m_strUserName;

	//����
	std::string	m_strPassword;

	//������(�޸�����ʱ�Ż��õ�)
	std::string m_strNewPwd;

	//�������uid
	int m_iTryGameUId;

	//���ε�½�Ƿ�ʹ�������ʺţ���¼������
	bool m_bIsLastLoginTryGame;

	// ��������ʾ;
	Text*  m_txtUpgradeTips;
};

#endif // LayerLogin_h__
