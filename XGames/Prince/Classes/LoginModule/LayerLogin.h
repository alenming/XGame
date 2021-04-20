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
	// UI根节点层容器
	Layout*	pRoot;

	/********六大面板********/
	Layout* pLoginPnl;				//账号登录面板
	Layout* pRegisterPnl;			//快速注册面板
	Layout* pModifyPwdPnl;			//修改密码面板
	Layout* pActivatePnl;			//激活试玩账号面板
	Layout* pLoginTipsPnl;			//登录中提示面板
	Layout* pLoginTipsTryPnl;		//试玩账号登录提示面板
	Button* BtnLoginSdk;			//集成安卓sdk时登录按钮


	/***********登录面板*************/
	TextField*	pInpLoginName;		//登录子界面内，账号输入框
	EditBoxEx*	pEbxLoginName;
	TextField*	pInpLoginPwd;		//登录子界面内，密码输入框
	EditBoxEx*	pEbxLoginPwd;

	Button* pBtnModifyPwd;			//修改密码
	Button* pBtnRegister;			//快速注册
	Button* pBtnTryPlay;			//试玩
	Button* pBtnLogin;				//登录

	/***********快速注册面板*************/
	TextField	*pInpRegName;		//注册子界面内，账号输入框
	EditBoxEx	*pEbxRegName;
	TextField	*pInpRegPwd;		//注册子界面内，密码输入框
	EditBoxEx	*pEbxRegPwd;
	TextField	*pInpSurePwd;		//注册子界面内，确认密码输入框
	EditBoxEx	*pEbxSurePwd;

	CheckBox* pBtnShowPwd;			//显示密码
	CheckBox* pBtnShowSurePwd;		//显示确认密码
	Button* pBtnReg;				//注册并登录新账号
	Button* pBtnRegBack;			//返回

	Text* pTextAgreement;			//游戏协议书

	/***********修改密码面板*************/
	TextField	*pInpOldName;		//改密子界面内，账号输入框
	EditBoxEx	*pEbxOldName;
	TextField	*pInpOldPwd;		//改密子界面内，原密码输入框
	EditBoxEx	*pEbxOldPwd;
	TextField	*pInpNewPwd;		//改密子界面内，新密码输入框
	EditBoxEx	*pEbxNewPwd;
	TextField	*pInpSureNewPwd;	//改密子界面内，新密码确认框
	EditBoxEx	*pEbxSureNewPwd;

	CheckBox* pBtnShowOldPwd;		//显示旧密码
	CheckBox* pBtnShowNewPwd;		//显示新密码
	CheckBox* pBtnShowSureNewPwd;	//显示确认密码
	Button* pBtnSureModify;			//确认修改
	Button* pBtnModBack;			//返回

	/***********激活账号面板*************/
	TextField	*pInpActName;		//激活子界面内，账号输入框
	EditBoxEx	*pEbxActName;
	TextField	*pInpActPwd;		//激活子界面内，密码输入框
	EditBoxEx	*pEbxActPwd;

	CheckBox* pBtnShowActPwd;		//显示密码
	Button* pBtnActAccout;			//激活账号
	Button* pBtnActBack;			//返回

	Text* pTextSwitchAccout;		//切换账号

	/***********登录提示面板*************/
	Text* pTextCntDown;				//倒计时
	Text* pTextClick;				//切换账号文本

	Text* pTextTryCntDown;			//试玩登录倒计时
	Text* pTextTryClick;			//激活账号文本


	// 8荣8耻;
	Text*  txt88;
	

	/*********************************老界面***************************************/
	/* 
	// UI根节点层容器
	cocos2d::ui::Layout	*pRoot;
	// 游戏入口子界面的容器
	cocos2d::ui::Layout	*pPnlEnter;
	cocos2d::ui::Button *BtnLoginSdk;

	// 入口子界面内，进入游戏按钮
	cocos2d::ui::Button	*pBtnEnter;

	cocos2d::ui::ImageView	*pImgServer;
	cocos2d::ui::ImageView	*pImgReg;
	cocos2d::ui::ImageView	*pImgMod;


	// 登录子界面内，登录账号输入框
	cocos2d::ui::TextField	*pInpLoginName;
	EditBoxEx	*pEbxLoginName;
	// 登录子界面内，登录密码输入框
	cocos2d::ui::TextField	*pInpLoginPwd;
	EditBoxEx	*pEbxLoginPwd;

	// 登录子界面内，登录按钮
	cocos2d::ui::Button	*pBtnLoginOK;
	// 登录子界面内，注册新用户按钮
	cocos2d::ui::Button	*pBtnReg;
	// 登录子界面内，修改密码按钮
	cocos2d::ui::Button	*pBtnMod;
	//选服
	cocos2d::ui::TextField*	pInpServerName;
	EditBoxEx*				pEbxServerName;

	// 8荣8耻;
	Text*  txt88;                                                                     
	*/
	/*********************************老界面***************************************/

	UI_LOGIN()
	{
		memset(this, 0, sizeof(UI_LOGIN));
	}
};

