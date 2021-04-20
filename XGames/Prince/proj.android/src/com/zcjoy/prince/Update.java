package com.zcjoy.prince;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.math.BigInteger;
import java.security.MessageDigest;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

public class Update {
	
	private static Activity appActivity = null;
	private static Handler mHandler = null;
	private static final int _MSG_UPDATE_SD_NOT_EXIST = 10001;
	private static final int _MSG_UPDATE_ROM_MEMORY = 10002;
	private static final int _MSG_UPDATE_SD_MEMORY = 10003;
	private static final int _MSG_UPDATE_PKG_NOT_FOUND = 10004;
	private static final int _MSG_UPDATE_PKG_MODED = 10005;
	
	private static String oldFile_path = "";
	private static String newFile_path = "";
	private static String patchFile_path = "";
	private static String gameUpdateDir = "/ZCJoy/Prince/Update/";
	private static String fullValid_gameUpdateDir = "";
	private static String tmpApk_Name0 = "tmp0.apk";
	private static String newApk_Name = "update_version.apk";
	private static byte[] apkComment = null;
	private static String installMd5 = "";
	
	/************************************************** 
	* @Title: initUpdateEnv 
	* @Description: 初始化游戏更新环境
	* @param context
	* @return void
	**************************************************/
	public static void initUpdateEnv(Activity context){
		appActivity = context;
		
		// 消息弹窗;
		mHandler = new Handler(Looper.getMainLooper()) {
		    @Override
		    public void handleMessage(Message message) {
		    	String strReason = "";
		    	switch (message.what)
		    	{
		    	case _MSG_UPDATE_SD_NOT_EXIST:
		    		strReason = "SD卡不存在";
		    		break;
		    		
		    	case _MSG_UPDATE_ROM_MEMORY:
		    	case _MSG_UPDATE_SD_MEMORY:
		    		strReason = "存储空间不足，无法完成更新";
		    		break;
		    		
		    	case _MSG_UPDATE_PKG_NOT_FOUND:
		    		strReason = "未找到原始安装包，无法完成更新，请重新下载安装";
		    		break;
		    		
		    	case _MSG_UPDATE_PKG_MODED:
		    		strReason = "安装包已损坏或被修改，请重新下载安装";
		    		break;
		    	
		    	default:
		    		break;
		    	}
		    	
		    	// 弹窗;
		    	/*new  AlertDialog.Builder(appActivity)
								.setTitle("ERROR")
								.setMessage(strReason)
								.setPositiveButton("确定", new DialogInterface.OnClickListener() {
						            public void onClick(DialogInterface dialog, int which) {
						            	dialog.dismiss();
						                System.exit(0);
						            }
						        })
								.show();*/

		    	// 改为游戏内弹窗;
		    	AppActivity.showUpdateMessage(strReason);
		    }
		};
		
		// 存储空间先不检查，等到有更新时再检查空间;
		// 这样保证无更新时无剩余空间也可以游戏;
		
	}
	
	/************************************************** 
	* @Title: fullUpdate 
	* @Description: 全量更新入口
	* @param context  
	* @param fullApk  完整的APK安装包路径
	* @return void
	**************************************************/
	public void fullUpdate(String fullApk){
		Log.d("com.zcjoy.prince:fullUpdate", "fullAPK = " + fullApk);
		
		// 存储空间在initUpdateStoragePath中已得到确认;
		// 可能是SD卡，也可能是ROM;
		if (fullValid_gameUpdateDir.isEmpty()) {
			return;
		}
		
		// 修改全量更新包访问权限;
		FileUtils.getPermission(fullApk);
		
		// 拷贝至更新目录;
		newFile_path = fullValid_gameUpdateDir + newApk_Name;
		if (FileUtils.copyFile(fullApk, newFile_path)) {
			
			// 删除旧文件;
			File pre = new File(fullApk);
			if (pre.exists()) {
				pre.delete();
			}
			
			// 执行安装;
			FileUtils.installApplication(appActivity, newFile_path);
		}
	}
	
