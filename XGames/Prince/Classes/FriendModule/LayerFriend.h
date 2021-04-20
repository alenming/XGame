#ifndef LayerFriend_h__
#define LayerFriend_h__


#include "CocosGUI.h"

#include "FriendController.h"
#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"

class EditBoxEx;
class LayerLoading;



class LayerFriend: public ModalLayer,public Observer
{

public:
	LayerFriend();
	~LayerFriend();

	CREATE_FUNC(LayerFriend);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
public:	
	
protected:
	void	initUI();	

	void	initUIFriend();
	void    swichFriendState();

	//按钮事件
	void	onBtnNotify(Ref* sender, Widget::TouchEventType type);
	void    onCheckNotify(Ref* sender, CheckBox::EventType type);

	void	onDeleteFriendCallback(Ref* sender, CustomRetType type);
private:
	void	updateMyFriend(); //好友列表
	void    updataMyFriendListItemGive(int idx); //更新好友列表某个节点已经赠送
	void    updataMyFriendListItemGet(int idx); //更新好友列表某个节点已经领取

	void	updateAddFriend();
	void    updataAddFriendListItemGet(int idx); //更新添加好友列表某个节点已经领取
	void	updateSearchFriend();

	void	updateApplyFriend(); //申请列表
	void	updateApplyFriendItem(); //申请列表

private:
	// UI控件集合
	UI_Friend_Friend		m_ui;

	Friend_type m_Friend_type;
	Friend_add_type m_Friend_add_type;

	int m_friendIndx;

	//是否是第一次拉取好友协议
	bool mFirstGetFriendList;
};

#endif // 