typedef enum UI_BTN_FALG
{
	Btn_ModifyPwd,			//修改密码
	Btn_Register,			//快速注册
	Btn_TryPlay,			//试玩
	Btn_Login,				//登录
	Btn_Reg,				//注册并登录新账号
	Btn_RegBack,			//返回
	Btn_SureModify,			//确认修改
	Btn_ModBack,			//返回
	Btn_ActAccout,			//激活账号
	Btn_ActBack,			//返回
	Text_SwitchAccount,		//登录时点击切换账号
	Text_ActAccount,		//登录时点击激活账号
	Text_Agreement,			//免责协议书

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

	// 更新完毕;
	void  onUpgradeDone(string strMsg = "");

	// sdk登陆;
	void  loginSdk();

protected:

	void	initUI();

	//初始化账号登录面板
	void    initLoginUI();
	//初始化快速注册面板
	void    initRegiterUI();
	//初始化修改密码面板
	void    initModifyPwdUI();
	//初始化激活账号面板
	void    initActivateUI();
	//初始化登录中提示面板
	void    initLoginTipsUI();

	void	onBtnClick( Ref* ref, Widget::TouchEventType type, eUI_BTN_FALG nWidgetName );
	void	onCheckBoxShowPwd(Ref* ref, CheckBoxEventType type);

	//处理登录事件
	void    onEventLogin();
	void    onEventModifyPwd();
	void    onEventRegister();
	void    onEventTryGame();
	void    onEventActivate();

	//倒计时
	void    downTime(float dt);

	//检测帐号格式
	bool    checkNameFormat(const char* userName);
	//检测密码格式
	bool    checkPwdFormat(const char* password);

	void	loadLocalAccount();
	void	saveLocalAccount();
	void    saveTryGameLocalAccount();		//记录试玩帐号
	void    saveLastAccountIsTryGame(bool isTryGame);		//记录本次玩家登陆是否是试玩帐号

	// 检查更新;
	void    checkUpdate();
	void	lbCheckUpdate();
	void    showUpgradeTips(bool bShow = true);

private:

	//UI控件集合
	UI_LOGIN m_ui;

	//当前显示的界面
	Layout* m_pnlCurShow;

	//登录中切换倒计时
	int m_iDownTime;

	//用户名
	std::string	m_strUserName;

	//密码
	std::string	m_strPassword;

	//新密码(修改密码时才会用到)
	std::string m_strNewPwd;

	//试玩玩家uid
	int m_iTryGameUId;

	//本次登陆是否使用试玩帐号，记录到本地
	bool m_bIsLastLoginTryGame;

	// 检查更新提示;
	Text*  m_txtUpgradeTips;
};

#endif // LayerLogin_h__