	/************************************************** 
	* @Title: update 
	* @Description: 增量更新入口
	* @param context 
	* @param patch   补丁文件全路径
	* @param oriMd5  初始安装版本的MD5
	* @return void
	**************************************************/
	public void update(String patch, String inMd5) {
		
		Log.d("com.zcjoy.prince:Update", "patch = " + patch);
		Log.d("com.zcjoy.prince:Update", "oriMd5 = " + inMd5);

        // 存储空间在initUpdateStoragePath中已得到确认;
		// 可能是SD卡，也可能是ROM;
		if (fullValid_gameUpdateDir.isEmpty()) {
			return;
		}
		
		// 原始安装包的MD5;
		installMd5 = inMd5;
		
		// 修改patch文件访问权限;
		FileUtils.getPermission(patch);
        
		// 在initUpdateStoragePath中查找过原始安装包，如果找到，会给oldFile_path赋值
    	// 确认原始安装包是否找到;
    	if(oldFile_path.isEmpty()){
    		popUpdateMessage(_MSG_UPDATE_PKG_NOT_FOUND);
			return;
    	}
    	
    	// 确认原始安装包是否有额外的注释信息，如有，清除之，同时检查MD5;
    	String tmpOldFilePath = cleanApk(oldFile_path);
    	if(tmpOldFilePath.isEmpty()) {
    		popUpdateMessage(_MSG_UPDATE_PKG_MODED);
    		return;
    	}
    	oldFile_path = tmpOldFilePath;
		
		// 要合成的新APK路径;
		newFile_path = fullValid_gameUpdateDir + newApk_Name;
		
		// 补丁文件路径;
		patchFile_path = patch;

		// 执行合成;
		exAsyncTask();
	}
	
