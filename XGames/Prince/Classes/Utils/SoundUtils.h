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

	//���ű������֣�ע��ֻ�贫���ļ��������贫���ļ���׺��
	static void playSoundMusic(const string& name);

	//������Ч��ע��ֻ�贫���ļ��������贫���ļ���׺��
	static void playSoundEffect(const string& name);

public:

	//�������ֹر�/����
	void setSoundMusicOn(bool isOn);

	//������Ч�ر�/����
	void setSoundEffectOn(bool isOn);

	//��ȡ�����Ƿ�ر�
	bool getSoundMusicOn();

	//��ȡ��Ч�Ƿ�ر�
	bool getSoundEffectOn();

	//��������
	void playMusic(const string& name);

	//������Ч
	void playEffect(const string& name);

private:

	static SoundUtils*  m_pInstance;

	//�����Ƿ��
	int mIsSoundMusicOn;

	//��Ч�Ƿ��
	int mIsSoundEffectOn;

	//��ǰ���ŵ���������
	string mCurrMusicName;
};



#endif //SoundUtils__