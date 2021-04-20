

/* 滑动选择物品数量的进度条控件 

目录：Widget/QuantitySlideBar.h
类名：QuantitySlideBar

***********接口说明***********

创建：QuantitySlideBar::create();

1. 设置滑动时最大数量值，必调接口，不调用默认为1
setMaxQuantity(int maxNum)

2. 如有需要，设置其它随着数量变化的文本指针和单价等类型参数，参考: 背包出售栏中的总出售价格数字文本	
initDivisorText(Text* pText, int unitData)

3. 如有需要，针对上一方法的文本描边，创建时调用一下该方法，不调用则默认不描边
isNeedStroke(bool isStroke, Color3B color, int sizeWidth);

*/

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "CocoStudio.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

class QuantitySlideBar : public Node
{
public:
	QuantitySlideBar();
	~QuantitySlideBar();

	CREATE_FUNC(QuantitySlideBar);
public:
	virtual bool init();
	
	//接口1. 获取当前选中的数目
	int getCurrentQuantity();

	//接口2. 设置最大数量，必调，默认是1
	void setMaxQuantity(int maxNum);

	//接口3. 设置其它随着数量变化的文本指针和单价等类型参数	
	//参数1. 文本指针， 参数2. 单价等
	void initDivisorText(Text* pText, int unitData);
	//该文本是否需要描边
	void isNeedStroke(bool isStroke, Color3B color, int sizeWidth);

private:
	//选择最大数量
	void selectMaxValue(Ref* pSender, ui::Widget::TouchEventType type);

	//选择最小数量
	void selectMinValue(Ref* pSender, ui::Widget::TouchEventType type);

	//滑动监控
	void valueChanged(Ref* pSender, Control::EventType type);

	//设置其它随着数量变化的文字数据
	void setDivisorText();

private:
	//数量
	int m_iQuantity;	

	//最大值
	int m_iMaxQuantity;

	//数字文本
	Text* m_numText;

	//其它和数量相关的外部文本指针
	Text* m_extenalText;
	int m_unitData;
	bool m_bIsNeedStroke;
	Color3B m_color;
	int m_iSizeWidth;
};