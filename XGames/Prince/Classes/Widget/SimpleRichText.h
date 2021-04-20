#ifndef SimpleRichText_h__
#define SimpleRichText_h__

#include "cocos2d.h"
#include "UIRichText.h"

USING_NS_CC;

using namespace ui;
using namespace std;

class SimpleRichText : public RichText
{

public:

	SimpleRichText();
	virtual ~SimpleRichText();

	CREATE_FUNC(SimpleRichText);

public:

	//设置富文本
	void setString(const string& richText, int defaultFontSize, Color3B defaultFontColor);

private:

	//富文本元素列表
	vector<RichElement*> mVecRichElement;

};


#endif //SimpleRichText_h__