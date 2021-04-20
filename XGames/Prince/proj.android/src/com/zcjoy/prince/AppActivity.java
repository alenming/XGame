/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package com.zcjoy.prince;


import java.util.Locale;

import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import com.excelliance.kxqp.sdk.GameSdk;
import com.excelliance.kxqp.sdk.IQueryUpdateCallback;
import com.u8.sdk.U8CocosActivity;
import com.u8.sdk.U8SDK;

import android.R.string;
import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;

public class AppActivity extends U8CocosActivity{

	private static AppActivity mInstance;
	
	
	public static String hostip;             //本机IP     
    public static String hostmac;            //本机MAC
    public static String hostIMEI;	           //本机IMEI
    private static PowerManager powerManager = null;
	private static WakeLock wakeLock = null;  
    
	public AppActivity(){
		mInstance = this;
	}
	
	@SuppressLint("InlinedApi") @SuppressWarnings("deprecation")
	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		// 初始化时读取manifest中的版本号，并写入xml;
		String strVersion = getVersion();
		Log.d("AppActivity", "++++++++++ read version:" + strVersion);
		writeVersion(strVersion);
		
		// 初始化更新环境;
		Update.initUpdateEnv(this);
		Log.d("AppActivity", "++++++++++ initUpdateEnv done");
		
		hostip = getLocalIpAddress();  //获取本机IP
        hostmac = getLocalMacAddress();//获取本机MAC
		hostIMEI = getLocalIMEI();

