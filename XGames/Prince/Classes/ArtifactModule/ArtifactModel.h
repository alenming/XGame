#ifndef ArtifactModel_h__
#define ArtifactModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "ArtifactDataDef.h"
#include "DataManager/DataManager.h"

class ArtifactParam : public Ref
{
public:
	static ArtifactParam* create();
	
	~ArtifactParam();

	//神器是否激活
	bool isActived();

	//是否洗练满属性
	bool isRefineAll();

	//神器激活条件数据
	CC_SYNTHESIZE(vector<sCondition>, m_vActiveCondition, ActiveCondition);
	CC_SYNTHESIZE(vector<FightAttri>, m_vFightAttri, FightAttribute);
	CC_SYNTHESIZE(sCommonAttri, m_sCommonAttri, CommonAttribute);

private:
	ArtifactParam();

};


class ArtifactModel : public Observable
{
public:
	static ArtifactModel* getInstance(bool isInit = false);
	static void destroyInstance();

public:
	//根据序列号获取神器
	ArtifactParam* getArtifactByOrder(int orderNum);

	//获取所有神器
	Vector<ArtifactParam*> getAllArtifact();

	//获取属性图标路径, param: 属性类型
	string getAttriIconPath(eAttriType type);

	//初始化表格数据
	void initLocalData();

	//激活条件
	void initActiveCondition(RowData* rowData, vector<sCondition>* vecCondition);

	//神器战斗属性
	void initFightAttri(RowData* rowData, vector<sFightAttri>* vecAttri);

	//解析服务器神器主界面数据
	void updateDataFromSvr(Json::Value& data);
	//测试主界面数据
	void updateDataFromSvr();

	//更新激活之后的数据
	void updateActiveData();

	//更新洗练之后的数据
	void updateRefineData(Json::Value& data);

	//检测洗练已满的神器
	void checkRefineAll();

	//测试洗练之后的数据
	void updateRefineData();

	//更新保存洗练之后的数据
	void updateSaveData();

	//洗练结果取消时清空临时值
	void resetCancelRefine();

	//获取洗练消耗
	sRefineCost getRefineCost();

	//设置洗练次数
	void setRefineType(int times);

	//获取本次洗练次数
	int getRefineType();

	//记录当前和服务器通信的神器ID
	CC_SYNTHESIZE(int, m_iCurArtifactID, CurArtifactID);

private:
	ArtifactModel();
	virtual ~ArtifactModel();

	//洗练消耗
	void initRefineCost();

private:

	static ArtifactModel* ms_pInstance;

	Vector<ArtifactParam*> m_vArtifact;

	//洗练消耗
	sRefineCost m_sCost;

	//记录洗练的方式(1次，10次, talkingdata使用)
	int m_iRefType;
};

#endif