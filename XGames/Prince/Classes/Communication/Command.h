#ifndef client1_commond_h
#define client1_commond_h

#define nNET_CMD_NET_HTTP_ERROR					0x00fff003	//网络http错误

#define nNET_CMD_HEART_BEAT						1		// 心跳;
#define nNET_CMD_SERVER_CLOSE					2		// 服务器关闭连接;
#define nNET_CMD_RECONNECT						9999	// 断线重连;
#define nNET_CMD_LOGOUT							9998	// 登出操作
#define nNET_CMD_REPEAT_LOGIN					905		// 重复登录踢人消息


//////登录注册相关
#define nMAIN_CMD_LOGIN							100001	//http登录
#define nMAIN_CMD_REGISTER						100003	//注册帐号
#define nMAIN_CMD_MODIFY_PWD					100004	//修改密码
#define nMAIN_CMD_TRY_GAME						100005	//游客试玩, 协议同100001
#define nMAIN_CMD_FIRST_TRY_GAME				100006	//游客第一次试玩，另有协议
#define nMAIN_CMD_ACTIVATE						100007	//激活试玩帐号
#define nMAIN_CMD_TT_CHECK_ACCOUNT				100008	//校验账户是否可用[天拓登陆];
#define nMAIN_CMD_SEND_DEV_ID					100009	//发送设备唯一标识

#define nMAIN_CMD_GET_ZONE_LIST					100002	//拉取分区列表

#define nMAIN_CMD_LOGIN_ROLE					10000	//角色登录
#define nMAIN_CMD_CREATE_ROLE					10001	//角色创建
#define nMAIN_CMD_DOWNLOAD_FORMATION			10002	//获取阵型信息;
#define nMAIN_CMD_UPLOAD_FORMATION				10003	//上传阵型信息;
#define nMAIN_CMD_LEVEL_UP						10004	//角色升级
//#define nMAIN_CMD_MODIFY_PWD					10005	//修改密码主命令码
//#define nMAIN_CMD_REQUEST_ROLE				10006	//创建role

// SDK登陆游戏相关;
#define nMAIN_CMD_SDK_LOGIN_ROLE				11000	// sdk角色登陆;
#define nMAIN_CMD_SDK_CREATE_ROLE				98001	// sdk角色创建;

//#define nMAIN_CMD_HERO_REPLACE					10004	//英雄替换
#define nMAIN_CMD_HERO_INFO						10005	//获取角色全部信息
//#define nMAIN_CMD_HERO_PREVIEW_ADVANCE_INFO		10006	//进阶预览消息
#define nMAIN_CMD_HERO_ADVANCE_INFO				10007	//进阶消息
#define nMAIN_CMD_HERO_STAR_UP_INFO				10008	//升星消息;
#define nMAIN_CMD_TRAINING_ADD					10009	//训练模块新增或替换英雄;
#define nMAIN_CMD_TAKE_CARD_INFO				10010   //获取抽卡信息
#define nMAIN_CMD_TAKE_CARD						10011   //获取抽卡信息
#define nMAIN_CMD_GET_STAR_INFO					10012   //获取星宿信息
#define nMAIN_CMD_LIGHT_STAR					10013   //点亮星星
#define nMAIN_CMD_GET_STAR_CAMP_INFO			10014   //获取星盘信息
#define nMAIN_CMD_SHOP_STAR_STONE				10015   //购买星石
#define nMAIN_CMD_XIANG_QIAN_STAR_STONE			10017   //获取镶嵌晶石
//#define nMAIN_CMD_ACTIVE_FATE					10015   //激活羁绊
#define nMAIN_CMD_TRAINING_ADD_EXP				10016	//训练模块通过道具给英雄增加经验;
//#define nMAIN_CMD_TRAINING_UNLOCK_POS			10017	//训练模块解锁一个训练位;
#define nMAIN_CMD_COMMON_SHOP_CHOU				10019	//道具抽单抽十连抽
#define nMAIN_CMD_COMMON_FREE_CHOU				10024	//是否为免费抽
#define nMAIN_CMD_EXCHANGE_CHIPS				10009	//兑换万能碎片

//阵法相关协议
#define nMAIN_CMD_TACTICS_MAIN_LIST				10020	//阵法列表
#define nMAIN_CMD_TACTICS_USE_TACTICS			10021	//使用阵法
#define nMAIN_CMD_TACTICS_ROW_UPGRADE			10022	//排强化
#define nMAIN_CMD_TACTICS_UPGRADE				10023	//阵法强化

