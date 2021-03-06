package org.cocos2dx.lib;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.lang.reflect.Method;
import java.net.URI;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.widget.Toast;

public class Cocos2dxWebView extends WebView {
    private static final String TAG = Cocos2dxWebViewHelper.class.getSimpleName();

    private int mViewTag;
    private String mJSScheme;
    private Cocos2dxActivity sCocos2dxActivity;
    ValueCallback<Uri> mUploadMessage;

    public Cocos2dxWebView(Context context) {
        this(context, -1);
    }

    @SuppressLint("SetJavaScriptEnabled")
    public Cocos2dxWebView(Context context, int viewTag) {
        super(context);
        this.mViewTag = viewTag;
        this.mJSScheme = "";

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);

        this.getSettings().setSupportZoom(false);

        this.getSettings().setJavaScriptEnabled(true);
        

        this.sCocos2dxActivity = (Cocos2dxActivity) Cocos2dxActivity.getContext();

        // `searchBoxJavaBridge_` has big security risk. http://jvn.jp/en/jp/JVN53768697
        try {
            Method method = this.getClass().getMethod("removeJavascriptInterface", new Class[]{String.class});
            method.invoke(this, "searchBoxJavaBridge_");
        } catch (Exception e) {
            Log.d(TAG, "This API level do not support `removeJavascriptInterface`");
        }

