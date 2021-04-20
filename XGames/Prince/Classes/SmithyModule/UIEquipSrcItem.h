#ifndef UIEquipSrcItem_h__
#define UIEquipSrcItem_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"

#include "UIEquipSrcItem.h"

const int MAX_SRC_NORMAL_ITEM_WIDTH = 458;
const int MAX_SRC_NORMAL_ITEM_HEIGHT = 134;


class UIEquipSrcItem: public cocos2d::ui::Button
{
public:
		
	static UIEquipSrcItem* create(const std::string& mainImage, const std::string& normalImage, const std::string& selectedImage = "");

	//
	virtual void onPressStateChangedToNormal() override;
	virtual void onPressStateChangedToPressed() override;
	virtual void onPressStateChangedToDisabled() override;
	
	//关卡名称
	void setBarrierName( const std::string& name);
	//章节id
	void setChapterId( const int chapterId);
	//设置关卡开启状态
	void setBarrierOpenFlag(bool flag);
	bool isBarrierOpen();
	//重置状态
	void reset();

	void setStateImg(const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");

	void setSelected(const bool& isSelected);
	const bool& isSelected();
protected:
	
	std::string getChapterImgName(const int chapterId);
private:
	UIEquipSrcItem();
	
	//关卡名称
	cocos2d::ui::Text *m_pBarrierName;
	//章节id
	cocos2d::ui::ImageView *m_pImgChapter;

	cocos2d::ui::ImageView *m_pImgBarrierOpen;

	//蒙版
	cocos2d::ui::ImageView* m_pFrontImgUnselected;
	cocos2d::ui::ImageView* m_pFrontImgSelected;


	bool m_isSelected;
	bool m_isBarrierOpen;
};

#endif //
