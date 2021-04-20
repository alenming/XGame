#ifndef SuperRichText_h__
#define SuperRichText_h__

#include "cocos2d.h"
#include "CocosGUI.h"

USING_NS_CC;

using namespace ui;
using namespace std;

enum MyRichType
{
	MyRichType_Text,
	MyRichType_Img,
};

struct MyRichStruct
{
	MyRichType type;
	Widget* widget;
};

class SuperRichText : public Layout
{

public:
	static SuperRichText* create();
	SuperRichText();
	virtual ~SuperRichText();

public:

	//设置富文本
	void setString(const string& richText, int defaultFontSize, Color3B defaultFontColor);

	//设置描边大小与颜色
	void setStroke(Color3B c3b,int size);
	float getWidth() {return width;};
private:
	bool init();

	//富文本元素列表
	std::vector<MyRichStruct> mVecRich;
	float width;

};


#endif //SimpleRichText_h__