#include "FateModel.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "../ToolModule/ToolModel.h"
#include "Communication/Command.h"
#include "Utils/ResourceUtils.h"

using namespace std;

FateModel* FateModel::mInstance = nullptr;

FateModel::FateModel()
{
	m_IsInit = false;
}

FateModel::~FateModel()
{
}

FateModel* FateModel::getInstance()
{
	if (!mInstance)
	{
		mInstance = new FateModel();
	}
	return mInstance;
}

void FateModel::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

//该函数只有初始化时才能调用
void FateModel::setInited()
{
	m_IsInit = true;
}
bool FateModel::isInited()
{
	return m_IsInit;
}

void FateModel::clearFateInfos()
{
	m_vecFateItemInfo.clear();
}
void FateModel::addFateItem(FateItemInfo itemInfo)
{
	m_vecFateItemInfo.push_back(itemInfo);
}
vector<FateItemInfo>& FateModel::getFateInfos()
{
	return m_vecFateItemInfo;
}
void FateModel::removeFateItem(int heroId)
{
	for (auto iter=m_vecFateItemInfo.begin(); iter!=m_vecFateItemInfo.end(); iter++)
	{
		if (iter->heroId == heroId)
		{
			m_vecFateItemInfo.erase(iter);
			return;
		}
	}
}

FateItemInfo* FateModel::getFateInfo(int heroId)
{
	for (auto& iter : m_vecFateItemInfo)
	{
		if (iter.heroId == heroId)
		{
			return (FateItemInfo*)&iter;
		}
	}
	return nullptr;
}

FateItemInfo* FateModel::getFateInfoByPos(int pos)
{
	for (auto& iter : m_vecFateItemInfo)
	{
		if (iter.pos == pos)
		{
			return (FateItemInfo*)&iter;
		}
	}
	return nullptr;
}

void FateModel::updateFateInfo(int heroId, string proType,	float fProVal, int fProQua)
{
	for (auto& iter : m_vecFateItemInfo)
	{
		if (iter.heroId == heroId)
		{
			iter.proType = proType;
			iter.fProVal = fProVal;
			iter.fProQua = fProQua;
			iter.isTied  = true;
			break;
		}
	}
}

bool FateModel::heroIsTiedFate(int heroId)
{
	for (auto& iter:m_vecFateItemInfo)
	{
		if (iter.heroId == heroId && iter.isTied)
		{
			return true;
		}
	}
	return false;
}

//获取属性图标
string FateModel::getAttriIconName(const string& strKey)
{
	if (strKey.compare("hp") == 0 || strKey.compare("hpCoe") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_xueliang.png");			//血量
	}
	else if (strKey.compare("atk") == 0 || strKey.compare("atkCoe") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_gongji.png");				//攻击
	}
	else if (strKey.compare("pdd") == 0 || strKey.compare("pddCoe") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_wufang.png");				//物防
	}
	else if (strKey.compare("mdd") == 0 || strKey.compare("mddCoe") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_mofang.png");				//法防
	}
	else if (strKey.compare("hit") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_shuxing_mingzhong.png");	//命中
	}
	else if(strKey.compare("miss") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_shuxing_shanbi.png");		//闪避
	}
	else if(strKey.compare("criPrb") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_shuxing_baoji.png");		//暴击
	}
	else if(strKey.compare("criRes") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_shuxing_baokang.png");		//暴抗
	}

	//以下四个新属性没图片资源，暂时先让服务器屏蔽发送这四个
	/*
	else if (strKey.compare("criCoe") == 0)
	{
		return ResourceUtils::getIconGlobalPath("img_shuxing_baoshang.png");	//暴击伤害加深
	}	else if (strKey.compare("coeRes") == 0)	{		return ResourceUtils::getIconGlobalPath("img_shuxing_wuli.png");		//暴击伤害减免	}	else if (strKey.compare("damCoe") == 0)	{		return ResourceUtils::getIconGlobalPath("img_shuxing_wuli.png");		//伤害加深	}	else if (strKey.compare("damRes") == 0)	{		return ResourceUtils::getIconGlobalPath("img_shuxing_wuli.png");		//伤害减免	}	*/
	return "";
	
}

//结缘消耗结缘丹数量
bool FateModel::getTiedFatesCostRed(int& id, int& count)
{
	//
	int canCount = 0;
	//
	RowData* sysData = DataManager::getInstance()->searchToolSystemById(10049);
	if (sysData != nullptr)
	{
		string strValues = sysData->getStringData("value");
		vector<int>vecValue;
		StringFormat::parseCsvStringVec(strValues, vecValue);
		if (vecValue.size()>1)
		{
			id = vecValue.at(0);
			count = vecValue.at(1);
			return true;
		}
	}
	return false;
}
//刷新消耗数量
bool FateModel::getRefreshCost(int& id, int& count)
{
	RowData* sysData = DataManager::getInstance()->searchToolSystemById(10050);
	if (sysData != nullptr)
	{
		string strValues = sysData->getStringData("value");
		vector<int>vecValue;
		StringFormat::parseCsvStringVec(strValues, vecValue);
		if (vecValue.size()>1)
		{
			id = vecValue.at(0);
			count = vecValue.at(1);
			return true;
		}
	}
	return false;
}

cocos2d::Color3B FateModel::getColorByQua(const int& nQua)
{
	if (nQua == TOOL_ITEM_QUA_GREEN)//绿
	{
		return cocos2d::Color3B(0x8d, 0xff, 0x4f);
	}
	else if (nQua == TOOL_ITEM_QUA_BLUE)//蓝
	{
		return cocos2d::Color3B(0x54, 0xf3, 0xf9);
	}
	else if (nQua == TOOL_ITEM_QUA_PURPLE)//紫
	{
		return cocos2d::Color3B(0xff, 0x6d, 0xfd);
	}
	else if (nQua == TOOL_ITEM_QUA_ORANGE)//橙
	{
		return cocos2d::Color3B(0xff, 0xd6, 0x16);
	}
	else
	{
		return cocos2d::Color3B(0xe2, 0xe2, 0xe2);
	}
}






