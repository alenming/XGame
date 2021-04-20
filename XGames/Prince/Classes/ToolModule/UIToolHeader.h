#ifndef UIToolHeader_h__
#define UIToolHeader_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "UI/UIDef.h"
#include "ToolDef.h"
#include "ItemTipInfo.h"

//��������Ǽ�
#define MAX_STARS	6
//װ������Ǽ�
#define MAX_EQUIPSTARS	4


enum EnumStarContain
{
	STAR_CONTAIN_TOP,
	STAR_CONTAIN_TOP_BESIDE,
	STAR_CONTAIN_BOTTOM,
	STAR_CONTAIN_BOTTOM_BESIDE,
};

class UIToolHeader: public cocos2d::ui::Button //ImageView 
{
public:

	//����ͨ�õ���ͼƬ������������Ƭ
	static UIToolHeader* create(int templateId);


	static UIToolHeader* create();	
	static UIToolHeader* create(const std::string& mainImage, const int id = -1);
	static UIToolHeader* create(const std::string& mainImage, const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");

//	static UIToolHeader* create(const int& nQua);

	static UIToolHeader* createEx(const std::string& selImage, const std::string& unselImage, const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");

	//
	virtual void onPressStateChangedToNormal() override;
	virtual void onPressStateChangedToPressed() override;
	virtual void onPressStateChangedToDisabled() override;
	//
	void initUI();
	void setIconQua(const int& nQua);
	void setMainIcon(const std::string& normalImage);
	void setMainIconVisible(bool isVisible);
	void setSelectedImg(const std::string& normalImage);
	void setSelected(const bool& isSelected);
	const bool& isSelected();
	//���Ͻǵļ���ͼ��
	void setSkillIcon( const std::string& normalImage);
	//���½�ʹ��ͼ��
	void setUseIcon( const std::string& normalImage);
	void setUseText( const std::string& text);
	//���Ͻǽ�Եͼ��
	void setTiedFateIcon( const std::string& normalImage);
	void setTiedFateIconFlag(bool isVisible);
	//���½�С���ͼ��
	void setRedIcon( const std::string& normalImage);
	void setRedIconFlag( bool flag);
	void setRedDot(bool flag);
	//����Ѫ�� // Parameter: float fPercent����Χ[0,100]
	void setBlood(const float& fPercent);
	//�����������
	void setDiedFlag( bool flag);
	//����ͼ��
	void setMaskIcon( const std::string& normalImage);
	void setMaskFlag( bool isMask, bool isSelected=true);
	//������Ƭ����
	void checkChip(const int nItemId);
	//װ���ɽ���
	void setAdvFlag( bool flag);
	//����װ���Ǽ�
	void setEquipStars(int stars);
	//����
	void setNumEx(int num, cocos2d::Color3B color = cocos2d::Color3B(0xff,0xff,0xff));
	const int getNum();
	//�Ǽ� isContain �Ƿ����ͷ����
	void setStarsNum(int num, EnumStarContain starContain=STAR_CONTAIN_BOTTOM_BESIDE);
	//����Ӣ��Ʒ�ʽǱ�  ��ģ��id�����׵ȼ����Ƿ��ǻ�����
	void setQuaCornerIcon(int heroTemplateId, int heroAdvLvl, bool isRobot = false);
	void setQuaCornerIcon(int heroAdvanceId, bool isRobot = false);
	//�ȼ�
	void setLvl(int nLvl);
	//���ò���ԲȦ�����ĵȼ���������
	void setLvlWithoutBg(int nLvl);
	//���׵ȼ�
	void setJJLvl(TOOL_ITEM_QUA nQua, int nLv);

	//���õ����������������
	void setCurNumAndNeedNum(int curNum, int needNum);
	void setCurNumAndNeedNumColor(cocos2d::Color3B clr);
	//���õײ�����
	void setBottomNum(std::string strText);
	void setBottomNumColor(Color3B color);
	//�������Ͻ�+��
	void setAddCornerIcon(bool bAdd = true);
	//����״̬
	void reset();

	void setGray(const bool isGray);

	void setScale(float fScale);
//protected:
	void setStateImg(const std::string &normalImage,
		const std::string& selectedImage = "",
		const std::string& disableImage = "");	

	void setToolId(const int& id);
	int getToolId();
	void setToolDisIndex(const int& index);
	int getToolDisIndex();
	//��ʾװ����Ч
	void showEquipEffe();
	void removeEquipEffe();

	//��ʾ������Ч
	void showMatEffect(bool showEffect);

	// ��ʽ�趨�����Ƿ���Ҫtips;
	void setTipsEnabled(bool bEnabled);

	// ��ʾ�������;
	void showTianMingFlag(bool bShow);

	// �����ȼ�;
	void setCLLevel(int nCLLevel);

	// ����;
	void lock(bool blocked);

	//�������������Ч���ֶ�����
	void showLightEff();

	//��Ӹ��ӿڣ��ⲿ��д�����¼�Ҫ��showTips������ʾ			added by Jerry 2016/4/19
	void showTipsNow(bool isShow, float pos);

	//�����δӵ�е�Ӣ��ͼ�꣬��Ҫ��ʾ��ʼӢ���Ǽ�(pet_base����Ǽ�)��ͬʱ����ʾӢ������
	void showPetBaseHeroStars();

private:
	UIToolHeader();	

	string getFilenameByQua(const int& nQua);

	// ��ʾtips;
	void    showTips(bool bShow);
	

	// ������Ƭ����;
	void setChipMaskIcon(TOOL_ITEM_TYPE itemType, TOOL_ITEM_QUA itemQua);

private:
	cocos2d::ui::ImageView* m_pMainIcon;
	//�ɰ�
	cocos2d::ui::ImageView* m_pFrontImgUnselected;
	cocos2d::ui::ImageView* m_pFrontImgSelected;
	//���Ͻǵļ���ͼ��
	cocos2d::ui::ImageView	*m_pSkillIcon;
	//���½�ʹ��ͼ��
	cocos2d::ui::ImageView	*m_pUseIcon;
	cocos2d::ui::Text		*m_pUseText;
	//���½�С���ͼ��
	cocos2d::ui::ImageView	*m_pRedIcon;
	cocos2d::ui::ImageView	*m_pRedDot;
	//
	cocos2d::ui::ImageView	*m_pTiedFateIcon;
	//���½ǽ���ͼ��
	cocos2d::ui::ImageView	*m_pAdvIcon;
	//��Ƭ����ͼ��
	cocos2d::ui::ImageView	*m_pChipMaskIcon;
	//����ͼ�� ������ר��
	cocos2d::ui::ImageView	*m_pMaskIcon;
	cocos2d::ui::ImageView	*m_pMaskSelectIcon;
	//����ͼ��
	cocos2d::ui::ImageView	*m_pDiedIcon;
	//�Ǽ�
	cocos2d::ui::ImageView *m_pStar[MAX_STARS];
	//���Ͻ�����Ǳ�
	cocos2d::ui::ImageView *m_pHeroConerIcon;
	//װ���Ǽ�
	cocos2d::ui::ImageView *m_pEquipStar[MAX_EQUIPSTARS];
	//����
	cocos2d::ui::Text *m_pName;
	//Ѫ��
	cocos2d::ui::ImageView *m_pBloodBg;
	cocos2d::ui::LoadingBar *m_pBloodBar;
	//�ȼ�����
	cocos2d::ui::ImageView	*m_pLvlIcon;
	cocos2d::ui::Text *m_pLvlText;
	//����
	cocos2d::ui::Text *m_pNum;	
	int m_Num;
	cocos2d::ui::ImageView	*m_pNumExIcon;
	cocos2d::ui::Text *m_pNumEx;

	//���ߵ�ǰ��������������
	cocos2d::ui::Text* m_pCurNumAndNeedNum;
	//���õײ�����
	cocos2d::ui::Text* m_pTextBottomNum;

	//�������Ͻ�+��
	cocos2d::ui::ImageView* m_pAddIcon;
	bool m_isSelected;

	float m_fScaleValue;

	//Ʒ��
	int		m_nIconQua;

	//����id
	int m_ntoolId;
	//���߷ָ��Ӻ����ʾλ��
	int m_ntoolDisIndex;

	// �����Ƿ���Ҫ��ʾtips;
	bool  m_bEnableTips;

	// ��ʾ��;
	ItemTipInfo*  m_pnlTips;

	// �������;
	cocos2d::ui::ImageView* m_imgTianMing;

	// ��;
	cocos2d::ui::ImageView* m_imgLock;

	// ���׵ȼ�;
	cocos2d::ui::ImageView*	m_imgJJLv;
};

#endif
