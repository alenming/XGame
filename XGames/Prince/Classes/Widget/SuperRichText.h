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

	//���ø��ı�
	void setString(const string& richText, int defaultFontSize, Color3B defaultFontColor);

	//������ߴ�С����ɫ
	void setStroke(Color3B c3b,int size);
	float getWidth() {return width;};
private:
	bool init();

	//���ı�Ԫ���б�
	std::vector<MyRichStruct> mVecRich;
	float width;

};


#endif //SimpleRichText_h__