#ifndef UIToolHeader_h__
#define UIToolHeader_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "UI/UIDef.h"
#include "ToolDef.h"
#include "ItemTipInfo.h"

//人物最高星级
#define MAX_STARS	6
//装备最高星级
#define MAX_EQUIPSTARS	4


enum EnumStarContain
{
	STAR_CONTAIN_TOP,
	STAR_CONTAIN_TOP_BESIDE,
	STAR_CONTAIN_BOTTOM,
	STAR_CONTAIN_BOTTOM_BESIDE,
};

class UIToolHeader: public cocos2d::ui::Button //ImageView 
{
public:

	//创建通用道具图片方法，包括碎片
	static UIToolHeader* create(int templateId);


	static UIToolHeader* create();	
	static UIToolHeader* create(const std::string& mainImage, const int id = -1);
	static UIToolHeader* create(const std::string& mainImage, const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");

//	static UIToolHeader* create(const int& nQua);

	static UIToolHeader* createEx(const std::string& selImage, const std::string& unselImage, const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");

	//
	virtual void onPressStateChangedToNormal() override;
	virtual void onPressStateChangedToPressed() override;
	virtual void onPressStateChangedToDisabled() override;
	//
	void initUI();
	void setIconQua(const int& nQua);
	void setMainIcon(const std::string& normalImage);
	void setMainIconVisible(bool isVisible);
	void setSelectedImg(const std::string& normalImage);
	void setSelected(const bool& isSelected);
	const bool& isSelected();
	//左上角的技能图标
	void setSkillIcon( const std::string& normalImage);
	//右下角使用图标
	void setUseIcon( const std::string& normalImage);
	void setUseText( const std::string& text);
	//右上角结缘图标
	void setTiedFateIcon( const std::string& normalImage);
	void setTiedFateIconFlag(bool isVisible);
	//右下角小红点图标
	void setRedIcon( const std::string& normalImage);
	void setRedIconFlag( bool flag);
	void setRedDot(bool flag);
	//设置血量 // Parameter: float fPercent，范围[0,100]
	void setBlood(const float& fPercent);
	//设置阵亡标记
	void setDiedFlag( bool flag);
	//遮罩图标
	void setMaskIcon( const std::string& normalImage);
	void setMaskFlag( bool isMask, bool isSelected=true);
	//设置碎片遮罩
	void checkChip(const int nItemId);
	//装备可进阶
	void setAdvFlag( bool flag);
	//设置装备星级
	void setEquipStars(int stars);
	//数量
	void setNumEx(int num, cocos2d::Color3B color = cocos2d::Color3B(0xff,0xff,0xff));
	const int getNum();
	//星级 isContain 是否放在头像内
	void setStarsNum(int num, EnumStarContain starContain=STAR_CONTAIN_BOTTOM_BESIDE);
	//设置英雄品质角标  传模板id，进阶等级，是否是机器人
	void setQuaCornerIcon(int heroTemplateId, int heroAdvLvl, bool isRobot = false);
	void setQuaCornerIcon(int heroAdvanceId, bool isRobot = false);
	//等级
	void setLvl(int nLvl);
	//设置不带圆圈背景的等级，新需求
	void setLvlWithoutBg(int nLvl);
	//进阶等级
	void setJJLvl(TOOL_ITEM_QUA nQua, int nLv);

	//设置道具所需数量与道具
	void setCurNumAndNeedNum(int curNum, int needNum);
	void setCurNumAndNeedNumColor(cocos2d::Color3B clr);
	//设置底部数字
	void setBottomNum(std::string strText);
	void setBottomNumColor(Color3B color);
	//设置右上角+号
	void setAddCornerIcon(bool bAdd = true);
	//重置状态
	void reset();

	void setGray(const bool isGray);

	void setScale(float fScale);
//protected:
	void setStateImg(const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");	

	void setToolId(const int& id);
	int getToolId();
	void setToolDisIndex(const int& index);
	int getToolDisIndex();
	//显示装备特效
	void showEquipEffe();
	void removeEquipEffe();

	//显示材料特效
	void showMatEffect(bool showEffect);

	// 显式设定此物是否需要tips;
	void setTipsEnabled(bool bEnabled);

	// 显示天命标记;
	void showTianMingFlag(bool bShow);

	// 淬炼等级;
	void setCLLevel(int nCLLevel);

	// 加锁;
	void lock(bool blocked);

	//特殊道具流光特效，手动调用
	void showLightEff();

	//添加个接口，外部重写触摸事件要求showTips立刻显示			added by Jerry 2016/4/19
	void showTipsNow(bool isShow, float pos);

	//如果是未拥有的英雄图标，需要显示初始英雄星级(pet_base表格星级)，同时不显示英雄数量
	void showPetBaseHeroStars();

private:
	UIToolHeader();	

	string getFilenameByQua(const int& nQua);

	// 显示tips;
	void    showTips(bool bShow);
	

	// 设置碎片遮罩;
	void setChipMaskIcon(TOOL_ITEM_TYPE itemType, TOOL_ITEM_QUA itemQua);

private:
	cocos2d::ui::ImageView* m_pMainIcon;
	//蒙版
	cocos2d::ui::ImageView* m_pFrontImgUnselected;
	cocos2d::ui::ImageView* m_pFrontImgSelected;
	//左上角的技能图标
	cocos2d::ui::ImageView	*m_pSkillIcon;
	//右下角使用图标
	cocos2d::ui::ImageView	*m_pUseIcon;
	cocos2d::ui::Text		*m_pUseText;
	//右下角小红点图标
	cocos2d::ui::ImageView	*m_pRedIcon;
	cocos2d::ui::ImageView	*m_pRedDot;
	//
	cocos2d::ui::ImageView	*m_pTiedFateIcon;
	//右下角进阶图标
	cocos2d::ui::ImageView	*m_pAdvIcon;
	//碎片遮罩图标
	cocos2d::ui::ImageView	*m_pChipMaskIcon;
	//遮罩图标 阵型中专用
	cocos2d::ui::ImageView	*m_pMaskIcon;
	cocos2d::ui::ImageView	*m_pMaskSelectIcon;
	//阵亡图标
	cocos2d::ui::ImageView	*m_pDiedIcon;
	//星级
	cocos2d::ui::ImageView *m_pStar[MAX_STARS];
	//右上角人物角标
	cocos2d::ui::ImageView *m_pHeroConerIcon;
	//装备星级
	cocos2d::ui::ImageView *m_pEquipStar[MAX_EQUIPSTARS];
	//名字
	cocos2d::ui::Text *m_pName;
	//血量
	cocos2d::ui::ImageView *m_pBloodBg;
	cocos2d::ui::LoadingBar *m_pBloodBar;
	//等级文字
	cocos2d::ui::ImageView	*m_pLvlIcon;
	cocos2d::ui::Text *m_pLvlText;
	//数量
	cocos2d::ui::Text *m_pNum;	
	int m_Num;
	cocos2d::ui::ImageView	*m_pNumExIcon;
	cocos2d::ui::Text *m_pNumEx;

	//道具当前数量与所需数量
	cocos2d::ui::Text* m_pCurNumAndNeedNum;
	//设置底部数字
	cocos2d::ui::Text* m_pTextBottomNum;

	//设置右上角+号
	cocos2d::ui::ImageView* m_pAddIcon;
	bool m_isSelected;

	float m_fScaleValue;

	//品质
	int		m_nIconQua;

	//道具id
	int m_ntoolId;
	//道具分格子后的显示位置
	int m_ntoolDisIndex;

	// 此物是否需要显示tips;
	bool  m_bEnableTips;

	// 提示框;
	ItemTipInfo*  m_pnlTips;

	// 天命标记;
	cocos2d::ui::ImageView* m_imgTianMing;

	// 锁;
	cocos2d::ui::ImageView* m_imgLock;

	// 进阶等级;
	cocos2d::ui::ImageView*	m_imgJJLv;
};

#endif