	/************************************************** 
	* @Title: getOriPackage 
	* @Description: 获取原始安装包
	* @return PackageInfo
	**************************************************/
	private static PackageInfo getOriPackage() {
		PackageInfo info = null;
		
		try {
			// 找到系统备份的原始安装包，名称暂时这样处理(系统一般在1-2循环命名);
			int nMax = 9;
			for (int nIndex = 1; nIndex < nMax; nIndex++){
				oldFile_path = "/data/app/" + appActivity.getPackageName() + "-" + nIndex + ".apk";
				File file = new File(oldFile_path);
				if (file.exists()){
					Log.d("com.zcjoy.prince:Update", "found ori apk = " + oldFile_path);
					PackageManager pm = appActivity.getPackageManager();
					info = pm.getPackageArchiveInfo(oldFile_path, PackageManager.GET_ACTIVITIES);
					break;
				} else {
					oldFile_path = "";
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
    	
		return info;
	}
	
	/************************************************** 
	* @Title: cleanApk 
	* @Description: 校验安装包是否经过修改
	* @param apkFileName  备份目录下拿到的原始安装包
	* @return String   如有修改，返回处理过的安装包
	**************************************************/
	private String cleanApk(String apkFileName){
		
		Log.d("com.zcjoy.prince:cleanApk", "apkFileName = " + apkFileName);
		
		// 保存传入文件名，若校验相同，原样返回;
		String strApkNameRet = "";
		
		try {
			// 校验原始安装包的MD5，若不同，可能有额外的注释信息;
			if(!checkMd5(apkFileName, installMd5)) {
				
				// 1. 拷贝文件至临时目录;
				String tmpFileName0 = fullValid_gameUpdateDir + tmpApk_Name0;
				Log.d("com.zcjoy.prince:cleanApk", "tmpFileName0 = " + tmpFileName0);
				if (FileUtils.copyFile(apkFileName, tmpFileName0)) {

					// 2. 暂存注释信息;
					apkComment = extractZipComment(tmpFileName0);
					for (int i = 0; i < apkComment.length; ++i) {
						Log.d("com.zcjoy.prince:cleanApk", "\npre-----get comment: apkComment[" + i + "] = " + apkComment[i]);
					}

					// 3. 将APK内的注释信息干掉;
					removeZipComment(tmpFileName0);
					
					// test
					/*
					String testFile = fullValid_gameUpdateDir + "self_1.4.apk";
					Log.d("cleanApk", "---------------------------------------- pre ---------------------------------");
					String premd5 = "3C73DDA3B335688D4B67342E508224A5";
					checkMd5(testFile, premd5.toLowerCase(Locale.getDefault()));
					setZipComment(testFile, apkComment, apkComment.length);
					Log.d("cleanApk", "---------------------------------------- pst ---------------------------------");
					String pstmd5 = "24FB3229BCC84B4051A5DE8AD83C8D9E";
					checkMd5(testFile, pstmd5.toLowerCase(Locale.getDefault()));
					Log.d("cleanApk", "---------------------------------------- end ---------------------------------");
					String testcopyFile = fullValid_gameUpdateDir + "self_1.4_comment.apk";
					copyFile(testFile, testcopyFile);
					*/
					
					// 4. 再次校验MD5，清理过的APK是否与纯净安装包相同;
					if(checkMd5(tmpFileName0, installMd5)) {
						strApkNameRet = tmpFileName0;
					} else {
						// 二次校验失败;
						strApkNameRet = "";
					}
					
				} else {
					// copy失败;
					strApkNameRet =  "";
				}
			}
			else {
				strApkNameRet = apkFileName;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return strApkNameRet;
	}
	
	/************************************************** 
	* @Title: extractZipComment 
	* @Description: 获取Zip文件注释内容
	* @param filename  Zip文件路径
	* @return byte[]  注释内容
	**************************************************/
	public byte[] extractZipComment (String filename) {
		//String retStr = null;
		byte[] retb = null;
		try {
			File file = new File(filename);
			if (!file.exists()) {
				return null;
			}
			int fileLen = (int)file.length();
			FileInputStream in = new FileInputStream(file);
			/* The whole ZIP comment (including the magic byte sequence)
			 * MUST fit in the buffer
			 * otherwise, the comment will not be recognized correctly
			 *
			 * You can safely increase the buffer size if you like 
			 */
			byte[] buffer = new byte[Math.min(fileLen, 8192)];	
			int len;
			in.skip(fileLen - buffer.length);
			if ((len = in.read(buffer)) > 0) {
				retb = getZipCommentFromBuffer(buffer, len);
			}
			in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return retb;
	}

	/************************************************** 
	* @Title: getZipCommentFromBuffer 
	* @Description: 获取Zip文件注释内容
	* @param buffer  Zip分割缓冲区
	* @param len  缓冲区长度
	* @return byte[]  注释内容
	**************************************************/
	private byte[] getZipCommentFromBuffer (byte[] buffer, int len) {
		byte[] magicDirEnd = {0x50, 0x4b, 0x05, 0x06};
		int buffLen = Math.min(buffer.length, len);
		
		//Check the buffer from the end
		for (int i = buffLen/*-magicDirEnd.length*/-22; i >= 0; i--) {
			boolean isMagicStart = true;
			for (int k=0; k < magicDirEnd.length; k++) {
				if (buffer[i+k] != magicDirEnd[k]) {
					isMagicStart = false;
					break;
				}
			}
			
			if (isMagicStart) {
				//Magic Start found!
				int commentLen = buffer[i+20] + buffer[i+21]*256;
				if (commentLen < 0) {
					commentLen = commentLen * -1;
				}
				Log.d("getZipCommentFromBuffer", "i = " + i + ", buff[i+20] = " + buffer[i+20] + ", buffer[i+21] = " + buffer[i+21]);
				int realLen = buffLen - i - 22;
				System.out.println ("ZIP comment found at buffer position " + (i+22) + " with len="+commentLen+", good!");
				if (commentLen != realLen) {
					System.out.println ("WARNING! ZIP comment size mismatch: directory says len is "+ commentLen+", but file ends after " + realLen + " bytes!");
				}

				byte[] retb = new byte[realLen];
				for (int j = 0; j < realLen; j++) {
					retb[j] = buffer[i+22+j];
				}
				return retb;
			}
		}
		System.out.println ("ERROR! ZIP comment NOT found!");
		return null;
	}
	
	/************************************************** 
	* @Title: setZipComment 
	* @Description: 将注释写入Zip文件
	* @param srcFile  待写入的Zip文件
	* @param comment  注释内容
	* @param len      注释长度
	* @return void
	**************************************************/
	public void setZipComment(String srcFile, byte[] comment, int len) {
		
		// 超过注释规定长度，不处理;
		if (len > 65535) {
			return;
		}
		
		File file = new File(srcFile);
		if (!file.exists()) {
			return;
		}
		
		// 先清掉原有comment;
		removeZipComment(srcFile);
		
		try {
			// 打开一个随机访问文件流，按读写方式;
			RandomAccessFile randomFile = new RandomAccessFile(srcFile, "rw");
			
			// 文件长度，字节数;
			long fileLength = randomFile.length();
			
			// 写入注释长度信息;
			long pos = fileLength - 2;
			for (int i = 0; i < 2; i++) {
				randomFile.seek(pos + i);
				
				if (i == 0) {
					if (len > 0xFF) {
						randomFile.write(len%256);
					} else {
						randomFile.write(len);
					}
				} else if (i == 1) {
					if (len > 0xFF) {
						randomFile.write(len/256);
					} else {
						randomFile.write(0);
					}
				}
			}
		
			// 将写文件指针移到文件尾，写入新的comment;
			randomFile.seek(fileLength);
			randomFile.write(comment);
			randomFile.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/************************************************** 
	* @Title: removeZipComment 
	* @Description: 清除Zip文件的注释
	* @param srcFile 待清理的Zip文件
	* @return void
	**************************************************/
	public void removeZipComment(String srcFile) {
		try {
			// 打开一个随机访问文件流，按读写方式
			RandomAccessFile randomFile = new RandomAccessFile(srcFile, "rw");
			
			// 将写文件指针移到文件尾EOCD CommentLength部分，写入00;
			long pos = getZipEOCD_CommentLengthFieldPos(srcFile);
			Log.d("removeZipComment", "pos = " + pos);
			if(pos != 0) {
				for (int i = 0; i < 2; i++) {
					randomFile.seek(pos + i);
					randomFile.write(0);
				}
				// 重置文件结尾，即截断;
				if (randomFile.length() > pos+2) {
					randomFile.setLength(pos+2);
				}
			}
			randomFile.close();
		} catch (IOException e){
			e.printStackTrace();
		}
	}
	
	/************************************************** 
	* @Title: getZipEOCD_CommentLengthFieldPos 
	* @Description: 查询Zip文件内标识注释长度的字段位置
	* @param srcFile 待查询的Zip文件
	* @return long
	**************************************************/
	private long getZipEOCD_CommentLengthFieldPos(String srcFile) {
		long pos = 0;
		try {
			File file = new File(srcFile);
			if (!file.exists()) {
				return pos;
			}
			int fileLen = (int)file.length();
			FileInputStream in = new FileInputStream(file);
			byte[] buffer = new byte[Math.min(fileLen, 8192)];	
			int len;
			int skipLen = fileLen - buffer.length;
			in.skip(skipLen);
			if ((len = in.read(buffer)) > 0) {
				
				byte[] magicDirEnd = {0x50, 0x4b, 0x05, 0x06};
				int buffLen = Math.min(buffer.length, len);
				
				//Check the buffer from the end
				for (int i = buffLen-22; i >= 0; i--) {
					boolean isMagicStart = true;
					for (int k=0; k < magicDirEnd.length; k++) {
						if (buffer[i+k] != magicDirEnd[k]) {
							isMagicStart = false;
							break;
						}
					}
					
					if (isMagicStart) {
						//Magic Start found!
						pos = i + 20 + skipLen;
						Log.d("getZipEOCD_CommentLengthFieldPos", "pos = " + pos + ", i = " + i + ", skipLen = " + skipLen);
						Log.d("getZipEOCD_CommentLengthFieldPos", "buffer[" + i + "] = " + buffer[i] + ", buffer[" + (i+20) + "] =" + buffer[i+20]);
						break;
					}
				}
			}
			in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return pos;
	}
	
	/************************************************** 
	* @Title: checkMd5 
	* @Description: 校验文件的MD5是否与指定值相同
	* @param fileName  待校验的文件
	* @param dstMD5    待比较的MD5
	* @return boolean  比较结果
	**************************************************/
	private boolean checkMd5(String fileName, String dstMD5){
		
		Log.d("com.zcjoy.prince:checkMd5", "fileName = " + fileName + ", dstMD5 = " + dstMD5);
		
		// 校验文件;
		File file = new File(fileName);
		if (!file.exists()){
			return false;
		}
		
		// 计算MD5;
	    MessageDigest digest = null;
	    FileInputStream in = null;
	    byte buffer[] = new byte[1024];
	    int len;
	    try {
	      digest = MessageDigest.getInstance("MD5");
	      in = new FileInputStream(file);
	      while ((len = in.read(buffer, 0, 1024)) != -1) {
	        digest.update(buffer, 0, len);
	      }
	      in.close();
	    } catch (Exception e) {
	      e.printStackTrace();
	      return false;
	    }
	    BigInteger bigInt = new BigInteger(1, digest.digest());
	    String strMD5 = bigInt.toString(16);
	    
	    Log.d("com.zcjoy.prince:checkMd5", "calcMD5 = " + strMD5);
	    Log.d("com.zcjoy.prince:checkMd5", "compareResult = " + strMD5.compareTo(dstMD5));
	    
	    // 比较MD5值并返回;
		return (strMD5.compareTo(dstMD5) == 0);
	}
	
	/************************************************** 
	* @Title: exAsyncTask 
	* @Description: 执行合成APK操作
	* @return void
	**************************************************/
	private void exAsyncTask() {
		AsyncTask<Void, Void, Void> task = new AsyncTask<Void, Void, Void>() {

			@Override
			protected void onPreExecute() {
				super.onPreExecute();
			}

			@Override
			protected Void doInBackground(Void... arg0) {
				
				// 如果有同名文件，先清理掉；
				File file_new = new File(newFile_path);
				if (file_new.exists()) {
					file_new.delete();
				}

				// 执行合成;
				if (0 == patcher(oldFile_path, newFile_path, patchFile_path))
				{
					// 修改合成文件访问权限，以便安装;
					FileUtils.getPermission(newFile_path);
					
					// 重新写入注释信息;
					if (apkComment != null) {
						setZipComment(newFile_path, apkComment, apkComment.length);
					}
				}

				return null;
			}

			@Override
			protected void onPostExecute(Void result) {
				FileUtils.installApplication(appActivity, newFile_path);
			}
		};
		task.execute();
	}
	
	/************************************************** 
	* @Title: checkAndCleanUpdateDir 
	* @Description: 检查并清理更新目录
	* @param curVersion 当前安装版本号
	* @return void
	**************************************************/
	public static void checkAndCleanUpdateDir(String curVersion){
		// 分别检查SD卡和ROM;
		try {
			// 1-SD
			String sdPath = FileUtils.getSDPath();
			if (!sdPath.isEmpty()) {
				// 检查是SD卡下是否有更新目录;
				String sdUpdatePath = sdPath + gameUpdateDir;
				Log.d("Update", "++++++++++ checkAndCleanUpdateDir sdUpdatePath:" + sdUpdatePath);
				File file = new File(sdUpdatePath);
				if (file.exists() && file.isDirectory()) {
					checkAndCleanUpdateFile(sdUpdatePath, curVersion);
				}
			}
			
			// 2-ROM
			// 检查ROM是否有更新目录;
			String romUpdatePath = Cocos2dxHelper.getCocos2dxWritablePath() + gameUpdateDir;
			Log.d("Update", "++++++++++ checkAndCleanUpdateDir romUpdatePath:" + romUpdatePath);
			File file = new File(romUpdatePath);
			if (file.exists() && file.isDirectory()) {
				checkAndCleanUpdateFile(romUpdatePath, curVersion);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/************************************************** 
	* @Title: checkAndCleanUpdateFile 
	* @Description: 检查并清理更新残留文件
	* @param dstPath 更新目录
	* @param curVersion 游戏版本号
	* @return void
	**************************************************/
	public static void checkAndCleanUpdateFile(String dstPath, String curVersion) {
		if (!dstPath.isEmpty()) {
			return;
		}
		
		File dir = new File(dstPath);
		Log.d("Update", "++++++++++ checkAndCleanUpdateFile dstPath:" + dstPath);

		// 遍历更新目录;
		String dstFileName = "";
		File[] files = dir.listFiles();
		if (files != null) {
			for (int i = 0; i < files.length; i++) {
				String fileName = files[i].getName();
				if (files[i].isDirectory()) {
					continue;
				} else if (fileName.endsWith("apk")) {
					String strFileName = files[i].getAbsolutePath();
					String existVersion = FileUtils.getApkFileVersion(appActivity, strFileName);
					
					// 比较版本;
					if (curVersion.compareTo(existVersion) < 0) {
						dstFileName = strFileName;
						break;
					}
					else{
						FileUtils.deleteFile(files[i]);
					}
				} else {
					continue;
				}
			}
        }
		
		Log.d("Update", "++++++++++ checkAndCleanUpdateFile dstFileName:" + dstFileName);
		
		// 有没有找到符合条件可安装的apk文件;
		if (!dstFileName.isEmpty()) {
			FileUtils.installApplication(appActivity, dstFileName);
		} else {
			// 清掉update目录;
			FileUtils.deleteFile(new File(dstPath));
		}
	}
	
	/************************************************** 
	* @Title: popUpdateMessage 
	* @Description: 更新提示消息
	* @param msgCode  消息码
	* @return void
	**************************************************/
	public static void popUpdateMessage(int msgCode) {
		Message message = mHandler.obtainMessage();
    	message.what = msgCode;
        message.sendToTarget();
	}
	
	/************************************************** 
	* @Title: initUpdateStoragePath 
	* @Description: 初始化更新可用的存储路径(优先SD卡，其次手机内存)
	* @return int  0-可用  else-不可用
	**************************************************/
	public static int initUpdateStoragePath() {
		try {
			// 做可用空间检查，优先SD卡(至少2倍于安装包大小, 安全起见加多20MB);
			PackageInfo info = getOriPackage();
			if (info == null) {
				return -1;
			}
			long requireSize_M = (new File(oldFile_path).length() / (1024*1024)) * 2 + 20;
			
			long sdLeftMemory_M = FileUtils.getSDCardMemory();
			long romLeftMemory_M = FileUtils.getRomMemory();
			if (sdLeftMemory_M > requireSize_M) {
				// 建立SD卡更新目录;
				String updatePathInSDCard = FileUtils.getSDPath() + gameUpdateDir;
				File newfile = new File(updatePathInSDCard);
				if (!newfile.exists()) {
					newfile.mkdirs();
				}
				fullValid_gameUpdateDir = updatePathInSDCard;
			} else if (romLeftMemory_M > requireSize_M) {
				// 建立data/app-name下的更新目录;
				String updatePathInRom = Cocos2dxHelper.getCocos2dxWritablePath() + gameUpdateDir;
				File newfile = new File(updatePathInRom);
				if (!newfile.exists()) {
					newfile.mkdirs();
				}
				fullValid_gameUpdateDir = updatePathInRom;
			} else {
				popUpdateMessage(_MSG_UPDATE_SD_MEMORY);	// 报哪个都行;
				return -1;
			}
			return 0;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return -1;
	}
	
	/************************************************** 
	* @Title: patcher 
	* @Description: 合成安装包
	* @param old  旧版本安装包
	* @param newapk  新版本安装包
	* @param patch  补丁文件
	* @return int  状态码
	**************************************************/
	public static native int patcher(String old, String newapk, String patch);
}