#define nMAIN_CMD_GUIDE_PROCESS					10100	//新手引导;

#define nMAIN_CMD_TALENT_REPLACE				10018	//主角技能替换(天赋)

#define nMAIN_CMD_RANDOM_NAMES					10101	//返回随机名字列表，角色创建使用

#define nMAIN_CMD_GET_TASK_NEW_INFO				11001   //获取新服奖励信息
#define nMAIN_CMD_GET_TASK_NEW_REWARD			11002   //获取新服奖励
#define nMAIN_CMD_NEWAREA_ACT_PUSH_INFO			11010	//新服奖励推送
#define nMAIN_CMD_GET_TASK_SIGN_INFO			11003   //获取签到奖励信息
#define nMAIN_CMD_GET_TASK_SING_REWARD			11004   //获取签到奖励
#define nMAIN_CMD_OPERATE_ACT_INFO              11011   //获取运营活动信息
#define nMAIN_CMD_OPERATE_ACT_TASK              11012   //获取运营活动奖励task
#define nMAIN_CMD_OPERATE_ACT_FINANCIAL         11013   //获取理财通
#define nMAIN_CMD_OPERATE_SUPPLEMENT            11015   //获取活动配置
#define nMAIN_CMD_OPERATE_VIPGIFT               11016   //超值礼包


#define nMAIN_CMD_GET_ALL_TOOL_INFO				12001   //获取玩家所有道具信息
#define nMAIN_CMD_EQUIP_STRENGTHEN				12002	//道具强化
#define nMAIN_CMD_EQUIP_STRENGTHEN_HORSE_BOOK	12003	//马书强化
#define nMAIN_CMD_TOOL_EQUIP_CL					12004	//装备淬炼
#define nMAIN_CMD_TOOL_EQUIP_CL_DEGRADE			12013	//装备淬炼降级
#define nMAIN_CMD_EQUIP_ADVANCED				12005	//道具进阶（合成）
#define nMAIN_CMD_TOOL_PUTON					12006	//穿上某个部位装备
#define nMAIN_CMD_TOOL_USE						12008	//使用道具
#define nMAIN_CMD_TOOL_SYNTHESIS				12009	//合成道具

#define nMAIN_CMD_REFINE_CARD_SKILL			12010	//炼化技能

#define nMAIN_CMD_GET_SHOP_TOOLS			12014   //获取道具列表
#define nMAIN_CMD_BUY_SHOP_TOOLS			12015   //购买道具
#define nMAIN_CMD_RECYCLE_SHOP_TOOLS		12016   //回收道具
#define nMAIN_CMD_OPEN_CHEST				12018	//开宝箱
//#define nMAIN_CMD_REFRESH_SCORE_STORE		12019	//竞技场积分商城
//#define nMAIN_CMD_SCORE_STORE_BUY			12020	//购买竞技场积分商城道具

#define nMAIN_CMD_GET_SHOP_TOOLS_VIP			12021	//获取道具商城列表
#define nMAIN_CMD_BUY_SHOP_TOOLS_VIP			12022	//购买道具商城道具

#define nMAIN_CMD_FILISH_EXPERIENCE				13001	//完成阅历
#define nMAIN_CMD_HERO_FATE_GET_INFO			13002	//获取主角缘分主界面信息
#define nMAIN_CMD_HERO_FATE_TIED_FATES			13003	//结识主角缘分
#define nMAIN_CMD_HERO_FATE_CANCEL_FATES		13004	//取消主角缘分
#define nMAIN_CMD_HERO_FATE_REFRESH_FATES		13005	//刷新缘分
#define nMAIN_CMD_HERO_FATE_REPLACE_FATES		13006	//替换缘分

