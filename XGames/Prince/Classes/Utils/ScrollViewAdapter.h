#ifndef __ScrollViewAdapter_H__
#define __ScrollViewAdapter_H__

#include "cocos2d.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace ui;

class ScrollViewAdapter
{

public:
	
	ScrollViewAdapter(ScrollView* scrollView)
		: mScrollView(nullptr)
		, mHorizontalSpacing(0)
		, mHorizontalPadding(0)
		, mVerticalSpacing(0)
		, mVerticalPadding(0)
	{}

	~ScrollViewAdapter(){};

public:

	//添加节点
	void addChild(Node* node);

	//设置子组件横向间距
	void setHorizontalSpacing(int spacing);

	//设置滚动区域横向边距
	void setHorizontalPadding(int padding);

	//设置子组件纵向间距
	void setVerticalSpacing(int spacing);

	//设置滚动区域纵向边距
	void setVerticalPadding(int padding);

private:

	//滚动组件
	ScrollView* mScrollView;

	//子组件横向间距
	int mHorizontalSpacing;

	//滚动区域横向边距
	int mHorizontalPadding;

	//子组件纵向间距
	int mVerticalSpacing;

	//滚动区域纵向边距
	int mVerticalPadding;


};



#endif //__ScrollViewAdapter_H__