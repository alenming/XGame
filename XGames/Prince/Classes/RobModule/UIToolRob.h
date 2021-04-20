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
	//����
	void setTitle( const std::string& name);
	//����
	void setNumEx(int num);
	void setNum(int num);
	const int getNum();
	//�Ǽ� isContain �Ƿ����ͷ����
	void setStarsNum(int num, bool isContain=false);
	//�ȼ�
	void setLvl(int nLvl);
	//���׵ȼ�
	void setAdvLvl(int nLvl);
	
	//����״̬
	void reset();

	void setGray(const bool isGray);


private:
	UIToolRob();	

private:
	cocos2d::ui::ImageView* m_pMainIcon;
	//�ɰ�
	cocos2d::ui::ImageView* m_pFrontImgUnselected;
	cocos2d::ui::ImageView* m_pFrontImgSelected;
	
	//�Ǽ�
	cocos2d::ui::ImageView *m_pStar[5];
	//����
	cocos2d::ui::Text *m_pName;

	cocos2d::ui::ImageView	*m_pLableIcon;
	//�ȼ�����
	cocos2d::ui::ImageView	*m_pLvlIcon;
	cocos2d::ui::Text *m_pLvlText;
	//���׵ȼ�����
	cocos2d::ui::Text *m_pAdvLvl;
	//����
	cocos2d::ui::Text *m_pNum;	
	int m_Num;
	cocos2d::ui::ImageView	*m_pNumExIcon;
	cocos2d::ui::Text *m_pNumEx;	

	float m_fScaleValue;
};

#endif
