#include "ActionCreator.h"


void ActionCreator::runCommBtnAction( Node* node, const std::function<void(void)>& callBack)
{
	float duration = 0.18f;
	float toScale = 0.1f;
	float originScale = node->getScale();
	node->setScale(toScale);
	Sequence* seq = Sequence::create(
		EaseBackOut::create(ScaleTo::create(duration, originScale)),
		CallFunc::create(callBack), nullptr);
	node->runAction(seq);
}

void ActionCreator::runCommDlgAction(Node* node, const std::function<void(void)>& callBack)
{
	float duration = 0.2f;
	float toScale = 1.0f;
	Sequence* seq = Sequence::create(
		EaseBackOut::create(ScaleTo::create(duration, toScale)), 
		CallFunc::create(callBack), nullptr);
	//auto action = EaseBackOut::create(ScaleTo::create(duration, toScale));
	node->setScale(0);
	node->runAction(seq);
}

void ActionCreator::runCommContinueAction(Node* node)
{
	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	node->runAction(RepeatForever::create(seq));
}

/********************************************************************
*  函数名称: runCommCardFlow;
*  功能描述: 卡牌堆叠效果;
*  参数列表: vcNodes : 节点列表;
*			 fSpace : 未选中的卡牌间隔;
*			 fClickSpace: 选中卡牌时的间隔;
*  返回值  : (void);
********************************************************************/
void ActionCreator::runCommCardFlow( vector<Node*>& vcNodes, float fSpace, float fClickSpace, CardFlowClickedCallBack callback /*= nullptr*/ )
{
	// 少于两个节点，不处理;
	if (vcNodes.size() < 2)
		return;

	// 查找Node索引;
	auto findIndex = [=](Node* src)->int {
		int index = -1;
		for (int k = 0; k < vcNodes.size(); ++k)
		{
			if (vcNodes.at(k) == src)
			{
				index = k;
				break;
			}
		}
		return index;
	};

	// 排列所有节点，以第1个节点位置为准;
	int baseLocalZOrder = vcNodes.at(0)->getLocalZOrder();
	Vec2 basePos = vcNodes.at(0)->getPosition();
	for (int i = 0; i < vcNodes.size(); ++i)
	{
		// 初始化位置;
		vcNodes.at(i)->setPosition(Vec2(basePos.x + i * fSpace, basePos.y));

		// 初始化ZOrder;
		vcNodes.at(i)->setLocalZOrder(baseLocalZOrder + i);

		// 为所有节点添加回调;
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [=](Touch* touch, Event* event)
		{
			Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			Size size = event->getCurrentTarget()->getContentSize();
			if (Rect(0, 0, size.width, size.height).containsPoint(locationInNode))
			{
				if (callback)
					callback(event->getCurrentTarget(), Widget::TouchEventType::BEGAN, findIndex(event->getCurrentTarget()));

				return true;
			}
			return false;
		};
		listener->onTouchEnded = [=](Touch* touch, Event* event)
		{
			// 执行Move动画;
			int nodeIndex = findIndex(event->getCurrentTarget());
			startFlowAnimationOneByOne(0, nodeIndex, vcNodes, fSpace, fClickSpace);
			if (callback)
				callback(event->getCurrentTarget(), Widget::TouchEventType::ENDED, nodeIndex);
		};
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, vcNodes.at(i));
	}
}

void ActionCreator::startFlowAnimationOneByOne( int index, int baseIndex, const vector<Node*>& vcNodes, float fSpace, float fClickSpace )
{
	if (index < 0 || baseIndex < 0 || index >= vcNodes.size() || baseIndex >= vcNodes.size())
		return;

	Vec2 dstPos;
	if (index <= baseIndex)
	{
		dstPos = Vec2(vcNodes.at(0)->getPosition().x + index * fSpace, vcNodes.at(0)->getPosition().y);
	}
	else if (index > baseIndex)
	{
		dstPos = Vec2(vcNodes.at(0)->getPosition().x + (index-1) * fSpace + fClickSpace, vcNodes.at(0)->getPosition().y);
	}

	// 相同位置不移动;
	if (dstPos.x == vcNodes.at(index)->getPosition().x)
	{
		startFlowAnimationOneByOne(index+1, baseIndex, vcNodes, fSpace, fClickSpace);
	}
	else
	{
		Sequence* seq = Sequence::create(EaseSineInOut::create(MoveTo::create(0.1f, dstPos)),
			CallFuncN::create([=](Ref* pSender){
				startFlowAnimationOneByOne(index+1, baseIndex, vcNodes, fSpace, fClickSpace);
		}), nullptr);
		vcNodes.at(index)->runAction(seq);
	}
}
