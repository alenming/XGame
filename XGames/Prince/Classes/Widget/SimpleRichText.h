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

	//���ø��ı�
	void setString(const string& richText, int defaultFontSize, Color3B defaultFontColor);

private:

	//���ı�Ԫ���б�
	vector<RichElement*> mVecRichElement;

};


#endif //SimpleRichText_h__