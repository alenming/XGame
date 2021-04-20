#ifndef JoinGroupDef_h__
#define JoinGroupDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include <string>
#include "Utils/SoundUtils.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/Observer.h"
#include "Communication/Json/json.h"
#include "Communication/ICommListener.h"
#include "MainModule/MainCityScene.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Temp/CustomGoldTips.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int GROUP_COUNT = 4;

struct UI_JOIN_GROUP
{
	// UI根节点层容器
	Layout		*pRoot;

	ImageView   *ImgTips;  //提示信息
	Layout	    *Img_di1;
	Button      *bTnFind;   //查找
	Button      *bTnCreate;   //创建
	Button      *bTnViewAll;   //显示全部
	Button      *bTnApplyList;   //正在申请
	Button      *bTnViewLeft;   //显示左
	Button      *bTnViewRight;   //显示右
	Text        *labPage;  //翻页数
	Layout      *group[GROUP_COUNT]; //公会

	Layout         *Panel_Flag;
	ImageView      *groupIcon; //修改公会图标
	Button         *btnCloseIcon; //关闭徽记
	ScrollView     *iconScrollView; //徽记列表

	Layout         *Panel_create;
	ImageView      *groupCreate; //创建公会
	Button         *BtnCloseCreate; //关闭创建
	Button         *BtnModifyCreate; //修改图标
	Button         *BtnCreateEnter; //确认创建
	EditBoxEx      *inputName; //帮派名称
	Text           *createPrize;//创建花费
	Layout         *PnlCreateIcon;

	Layout         *Panel_Found;
	ImageView      *findGroup;//查找帮派
	Button         *btnCloseFind; //关闭查找
	Button         *btnFind; //确定查找
	EditBoxEx      *inputNameFind; //帮派名称


	//////////////////////////////////////
	Layout          *infoPanle;
	ImageView       *infoPanledi;

	//头像
	Button          *infoPanleCloseLine;
	Layout			*infoPanleFlag;
	Text			*infoPanleGangLevel;
	Text			*infoPanleGangName ;
	Text			*infoPanleWang;
	Text			*infoPanleManage;
	Text			*infoPanleID;
	Text			*infoPanleRank;
	Text			*infoPanlePublic;
};

struct GROUP_INFO
{
	int orderNum; //帮派排名
	int resId ;//头像res
	string name; //帮派名字
	int level; //帮派等级
	int curNum; //当前人数
	int maxNum; //最高上限
	int id;//帮派id
	int needLvl; //加入等级
	bool checkFlag; //需要审核
	string remark2; //对外宣传
	string leader; //公会会长名
};
typedef vector<GROUP_INFO>    VEC_GROUP_INFO;

enum SEARCH_TYPE
{
	SEARCH_TYPE_ALL = 1,
	SEARCH_TYPE_APPLY,
	SEARCH_TYPE_ID,
};
const int GROUP_APPLY_COUNT = 5;

#endif
