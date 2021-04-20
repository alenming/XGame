#ifndef LayerChatMessage_h__
#define LayerChatMessage_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "IChatView.h"
#include "ChatController.h"
//#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"

class EditBoxEx;
class LayerLoading;


class LayerChatMessage: public Layer, INoticeView
{

public:
	LayerChatMessage();
	~LayerChatMessage();

	CREATE_FUNC(LayerChatMessage);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	virtual void	onRecvNoticeMessage();
public:	
	//显示系统公告
	void showNotice();
	void callBackRollNotice(float dt);
protected:
	void	initUI();	

private:
	// UI控件集合
	UI_Friend_Chat_Message			m_ui;	

	ChatController* m_pChatController;

	std::string m_strNoticeContent;
	int		m_nNoticeContentStartPos;
	//是否自减一条数量
	bool    m_DelShowNotice;

	SYS_NOTICE* m_curSysNotice;
};

#endif // 
