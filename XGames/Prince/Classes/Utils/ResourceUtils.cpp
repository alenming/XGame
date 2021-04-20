#include "ResourceUtils.h"
#include <sstream>
#include "cocos2d.h"


std::string ResourceUtils::getSmallIconPath( int iconId )
{
	stringstream ss;
	ss << "Image/Icon/Small/" << iconId << ".png";
	return ss.str();
}

std::string ResourceUtils::getSmallIconPath( string iconId )
{
	stringstream ss;
	ss << "Image/Icon/Small/" << iconId << ".png";
	return ss.str();
}

std::string ResourceUtils::getCornerIconPatn(string iconId)
{
	stringstream ss;
	ss << "Image/Icon/global/" << iconId << ".png";
	return ss.str();
}

std::string ResourceUtils::getHalfCardPath(const int iconId)
{
	stringstream ss;
	ss << "Image/Icon/halfcard/" << iconId << ".png";
	return ss.str();
}

std::string ResourceUtils::getHeroCompleteCardPath(const int iconId)
{
	stringstream ss;
	ss << "Image/Icon/integrityCard/" << iconId << ".png";
	return ss.str();
}

std::string ResourceUtils::getHeroCardPath(const int iconId )
{
	stringstream ss;
	ss << "Image/Icon/newcard/" << iconId << ".png";
	return ss.str();
}

//��ȡӢ����Ƭ·��
std::string ResourceUtils::getHeroChipPath(const int iconId )
{
	stringstream ss;
	ss << "Image/Icon/shop/" << iconId << ".png";
	return ss.str();
}

//��ȡ�ͼ��·��
string ResourceUtils::getFateIconPath(int iconId)
{
	stringstream ss;
	ss << "Image/Icon/Fate/" << iconId << ".png";
	return ss.str();
}

//��ȡ����·��
string ResourceUtils::getStarSpritePath(int iconId)
{
	stringstream ss;
	ss << "Image/Icon/star/" << iconId << ".png";
	return ss.str();
}

//��ȡ�Թ���ͼ·��
string ResourceUtils::getMazeIconPath(int iconId)
{
	stringstream ss;
	ss << "Image/Icon/maze/" << iconId << ".png";
	return ss.str();
}

//��ȡ��ɫ��������·��
string ResourceUtils::getFighterSkeletonPath(int resId)
{
	stringstream ss;
	ss << "Image/HeroAction/" << resId << "/";
	return ss.str();
}

//��ȡ���ܹ�������·��
string ResourceUtils::getSkillSkeletonPath(int resId)
{
	stringstream ss;
	ss << "Image/HeroEffect/" << resId << "/";
	return ss.str();
}

//��ȡս������ͼ·��
string ResourceUtils::getFightBgPath(int resId)
{
	stringstream ss;
	ss << "Image/Fight/FightBg" << resId << "/";
	return ss.str();
}

//��ȡUI��Դ��·��
string ResourceUtils::getUIPath(string uiName)
{
	stringstream ss;
	ss << "Image/" << uiName << "/";
	return ss.str();
}

string ResourceUtils::getFightSkillText(int resId)
{
	stringstream ss;
	ss << "Image/Icon/fightSkill/" << resId << ".png";
	return ss.str();
}

string ResourceUtils::getFightTipsText(int resId)
{
	stringstream ss;
	ss << "Image/Icon/fightInfo/" << resId << ".png";
	return ss.str();
}

string ResourceUtils::getGuideResPath(const string& name)
{
	stringstream ss;
	ss << "Image/UIguideChain1/" << name << ".ExportJson";
	return ss.str();
}

//��ȡ����Ի�NPCͷ��
string ResourceUtils::getChatNpcPath(int resId)
{
	stringstream ss;
	ss << "Image/Icon/ChatNpc/" << resId << ".png";
	return ss.str();
}

string ResourceUtils::getTaskIconPath(int resId)
{
	stringstream ss;
	ss << "Image/Icon/task/" << resId << ".png";
	return ss.str();
}

string ResourceUtils::getIconGlobalPath(const string& name)
{
	stringstream ss;
	ss << "Image/Icon/global/" << name;
	return ss.str();
}

string ResourceUtils::getIconJobPath(const string& name)
{
	stringstream ss;
	ss << "Image/Icon/global/" << name;
	return ss.str();
}

string ResourceUtils::getNickNamePath(int resId)
{
	stringstream ss;
	ss << "Image/Icon/nickname/" << resId << ".png";
	return ss.str();
}

//��ȡ��������·��
string ResourceUtils::getSoundMusicPath(const string& name)
{
	stringstream ss;
	ss << "Sound/music/" << name << ".mp3";
	return ss.str();
}

//��ȡ��Ч·��
string ResourceUtils::getSoundEffectPath(const string& name)
{
	//��Ч��ʽ��ƽ̨���
	string suffix = "";
	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		suffix = ".wav";
	}
	else if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		suffix = ".m4a";
	}
	else if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	{
		suffix = ".ogg";
	}

	stringstream ss;
	ss << "Sound/effect/" << name << suffix;
	return ss.str();
}
string ResourceUtils::getStarCampPathById(int id)
{
	stringstream ss;
	ss << "Image/Icon/xingpan/" << id << ".png";
	return ss.str();
}
string ResourceUtils::getStarCampPathByStr(string name)
{
	stringstream ss;
	ss << "Image/Icon/xingpan/" << name << ".png";
	return ss.str();
}

std::string ResourceUtils::getStarCampDiPathById( int id )
{
	stringstream ss;
	ss << "Image/Icon/xingpan/btn_xingwei_" << id << ".png";
	return ss.str();
}

std::string ResourceUtils::getStarCampBuffPathById( int id )
{
	stringstream ss;
	ss << "Image/Icon/xingpan/img_xingwei_" << id << ".png";
	return ss.str();
}

std::string ResourceUtils::getStarCampDiPathByStr( string name )
{
	stringstream ss;
	ss << "Image/Icon/xingpan/btn_xingwei_" << name << ".png";
	return ss.str();
}

std::string ResourceUtils::getHeroTalentPath( int heroTalent )
{
	stringstream ss;
	ss << "Image/Icon/global/img_zizhi_" << heroTalent << ".png";
	return ss.str();
}

