#include "UIBuffDelegate.h"
#include "Utils/StringFormat.h"
#include "Skill/SkillResultParam.h"
#include "Fighter.h"
#include "FightModule/Buff/AbstractBuff.h"

UIBuff::UIBuff()
	: mBuffId(-1)
	, mBuffType(-1)
	, mBuffEffect(nullptr)
{

}

UIBuff::~UIBuff()
{

}

UIBuff* UIBuff::create(int buffId, int buffType, int resId, bool isOnce, bool isFlip)
{
	UIBuff* pRet = new UIBuff();
	if (pRet && pRet->init(buffId, buffType, resId, isOnce, isFlip))
	{
		pRet->autorelease();
		return pRet;
	}

	CC_SAFE_DELETE(pRet);
	return nullptr;

}

bool UIBuff::init(int buffId, int buffType, int resId, bool isOnce, bool isFlip)
{
	if (!Node::init())
	{
		return false;
	}

	if (buffId < 0 || buffType < 0 || resId < 0)
	{
		return false;
	}

	mBuffId = buffId;
	mBuffType = buffType;
		
	if (resId > 0)
	{
		string strResId = TO_STR(resId);
		ArmatureDataManager::getInstance()->addArmatureFileInfo(
			"Image/AniBuff/" + strResId + "/" + strResId + "0.png",
			"Image/AniBuff/" + strResId + "/" + strResId + "0.plist",
			"Image/AniBuff/" + strResId + "/" + strResId + ".ExportJson");

		mBuffEffect = Armature::create(strResId);
		this->addChild(mBuffEffect);

		if (isFlip)
		{
			this->setScaleX(-1);
		}

		if (isOnce)
		{
			playOnceEffect();
		}
		else
		{
			playCircleEffect();
		}
	}

	return true;
}

void UIBuff::playOnceEffect()
{
	mBuffEffect->getAnimation()->play("once");
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		//����ɾ��
		this->removeFromParent();
	};
	mBuffEffect->getAnimation()->setMovementEventCallFunc(aniCallBack);
}

void UIBuff::playCircleEffect()
{
	mBuffEffect->getAnimation()->play("circle");
}

int UIBuff::getBuffId()
{
	return mBuffId;
}

int UIBuff::getBuffType()
{
	return mBuffType;
}


//////////////////////////////�����////////////////////////////////////////////



UIBuffDelegate::UIBuffDelegate()
	: mUIFighter(nullptr)
{

}

UIBuffDelegate::~UIBuffDelegate()
{
	removeAllBuff();
}

UIBuffDelegate* UIBuffDelegate::create(UIFighter* fighter)
{
	UIBuffDelegate* ret = new UIBuffDelegate();
	if (ret && ret->init(fighter))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool UIBuffDelegate::init( UIFighter* fighter )
{
	if (!fighter)
	{
		return false;
	}

	mUIFighter = fighter;
	return true;
}

void UIBuffDelegate::updateBuff( BuffUpdateParam* buffUpdateParam )
{
	//���buff
	for (size_t i = 0; i < buffUpdateParam->mVecAddBuff.size(); i++)
	{
		BuffInfo& buffInfo = buffUpdateParam->mVecAddBuff.at(i);

		//��������ڳ�����Ч�����򲥷ų�����Ч��
		if (buffInfo.mCircleResId > 0)
		{
			if (!isUIBuffExist(buffInfo.mBuffType))
			{
				UIBuff* buff = UIBuff::create(buffInfo.mBuffId, buffInfo.mBuffType, buffInfo.mCircleResId,
					false, mUIFighter->getSide() == FightStateMachine::MY_SIDE && buffInfo.mCircleFlip);
				mVecUIBuff.push_back(buff);
				mUIFighter->addChild(buff, UIFighter::ZORDER_BUFF_TOP);
			}
		}

		//���һ����buff��Ϊ�գ�ֱ�Ӳ���
		if (buffInfo.mOnceResId > 0)
		{
			UIBuff* buff = UIBuff::create(buffInfo.mBuffId, buffInfo.mBuffType, buffInfo.mOnceResId,
				true, mUIFighter->getSide() == FightStateMachine::MY_SIDE && buffInfo.mOnceFlip);
			mUIFighter->addChild(buff, UIFighter::ZORDER_BUFF_TOP);
		}
	}

	//ɾ��buff
	Vector<AbstractBuff*>* vecBuff = mUIFighter->getFighterData()->getVecBuff();
	for (size_t i = 0; i < buffUpdateParam->mVecRemoveBuff.size(); i++)
	{
		BuffInfo& buffInfo = buffUpdateParam->mVecRemoveBuff.at(i);

		//�ж�����ģ��fighter�е�buff�б����Ƿ��и�buff����
		bool isExistInModel = false;
		for (size_t j = 0; j < vecBuff->size(); j++)
		{
			AbstractBuff* buff = vecBuff->at(j);
			if (buff->getBuffType() == buffInfo.mBuffType)
			{
				isExistInModel = true;
			}
		}

		//�������ģ���в�������ɾ��UIBuff
		if (!isExistInModel)
		{
			//�ҵ�UIBuff�б��ж�Ӧ��UIBuff��ɾ��
			UIBuff* deleteBuff = nullptr;
			vector<UIBuff*>::iterator iter;
			for (iter = mVecUIBuff.begin(); iter != mVecUIBuff.end(); iter++)
			{
				UIBuff* buff = *iter;
				if (buff->getBuffType() == buffInfo.mBuffType)
				{
					deleteBuff = buff;
					break;
				}
			}

			if (deleteBuff)
			{
				mUIFighter->removeChild(deleteBuff);
				mVecUIBuff.erase(iter);
			}
		}
	}
}

UIBuff* UIBuffDelegate::isUIBuffExist(int buffType)
{
	for (UIBuff* buff : mVecUIBuff)
	{
		if (buff->getBuffType() == buffType)
		{
			return buff;
		}
	}
	return nullptr;
}

void UIBuffDelegate::removeAllBuff()
{
	for (UIBuff* buff : mVecUIBuff)
	{
		mUIFighter->removeChild(buff);
	}

	mVecUIBuff.clear();
}