// PVE关卡/战斗相关消息定义;
#define nMAIN_CMD_FIGHT_RECORD				14001	// 关卡进度;
#define nMAIN_CMD_FIGHT_HERO_INFO			14002	// 取战斗英雄的全部信息;
#define nMAIN_CMD_FIGHT_MAZE				14040	// 迷宫战斗
#define nMAIN_CMD_SETTLEMENT				14003	// 结算;
#define nMAIN_CMD_STAR_REWARD				14004	// 领取星级奖励;
#define nMAIN_CMD_SWEEP_START				14005	// 扫荡开始;
#define nMAIN_CMD_SWEEP_SETTLEMENT			14006	// 扫荡结算;
#define nMAIN_CMD_ACTIVITY					14007	// 请求活动关卡进度数据;
#define nMAIN_CMD_ACTIVITY_BUY				14060	// 购买活动关卡的通关次数;
#define nMAIN_CMD_RESET_BARRIER				14008	// 重置关卡;
#define nMAIN_CMD_ACTIVITY_FIGHT			14061	// 活动关卡战斗;
#define nMAIN_CMD_ACTIVITY_SETTLEMENT		14062	// 活动关卡结算;
#define nMAIN_CMD_ACTIVITY_SWEEP			14063	// 活动关卡扫荡;

// PVP竞技相关消息定义;
#define nMAIN_CMD_ARENA_MAIN				14009	// 竞技场主界面信息;
#define nMAIN_CMD_PVP_FIGHT_INFO			14010	// PVP战斗数据请求;
#define nMAIN_CMD_PVP_FIGHT_SETTLEMENT		14011	// PVP战斗结果;
#define nMAIN_CMD_ARENA_CHANGE_OP			14012	// 竞技场换批对手;
#define nMAIN_CMD_ARENA_RANK				14013	// 竞技场排行榜;
#define nMAIN_CMD_ARENA_GET_PRIZE			14054	// 领竞技场胜场奖励;
#define nMAIN_CMD_ARENA_GET_PVP_SHOP		14055	// 获取荣誉商店商品;
#define nMAIN_CMD_ARENA_SINGLE_OP			14049	// 竞技场单人数据更新;
#define nMAIN_CMD_ARENA_BUY_COUNT			14055	// 竞技场购买次数;

#define nMAIN_CMD_MAZE_GET_ALLINFO			14024	// 获取迷宫信息
#define nMAIN_CMD_MAZE_RANDOM_ROAD			14025	// 迷宫随机路口事件
#define nMAIN_CMD_MAZE_EVENT_THINGS_INFO    14026	// 迷宫获取各个事件个信息

#define nMAIN_CMD_MAZE_OPEN_BOX				14027	// 迷宫打开宝箱
#define nMAIN_CMD_MAZE_OPEN_MAP_BOX		    14028	// 迷宫打开藏宝图宝箱翻牌
#define nMAIN_CMD_MAZA_OPEN_BOX_END         14029   // 迷宫结束打开宝箱
#define nMAIN_CMD_MAZE_LIMIT_BUY			14041	// 迷宫限时购买

// 掠夺消息定义;
#define nMAIN_CMD_ROB_REFRESH_ENEMY			14042	// 掠夺刷新对手;
#define nMAIN_CMD_ROB_GET_FIGHT_INFO		14043	// 掠夺获取战斗信息;
#define nMAIN_CMD_ROB_SAVE_FIGHT_INFO		14044	// 掠夺保存战斗信息;
#define nMAIN_CMD_ROB_SAVE_BOX_INFO			14045	// 掠夺抽卡;
#define nMAIN_CMD_ROB_GET_ROB_RECORD		14046	// 掠夺获取掠夺记录;
#define nMAIN_CMD_ROB_GET_ROB_TEN_INFO		14047	// 掠夺10次结果信息;
#define nMAIN_CMD_ROB_GET_ROB_TEN_REWARD	14048	// 掠夺10领奖;
#define nMAIN_CMD_ROB_GET_CHIPS				14021	// 掠夺获取宝物碎片;
#define nMAIN_CMD_ROB_AVOID_WAR				14022	// 掠夺免战;
#define nMAIN_CMD_ROB_COMPOSE				14023	// 掠夺合成宝物;
#define nMAIN_CMD_ROB_PUSH_UPDATE			99005	// 掠夺推送更新;

// 世界Boss协议
#define nMAIN_CMD_BOSS_MAIN_LIST			17001	// boss主界面
#define nMAIN_CMD_BOSS_FIGHT				17002	// boss战斗
#define nMAIN_CMD_BOSS_SETTLEMENT			17003	// boss结算
#define nMAIN_CMD_BOSS_INSPIRE				17004	// boss鼓舞
#define nMAIN_CMD_BOSS_CLEAR_COOLTIME		17005	// boss清除冷却
#define nMAIN_CMD_BOSS_GET_REWARD			17006	// boss领取奖励
#define nMAIN_CMD_BOSS_CUR_DAMAGE_RANK		17007	// boss本轮伤害排名

