#ifndef LayerChat_h__
#define LayerChat_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "IChatView.h"
#include "ChatController.h"
//#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"

//引入头文件和命名空间
#include "GUI/CCEditBox/CCEditBox.h"
using namespace cocos2d::ui;

class EditBoxDelegate;

class LayerChat: public Layer, public IChatView, public cocos2d::extension::EditBoxDelegate
{

public:
	LayerChat();
	~LayerChat();

	CREATE_FUNC(LayerChat);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	virtual void	onRecvData(int nResult, const char* data);
	virtual void	onRecvChatMessage();

	//重写输入框事件
	virtual void editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox);
	virtual void editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox);
	virtual void editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string &text);
	virtual void editBoxReturn(cocos2d::extension::EditBox *editBox);
protected:
	void	initUI();	
	void	initPopupUI();
	
	void	onBtnChatClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id, std::string name, int resId, int level);
	void	onBtnNotify(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void    onBtnClick(Ref* sender, CheckBox::EventType type);
	//销毁回调
	void    callBackDispose(float dt);
private:

	void updateChatContent(FRIEND_CHAT_TYPE chatType);
	void switchState(FRIEND_CHAT_TYPE chatType); //切换聊天状态

private:
	// UI控件集合
	UI_Friend_Chat			m_ui;
	UI_Friend_Chat_Popup	m_chatPopup;
	// 
	ChatController* m_pChatController;

	FRIEND_CHAT_TYPE m_chatType;

	int	     m_nFriendId;
	long     m_nlastSendTime;
};

#endif // 
