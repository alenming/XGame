#ifndef LocalDef_h__
#define LocalDef_h__

typedef enum tagLOCAL_KEY
{
	LOCAL_KEY_STR_USERNAME,				//账号
	LOCAL_KEY_STR_PASSWORD,				//密码

	LOCAL_KEY_STR_INSTALL_VERSION,		//安装版本号
	LOCAL_KEY_STR_CURRENT_VERSION,		//当前版本号
	LOCAL_KEY_STR_DOWNLOADED_VERSION,	//下载版本号

	LOCAL_KEY_BOOL_AUTO_FIGHT,			//是否战斗加速
	LOCAL_KEY_INT_STR_FIGHT_SPEED,		//战斗速度

	LOCAL_KEY_STR_DEFAULT_SERVER_ID,	//上次登陆服务器ID
	LOCAL_KEY_STR_TOTAL_PAY_SRV,		//服务器下发的充值总额
	LOCAL_KEY_STR_TOTAL_PAY_LOCAL,		//本地记录的充值总额

	LOCAL_KEY_INT_ROLE_ID,				//上次玩家登录的角色ID(已废弃，但不能删除)
	LOCAL_KEY_STR_ROLE_ID,				//上次玩家登录的角色唯一标识(渠道id+分服id+角色id)

	LOCAL_KEY_INT_USERID,				//试玩玩家UID
	LOCAL_KEY_BOOL_LASTLOGIN_ACCOUT,	//上次玩家登陆是否是试玩

	LOCAL_KEY_STR_TRY_IDENTIFY			//试玩用户登录标识

}LOCAL_KEY;

#endif // LocalDef_h__
