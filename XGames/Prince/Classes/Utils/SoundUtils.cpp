#include "SoundUtils.h"
#include "ResourceUtils.h"
#include "SimpleAudioEngine.h"

SoundUtils* SoundUtils::m_pInstance = nullptr;

SoundUtils::SoundUtils()
{
	mIsSoundMusicOn = UserDefault::getInstance()->getBoolForKey("isMusicOn", true);
	mIsSoundEffectOn = UserDefault::getInstance()->getBoolForKey("isEffectOn", true);
	//SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.3f);
}

SoundUtils::~SoundUtils()
{

}

SoundUtils* SoundUtils::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new SoundUtils();
	}
	return m_pInstance;
}

void SoundUtils::destroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
	}
}

void SoundUtils::playSoundMusic( const string& name )
{
	SoundUtils::getInstance()->playMusic(name);
}

void SoundUtils::playSoundEffect( const string& name )
{
	SoundUtils::getInstance()->playEffect(name);
}

//设置音乐关闭/开启
void SoundUtils::setSoundMusicOn(bool isOn)
{
	mIsSoundMusicOn = isOn;
	UserDefault::getInstance()->setBoolForKey("isMusicOn", isOn);
	if (!isOn)
	{
		//关闭音效需要清空记录的当前播放的音乐
		mCurrMusicName = "";
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
}

//设置音效关闭/开启
void SoundUtils::setSoundEffectOn(bool isOn)
{
	mIsSoundEffectOn = isOn;
	UserDefault::getInstance()->setBoolForKey("isEffectOn", isOn);
}

//获取音乐是否关闭
bool SoundUtils::getSoundMusicOn()
{
	return mIsSoundMusicOn;
}

//获取音效是否关闭
bool SoundUtils::getSoundEffectOn()
{
	return mIsSoundEffectOn;
}

//播放音乐
void SoundUtils::playMusic(const string& name)
{
	if(mIsSoundMusicOn)
	{
		//需判定当前播放音乐与要播放的音乐是否相同，如果不相同再播放
		if (mCurrMusicName != name)
		{
			mCurrMusicName = name;
			SimpleAudioEngine::getInstance()->playBackgroundMusic(
				ResourceUtils::getSoundMusicPath(name).c_str(), true);
		}
	}	
}

//播放音效
void SoundUtils::playEffect(const string& name)
{
	if(mIsSoundEffectOn)
	{
		SimpleAudioEngine::getInstance()->playEffect(
			ResourceUtils::getSoundEffectPath(name).c_str());
	}
}