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
#ifndef StarsCampDef_h__
#define StarsCampDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "FormationModule/CustomScrollBar.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "2d/CCActionInterval.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainCityScene.h"
#include "Utils/StringFormat.h"
#include "Common/Common.h"
#include "Temp/PopReward.h"
#include "SmithyModule/LayerEquipSource.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define FIVEHERO	5

#define CollectID	1
#define QualityID	2
#define JewelID		3
#define StarID		4

enum SC_EVENT_TYPE
{
	SC_EVENT_TYPE_GET_INFO,
	SC_EVENT_TYPE_SHOP,
	SC_EVENT_TYPE_XIANGQIAN
};

//底图类型,星盘类型
enum SC_TYPE
{
	SC_FA = 1,
	SC_FANG,
	SC_FU,
	SC_HOU,
	SC_MU,
	SC_NAN,
	SC_NV,
	SC_SHUI,
	SCP_WU
};

struct Power_Introduce
{
	//属性图片
	ImageView	*Image_Power_1;
	//属性名字
	Text		*Label_power;
	//当前数值
	Text		*Label_Power_number_1;
	//最大数值
	Text		*Label_Power_number_Max_1;
};
//大界面
struct UI_StarsCamp_AllCamp
{
	Layout *ui_Root;		//根结点

	Layout		*laStarLayou;	//星星结点
	Button      *btnLeft;   //左翻页
	Button      *btnRight;   //右翻页
	Button      *Button_Tip;   //说明
	PageView	*pAstrolabe; //翻页容器

	ImageView	*ImageClassificationParent;
	ImageView	*ImageClassification; //加成类型图片

	ImageView   *ImgBg;  //加成属性背景

	std::vector<Power_Introduce> attribute_Introduce;//4条属性

	Button      *btnGoToFace;   //进入某一星盘界面

	//星星页面

};

struct UI_Left_attribute
{
	//英雄名字
	Text		*Label_Hero;
	//五属性晶石名字
	Text		*Label_Jewel;
};
struct UI_Right_attribute
{
	//属性图片
	ImageView	*Image_Power_1;
	//属性名字
	Text		*Label_power;
	//当前数值
	Text		*Label_Power_number_1;
	//加成 
	Text		*Label_Additional;
};

//点击英雄跳出界面
struct UI_JumpLayer
{
	Layout		*ui_Root;
	ImageView	*Image_Bg_set; //容器
	Button		*Button_Buy;//购买按钮
	Button		*Button_Change;;//更换按钮
	Button		*Button_Close;//退出
	std::vector<UI_Right_attribute> fourAttribute; //4条属性

	Text	*Label_Jewel;	//镶嵌的晶石名字
	ImageView	*Image_Plus;
	ImageView	*Image_Classification; //加成类型图片

	Text		*Label_Power_number_1;//当前数值
	Text		*Label_Name;//英雄名字
	Layout		*Panel_Hero;//英雄容器

	Text		*Label_Collect_1;//收集加成
	ImageView	*Image_Star;
	ImageView	*Image_Quality;
	SimpleRichText		*Label_Star_1;//星级加成
	SimpleRichText		*Label_Quality_1;//品质加成
	Text		*Label_Jewel_1;//数量加成
};

//单一星盘界面
struct UI_StarsCamp_OneCamp
{
	Layout		*ui_Root;			 //根结点
	Layout		*Panel_Astrolabe_3;  //中间星盘容器
	Layout		*midLa;
	Text		*teTip;
	ImageView	*Image_Bg_left;  //左边容器
	ImageView	*Image_Bg_right; //右边容器

	ImageView	*Image_Bg_set; //右边容器
	//左边属性
	std::vector<UI_Left_attribute> leftAttribute;
	//右边属性
	std::vector<UI_Right_attribute> rightAttribute;

	UI_JumpLayer lJumpLayer;

};
//一个石头
struct UI_StarsCamp_OneSto
{
	Layout		*ui_Root;

	Text		*stoName;//石头名字
	Layout		*Panel_Jewel;//放石头
	std::vector<UI_Right_attribute> _fourAttribute;//4条属性

	ImageView	*Image_Coin;	//金钱图片
	Text		*Label_Number;	//金钱文本
	Button		*Button_Buy;	//购买
	Button		*Button_Set;	//镶嵌
	Button		*Image_Setting; //镶嵌中

};

struct UI_StarsCamp_Shop
{
	Layout		*ui_Root;			 //根结点
	Layout		*Panel_Jewels;		//中间容器

	Button		*Button_Jewel_Earth;
	Button		*Button_Jewel_Wood;
	Button		*Button_Jewel_Water;
	Button		*Button_Jewel_Fire;
	Button		*Button_Jewel_Gold;

	ImageView	*Image_Lock_35;
	ImageView	*Image_Lock_45;
	ImageView	*Image_Lock_55;
	ImageView	*Image_Lock_65;
	ImageView	*Image_Lock_75;

	Text		*Label_Lv35;
	Text		*Label_Lv45;
	Text		*Label_Lv55;
	Text		*Label_Lv65;
	Text		*Label_Lv75;
};

//英雄数据
struct One_Hero_Attribute
{
	int uId;  //英雄ID
	int stoneId;//石头ID
	int Collect_ID;
	int Quality_ID;
	int Star_ID;
	int Jewel_ID;

	float jiaChen;
	long int  gongji; //攻击
	long int xueliang; //血量
	long int wufang;	//物防
	long int mofang;	//魔防

	std::string stoneName; //石头名字
	std::string stoneiconID;  //石头iconid;
	std::vector<int> haveStroneId; //拥有的石头
	One_Hero_Attribute():uId(0),stoneId(0),Collect_ID(0),Quality_ID(0),Star_ID(0),Jewel_ID(0)
		,gongji(0),xueliang(0),wufang(0),mofang(0),jiaChen(0.0)
	{}

};

struct One_StarsCamp_Attribute
{
	int uid;		//星盘ID;
	std::vector<One_Hero_Attribute> heroId; //五个英雄
	int type;   //底图类型
	//int tagTyp; //类型(攻防辅)
	int goLevel; //开放等级

	int maxgj;
	int maxxl;
	int maxmf;
	int maxwf;

	long int  gongji; //攻击
	long int xueliang; //血量
	long int wufang;	//物防
	long int mofang;	//魔防

	One_StarsCamp_Attribute():uid(0),type(0),maxwf(0),maxgj(0),maxxl(0),maxmf(0)
	,gongji(0),xueliang(0),wufang(0),mofang(0)
	{
	}
};


#endif //StarsCampDef_h__