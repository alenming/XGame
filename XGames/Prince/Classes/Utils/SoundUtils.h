#ifndef SoundUtils__
#define SoundUtils__

#include <string>
#include "cocos2d.h"

using namespace std;
USING_NS_CC;
using namespace CocosDenshion;

class SoundUtils
{

public:

	SoundUtils();
	~SoundUtils();

	static SoundUtils* getInstance();

	static void destroyInstance();

public:

	//播放背景音乐，注意只需传递文件名，不需传递文件后缀名
	static void playSoundMusic(const string& name);

	//播放音效，注意只需传递文件名，不需传递文件后缀名
	static void playSoundEffect(const string& name);

public:

	//设置音乐关闭/开启
	void setSoundMusicOn(bool isOn);

	//设置音效关闭/开启
	void setSoundEffectOn(bool isOn);

	//获取音乐是否关闭
	bool getSoundMusicOn();

	//获取音效是否关闭
	bool getSoundEffectOn();

	//播放音乐
	void playMusic(const string& name);

	//播放音效
	void playEffect(const string& name);

private:

	static SoundUtils*  m_pInstance;

	//音乐是否打开
	int mIsSoundMusicOn;

	//音效是否打开
	int mIsSoundEffectOn;

	//当前播放的音乐名称
	string mCurrMusicName;
};



#endif //SoundUtils__