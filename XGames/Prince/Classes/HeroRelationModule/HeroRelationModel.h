
/******************英雄缘分数据解析**********************/

#ifndef HeroRelationModel_h__
#define HeroRelationModel_h__

#include "cocos2d.h"
#include "HeroRelationDataDef.h"
#include "DataManager/DataManager.h"


class HeroRelationModel
{
public:

	static HeroRelationModel* getInstance();
	static void destroyInstance();

	/******************************使用注意****************************/
	//由于互相关联的英雄数据会随着游戏进度而变化，当传入的英雄作为*受缘分英雄*时
	//***必须***先调用下面方法重新解析缘分!
	void initFatedData(int templateId);

	//调用上面方法初始化之后，可以获取的相关数据，以传入数据结构进行构造的形式


	//获取会影响到A的其它英雄ID
	void getRelationHeroId(vector<sFateHeroOrder>& vFateHeroId);

	//获取A的缘分激活条件
	void getRelationActLimit(vector<sFateHeroOrder>& vFateActLimit);

	//获取A的当前缘分ID列表
	void getRelationIDList(vector<int>& vFateID);

	//获取A的当前缘分属性列表
	void getRelationAttriList(vector<sFateAttri>& vFateAttri);



	//获取受缘英雄ID列表
	void getFatedHeroId(int heroId, vector<sFateHeroOrder>& vFatedHeroID);

	//获取受缘英雄缘分ID列表
	void getFatedIdList(vector<int>& vFatedId);

	//获取受缘英雄缘分属性
	void getFatedAttri(vector<sFateAttri>& vFatedAttri);

private:

	void initData(int templateId);

	//计算英雄缘分表ID
	void calculateFateId(int templateId);

	//解析缘分属性
	void parseFateAttri(int templateId);
	

	//A作为主缘分英雄，解析缘分数据
	void parseFatedAttri(int templateId);
	
private:
	HeroRelationModel();
	virtual ~HeroRelationModel();

private:

	static HeroRelationModel* ms_pInstance;

	//关联的英雄ID
	vector<sFateHeroOrder> m_vFateHeroId;

	//激活条件
	vector<sFateHeroOrder> m_vFateLimit;

	//缘分ID列表
	vector<int> m_vFateId;

	//缘分属性列表
	vector<sFateAttri> m_vFateAttri;
	
	//关联的英雄ID
	vector<sFateHeroOrder> m_vFatedHeroId;

	//缘分ID列表
	vector<int> m_vFatedId;

	//缘分属性列表
	vector<sFateAttri> m_vFatedAttri;
};

#endif