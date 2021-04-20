#ifndef __RESOURCE_UTILS_H__
#define __RESOURCE_UTILS_H__

#include <string>

using namespace std;

class ResourceUtils
{

public:

	//��ȡСͼ��·��
	static string getSmallIconPath(int iconId);
	static string getSmallIconPath(string iconId);

	//��ȡӢ�۽Ǳ�·��
	static string getCornerIconPatn(string iconId);

	//��ȡ����(�鿨���Թ��õ�)·��
	static std::string getHalfCardPath(const int iconId );
	//��ȡ������û��ȡ������·��
	static std::string getHeroCompleteCardPath(const int iconId );
	//��ȡ����·��
	static std::string getHeroCardPath(const int iconId );
	//��ȡӢ����Ƭ·��
	static std::string getHeroChipPath(const int iconId );
	//��ȡ�ͼ��·��
	static string getFateIconPath(int iconId);
	//��ȡ����·��
	static string getStarSpritePath(int iconId);
	//��ȡ�Թ���ͼ·��
	static string getMazeIconPath(int iconId);

	//��ȡ��ɫ��������·��
	static string getFighterSkeletonPath(int resId);

	//��ȡ���ܹ�������·��
	static string getSkillSkeletonPath(int resId);

	//��ȡս������ͼ·��
	static string getFightBgPath(int resId);

	//��ȡUI��Դ��·��
	static string getUIPath(string uiName);

	//��ȡս����������·��
	static string getFightSkillText(int resId);

	//��ȡս����ʾ����·��
	static string getFightTipsText(int resId);

	//��ȡ������������·��
	static string getGuideResPath(const string& name);

	//��ȡ����Ի�NPCͷ��
	static string getChatNpcPath(int resId);

	//��ȡ����ͼ��
	static string getTaskIconPath(int resId);

	//��ȡ����ͼ��
	static string getIconGlobalPath(const string& name);

	//��ȡְҵͼ��
	static string getIconJobPath(const string& name);

	//��ȡ��ɫ�ƺ�ͼƬ
	static string getNickNamePath(int resId);

	//��ȡ��������·��
	static string getSoundMusicPath(const string& name);

	//��ȡ��Ч·��
	static string getSoundEffectPath(const string& name);

	//��ȡ����ͷ��ͼƬ
	static string getStarCampPathById(int id);
	static string getStarCampPathByStr(string name);

	//��ȡ���̵ײ�ͼƬ 
	static string getStarCampDiPathById(int id);
	static string getStarCampDiPathByStr(string name);
	//��ȡ���̼ӳ�����ͼƬ
	static string getStarCampBuffPathById(int id);

	//��ȡӢ�����ʵȼ�ͼƬ
	static string getHeroTalentPath(int heroTalent);
};

#endif //__RESOURCE_UTILS_H__