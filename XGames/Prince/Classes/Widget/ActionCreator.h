#ifndef ActionCreator__
#define ActionCreator__

#include "cocos2d.h"
#include "CocosGUI.h"
#include <functional>
#include <vector>
#include <algorithm>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef std::function<void(Node*, Widget::TouchEventType, int)>  CardFlowClickedCallBack;

class ActionCreator
{

public:

	//ͨ�ð�ť����Ч��
	static void runCommBtnAction(Node* node, const std::function<void(void)>& callBack = nullptr);

	//ͨ�õ�������Ч��
	static void runCommDlgAction(Node* node, const std::function<void(void)>& callBack = nullptr);

	//ͨ�á������������˸Ч��
	static void runCommContinueAction(Node* node);

	// ���ƶѵ�Ч��;
	static void runCommCardFlow(vector<Node*>& vcNodes, float fSpace, float fClickSpace, CardFlowClickedCallBack callback = nullptr);
	static void startFlowAnimationOneByOne(int index, int baseIndex, const vector<Node*>& vcNodes, float fSpace, float fClickSpace);
};

#endif //ActionCreator__