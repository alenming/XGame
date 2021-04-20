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

	//通用按钮弹出效果
	static void runCommBtnAction(Node* node, const std::function<void(void)>& callBack = nullptr);

	//通用弹窗弹出效果
	static void runCommDlgAction(Node* node, const std::function<void(void)>& callBack = nullptr);

	//通用“点击继续”闪烁效果
	static void runCommContinueAction(Node* node);

	// 卡牌堆叠效果;
	static void runCommCardFlow(vector<Node*>& vcNodes, float fSpace, float fClickSpace, CardFlowClickedCallBack callback = nullptr);
	static void startFlowAnimationOneByOne(int index, int baseIndex, const vector<Node*>& vcNodes, float fSpace, float fClickSpace);
};

#endif //ActionCreator__