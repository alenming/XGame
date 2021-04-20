#ifndef LayoutRoleSel_h__
#define LayoutRoleSel_h__

#include "UI/UIDef.h"
#include "Widget/EditBoxEx.h"

struct UI_ROLE_SEL
{
	cocos2d::ui::Layout       *pRoot;

	cocos2d::ui::PageView		*pPgvRole;

	cocos2d::ui::CheckBox		*pChkMale;
	cocos2d::ui::CheckBox		*pChkFemale;
	cocos2d::ui::Button		*pBtnRoleMale;
	cocos2d::ui::Button		*pBtnRoleFemale;
	cocos2d::ui::Button		*pBtnMale;
	cocos2d::ui::Button		*pBtnFemale;
	cocos2d::ui::Button		*pBtnOK;


	UI_ROLE_SEL()
	{
		memset(this, 0, sizeof(UI_ROLE_SEL));
	}
};

typedef enum
{
	ROLE_MALE = 0,
	ROLE_FEMALE,
	ROLE_TYPE_TOTAL
}ROLE_TYPE;

class LayerRoleSelect: public cocos2d::Layer
{
public:
	LayerRoleSelect();
	~LayerRoleSelect();
	CREATE_FUNC(LayerRoleSelect);

	virtual bool	init();
	virtual void	onEnter();
	virtual void	onExit();

protected:
	void			initUI();

	void			onPageRole(Ref* pSender, cocos2d::ui::PageView::EventType type);
	void			onBtnRole(Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
	void			onSelectRole(Ref* pSender, cocos2d::ui::CheckBox::EventType type);

	void			selectRole(int nType);
	void			selectRoleMale();
	void			selectRoleFemale();
	UI_ROLE_SEL		m_ui;

private:
};

#endif // LayoutRoleSel_h__
