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

	//��ӽڵ�
	void addChild(Node* node);

	//���������������
	void setHorizontalSpacing(int spacing);

	//���ù����������߾�
	void setHorizontalPadding(int padding);

	//���������������
	void setVerticalSpacing(int spacing);

	//���ù�����������߾�
	void setVerticalPadding(int padding);

private:

	//�������
	ScrollView* mScrollView;

	//�����������
	int mHorizontalSpacing;

	//�����������߾�
	int mHorizontalPadding;

	//�����������
	int mVerticalSpacing;

	//������������߾�
	int mVerticalPadding;


};



#endif //__ScrollViewAdapter_H__