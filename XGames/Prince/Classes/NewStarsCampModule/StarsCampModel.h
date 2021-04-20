/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampModel
 *  Description:	星盘数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			02/19/2016
 *  Others:			
 *
 ******************************************************************/
#include "StarsCampDef.h"


#ifndef StarsCampModel_h_
#define StarsCampModel_h_

class StarsCampModel:public Observable
{
public:
	~StarsCampModel();

	//活动单例
	static StarsCampModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(int eventType, void* data = nullptr);

	void parseCampInfoData(const Json::Value &data); //解析星盘信息
	void parseCampShopDate(const Json::Value &data); //购买
	void parseCampXiangQianDate(const Json::Value &data); //镶嵌

	void initDonateLocalData();

	//根据ID获取一个星盘的信息
	One_StarsCamp_Attribute* getOndeStarCampInfoByUid(int uId);
	//根据位置获取一个星盘的信息
	One_StarsCamp_Attribute& getOndeStarCampInfoByWz(int wz);
	//根据位置获取一个英雄的信息
	One_Hero_Attribute getOndeHeroInfoByWz(int wz);
	//计算每个英雄的加成属性
	void computeOneHeroAttrubute();

	std::vector<int> startcampOpenLevel;

	CC_SYNTHESIZE(int,_newcampId,NewCampId);
	std::vector<One_StarsCamp_Attribute>& getAllStarCampInfo();
	

	CC_SYNTHESIZE(int,_cleckWeiZhi,CleckWZ);		//哪个盘
	CC_SYNTHESIZE(int,_cleckHero,CleckHero);		//哪个英雄
	CC_SYNTHESIZE(int,_shopStoneId,ShopStoneId);		//购买和镶嵌的ID
	CC_SYNTHESIZE(bool,_isNewCamp,IsNewCamp);


	int getYeShu();

private:
	StarsCampModel();

	static StarsCampModel* mInstance;

	//所有星盘的数据
	std::vector<One_StarsCamp_Attribute> _StarsCampInfo;
	
};

#endif