// 爬塔/试炼相关消息定义;
#define nMAIN_CMD_TRAIL_MAIN_INFO			14031	// 试炼主界面信息;
#define nMAIN_CMD_TOWER_MAIN_INFO			14014	// 爬塔主界面信息;
#define nMAIN_CMD_TRAIL_GET_FORMATION		14032	// 获取试炼阵型;
#define nMAIN_CMD_TRAIL_SET_FORMATION		14033	// 设置试炼阵型;
#define nMAIN_CMD_TRAIL_RESET				14034	// 重置试炼;
#define nMAIN_CMD_TOWER_RESET				14020	// 重置爬塔;
#define nMAIN_CMD_TRAIL_FIGHT_INFO			14035	// 试炼战斗;
#define nMAIN_CMD_TRAIL_SETTLEMENT			14036	// 试炼战斗结算;
#define nMAIN_CMD_TRAIL_BUY_BUFF			14037	// 试炼兑换buff;
#define nMAIN_CMD_TRAIL_RANKING				14038	// 试炼排行榜;
#define nMAIN_CMD_TRAIL_REWARD				14039	// 获取试炼奖励;
#define nMAIN_CMD_TRAIL_GET_TRAIL_OP		14050	// 获取试炼对手;
#define nMAIN_CMD_TRAIL_OPEN_TREASURE		14051	// 开启一个宝藏;
#define nMAIN_CMD_TRAIL_REFRESH_TREASURE	14052	// 刷新宝藏;
#define nMAIN_CMD_TOWER_BUY_CHALLENGE_TIMES	14053	// 爬塔购买挑战次数;
#define nMAIN_CMD_TOWER_FIGHT_INFO			14015	// 爬塔战斗;
#define nMAIN_CMD_TOWER_SETTLEMENT			14016	// 爬塔战斗结算;
#define nMAIN_CMD_TOWER_SWEEP				14017	// 爬塔扫荡;
#define nMAIN_CMD_TOWER_SWEEP_SETTLEMENT	14018	// 爬塔扫荡结算;
#define nMAIN_CMD_TOWER_RANKING				14019	// 爬塔排行榜;

// 公共商店消息;
#define	nMAIN_CMD_SHOPEXC_REFRESH			12019	// 刷新;
#define	nMAIN_CMD_SHOPEXC_BUY				12020	// 购买道具;帮派BOSS伤害奖励领取

// 炼化炉商店消息;
#define nMAIN_CMD_SHOPEXC_REFINE_REFRESH	12011	// 炼化炉商店刷新;
#define nMAIN_CMD_SHOPEXC_REFINE_BUY		12012	// 炼化炉商店购买;

// 货币购买
#define nMAIN_CMD_COMMON_BUY_GOLD_POWER		12023	// 金币购买体力;
#define nMAIN_CMD_COMMON_BUY_GOLD_ENERGY	12024	// 金币购买精力;
#define nMAIN_CMD_COMMON_BUY_GOLD_COIN		12025	// 金币购买银币;
#define nMAIN_CMD_COMMON_BUY_GOLD_MAZEPOWER	14030	// 金币购买迷宫行动力;

#define nMAIN_CMD_LOVE_GET_INFO				15001	// 主界面信息;
#define nMAIN_CMD_LOVE_GIVE_GIFT			15002	// 送礼
#define nMAIN_CMD_LOVE_PLAY					15003	// 游玩
#define nMAIN_CMD_LOVE_PLAY_MAP_CHOOSE		15004	// 选择任务地图
#define nMAIN_CMD_LOVE_PLAY_MAP_EVENT		15005	// 地图事件
#define nMAIN_CMD_LOVE_FINGER_GUESS		    15007	// 出拳
#define nMAIN_CMD_LOVE_GET_BUY			    15008	// 购买礼物
#define nMAIN_CMD_LOVE_GET_BUY_GUESS_INFO	15009	// 获取购买猜拳信息
#define nMAIN_CMD_LOVE_GET_BUY_GUESS		15010	// 获取购买猜拳次数
#define nMAIN_CMD_LOVE_ADD_ATTRIBUTE		15013	// 增加属性
#define nMAIN_CMD_LOVE_RESET_ATTRIBUTE		15014	// 重置属性
#define nMAIN_CMD_LOVE_CHANGE_GUESS 		15011	// 修改猜拳结果
#define nMAIN_CMD_LOVE_GET_GUESS_PRIZE      15012   // 领取奖励

