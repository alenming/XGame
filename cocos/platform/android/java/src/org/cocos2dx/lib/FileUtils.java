package org.cocos2dx.lib;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

public class FileUtils {

	/**
	 * 閹跺﹤娴橀悧鍥у竾缂傗晛鍩�200K
	 * 
	 * @param oldpath
	 *            閸樺缂夐崜宥囨畱閸ュ墽澧栫捄顖氱窞
	 * @param newPath
	 *            閸樺缂夐崥搴ｆ畱閸ュ墽澧栫捄顖氱窞
	 * @return
	 */
	/**
	 * 閹跺﹤娴橀悧鍥у竾缂傗晛鍩�200K
	 * 
	 * @param oldpath
	 *            閸樺缂夐崜宥囨畱閸ュ墽澧栫捄顖氱窞
	 * @param newPath
	 *            閸樺缂夐崥搴ｆ畱閸ュ墽澧栫捄顖氱窞
	 * @return
	 */
	public static File compressFile(String oldpath, String newPath) {
		Bitmap compressBitmap = FileUtils.decodeFile(oldpath);
		Bitmap newBitmap = ratingImage(oldpath, compressBitmap);
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		if(newBitmap == null)
		{
			return null;
		}
		newBitmap.compress(CompressFormat.PNG, 100, os);
		byte[] bytes = os.toByteArray();
		
		File file = null ;
		try {
			file = FileUtils.getFileFromBytes(bytes, newPath);
		} catch (Exception e) {
			e.printStackTrace();
		}finally{
			if(newBitmap != null ){
				if(!newBitmap.isRecycled()){
					newBitmap.recycle();
				}
				newBitmap  = null;
			}
			if(compressBitmap != null ){
				if(!compressBitmap.isRecycled()){
					compressBitmap.recycle();
				}
				compressBitmap  = null;
			}
		}
		return file;
	}
	
	public static Bitmap ratingImage(String filePath,Bitmap bitmap){
		int degree = readPictureDegree(filePath);
		return rotaingImageView(degree, bitmap);
	}
	
	/**
	 *  閺冨娴嗛崶鍓у
	 * @param angle
	 * @param bitmap
	 * @return Bitmap
	 */
	public static Bitmap rotaingImageView(int angle , Bitmap bitmap) {
        //閺冨娴嗛崶鍓у 閸斻劋缍�
		Matrix matrix = new Matrix();;
        matrix.postRotate(angle);
        System.out.println("angle2=" + angle);
        // 閸掓稑缂撻弬鎵畱閸ュ墽澧�
        if(bitmap == null)
        {
        	return null;
        }
        Bitmap resizedBitmap = Bitmap.createBitmap(bitmap, 0, 0,
        		bitmap.getWidth(), bitmap.getHeight(), matrix, true);
		return resizedBitmap;
	}
	
	/**
	 * 鐠囪褰囬崶鍓у鐏炵偞锟窖嶇窗閺冨娴嗛惃鍕潡鎼达拷
	 * @param path 閸ュ墽澧栫紒婵嗩嚠鐠侯垰绶�
	 * @return degree閺冨娴嗛惃鍕潡鎼达拷
	 */
    public static int readPictureDegree(String path) {
        int degree  = 0;
        try {
                ExifInterface exifInterface = new ExifInterface(path);
                int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);
                switch (orientation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                        degree = 90;
                        break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                        degree = 180;
                        break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                        degree = 270;
                        break;
                }
        } catch (IOException e) {
                e.printStackTrace();
        }
        return degree;
    }

	/**
	 * 閹跺﹤鐡ч懞鍌涙殶缂佸嫪绻氱�涙ü璐熸稉锟芥稉顏呮瀮娴狅拷
	 * 
	 * @param b
	 * @param outputFile
	 * @return
	 */
	public static File getFileFromBytes(byte[] b, String outputFile) {
		File ret = null;
		BufferedOutputStream stream = null;
		try {
			ret = new File(outputFile);
			FileOutputStream fstream = new FileOutputStream(ret);
			stream = new BufferedOutputStream(fstream);
			stream.write(b);
		} catch (Exception e) {
			// log.error("helper:get file from byte process error!");
			e.printStackTrace();
		} finally {
			if (stream != null) {
				try {
					stream.close();
				} catch (IOException e) {
					// log.error("helper:get file from byte process error!");
					e.printStackTrace();
				}
			}
		}
		return ret;
	}

	/**
	 * 閸ュ墽澧栭崢瀣級
	 * 
	 * @param fPath
	 * @return
	 */
	public static Bitmap decodeFile(String fPath) {
		BitmapFactory.Options opts = new BitmapFactory.Options();
		opts.inJustDecodeBounds = true;
		opts.inDither = false; // Disable Dithering mode
		opts.inPurgeable = true; // Tell to gc that whether it needs free
		opts.inInputShareable = true; // Which kind of reference will be used to
		BitmapFactory.decodeFile(fPath, opts);
		final int REQUIRED_SIZE = 200;
		int scale = 1;
		if (opts.outHeight > REQUIRED_SIZE || opts.outWidth > REQUIRED_SIZE) {
			final int heightRatio = Math.round((float) opts.outHeight
					/ (float) REQUIRED_SIZE);
			final int widthRatio = Math.round((float) opts.outWidth
					/ (float) REQUIRED_SIZE);
			scale = heightRatio < widthRatio ? heightRatio : widthRatio;//
		}
		Log.i("scale", "scal ="+ scale);
		opts.inJustDecodeBounds = false;
		opts.inSampleSize = scale;
		if (BitmapFactory.decodeFile(fPath, opts) == null)
		{
			return null;
		}
		Bitmap bm = BitmapFactory.decodeFile(fPath, opts).copy(Config.ARGB_8888, false);
		return bm;
	}
	
	
	
	/**
	 * 閸掓稑缂撻惄顔肩秿
	 * @param path
	 */
	public static void setMkdir(String path)
	{
		File file = new File(path);
		if(!file.exists())
		{
			file.mkdirs();
			Log.e("file", "閻╊喖缍嶆稉宥呯摠閸︼拷  閸掓稑缂撻惄顔肩秿    ");
		}else{
			Log.e("file", "閻╊喖缍嶇�涙ê婀�");
		}
	}
	
	/**
	 * 閼惧嘲褰囬惄顔肩秿閸氬秶袨
	 * @param url
	 * @return FileName
	 */
	public static String getFileName(String url)
	{
		int lastIndexStart = url.lastIndexOf("/");
		if(lastIndexStart!=-1)
		{
			return url.substring(lastIndexStart+1, url.length());
		}else{
			return null;
		}
	}
	
	/**
	 * 閸掔娀娅庣拠銉ф窗瑜版洑绗呴惃鍕瀮娴狅拷
	 * 
	 * @param path
	 */
	public static void delFile(String path) {
		if (!TextUtils.isEmpty(path)) {
			File file = new File(path);
			if (file.exists()) {
				file.delete();
			}
		}
	}
}