        this.setWebViewClient(new MyWebViewClient(sCocos2dxActivity));
        this.setWebChromeClient(new MyWebChromeClient());
    }

    public void setJavascriptInterfaceScheme(String scheme) {
        this.mJSScheme = scheme != null ? scheme : "";
    }

    public void setScalesPageToFit(boolean scalesPageToFit) {
        this.getSettings().setSupportZoom(scalesPageToFit);
    }

    private class MyWebViewClient extends WebViewClient{
        private Context mContext;
        public MyWebViewClient(Context context){
            super();
            mContext = context;
        }
        
        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon) {
            Log.d(TAG,"URL??????:" + url);
            super.onPageStarted(view, url, favicon);
        }

        @Override
        public void onPageFinished(WebView view, String url) {
            Log.i(TAG, "onPageFinished");
            super.onPageFinished(view, url);
            Cocos2dxWebViewHelper._didFinishLoading(mViewTag, url);
        }

        public boolean shouldOverrideUrlLoading(WebView view, String urlString) {
            URI uri = URI.create(urlString);
            if (uri != null && uri.getScheme().equals(mJSScheme)) {
                Cocos2dxWebViewHelper._onJsCallback(mViewTag, urlString);
                return true;
            }
            return Cocos2dxWebViewHelper._shouldStartLoading(mViewTag, urlString);
        }

        @Override
        public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
            super.onReceivedError(view, errorCode, description, failingUrl);
            Cocos2dxWebViewHelper._didFailLoading(mViewTag, failingUrl);
        }
    }
    public static final int FILECHOOSER_RESULTCODE = 1;
    public static final int REQ_CAMERA = FILECHOOSER_RESULTCODE+1;
    public static final int REQ_CHOOSE = REQ_CAMERA+1;

    private class MyWebChromeClient extends WebChromeClient {

        // For Android 3.0+
           public void openFileChooser(ValueCallback<Uri> uploadMsg, String acceptType) {  
               //Toast.makeText(sCocos2dxActivity, "???????????????",Toast.LENGTH_SHORT).show();
               if (mUploadMessage != null) return;
               mUploadMessage = uploadMsg;   
               selectImage();
//               Intent i = new Intent(Intent.ACTION_GET_CONTENT);
//               i.addCategory(Intent.CATEGORY_OPENABLE);
//               i.setType("*/*");
//                   startActivityForResult( Intent.createChooser( i, "File Chooser" ), FILECHOOSER_RESULTCODE );
           }
            // For Android < 3.0
            public void openFileChooser(ValueCallback<Uri> uploadMsg) {
                   openFileChooser( uploadMsg, "" );
            }
            // For Android  > 4.1.1
          public void openFileChooser(ValueCallback<Uri> uploadMsg, String acceptType, String capture) {
                  openFileChooser(uploadMsg, acceptType);
          }

    }
    
    /**
     * ??????SD???????????????
     * 
     * @return
     */
    public final boolean checkSDcard() {
        boolean flag = Environment.getExternalStorageState().equals(
                Environment.MEDIA_MOUNTED);
        if (!flag) {
            Toast.makeText(sCocos2dxActivity, "??????????????????????????????????????????",Toast.LENGTH_SHORT).show();
        }
        return flag;
    }
    String compressPath = "";
    
    protected final void selectImage() {
        if (!checkSDcard())
            return;
        String[] selectPicTypeStr = {"photo" };
        Builder builder = new AlertDialog.Builder(sCocos2dxActivity)
                .setItems(selectPicTypeStr,
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                switch (which) {
                                // ????????????
                                case 0:
                                	chosePic();
                                    break;
                                default:
                                    break;
                                }
                                compressPath = Environment
                                        .getExternalStorageDirectory()
                                        .getPath()
                                        + "/fuiou_wmp/temp";
                                new File(compressPath).mkdirs();
                                compressPath = compressPath + File.separator
                                        + "compress.jpg";
                            }
                        });
        builder.setCancelable(false);
        builder.show();
    }
    
    String imagePaths;
    Uri  cameraUri;
    /**
     * ???????????????
     */
    private void openCarcme() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
           
        imagePaths = Environment.getExternalStorageDirectory().getPath()
                + "/fuiou_wmp/temp/"
                + (System.currentTimeMillis() + ".jpg");
        // ?????????????????????????????????????????????????????????????????????
        File vFile = new File(imagePaths);
        if (!vFile.exists()) {
            File vDirPath = vFile.getParentFile();
            vDirPath.mkdirs();
        } else {
            if (vFile.exists()) {
                vFile.delete();
            }
        }
        cameraUri = Uri.fromFile(vFile);
        intent.putExtra(MediaStore.EXTRA_OUTPUT, cameraUri);
        sCocos2dxActivity.startActivityForResult(intent, REQ_CAMERA);
    }

    /**
     * ???????????????
     */
    public boolean afterOpenCamera() {
    	File f = new File(imagePaths);
        Bitmap compressBitmap = FileUtils.decodeFile(f.getPath());
        Bitmap newBitmap = FileUtils.ratingImage(f.getPath(), compressBitmap);
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        if (newBitmap == null) 
        {
            return false;
        }
       
        addImageGallery(f);
        File newFile = FileUtils.compressFile(f.getPath(), compressPath);
        return true;
    }

    /** ????????????????????????????????????????????? */
    private void addImageGallery(File file) {
        ContentValues values = new ContentValues();
        values.put(MediaStore.Images.Media.DATA, file.getAbsolutePath());
        values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
        sCocos2dxActivity.getContentResolver().insert(
                MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
    }

    /**
     * ????????????????????????
     */
    private void chosePic() {
        FileUtils.delFile(compressPath);
        Intent innerIntent = new Intent(Intent.ACTION_GET_CONTENT); // "android.intent.action.GET_CONTENT"
        String IMAGE_UNSPECIFIED = "image/*";
        innerIntent.setType(IMAGE_UNSPECIFIED); // ????????????
        Intent wrapperIntent = Intent.createChooser(innerIntent, null);
        sCocos2dxActivity.startActivityForResult(wrapperIntent, REQ_CHOOSE);
    }

    /**
     * ?????????????????????
     * 
     * @param data
     */
	Uri afterChosePic(Intent data) {
    	Log.d(TAG, "=========================afterChosePic==========================");
        // ????????????????????????
        String[] proj = { MediaStore.Images.Media.DATA };
        // ?????????android????????????????????????????????????????????????Android??????
        Cursor cursor = sCocos2dxActivity.getContentResolver().query(data.getData(), proj, null, null, null);
        if(cursor == null ){
            Toast.makeText(sCocos2dxActivity, "????????????????????????png???jpg??????",Toast.LENGTH_SHORT).show();
            return null;
        }
        // ?????????????????? ????????????????????????????????????????????????
        int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        // ????????????????????? ???????????????????????????????????????????????????
        cursor.moveToFirst();
        // ???????????????????????????????????????
        String path = cursor.getString(column_index);
        if(path != null && (path.endsWith(".png")||path.endsWith(".PNG")||path.endsWith(".jpg")||path.endsWith(".JPG")))
        {
            File newFile = FileUtils.compressFile(path, compressPath);
            if(newFile == null)
            {
            	Toast.makeText(sCocos2dxActivity, "????????????????????????png???jpg??????",Toast.LENGTH_SHORT).show();
                return null;
            }
            else
            {
            	return Uri.fromFile(newFile);
            }           
        }else{
            Toast.makeText(sCocos2dxActivity, "????????????????????????png???jpg??????",Toast.LENGTH_SHORT).show();
        }
        return null;
    }


    public void setWebViewRect(int left, int top, int maxWidth, int maxHeight) {
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        layoutParams.leftMargin = left;
        layoutParams.topMargin = top;
        layoutParams.width = maxWidth;
        layoutParams.height = maxHeight;
        layoutParams.gravity = Gravity.TOP | Gravity.LEFT;
        this.setLayoutParams(layoutParams);
    }
}