#define nMAIN_CMD_STARSPRITES_GET_ALL_INFOS	16001	//获取所有精灵信息
#define nMAIN_CMD_STARSPRITES_CALL			16002	//召唤精灵
#define nMAIN_CMD_STARSPRITES_GET_INFO		16003	//精灵详情接口
#define nMAIN_CMD_STARSPRITES_LIGHT			16004	//精灵升星
#define nMAIN_CMD_STARSPRITES_APPEND		16005	//精灵附体
#define nMAIN_CMD_STARSPRITES_FEED			16006	//精灵喂养
#define nMAIN_CMD_STARSPRITES_FEED_FULL		16007	//精灵喂养一键喂饱
#define nMAIN_CMD_STARSPRITES_SKILL_ADV		16008	//技能升级

//聊天、好友相关
#define nMAIN_CMD_CHAT_PUSH_MESSAGE				20000	// 系统消息(服务器推送过来)
#define nMAIN_CMD_CHAT_SYS_NOTICE				20001	// 跑马灯公告
#define nMAIN_CMD_CHAT_SEND_MESSAGE				20002	// 玩家发送聊天消息(客户端推送过去)
#define nMAIN_CMD_CHAT_PLAYER_BEHAVIOR_MESSAGE  20003	// 玩家行为消息
#define nMAIN_CMD_FRIEND_GET_INFOS				20004	// 获取好友信息
#define nMAIN_CMD_FRIEND_GET_FRIEND_LIST		20005	// 获取好友列表
#define nMAIN_CMD_FRIEND_SEND_POWER				20006	// 送体力
#define nMAIN_CMD_FRIEND_ONE_KEY_SEND			20007	// 一键赠送
#define nMAIN_CMD_FRIEND_GET_POWER			    20014	// 领体力
#define nMAIN_CMD_FRIEND_ONE_KEY_GET_POWER		20010	// 领取体力
#define nMAIN_CMD_FRIEND_APPLY           		20013	// 好友申请
#define nMAIN_CMD_FRIEND_ADD_FRIEND				20008	// 添加好友
#define nMAIN_CMD_FRIEND_DELETE_FRIEND			20009	// 删除好友
#define nMAIN_CMD_FRIEND_RECOMMEND_LIST			20011	// 获取推荐好友
#define nMAIN_CMD_FRIEND_SEARCH_FRIEND			20012	// 搜索好友
#define nMAIN_CMD_FRIEND_APPLY_FRIEND			20021	// 拒绝/同意好友
#define nMAIN_CMD_FRIEND_ONESKY_APPLY_FRIEND	20015	// 一键拒绝/同意好友
#define nMAIN_CMD_FRIEND_TUI_SONG           	20020	// 推送消息

#define nMAIN_CMD_MAIL_GET_MAILS				21001   //获取邮件
#define nMAIN_CMD_MAIL_OPEN_MAIL				21002   //打开邮件
#define nMAIN_CMD_MAIL_GET_ACCESSORY			21003   //领取附件
#define nMAIN_CMD_MAIL_DELETE_MAIL				21004   //删除邮件

#define nMAIN_CMD_TASK_PUSH_INFO				11005	//任务推送
#define nMAIN_CMD_TASK_RECEIVE					11006	//任务领取
#define nMAIN_CMD_TASK_GET_LIST					11007	//拉取任务列表

#define nMAIN_CMD_SIGN_GET_LIST                 11003   //拉取签到列表
#define nMAIN_CMD_SIGN_RECEIVE                  11004   //签到领取
#define nMAIN_CMD_MAR_ACTIVE		            11008	//精彩活动

//充值界面
#define nMAIN_CMD_VIP_GET_LIST					12026   //获取VIP主界面信息
#define nMAIN_CMD_VIP_BUY_GIFT					12027   //购买VIP获得的数据
#define nMAIN_CMD_VIP_RECHANGE					12028   //充值VIP
#define nMAIN_CMD_VIP_GET_ORDERID				12031   //获取订单号

//玩家信息界面
#define nMAIN_CMD_PLAYER_INFO_MODIFY_NAME		12029	//修改昵称
#define nMAIN_CMD_PLAYER_INFO_CDKEY				5721	//兑换码

#define nMAIN_CMD_RANK_TOP_LIST					12030	//排行榜协议

// 充值相关;
#define nMAIN_CMD_PAY_RESULT					99003	// 充值结果推送;

