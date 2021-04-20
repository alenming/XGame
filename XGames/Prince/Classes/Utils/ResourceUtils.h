#ifndef __RESOURCE_UTILS_H__
#define __RESOURCE_UTILS_H__

#include <string>

using namespace std;

class ResourceUtils
{

public:

	//获取小图标路径
	static string getSmallIconPath(int iconId);
	static string getSmallIconPath(string iconId);

	//获取英雄角标路径
	static string getCornerIconPatn(string iconId);

	//获取卡牌(抽卡，迷宫用到)路径
	static std::string getHalfCardPath(const int iconId );
	//获取整卡（没截取）卡牌路径
	static std::string getHeroCompleteCardPath(const int iconId );
	//获取卡牌路径
	static std::string getHeroCardPath(const int iconId );
	//获取英雄碎片路径
	static std::string getHeroChipPath(const int iconId );
	//获取羁绊图标路径
	static string getFateIconPath(int iconId);
	//获取星灵路径
	static string getStarSpritePath(int iconId);
	//获取迷宫宝图路径
	static string getMazeIconPath(int iconId);

	//获取角色骨骼动画路径
	static string getFighterSkeletonPath(int resId);

	//获取技能骨骼动画路径
	static string getSkillSkeletonPath(int resId);

	//获取战斗背景图路径
	static string getFightBgPath(int resId);

	//获取UI资源根路径
	static string getUIPath(string uiName);

	//获取战斗技能文字路径
	static string getFightSkillText(int resId);

	//获取战斗提示文字路径
	static string getFightTipsText(int resId);

	//获取新手引导画布路径
	static string getGuideResPath(const string& name);

	//获取剧情对话NPC头像
	static string getChatNpcPath(int resId);

	//获取任务图标
	static string getTaskIconPath(int resId);

	//获取公共图标
	static string getIconGlobalPath(const string& name);

	//获取职业图标
	static string getIconJobPath(const string& name);

	//获取角色称号图片
	static string getNickNamePath(int resId);

	//获取背景音乐路径
	static string getSoundMusicPath(const string& name);

	//获取音效路径
	static string getSoundEffectPath(const string& name);

	//获取星盘头像图片
	static string getStarCampPathById(int id);
	static string getStarCampPathByStr(string name);

	//获取星盘底部图片 
	static string getStarCampDiPathById(int id);
	static string getStarCampDiPathByStr(string name);
	//获取星盘加成类型图片
	static string getStarCampBuffPathById(int id);

	//获取英雄资质等级图片
	static string getHeroTalentPath(int heroTalent);
};

#endif //__RESOURCE_UTILS_H__