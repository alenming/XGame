package com.excelliance.open;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.Dialog;
import android.os.Bundle;
import android.view.KeyEvent;
import android.os.Handler;
import android.os.Message;
import android.content.pm.PackageManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.ProviderInfo;
import android.content.pm.ServiceInfo;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.Build;
import android.os.IBinder;

import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;
import android.content.Context;
import android.content.pm.ResolveInfo;
import android.content.pm.ActivityInfo;
import android.content.ComponentName;
import android.content.res.Resources;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.net.Uri;

import java.lang.reflect.Method;
import java.lang.reflect.Constructor;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import android.graphics.drawable.Drawable;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.excelliance.open.platform.CustomDialog;
import com.excelliance.kxqp.sdk.GameSdk;

// 修改此文件前请和卓盟确认
// Please confirm with LEBIAN SDK provider before modifying this file
public class KXQP extends Activity {
    private final String TAG = "KXQPSDK";
    private Parcelable lbmain = null;
    boolean quit = false;
    private boolean previous = true;
    private boolean started = false;
    private boolean backFromWx = false;
    private boolean runOnVm = false;
    private boolean mFinished = false;
    private final Runnable mFinishCallBack = new Runnable() {
        public void run() {
            Log.d(TAG, "FinishCallBack: finish activity kxqp="+KXQP.this);
            finish();
        }
    };

    private final Runnable mWaitRemote = new Runnable() {
        public void run() {
            Log.d(TAG, "mWaitRemote kxqp="+KXQP.this);
            needStartMain = false;
            chkProcess(KXQP.this);
            startMainActivity(null);
        }
    };
    public static final int MSG_EXIT_GAME = 4;
    public static final int MSG_NOT_ENOUGH_SPACE = 20;
    public static final int MSG_START_AFTER_PLATFORM_UPDATE = 21;
	public static final int MSG_FIRST_START_PROGRESS = 22;
	public static final int MSG_FIRST_START_PROGRESS_STOP = 23;
    public static final int MSG_SHOW_LOADING = 28;
	public static final int MSG_SHOW_BACKGROUND = 31;
	public static final int MSG_SHOW_INSTALL = 32;
    public static final int MSG_NOT_ENOUGH_SPACE_ERROR = 33;
    