#define nMAIN_CMD_BOSS_ACT_STATE				99004	//BOSS活动开关状态推送

//首充奖励领取协议
#define nMAIN_CMD_FIRST_RECHARGE				11014	//首充奖励领取

//帮派相关协议
#define nMAIN_CMD_GROUP_CREATE                  30000   //创建帮派
#define nMAIN_CMD_GROUP_CHANGE_ICON             30001   //修改头像帮派
#define nMAIN_CMD_GROUP_SET                     30002   //帮派设置
#define nMAIN_CMD_GROUP_APPLY_JOIN              30003   //申请加入帮派
#define nMAIN_CMD_GROUP_CANCE_APLLY             30004   //取消申请帮派
#define nMAIN_CMD_GROUP_SEARCH                  30005   //帮派查找
#define nMAIN_CMD_GROUP_MAIN_INFO				30014	//拉取帮派信息		
#define nMAIN_CMD_GROUP_APPLY_CHECK				30006	//新人审批
#define nMAIN_CMD_GROUP_EXPEL					30007	//逐出帮派
#define nMAIN_CMD_GROUP_MODIFY_NOTICE			30008	//修改公告
#define nMAIN_CMD_GROUP_DYN_EVENT				30009	//帮派动态
#define nMAIN_CMD_GROUP_ABDICATE				30010	//帮派转让
#define nMAIN_CMD_GROUP_APPOINT					30011	//任命
#define nMAIN_CMD_GROUP_EXIT					30012	//退出帮派
#define nMAIN_CMD_GROUP_DISBAND					30013	//解散帮派
#define nMAIN_CMD_GROUP_BUILD_INFO				30015	//获取指定的建筑信息
#define nMAIN_CMD_GROUP_BUILD_DONATE			30016	//捐献指定的建筑
#define nMAIN_CMD_GROUP_APPLICANT_LIST			30017	//申请人列表
#define nMAIN_CMD_GROUP_ACTIVITY_CLICK			30018	//点击寻龙活动9个鬼东西
#define nMAIN_CMD_GROUP_ACTIVITY_REFRESH		30019	//寻龙活动刷新
#define nMAIN_CMD_GROUP_ACTIVITY_HISTORY		30020	//寻龙活动历史
#define nMAIN_CMD_GROUP_PUSH					99006	//推送审核通过，被逐出，被解散

//模拟神器协议
#define nMAIN_CMD_ARTIFACT_MAIN					16010	//神器主界面
#define nMAIN_CMD_ARTIFACT_ACTIVE				16011	//神器激活
#define nMAIN_CMD_ARTIFACT_REFINE				16012	//神器洗练
#define nMAIN_CMD_ARTIFACT_SAVE					16013	//神器洗练保存

//限时神将转议
#define nMAIN_CMD_LIMITTIMEHERO_CONFIG			10025	//活动配置 
#define nMAIN_CMD_LIMITTIMEHERO_INFO			10026	//活动里玩家的抽卡信息
#define nMAIN_CMD_LIMITTIMEHERO_CARD			10027	//抽卡和抽宝匣
#define nMAIN_CMD_LIMITTIMEHERO_GETBOX			10028	//领积分奖励

//金币兑换
#define nMAIN_CMD_GOLD_ACTIVITY_INFO            10029   //拉取信息
#define nMAIN_CMD_GOLD_ACTIVITY_CHANGE          10030   //抽奖
#define nMAIN_CMD_GOLD_ACTIVITY_CHANGEINFO      10031   //推送获取金币信息

//帮派boss协议
#define nMAIN_CMD_GROUP_BOSS_RANK				30021	//拉取排名
#define nMAIN_CMD_GROUP_BOSS_MAIN				30022	//拉取主界面
#define nMAIN_CMD_GROUP_BOSS_INSPIRE			30023	//鼓舞
#define nMAIN_CMD_GROUP_BOSS_GET_REWARD			30024	//领取奖励
#define nMAIN_CMD_GROUP_BOSS_EXCHANGE			30025	//锦囊兑换
#define nMAIN_CMD_GROUP_BOSS_FIGHT				30026	//战斗
#define nMAIN_CMD_GROUP_BOSS_SETTLEMENT			30027	//战斗结算
#define nMAIN_CMD_GROUP_BOSS_GET_BOSS_REWARD	30028	//领取boss宝箱奖励


#endif
