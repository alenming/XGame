package com.zcjoy.prince;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Environment;
import android.os.StatFs;
import android.util.Log;

public class FileUtils {

	/************************************************** 
	* @Title: getSDPath 
	* @Description: 获取SD卡全路径
	* @return String 
	**************************************************/
	public static String getSDPath(){
		String path = "";
		try {
			// 判断SD卡是否存在;
			boolean sdCardExist = Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED);
			if (sdCardExist)
			{
				File sdDir = Environment.getExternalStorageDirectory();
				path = sdDir.toString();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return path;
	}
	
	/************************************************** 
	* @Title: getRomMemory 
	* @Description: 获取磁盘剩余空间(单位MB)
	* @return long
	**************************************************/
	@SuppressWarnings("deprecation")
	public static long getRomMemory() {
		long validSize = 0;
		try {
			File path = Environment.getDataDirectory();  
			StatFs stat = new StatFs(path.getPath());  
			long nBlocSize = stat.getBlockSize();  
			long nAvailaBlock = stat.getAvailableBlocks();  
			validSize = nBlocSize * nAvailaBlock / (1024*1024);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return validSize;
	}
	
	/************************************************** 
	* @Title: getSDCardMemory 
	* @Description: 获取SD卡剩余空间(单位MB)
	* @return long 
	**************************************************/
	@SuppressWarnings("deprecation")
	public static long getSDCardMemory() {
		long validSize = 0;
		
		try {
			String sd = getSDPath();
			if (!sd.isEmpty()) {
				android.os.StatFs statfs = new android.os.StatFs(sd);
	
	            // 获取SDCard上BLOCK总数
	            //long nTotalBlocks = statfs.getBlockCount();
	
	            // 获取SDCard上每个block的SIZE
	            long nBlocSize = statfs.getBlockSize();
	
	            // 获取可供程序使用的Block的数量
	            long nAvailaBlock = statfs.getAvailableBlocks();
	
	            // 获取剩下的所有Block的数量(包括预留的一般程序无法使用的块)
	            //long nFreeBlock = statfs.getFreeBlocks();
	
	            // 计算SDCard 总容量大小MB
	            //long totalSize = nTotalBlocks * nBlocSize / (1024*1024);
	
	            // 计算 SDCard 剩余大小MB
	            validSize = nAvailaBlock * nBlocSize / (1024*1024);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return validSize;
	}
	
	/************************************************** 
	* @Title: deleteFile 
	* @Description: 删除文件(不包含文件夹)
	* @param dst 目标文件/文件夹
	* @return void
	**************************************************/
	public static void deleteFile(File dst) {
		try {
			if (dst.isDirectory()) {
				File[] files = dst.listFiles();
				for (File file : files) {
					if (!file.isDirectory()) {
						deleteFile(file);
					}
				}
			} else {
				dst.delete();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/************************************************** 
	* @Title: copyFile 
	* @Description: 复制文件
	* @param srcFile 源文件
	* @param dstFile 目的文件
	* @return boolean
	**************************************************/
	public static boolean copyFile(String srcFile, String dstFile) {
		Log.d("com.zcjoy.prince:copyFile", "enter: oldPath = " + srcFile);
		Log.d("com.zcjoy.prince:copyFile", "enter: newPath = " + dstFile);
		try {
			// 校验旧文件是否存在;
			File oldfile = new File(srcFile);
			if (oldfile.exists()) {
				Log.d("com.zcjoy.prince:copyFile", "oldfile size = " + oldfile.length());
				
				FileUtils.getPermission(srcFile);
				
				// 清理同名文件;
				File newfile = new File(dstFile);
				if (newfile.exists()) {
					newfile.delete();
				}
				newfile.createNewFile();
				
				// 执行拷贝(2MB缓存);
				int length = 2097152;
				FileInputStream in = new FileInputStream(new File(srcFile));
				FileOutputStream out = new FileOutputStream(new File(dstFile));
				FileChannel inC = in.getChannel();
				FileChannel outC = out.getChannel();
				Log.d("com.zcjoy.prince:copyFile", "inC.size = " + inC.size());
				while(true) {
					if(inC.position()==inC.size()) {
						in.close();
						out.close();
						inC.close();
						outC.close();
						
						// 校验新文件是否存在
						File checknewFile = new File(dstFile);
						if (!checknewFile.exists()) {
							return false;
						}
						Log.d("com.zcjoy.prince:copyFile", "newFile size = " + checknewFile.length());
						return true;
					}
					if((inC.size()-inC.position()) < 2097152) {
						length = (int)(inC.size()-inC.position());
						//Log.d("com.zcjoy.prince:copyFile", "1-length = " + length + ", inC.position = " + inC.position());
					} else {
						length=2097152;
						//Log.d("com.zcjoy.prince:copyFile", "2-length = " + length + ", inC.position = " + inC.position());
					}
					inC.transferTo(inC.position(), length, outC);
					inC.position(inC.position()+length);
		        }
			} else {
				return false;
			}
		} catch (Exception e) {
			System.out.println("复制单个文件操作出错");
			e.printStackTrace();
		}
		return true;
	}
	
	/************************************************** 
	* @Title: getPermission 
	* @Description: 获取文件读写权限
	* @param fileName  文件路径
	* @return void
	**************************************************/
	public static void getPermission(String fileName) {
		
		// 校验文件是否存在;
		File file = new File(fileName);
		if (!file.exists()) {
			return;
		}
		
		// 修改访问权限;
		String command = "chmod 777 " + fileName;
        Runtime runtime = Runtime.getRuntime(); 
        try {
			runtime.exec(command);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/************************************************** 
	* @Title: installApplication 
	* @Description: 安装APK文件
	* @param context   上下文环境
	* @param filePath  APK文件路径
	* @return void
	**************************************************/
	public static void installApplication(Activity context, String filePath){
		Log.d("Update", "++++++++++ installApplication");
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		i.setDataAndType(Uri.parse("file://" + filePath), "application/vnd.android.package-archive");
		context.startActivity(i);
	}

	/************************************************** 
	* @Title: getApkFileVersion 
	* @Description: 读取指定APK文件的版本号
	* @param context  上下文环境
	* @param filePath 文件路径
	* @return String  版本号
	**************************************************/
	public static String getApkFileVersion(Activity context, String filePath) {
		String v = "";
		try {
			// 校验文件是否存在;
			File file = new File(filePath);
			if (file.exists()) {
				PackageManager pm = context.getPackageManager();
				PackageInfo info = pm.getPackageArchiveInfo(filePath, PackageManager.GET_ACTIVITIES);
				if(info != null) {
					v = info.versionName; 	// 版本信息
			    }
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return v;
	}
}