	private Thread mProgressThread = null;
    private boolean stopProgress = false;
    private boolean remoteReady = false;
    private boolean needStartMain = false;
    private TextView firstText = null;
    private ProgressBar firstProgressBar = null;
    private int totalFirstProgress = 300;
    private int iCount = 0;
    private int FIRST_READY_TIME = 30;
    private boolean isBound = false;
    private boolean showBackground = false;
    private boolean prestartRemote = false;
    private Dialog exitDialog = null;
    private Dialog notEnoughSpaceDialog = null;
    private Dialog notEnoughSpaceErrorDialog = null;
    private ServiceConnection conn = new ServiceConnection() {
        public void onServiceDisconnected(ComponentName name) {
            Log.d(TAG, "PrestartService disconnected");
        }

        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.d(TAG, "PrestartService ready");
            remoteReady = true;
            IPrestart prestart = IPrestart$Stub.asInterface(service);
            try {
                prestart.empty();
            }
            catch (Exception e) {
                Log.d(TAG, "LBNOTE prestart.empty()");
            }

            if (lbmain != null) {
                callLBMain("remoteReady", (Class[])null, (Object[])null);
            }
            else if (needStartMain) {
                handler.removeCallbacks(mWaitRemote);
                startMainActivity(null);
            }

            try {
                unbindService(this);
                isBound = false;
            }
            catch (Exception e) {
                Log.d(TAG, "LBIGNORE unbindService");
            }
        }
    };
	
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String action = getIntent().getAction();
        int flags = getIntent().getFlags();
        Bundle extras = getIntent().getExtras();
        Log.d(TAG, "onCreate saved="+savedInstanceState+", extras="+extras+", action="+action+", flags="+flags);
    	quit = getIntent().getBooleanExtra("quit", false);
    	if(quit) {
    		Log.d(TAG, "finish immediately");
    		finish();
    		return;
    	}

        backFromWx = startedByWx();
        GlobalSettings.refreshState();
        SharedPreferences gisp = getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS);
        previous = gisp.getBoolean("previous", true);
        if (savedInstanceState != null) {
            GlobalSettings.USE_LEBIAN = savedInstanceState.getBoolean("uselebian");
            if (GlobalSettings.USE_LEBIAN)
                lbmain = savedInstanceState.getParcelable("lbmain");
            previous = savedInstanceState.getBoolean("previous");
            started = savedInstanceState.getBoolean("started");
        }
        if (GlobalSettings.USE_LEBIAN)
            runOnVm = GameSdk.isRunningOnVm(this);
        Log.d(TAG, "previous="+previous+", started="+started+", current="+GlobalSettings.USE_LEBIAN+", runOnVm="+runOnVm);
        if (runOnVm) return;
        if (previous != GlobalSettings.USE_LEBIAN) {
            previous = GlobalSettings.USE_LEBIAN;
            gisp.edit().putBoolean("previous", previous).commit();
            killRunning();
        }

        if (savedInstanceState == null) {
            showBackground = needShowBackground();
            prestartRemote = (showBackground && !SimpleUtil.hasNewVersion(this));
            if ((flags&Intent.FLAG_ACTIVITY_CLEAR_TOP)==0 && extras==null && gisp.getBoolean("started", false)) {
                Log.d(TAG, "not clean exit?");
                gisp.edit().remove("started").commit();
            }

            if (GlobalSettings.INSTALL_SHORTCUT) {
                int vercode = 0;
                try {
                    PackageManager pm = getPackageManager();
                    vercode = pm.getPackageInfo(getPackageName(), 0).versionCode;
                }
                catch (Exception e) {
                }
                
                int savedCode = gisp.getInt("excl_lb_lastcode", -1);
                Log.d(TAG, "vercode="+vercode+", saved="+savedCode);
                if (savedCode==-1 || vercode!=savedCode) {
                    addShortCut();
                    gisp.edit().putInt("excl_lb_lastcode", vercode).commit();
                }
            }

            if (GlobalSettings.USE_LEBIAN) {
                try {
                    Class clazz = Class.forName("com.excelliance.open.LBMain");
        			Constructor cons = clazz.getDeclaredConstructor(Parcel.class);
                    cons.setAccessible(true);
        			lbmain = (Parcelable)cons.newInstance((Parcel)null);
                    gisp.edit().putBoolean("excl_lb_haslbmain", true).commit();
                } catch (Exception e) {
                    Log.d(TAG, "no LBMain");
                    SharedPreferences extPref = getSharedPreferences("excl_lb_extractInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS);
                    Editor extEditor = extPref.edit();
                    if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.FROYO) {
                        extEditor.remove("resExtracted").commit();
                        extEditor.remove("components").commit();
                        extEditor.remove("gid").commit();
                    }
                    else {
                        extEditor.remove("resExtracted").apply();
                        extEditor.remove("components").apply();
                        extEditor.remove("gid").apply();
                        extEditor.commit();
                    }
					String currentVersion = String.valueOf(new File(getApplicationContext().getPackageResourcePath()).lastModified());
					extEditor.putString("lastVersion", currentVersion).commit();
					SimpleUtil.saveChInfo(this);
                    gisp.edit().putBoolean("excl_lb_haslbmain", false).commit();
                }
            }
        }

        if (GlobalSettings.USE_LEBIAN && lbmain!=null) {
            callLBMain("setConfig", new Class[] {boolean.class, boolean.class}, new Object[] {prestartRemote, showBackground});
            callLBMain("init", new Class[]{Activity.class, Handler.class, Bundle.class}, new Object[]{this, handler, savedInstanceState});
        }
        if (showBackground) {
            Drawable background = getBackGroundDrawable();
            if(background != null)
                getWindow().setBackgroundDrawable(background);

            if (prestartRemote) {
                try {
                    Log.d(TAG, "bind PrestartService");
                    Intent intent = new Intent(this, PrestartService.class);
                    bindService(intent, conn, Context.BIND_AUTO_CREATE);
                    isBound = true;
                }
                catch (Exception e) {
                    Log.d(TAG, "LBNOTE bindService failed");
                    remoteReady = true;
                    if (GlobalSettings.USE_LEBIAN && lbmain!=null) {
                        callLBMain("remoteReady", (Class[])null, (Object[])null);
                    }
                }
            }
        }
    }

    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (lbmain != null) {
            callLBMain("onSaveInstanceState", new Class[]{Bundle.class}, new Object[]{outState});
            outState.putParcelable("lbmain", lbmain);
        }
        outState.putBoolean("previous", previous);
        outState.putBoolean("started", started);
        outState.putBoolean("uselebian", GlobalSettings.USE_LEBIAN);
        Log.d(TAG, "save previous="+previous+", started="+started);
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (GlobalSettings.USE_LEBIAN && lbmain!=null) {
            Boolean ret = (Boolean)callLBMain("onKeyDown", new Class[]{int.class, KeyEvent.class}, new Object[]{keyCode, event});
            return ret.booleanValue();
        }

        return false;
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume enabled="+GlobalSettings.USE_LEBIAN+", started="+started);
        if (quit) return;
        if (!GlobalSettings.USE_LEBIAN || lbmain==null || backFromWx || runOnVm) {            
            if (!started) {
                started = true;
                if (backFromWx)
                    startMainActivity(getIntent());
                else {
                    if (!runOnVm) {
                        SharedPreferences gisp = getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS);
                        gisp.edit().putBoolean("started", started).commit();
                        if (GlobalSettings.USE_LEBIAN && lbmain==null)
                            GameSdk.getInstance().sdkInit(getApplicationContext());
                    }
                    if (!prestartRemote || remoteReady || runOnVm)
                        startMainActivity(null);
                    else {
                        needStartMain = true;
                        handler.postDelayed(mWaitRemote, 3000);
                    }
                }
            }
            else
                finish();

            return;
        }
        else
            callLBMain("onResume", null, null);
    }


    @Override
    protected void onPause() {
        super.onPause();
        if (isFinishing() && !mFinished) return;
        if ((backFromWx || runOnVm) && !isFinishing()) {
            handler.removeCallbacks(mFinishCallBack);
            handler.postDelayed(mFinishCallBack, 3000);
            return;
        }
        if (showBackground && lbmain==null)
            getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        if (!GlobalSettings.USE_LEBIAN || quit) return;
        if (GlobalSettings.USE_LEBIAN && lbmain!=null)
            callLBMain("onPause", null, null);
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (!GlobalSettings.USE_LEBIAN || lbmain==null || backFromWx || runOnVm) return;
        if (GlobalSettings.USE_LEBIAN && lbmain!=null)
            callLBMain("onStop", null, null);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy quit="+quit+", backFromWx="+backFromWx+", mFinished="+mFinished+", runOnVm="+runOnVm+", lbmain="+lbmain+", this="+this);
        try {
            if (isBound)
                unbindService(conn);
        }
        catch (Exception e) {
        }
		if (lbmain!=null)
			callLBMain("clear", null, null);
		if (!mFinished) return;
        if (backFromWx || runOnVm) return;
        if (started) {
            SharedPreferences gisp = getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS);
            Log.d(TAG, "started="+gisp.getBoolean("started", false));
            gisp.edit().remove("started").commit();
        }
        if (quit || !GlobalSettings.USE_LEBIAN) {
            getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("started").commit();
            getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("runningGameId").commit(); // GameConfig.DEF_GAMEID=20000

            android.os.Process.killProcess(android.os.Process.myPid());
            return;
        }
        else if (lbmain == null) {
            getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("started").commit();
            try {
                Class clazz = Class.forName("com.excelliance.open.LBMain");
                Constructor cons = clazz.getDeclaredConstructor(Parcel.class);
                cons.setAccessible(true);
                lbmain = (Parcelable)cons.newInstance((Parcel)null);
                getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().putBoolean("excl_lb_haslbmain", true).commit();
                callLBMain("prestartCheck", new Class[]{Activity.class}, new Object[]{this});
            } catch (Exception e) {
                Log.d(TAG, "still no LBMain");
                getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("runningGameId").commit(); // GameConfig.DEF_GAMEID=20000
                android.os.Process.killProcess(android.os.Process.myPid());
            }
            return;
        }
        GlobalSettings.refreshState();
        if (lbmain!=null)
            callLBMain("onDestroy", null, null);
    }

    public Resources getResources() {
        if (GlobalSettings.USE_LEBIAN && lbmain!=null)
            return (Resources)callLBMain("getResources", null, null);

        return super.getResources();
    }

    public void finish() {
        mFinished = true;
        super.finish();
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
	        switch (msg.what) {
                case MSG_EXIT_GAME:
                    showExitDialog();
                break;
	        	case MSG_FIRST_START_PROGRESS:
		            if (!mProgressThread.isInterrupted() && firstProgressBar!=null) {
		                firstProgressBar.setProgress(totalFirstProgress*msg.arg1/100);

		                if (iCount == 100) {
		                    firstProgressBar.setVisibility(View.GONE);
		                    firstText.setVisibility(View.GONE);
		                }

		            }
	            break;
				case MSG_FIRST_START_PROGRESS_STOP:
		            stopProgress = true;
					if (firstText!=null){
		                firstText.setVisibility(View.GONE);
		            }
		            if (firstProgressBar!=null){
		                firstProgressBar.setVisibility(View.GONE);
		            }
	            break;
				case MSG_SHOW_BACKGROUND:
					Drawable background = getBackGroundDrawable();
					if(background != null)
						getWindow().setBackgroundDrawable(background);
				break;
				case MSG_SHOW_LOADING:
					showLoadingProgress(false);
				break;
                case MSG_SHOW_INSTALL:
                    String path = msg.getData().getString("path");
                    String fup = msg.getData().getString("fup");
                    showInstallDialog(path, fup);
                    break;
                case MSG_NOT_ENOUGH_SPACE:
                    showNotEnoughSpaceDialog((Long)msg.obj);
                    break;
                case MSG_NOT_ENOUGH_SPACE_ERROR:
                    Long size = msg.getData().getLong("size");
                    String fu = msg.getData().getString("fup");
                    showNotEnoughSpaceErrorDialog(size, fu);
                    break;
				default:
		            if (GlobalSettings.USE_LEBIAN && lbmain!=null)
		                callLBMain("handleMessage", new Class[]{Message.class}, new Object[]{msg});
				break;
        	}
        }
    };

    private void addShortCut() {
        try {
            Intent shortcut = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");

            PackageManager packageManager = null;
            ApplicationInfo applicationInfo = null;
            try {
                packageManager = getApplicationContext().getPackageManager();
                applicationInfo = packageManager.getApplicationInfo(getPackageName(), 0);
            } catch (PackageManager.NameNotFoundException e) {
                applicationInfo = null;
            }
            String applicationName =  (String) packageManager.getApplicationLabel(applicationInfo);

            shortcut.putExtra(Intent.EXTRA_SHORTCUT_NAME, applicationName);
            int iconId = 0;
            try {
                iconId = getPackageManager().getApplicationInfo(getPackageName(), 0).icon;
            } catch (Exception e) {}
            shortcut.putExtra("duplicate", false);

            Parcelable icon = Intent.ShortcutIconResource.fromContext(this,iconId);
            shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE,icon);
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.setFlags(Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
            intent.addFlags(Intent.FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY);
            intent.addCategory(Intent.CATEGORY_LAUNCHER);
            intent.setClass(this, KXQP.class);
            String currentLauncherPackage = getLauncherPackageName(this);
            Log.d("addShortCut","addShortCut currentLauncherPackage:" + currentLauncherPackage);
            if((!currentLauncherPackage.equals("com.sec.android.app.twlauncher")) && (!currentLauncherPackage.equals("com.htc.launcher"))) {
                shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, intent);
                this.sendBroadcast(shortcut);
            }
        } catch (Exception e) {}
    }

    private String getLauncherPackageName(Context context) {
        final Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_HOME);
        final ResolveInfo res = context.getPackageManager().resolveActivity(intent, 0);
        if(res.activityInfo == null) {
            return "";
        }
        if(res.activityInfo.packageName.equals("android")) {
            return "";
        } else {
            return res.activityInfo.packageName;
        }
    }

	private void startMainActivity(Intent intent) {
		try {
            String className = null;
            if (intent == null) {
                if (!GlobalSettings.USE_LEBIAN || lbmain==null) {
                    handleComponents(2);
                }
			    intent = new Intent();
                ActivityInfo info=getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
                className=info.metaData.getString("mainActivity");
            }
            else {
                Bundle extras = intent.getExtras();
                if (extras != null && extras.containsKey("wx_callback")) {
                    intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TOP|Intent.FLAG_ACTIVITY_SINGLE_TOP);
                    try {
                        ActivityInfo info = getPackageManager().getActivityInfo(
                            new ComponentName(getPackageName(), "com.excelliance.open.KXQP"),
                            PackageManager.GET_META_DATA);
                        if (info != null) className = info.metaData.getString("mainActivity");
                    } catch (PackageManager.NameNotFoundException e) {
                        e.printStackTrace();
                    }
                }
            }

            if (className != null) {
                int dotIndex = className.indexOf('.');
                if (dotIndex == 0)
                    className = getPackageName()+className;
                else if (dotIndex == -1)
                    className = getPackageName()+'.'+className;
                Log.d(TAG, "startMainActivity className:"+className);
    			ComponentName cn = new ComponentName(getPackageName(), className);
    			intent.setComponent(cn);
                getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().putString("runningGameId", "20000").commit(); // GameConfig.DEF_GAMEID=20000
            }
            intent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
            intent.addCategory(Intent.CATEGORY_LAUNCHER);
            intent.setAction(Intent.ACTION_MAIN);
			startActivity(intent);
            overridePendingTransition(0, 0);
		} catch (Exception e) {
            Log.d(TAG, "startMainActivity e:"+e);
        }
	}

    private boolean startedByWx() {
        Intent intent = getIntent();
        Bundle extras = intent.getExtras();
        return (extras != null && extras.containsKey("wx_callback"));
    }

    private Object callLBMain(String name, Class[] type, Object[] args) {
        try {
            Class clazz = Class.forName("com.excelliance.open.LBMain");
			Method method = clazz.getDeclaredMethod(name, type);
			method.setAccessible(true);
			return method.invoke(lbmain, args);
        } catch (Exception e) {
            Log.d(TAG, "no LBMain or no such method? name="+name);
        }

        return null;
    }

    private void killRunning() {
        try {
			int thisUid = android.os.Process.myUid();
			int thisPid = android.os.Process.myPid();

			final ActivityManager activityManager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
            java.util.List<ActivityManager.RunningServiceInfo> runningServices = activityManager.getRunningServices(3000);
            for (ActivityManager.RunningServiceInfo serviceInfo : runningServices) {
                if (serviceInfo.uid==thisUid && !serviceInfo.process.endsWith(":lebian") && !serviceInfo.process.endsWith(":download") && !serviceInfo.process.endsWith(":lbmain")) {
                    Intent sIntent = new Intent();
                    sIntent.setComponent(serviceInfo.service);
                    stopService(sIntent);
                    Log.d(TAG, "stop services in pid="+serviceInfo.pid+", pname="+serviceInfo.process);
                }
            }

			final java.util.List<ActivityManager.RunningAppProcessInfo> services = activityManager.getRunningAppProcesses();
			for (ActivityManager.RunningAppProcessInfo runningProcessInfo : services) {
				if (runningProcessInfo.uid == thisUid && runningProcessInfo.pid != thisPid){
                    Log.d(TAG, "alive "+runningProcessInfo.processName+", pid="+runningProcessInfo.pid);
                    if (!runningProcessInfo.processName.endsWith(":lebian") && !runningProcessInfo.processName.endsWith(":download") && !runningProcessInfo.processName.endsWith(":lbmain"))
					    android.os.Process.killProcess(runningProcessInfo.pid);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
    }

    private void handleComponents(int flag) {
        SharedPreferences splb = getSharedPreferences("excl_lb_lbmain", Context.MODE_PRIVATE);
        int current = splb.getInt("comp_flag", 0); // 0-empty, 1-disabled, 2-default
        if (current != flag) {
            splb.edit().putInt("comp_flag", flag).commit();
            boolean disable = (flag==1);
            Log.d(TAG, "handleComponents flag="+flag);
            String packageName = getPackageName();
            PackageManager pMgr = getPackageManager();
            if (pMgr != null) {
                PackageInfo pinfo = null;
                try {
                    pinfo = pMgr.getPackageInfo(packageName, PackageManager.GET_RECEIVERS|PackageManager.GET_SERVICES|PackageManager.GET_PROVIDERS);
                } catch (Exception e) {
                    Log.d(TAG, "handleComponents");
                }
                if (pinfo != null) {
                    if (pinfo.receivers != null) {
                        for (ActivityInfo recv : pinfo.receivers) {
                            if (!recv.name.startsWith("com.excelliance")) {
                                ComponentName cn = new ComponentName(packageName, recv.name);
                                pMgr.setComponentEnabledSetting(cn, disable?PackageManager.COMPONENT_ENABLED_STATE_DISABLED:PackageManager.COMPONENT_ENABLED_STATE_DEFAULT, PackageManager.DONT_KILL_APP);
                            }
                        }
                    }
                    if (pinfo.services != null) {
                        for (ServiceInfo serv : pinfo.services) {
                            if (!serv.name.startsWith("com.excelliance") && !serv.name.endsWith(".LBService")) {
                                ComponentName cn = new ComponentName(packageName, serv.name);
                                pMgr.setComponentEnabledSetting(cn, disable?PackageManager.COMPONENT_ENABLED_STATE_DISABLED:PackageManager.COMPONENT_ENABLED_STATE_DEFAULT, PackageManager.DONT_KILL_APP);
                            }
                        }
                    }
                    if (pinfo.providers != null) {
                        for (ProviderInfo prov : pinfo.providers) {
                            if (!prov.name.startsWith("com.excelliance")) {
                                ComponentName cn = new ComponentName(packageName, prov.name);
                                pMgr.setComponentEnabledSetting(cn, disable?PackageManager.COMPONENT_ENABLED_STATE_DISABLED:PackageManager.COMPONENT_ENABLED_STATE_DEFAULT, PackageManager.DONT_KILL_APP);
                            }
                        }
                    }
                }
            }
        }
    }
	
	public Drawable getBackGroundDrawable(){
		Drawable background = null;
		int resId = getResources().getIdentifier("lebian_background", "drawable", getPackageName());
		if(resId == 0){
			resId = getResources().getIdentifier("lebian_main_background_normal", "drawable", getPackageName());
			if(resId == 0)
				return null;;
		}	
		background = getResources().getDrawable(resId);
		return background;
	}

    private void showLoadingProgress(boolean firstTime) {
        if (mProgressThread != null)
            return;
		Drawable background = getBackGroundDrawable();
		if(background != null)
			getWindow().setBackgroundDrawable(background);
        String packageName = getPackageName();
        int resId = getResources().getIdentifier("lebian_first_start", "layout", packageName);
        int resId2 = getResources().getIdentifier("lebian_text_first", "id", packageName);
        int resId3 = getResources().getIdentifier(firstTime?"lebian_first_time_start":"lebian_loading_pls_wait", "string", packageName);
        FIRST_READY_TIME = firstTime?8:5;

        int resId4 = getResources().getIdentifier("lebian_firstProgressBar", "id", packageName);
        setContentView(resId);
        firstText = (TextView)findViewById(resId2);
        firstText.setText(resId3);
        firstProgressBar = (ProgressBar)findViewById(resId4);
        totalFirstProgress = firstProgressBar.getMax();

        final int max = 100;
        final int half = max/2;
        final int eighty = max*4/5;

        mProgressThread = new Thread(new Runnable() {
            public void run() {
                final int interval = 200;
                int lastProgress = 0;
                try {
                    for (int i=0; i<=FIRST_READY_TIME*1000/interval; i++) {
                        Thread.sleep(interval);
                        if (stopProgress)
                            break;
                        final float times = 1000.0f*FIRST_READY_TIME/(3*interval);

                        if (iCount<half) {
                            iCount = (int)(half/times*i);
                        } else if (iCount<eighty) {
                            iCount = half+(int)((eighty-half)/times*(i-times));
                        } else
                            iCount = eighty+(int)((max-eighty)/times*(i-times*2));

                        if (lastProgress!=iCount && (firstProgressBar!=null && firstProgressBar.getVisibility()==View.VISIBLE && firstProgressBar.isShown())) {
                            lastProgress = iCount;
                            Message msg = handler.obtainMessage();
                            msg.what = MSG_FIRST_START_PROGRESS;
                            msg.arg1 = iCount;
                            handler.sendMessage(msg);
                        }
                    }
                } catch(Exception e) {
                    e.printStackTrace();
                }
            }
        });
        mProgressThread.start();
    }

    private void showExitDialog() {
        if (exitDialog == null) {
            CustomDialog.Builder builder = new CustomDialog.Builder(this);
            int resId = getResources().getIdentifier("lebian_exit_dialog_titile", "string", getPackageName());
            builder.setMessage(getResources().getString(resId));

            resId = getResources().getIdentifier("lebian_hint", "string", getPackageName());
            builder.setTitle(getResources().getString(resId));

            resId = getResources().getIdentifier("lebian_exit_dialog_yes", "string", getPackageName());
            builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    finish();
                    callLBMain("exit", null, null);
                }
            });

            resId = getResources().getIdentifier("lebian_exit_dialog_no", "string", getPackageName());
            builder.setNegativeButton(getResources().getString(resId), new OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {

                    dialog.dismiss();
                    exitDialog = null;
                }
            });
            exitDialog = builder.create();
        }
        if (!exitDialog.isShowing() && !isFinishing()) {
            exitDialog.show();
        }
    }

    private void showInstallDialog(final String apkPath, final String forceUpdate) {
		String packageName = getPackageName();
        Dialog installDialog = null;
		final boolean fUpdate = (forceUpdate == null || forceUpdate.equals("1"));
        CustomDialog.Builder builder = new CustomDialog.Builder(this);
        int resId = getResources().getIdentifier(fUpdate?"lebian_update_available_force_update_hint":"lebian_update_available_update_hint", "string", packageName);
        builder.setMessage(getResources().getString(resId));
        resId = getResources().getIdentifier("lebian_update_available", "string", packageName);
        builder.setTitle(getResources().getString(resId));
		
        resId = getResources().getIdentifier("lebian_install_yes", "string", packageName);
        builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                File file = new File(apkPath);
                Uri uri =  Uri.fromFile(file);
                Intent intent = new Intent(Intent.ACTION_VIEW);
                intent.setDataAndType(uri, "application/vnd.android.package-archive");
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
            }
        });

        resId = getResources().getIdentifier(fUpdate?"lebian_quit_game":"lebian_exit_dialog_no", "string", packageName);
        builder.setNegativeButton(getResources().getString(resId), new OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
				if(fUpdate){
					finish();
				}else{
					callLBMain("installShowed", null, null);
				}
            }
        });
		
        if(!isFinishing()) {
            installDialog = builder.create();
			installDialog.setCancelable(false);
            installDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
                @Override
                public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                    if (keyCode == KeyEvent.KEYCODE_SEARCH) {
                        return true;
                    } else {
                        return false;
                    }
                }
            });
			installDialog.show();  
        }

    }

    private void showNotEnoughSpaceDialog(long requiredSize) {
        if (notEnoughSpaceDialog == null) {
            boolean hasSD = android.os.Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED);
            CustomDialog.Builder builder = new CustomDialog.Builder(this);
            int resId = getResources().getIdentifier("lebian_not_enought_space", "string", getPackageName());
            builder.setTitle(getResources().getString(resId));
            //builder.setTitle(getResources().getString(R.string.not_enought_space));
            int resIdReq = getResources().getIdentifier("lebian_storage_space_requirement", "string", getPackageName());
            int resIdSd = getResources().getIdentifier("lebian_storage_sd", "string", getPackageName());
            int resIdPhone = getResources().getIdentifier("lebian_storage_phone", "string", getPackageName());
            String body = String.format(getResources().getString(resIdReq), ((float)requiredSize)/1024.0/1024.0, hasSD?getResources().getString(resIdSd):getResources().getString(resIdPhone));
            //String body = String.format(getResources().getString(R.string.storage_space_requirement), ((float)requiredSize)/1024.0/1024.0, hasSD?getResources().getString(R.string.storage_sd):getResources().getString(R.string.storage_phone));
            builder.setMessage(body);

            resId = getResources().getIdentifier("lebian_exit_dialog_yes", "string", getPackageName());
            builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {
                //builder.setPositiveButton(getResources().getString(R.string.exit_dialog_yes), new OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                    finish();
                }
            });
            notEnoughSpaceDialog = builder.create();
        }
        if (!notEnoughSpaceDialog.isShowing() && !isFinishing()) {
            notEnoughSpaceDialog.show();
        }
    }
    
	private void showNotEnoughSpaceErrorDialog(long requiredSize, final String forceUpdate) {
        if (notEnoughSpaceErrorDialog == null) {
			String packageName = getPackageName();
            CustomDialog.Builder builder = new CustomDialog.Builder(this);
            int resId = getResources().getIdentifier("lebian_not_enought_space", "string", packageName);
            builder.setTitle(getResources().getString(resId));
            resId = getResources().getIdentifier("lebian_space_not_enough_error", "string", packageName);
            builder.setMessage(getResources().getString(resId));
            resId = getResources().getIdentifier("lebian_exit_dialog_yes", "string", packageName);
            builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {
              @Override
              public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        if (forceUpdate.equals("1"))
                            finish();
                        else{
                            Message msg = handler.obtainMessage();
                            msg.what = MSG_START_AFTER_PLATFORM_UPDATE;
                            handler.sendMessageDelayed(msg,50);
                        }
                            
                    }
            });

            notEnoughSpaceErrorDialog = builder.create();
            notEnoughSpaceErrorDialog.setCancelable(false);
    	    notEnoughSpaceErrorDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
    		    @Override
    		    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event)
    		    {
    		    if (keyCode == KeyEvent.KEYCODE_SEARCH)
    		     {
    		      return true;
    		     }
    		     else
    		     {
    		      return false;
    		     }
    		    }
            });
        }
        if (!notEnoughSpaceErrorDialog.isShowing() && !isFinishing()) {
            notEnoughSpaceErrorDialog.show();
        }
    }

    private void chkProcess(Context context) {
        try {
            Runtime runtime = Runtime.getRuntime();
            java.lang.Process proc = runtime.exec("ps");
            BufferedReader br = new BufferedReader(new InputStreamReader(proc.getInputStream()));
            String line = null;
            String packageName = context.getPackageName();
            int colPID = 0, colPPID = 0;
            // get PPID and NAME index first
            while ((line=br.readLine()) != null) {
                if (line.startsWith("USER")) {
                    String[] header = line.split("\\s+");
                    for (int i=0; i<header.length; i++) {
                        if (header[i].equals("PID"))
                            colPID = i;
                        else if (header[i].equals("PPID"))
                            colPPID = i;
                    }
                    break;
                }
            }

            // kill processes
            while ((line=br.readLine()) != null) {
                if (line.endsWith(packageName)) {
                    String[] cmd = line.split("\\s+");
                    if (cmd[colPPID].equals("1") && cmd[cmd.length-1].equals(packageName)) {
                        Log.d(TAG, "found pid="+cmd[colPID]+", name="+packageName);
                        android.os.Process.killProcess(Integer.parseInt(cmd[colPID]));

                        int retry = 1;
                        int remotePid = 0;
                        do {
                            String remoteName = context.getPackageName();
                            ActivityManager mActivityManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
                            for (ActivityManager.RunningAppProcessInfo appProcess : mActivityManager.getRunningAppProcesses()) {
                                if (appProcess.processName.equals(remoteName)) {
                                    remotePid = appProcess.pid;
                                    break;
                                }
                            }
                            if (remotePid > 0) {
                                Log.d(TAG, "waiting remote "+remotePid+" exit retry="+retry);
                                Thread.sleep(50);
                            }
                        } while(remotePid>0 && ++retry<=6);
                    }
                }
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean needShowBackground() {
        boolean show = false;
        boolean hasNewVersion = SimpleUtil.hasNewVersion(this);
        if (!hasNewVersion && isGlobalEnabled(GlobalSettings.F_SHOW_OLD_VERSION))
            show = true;
        else if (hasNewVersion) {
            if (isGlobalEnabled(GlobalSettings.F_SHOW_NEW_VERSION_FIRSTTIME_ONLY) && new File("/data/data/"+getPackageName()+"/files/lb_fresh_tag").exists())
                show = true;
            else if (isGlobalEnabled(GlobalSettings.F_SHOW_NEW_VERSION))
                show = true;
        }
        return show;
    }

    private boolean isGlobalEnabled(int flag) {
        return ((GlobalSettings.SHOW_BACKGROUND_POLICY&flag)!=0);
    }
}
// 修改此文件前请和卓盟确认
// Please confirm with LEBIAN SDK provider before modifying this file



