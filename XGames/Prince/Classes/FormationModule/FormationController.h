/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		FormationController
 *  Description:	阵型模块控制器;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/09/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef FormationController_h__
#define FormationController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "FormationDef.h"


class FormationController : public ICommListener
{
public:
	~FormationController(void);

	static FormationController* getInstance();
	static void  releaseInstance();

	// 获取阵型数据;
	void  handleFormation();

	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// Hero上下阵;
	int   handleHeroField();

	// 预备替换Hero;
	void  handlePreReplace();

	// 交换站位;
	bool  handleSwapFormation(HERO_POS nPos1, HERO_POS nPos2);

	// 选中英雄;
	int   selectHero(int nHeroId);

	// 选中英雄;
	int   selectHeroByPos(HERO_POS nPos);

	// 选中阵型空位;
	void  handleSelectEmptyPos(HERO_POS nPos);

	// 上传英雄阵型;
	int  handleFormationLocal();

	// 检查试炼战斗前的阵型状态;
	int   checkTrailFormation();

private:
	FormationController(void);

	// 初次进入阵型模块;
	void  procFirstEnter(vector<int>& vcFormation);

	// 处理拉取到的阵型信息;
	void  procFormation(const char* szData);

	// 处理Hero上阵;
	int   procHeroOnField(int nHeroId, HERO_POS nPos = Hero_Pos_Invalid);

	// 处理Hero下阵;
	int   procHeroOffField(int nHeroId);

	// 替换Hero;
	int   handleReplaceHero(int HeroId1, int nHeroId2);

	// 上传阵型信息的结果;
	void  procUploadFormation(const char* szData);

	// 试炼阵型;
	void  procTrailFormation( const char* szData );

	// 首次进入试炼阵型;
	void  procTrailFirstEnter(vector<int>& vcTrailFormation);

private:

	// singleton;
	static FormationController* m_pInstance;

	// 当前是否正在替换;
	bool  m_bReplacing;

	// 当前是否正在等待服务器响应(上传阵型);
	bool  m_bIsWaittingResponse;

};


#endif // FormationController_h__

