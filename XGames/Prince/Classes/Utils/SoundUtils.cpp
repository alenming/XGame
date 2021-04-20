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

//�������ֹر�/����
void SoundUtils::setSoundMusicOn(bool isOn)
{
	mIsSoundMusicOn = isOn;
	UserDefault::getInstance()->setBoolForKey("isMusicOn", isOn);
	if (!isOn)
	{
		//�ر���Ч��Ҫ��ռ�¼�ĵ�ǰ���ŵ�����
		mCurrMusicName = "";
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
}

//������Ч�ر�/����
void SoundUtils::setSoundEffectOn(bool isOn)
{
	mIsSoundEffectOn = isOn;
	UserDefault::getInstance()->setBoolForKey("isEffectOn", isOn);
}

//��ȡ�����Ƿ�ر�
bool SoundUtils::getSoundMusicOn()
{
	return mIsSoundMusicOn;
}

//��ȡ��Ч�Ƿ�ر�
bool SoundUtils::getSoundEffectOn()
{
	return mIsSoundEffectOn;
}

//��������
void SoundUtils::playMusic(const string& name)
{
	if(mIsSoundMusicOn)
	{
		//���ж���ǰ����������Ҫ���ŵ������Ƿ���ͬ���������ͬ�ٲ���
		if (mCurrMusicName != name)
		{
			mCurrMusicName = name;
			SimpleAudioEngine::getInstance()->playBackgroundMusic(
				ResourceUtils::getSoundMusicPath(name).c_str(), true);
		}
	}	
}

//������Ч
void SoundUtils::playEffect(const string& name)
{
	if(mIsSoundEffectOn)
	{
		SimpleAudioEngine::getInstance()->playEffect(
			ResourceUtils::getSoundEffectPath(name).c_str());
	}
}