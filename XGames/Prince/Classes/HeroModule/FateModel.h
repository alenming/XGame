#ifndef FateModel_h__
#define FateModel_h__

#include <map>
#include <vector>
#include <string>
#include "base/CCVector.h"
#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "../ToolModule/ToolDef.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace std;

struct  FateItemInfo
{
	int id;
	int heroId;
	int fateId;
	string proType;		//属性类型
	float fProVal;		//刷新前的值
	float fNewProVal;	//刷新后的值
	int fProQua;		//刷新前品质
	int fNewQua;		//刷新后品质
	bool isTied;		//是否已经结缘
	int pos;

	FateItemInfo()
		:id(-1)
		,heroId(-1)
		,fateId(0)
		,proType("")
		,fProVal(0.0f)
		,fNewProVal(0.0f)
		,fProQua(0)
		,fNewQua(0)
		,isTied(false)
		,pos(-1) {}
};
class FateModel : public Observable
{
public:

	FateModel();
	virtual ~FateModel();

	static FateModel* getInstance();
	static void destroyInstance();

public:

	//数据初始化
	void setInited();
	bool isInited();
	void clearFateInfos();
	void addFateItem(FateItemInfo itemInfo);
	FateItemInfo* getFateInfo(int heroId);
	FateItemInfo* getFateInfoByPos(int pos);
	void updateFateInfo(int heroId, string proType,	float fProVal, int fProQua);
	void removeFateItem(int heroId);
	vector<FateItemInfo>& getFateInfos();
	//是否已经结识缘分
	bool heroIsTiedFate(int heroId);
	//获取属性图标
	string getAttriIconName(const string& strKey);
	//
	cocos2d::Color3B getColorByQua(const int& nQua);
	//结缘消耗结缘丹数量 id:模板id
	bool getTiedFatesCostRed(int& id, int& count);
	//刷新消耗数量 id:模板id
	bool getRefreshCost(int& id, int& count);


private:

	static FateModel* mInstance;

	//是否已经初始化
	bool m_IsInit;

	vector<FateItemInfo> m_vecFateItemInfo;

};


#endif // FateModel_h__
