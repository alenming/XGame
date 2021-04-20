#ifndef UIToolRob_h__
#define UIToolRob_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "UI/UIDef.h"

class UIToolRob: public cocos2d::ui::Button //ImageView 
{
public:
	static UIToolRob* create();	
	static UIToolRob* create(const std::string& mainImage);	
	//
	void initUI();
	void setMainIcon(const std::string& normalImage);
	
	void setLableIcon(const int& index);
	void setLableIcon(const std::string& normalImage);
	//名称
	void setTitle( const std::string& name);
	//数量
	void setNumEx(int num);
	void setNum(int num);
	const int getNum();
	//星级 isContain 是否放在头像内
	void setStarsNum(int num, bool isContain=false);
	//等级
	void setLvl(int nLvl);
	//进阶等级
	void setAdvLvl(int nLvl);
	
	//重置状态
	void reset();

	void setGray(const bool isGray);


private:
	UIToolRob();	

private:
	cocos2d::ui::ImageView* m_pMainIcon;
	//蒙版
	cocos2d::ui::ImageView* m_pFrontImgUnselected;
	cocos2d::ui::ImageView* m_pFrontImgSelected;
	
	//星级
	cocos2d::ui::ImageView *m_pStar[5];
	//名字
	cocos2d::ui::Text *m_pName;

	cocos2d::ui::ImageView	*m_pLableIcon;
	//等级文字
	cocos2d::ui::ImageView	*m_pLvlIcon;
	cocos2d::ui::Text *m_pLvlText;
	//进阶等级文字
	cocos2d::ui::Text *m_pAdvLvl;
	//数量
	cocos2d::ui::Text *m_pNum;	
	int m_Num;
	cocos2d::ui::ImageView	*m_pNumExIcon;
	cocos2d::ui::Text *m_pNumEx;	

	float m_fScaleValue;
};

#endif