        powerManager = (PowerManager) this.getSystemService(Service.POWER_SERVICE);
		wakeLock = AppActivity.powerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Lock");
		wakeLock.setReferenceCounted(false);
	}

	public static boolean setScreenLightOn()
	{
		wakeLock.acquire();
		return true;
	}
	
	public static boolean setScreenLightOff()
	{
		wakeLock.release();
		return true;
	}
	
	public static String getMacAdress()
	{
		return hostmac;
	}
	
	public static String getIMEI(){
        return hostIMEI;
    }
	
	public static String getIpAdress()
	{
		return hostip;
	}
	
	// override onCreateView (by Phil 12/08/2014 @zcjoy)
	public Cocos2dxGLSurfaceView onCreateView() {
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this); 
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8); 
        return glSurfaceView; 
    } 

	public static Object getJavaActivity(){
		return mInstance;
	}
	
	/************************************************** 
	* @Title: lb_checkUpdate
	* @Description: 检查更新;
	* @return 
	**************************************************/
	public void lb_checkUpdate() {
		final IQueryUpdateCallback callBack = new IQueryUpdateCallback() {
		    public void onUpdateResult(int result) {
		    	/*
		    	 * 请求更新结果result说明：-1：请求失败
		    	 * 							1：未知错误
		    	 * 							2：没有更新
		    	 * 							3：有非强更版本
				 *							4：有强更版本
		    	 */
		        Log.d("", "result=" + result);
		        
		        // 若需后续处理，可将此值回传至游戏逻辑层;
		        onLbCheckUpgradeDone(String.valueOf(result));
		    }
		};
		GameSdk.queryUpdate(getContext(), callBack,true);
	}
	
	/************************************************** 
	* @Title: initUpdateStoragePath
	* @Description: 初始化更新存储空间
	* @return String
	**************************************************/
	public static String initUpdateStoragePath() {
		int nRet = Update.initUpdateStoragePath();
		Log.d("AppAvtivity", "++++++++++ java: initUpdateStorage = " + nRet);
		return String.valueOf(nRet);
	}
	
	/************************************************** 
	* @Title: OnPatchFileDownloadFinished 
	* @Description: 增量更新完毕
	* @param patch_MD5 
	* @return void
	**************************************************/
	public void OnPatchFileDownloadFinished(String patch_MD5){
		Log.d("AppAvtivity", "++++++++++ patch file:" + patch_MD5);
		
		// 分割字符串;
		// [0]-补丁文件路径
		// [1]-当前安装版本的md5
		String param[] = patch_MD5.split("\\|");
		new Update().update(param[0], param[1].toLowerCase(Locale.getDefault()));
    }
	
	/************************************************** 
	* @Title: onFullApkDownloadFinished 
	* @Description: 全量更新完毕
	* @param fullApk 新包路径
	* @return void
	**************************************************/
	public void onFullApkDownloadFinished(String fullApk) {
		new Update().fullUpdate(fullApk);
	}
	
	/************************************************** 
	* @Title: checkAlreadyDownloaded 
	* @Description: 1.检查是否有已下载但是未安装的更新包
	* 				  2.清理更新目录
	* @return void
	**************************************************/
	public void checkAlreadyDownloaded(String strVersion) {
		Update.checkAndCleanUpdateDir(getVersion());
		Log.d("AppActivity", "++++++++++ checkAndCleanUpdateDir done");
	}
	
	public String getLocalMacAddress() {     
	        WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);     
	        WifiInfo info = wifi.getConnectionInfo();     
	        return info.getMacAddress();     
	}     
	
	public String getLocalIpAddress() {     
		WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);     
        WifiInfo info = wifi.getConnectionInfo();     
        int ipAddress = info.getIpAddress(); 
        String ip = intToIp(ipAddress);
        return ip;
    }
	
	public String getLocalIMEI(){
        if(hostIMEI!=null && hostIMEI.length()>0)
            return hostIMEI;
        try
        {
            TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE); 
            hostIMEI = tm.getDeviceId();
        }
        catch (Exception e) {
           //permission deny? 
        }
        return hostIMEI;
    }
	
	private String intToIp(int i) {
       return (i & 0xFF ) + "." +       
       ((i >> 8 ) & 0xFF) + "." +       
       ((i >> 16 ) & 0xFF) + "." +       
       ( i >> 24 & 0xFF);  
    } 
	
	// 获取logo;
	public static String getLogoImg(){
		String LogoImg = "";
		LogoImg = U8SDK.getInstance().getLogoImg();
		Log.d("AppAvtivity", "++++++++++ java: getLogoImg = " + LogoImg);
		return LogoImg;

	}

	// 获取版本号;
	public String getVersion(){
		try {
				//PackageManager manager = this.getPackageManager();
				//PackageInfo info = manager.getPackageInfo(this.getPackageName(), 0);
				//String version = info.versionName;
				return  U8SDK.getInstance().getVersionName();
		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
	}
    
    public static String getChannelId() {
    	int nChannelId = U8SDK.getInstance().getCurrChannel();
    	Log.d("AppAvtivity", "++++++++++ java: getChannelId = " + nChannelId);
    	return String.valueOf(nChannelId);
    }
    
    public static String getUaid() {
    	int uaid = U8SDK.getInstance().getUaid();
    	Log.d("AppAvtivity", "++++++++++ java: getUaid = " + uaid);
    	return String.valueOf(uaid);
    }
    
    public static String getUwid() {
    	int uwid = U8SDK.getInstance().getUwid();
    	Log.d("AppAvtivity", "++++++++++ java: getUaid = " + uwid);
    	return String.valueOf(uwid);
    }
    
    public static String getCentralServer() {
    	String server = U8SDK.getInstance().getCentralServer();
    	Log.d("AppAvtivity", "++++++++++ java: getCentralServer = " + server);
    	return server;
    }
    
    public static String isTianTuoConfig() {
    	String configFlag = U8SDK.getInstance().isTianTuoConfig();
    	Log.d("AppAvtivity", "++++++++++ java: isTianTuoConfig = " + configFlag);
    	return configFlag;
    }
    
    public static String getTianTuoGid() {
    	int gid = U8SDK.getInstance().getTianTuoGid();
    	Log.d("AppAvtivity", "++++++++++ java: getTianTuoGid = " + gid);
    	return String.valueOf(gid);
    }
    
    public static String getTianTuoKey() {
    	String tianTuoKey = U8SDK.getInstance().getTianTuoKey();
    	Log.d("AppAvtivity", "++++++++++ java: getTianTuoKey = " + tianTuoKey);
    	return tianTuoKey;
    }
    
    public static String getSubChannel() {
    	String subChannel = U8SDK.getInstance().getSubChannel();
    	Log.d("AppAvtivity", "++++++++++ java: getSubChannel = " + subChannel);
    	return subChannel;
    }

	public static native int writeVersion(String version);
	public static native int showUpdateMessage(String message);
	public static native int onLbCheckUpgradeDone(String resultCode);
}
