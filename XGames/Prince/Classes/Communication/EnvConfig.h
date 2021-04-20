#ifndef EnvConfig_h__
#define EnvConfig_h__

#define CONTACT(s1, s2, s3)	s1 s2 s3

//正式环境
//#define LOGIN_RELEASE

//测试环境
#define LOGIN_TEST

#ifdef LOGIN_TEST
#ifdef LOGIN_RELEASE
#undef LOGIN_RELEASE
#endif 

// [测试用]中心服务器IP地址;

//#define _TEST_CENTRAL_SERVER_IP_	"192.168.11.211"
#define _TEST_CENTRAL_SERVER_IP_	"157.119.247.3"

#endif

// PC/MAC下用到的和服务器约定的appID 和 签名
#define CONTACT_INT_APPID		1
#define CONTACT_KEY			"bfbeea4bbeb6883477295d950a2e41f2"

/**************** 新手引导配置 ********************/

//新手引导是否开放，关闭则无新手引导，正式发布打开此项
#define GUIDE_ENABLE			true
//新手引导进度是否开放，关闭此项则不初始化服务端引导记录，正式发布打开此项
#define GUIDE_PROCESS_ENABLE	true

/**************** 更新配置 ********************/
//是否开放更新，正式发布打开此项
// 0-NO  1-YES;
#define UPDATE_ENABLE		0

/**************** SDK环境识别配置; ********************/
// 0-NO  1-YES;
#define   LOGIN_WITH_SDK	0

/**************** 天拓数据统计环境 ********************/
// 0-test  1-release
#define	  PUBLISHER_ENV     0

/**************** TalkingDataSDK环境识别配置; ********************/
// 0-NO  1-YES;
#define   TALKING_DATA	0

/**************** 天拓官网包配置; ********************/
// 0-NO  1-YES
#define	  LOGIN_WITH_TT		0

#endif //EnvConfig_h__