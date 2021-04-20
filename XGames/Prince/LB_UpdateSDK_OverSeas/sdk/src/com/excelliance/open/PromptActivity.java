package com.excelliance.open;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.Context;
import android.content.res.Resources;

import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.util.Log;
import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;
import java.io.Serializable;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import android.os.Handler;

import com.excelliance.open.platform.CustomDialog;

public class PromptActivity extends Activity {
	private Parcelable promptActivityParcel = null;
	private final String TAG = "PromptActivity";
	Handler handler = new Handler();
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.d(TAG, "onCreate savedInstanceState="+savedInstanceState);
		super.onCreate(savedInstanceState);
		if (savedInstanceState != null) {
			promptActivityParcel = savedInstanceState.getParcelable("promptActivityParcel");
		}else {
			try {
                Class clazz = Class.forName("com.excelliance.open.PromptActivityParcel");
    			Constructor cons = clazz.getDeclaredConstructor(Parcel.class);
                cons.setAccessible(true);
    			promptActivityParcel = (Parcelable)cons.newInstance((Parcel)null);
            } catch (Exception e) {
                Log.d(TAG, "no PromptActivityParcel");
				 e.printStackTrace();
            }
		}
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,   
		WindowManager.LayoutParams.FLAG_FULLSCREEN);   
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		if(promptActivityParcel != null) {		
			callPromptActivityParcel("init", new Class[]{Activity.class, Handler.class, Bundle.class}, new Object[]{this, handler, savedInstanceState});
		}
	}
	
	@Override
	protected void onResume() {
		Log.d(TAG, "onResume");
		super.onResume();
		Intent intent = getIntent();
		Serializable detail = intent.getSerializableExtra("detail");

		switch (intent.getIntExtra("dialogtype", 1)) {
		case 2:
			showDownloadedDialog(detail);
			break;
		case 1:
			boolean dataConnection = intent.getBooleanExtra("dataConnection", false);
			String forceUpdate = intent.getStringExtra("forceUpdate");
			long size = intent.getLongExtra("size", (long) 0);
			String savePath = intent.getStringExtra("savePath");
			if (savePath != null) {
				boolean alreadyDownloaded = ((Boolean) callPromptActivityParcel("alreadyDownloaded", new Class[] { Serializable.class }, new Object[] { detail })).booleanValue();
				if (!alreadyDownloaded) {
					showUpdateDialog(detail, dataConnection, forceUpdate, size);
				} else {
					callPromptActivityParcel("restartGame", new Class[] { Serializable.class }, new Object[] { detail });
				}
			} else {
				Log.d(TAG, "space not enough");
				showNotEnoughSpaceDialog(forceUpdate, size);
			}
			break;

		}
		
	}
	
	protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
		if (promptActivityParcel != null) {
            outState.putParcelable("promptActivityParcel", promptActivityParcel);
        }
    }

	@Override
	protected void onPause() {
		Log.d(TAG, "onPause");
		super.onPause();
		callPromptActivityParcel("onPause", null, null);
	}
	
	
	// --------------------------------start----------------------------
	private void showDownloadedDialog(final Serializable detail) {
		CustomDialog.Builder builder = new CustomDialog.Builder(this);
		int resId = getResources().getIdentifier("lebian_downloaded_message_body", "string", getPackageName());
		builder.setMessage(getResources().getString(resId));
		resId = getResources().getIdentifier("lebian_hint", "string", getPackageName());
		builder.setTitle(getResources().getString(resId));

		resId = getResources().getIdentifier("lebian_downloaded_current", "string", getPackageName());
		builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				callPromptActivityParcel("restartGame", new Class[] { Serializable.class }, new Object[] { detail });
			}
		});

		resId = getResources().getIdentifier("lebian_downloaded_next", "string", getPackageName());
		builder.setNegativeButton(getResources().getString(resId), new OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				finish();
			}
		});

		Dialog updateDialog = builder.create();
		updateDialog.setCancelable(false);
		updateDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
			@Override
			public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
				if (keyCode == KeyEvent.KEYCODE_SEARCH) {
					return true;
				} else {
					return false;
				}
			}
		});
		if (!updateDialog.isShowing() && !isFinishing()) {
			updateDialog.show();
		}
	}

	// --------------------------------end----------------------------

	private void showUpdateDialog(final Serializable detail, final boolean dataConnection, final String force, final long size) {
		Log.d(TAG, "showUpdateDialog enter detail:"+detail+" dataConnection:"+dataConnection);
        final boolean forceUpdate = force.equals("1");
        CustomDialog.Builder builder = new CustomDialog.Builder(this);
		int resId = getResources().getIdentifier("lebian_dl_update_without_wifi", "string", getPackageName());
        int resId2 = getResources().getIdentifier("lebian_dl_update_wifi", "string", getPackageName());
        builder.setMessage(String.format(getResources().getString(dataConnection?resId:resId2),((float)(size))/(1024.0f * 1024.0f)));

        resId = getResources().getIdentifier("lebian_hint", "string", getPackageName());
        builder.setTitle(getResources().getString(resId));
        if(!forceUpdate){
			resId = getResources().getIdentifier("lebian_next_no_prompt", "string", getPackageName());
            builder.setCheckBox(getResources().getString(resId), false,new OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged (CompoundButton buttonView, boolean isChecked) {
                    getSharedPreferences("excl_lb_prompt", MODE_PRIVATE).edit().putBoolean("gameUpdate", isChecked).commit();
                }
            });
        }
        resId = getResources().getIdentifier("lebian_exit_dialog_yes", "string", getPackageName());
		resId2 = getResources().getIdentifier("lebian_download_background", "string", getPackageName());
        builder.setPositiveButton(getResources().getString(forceUpdate?resId:resId2), new OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                finish();
				if(forceUpdate){
                    if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.FROYO)
                        getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("runningGameId").commit();
                    else
                        getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().remove("runningGameId").apply();
                    getSharedPreferences("excl_lb_gameInfo", Build.VERSION.SDK_INT<Build.VERSION_CODES.HONEYCOMB?Context.MODE_PRIVATE:Context.MODE_MULTI_PROCESS).edit().putBoolean("selfKill", true).commit();
					boolean alreadyDownloaded = ((Boolean)callPromptActivityParcel("alreadyDownloaded", new Class[]{Serializable.class}, new Object[]{detail})).booleanValue();
					if(!alreadyDownloaded){
						callPromptActivityParcel("switchToUpdate", new Class[]{Serializable.class}, new Object[]{detail});
					}else{
						callPromptActivityParcel("restartGame", new Class[]{Serializable.class}, new Object[]{detail});
					}
				}
				else{
					boolean alreadyDownloaded = ((Boolean)callPromptActivityParcel("alreadyDownloaded", new Class[]{Serializable.class}, new Object[]{detail})).booleanValue();
					if(!alreadyDownloaded)
						callPromptActivityParcel("downloadComponent", new Class[]{Serializable.class}, new Object[]{detail});
				}
                getSharedPreferences("excl_lb_prompt", MODE_PRIVATE).edit().putBoolean("gameUpdate", false).commit();
				
            }
        });

        resId = getResources().getIdentifier("lebian_quit_game", "string", getPackageName());
        resId2 = getResources().getIdentifier("lebian_no_update", "string", getPackageName());
        builder.setNegativeButton(getResources().getString(forceUpdate?resId:resId2), new OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
				boolean alreadyDownloaded = ((Boolean)callPromptActivityParcel("alreadyDownloaded", new Class[]{Serializable.class}, new Object[]{detail})).booleanValue();
				if (GlobalSettings.DOWNLOAD_AFTER_QUIT && !alreadyDownloaded && !dataConnection && forceUpdate)
					callPromptActivityParcel("downloadComponent", new Class[]{Serializable.class, Boolean.class}, new Object[]{detail, true});
				finish();
				if(forceUpdate)
					callPromptActivityParcel("quitGame", null, null);
            }
        });

        Dialog updateDialog = builder.create();
        updateDialog.setCancelable(false);
	    updateDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
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
	    if (!updateDialog.isShowing() && !isFinishing()) {
            updateDialog.show();
        }
    }
	
    private void showNotEnoughSpaceDialog(final String force, final long size) {
        boolean hasSD = SimpleUtil.hasExternalStorage();
        CustomDialog.Builder builder = new CustomDialog.Builder(this);
        int resId = getResources().getIdentifier("lebian_not_enought_space", "string", getPackageName());
        builder.setTitle(getResources().getString(resId));
        long requiredSize = size;
        if (!hasSD)
            requiredSize += (20<<20);
        resId = getResources().getIdentifier("lebian_storage_space_requirement_dload", "string", getPackageName());
        int resId2 = getResources().getIdentifier("lebian_storage_sd", "string", getPackageName());
        int resId3 = getResources().getIdentifier("lebian_storage_phone", "string", getPackageName());
        String body = String.format(getResources().getString(resId), requiredSize/(float)(1024*1024), hasSD?getResources().getString(resId2):getResources().getString(resId3));
        builder.setMessage(body);

        resId = getResources().getIdentifier("lebian_exit_dialog_yes", "string", getPackageName());
        builder.setPositiveButton(getResources().getString(resId), new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
				finish();
				if (force.equals("1"))
					callPromptActivityParcel("quitGame", null, null);
				
            }
        });
        Dialog notEnoughSpaceDialog = builder.create();
        notEnoughSpaceDialog.setCancelable(false);
        notEnoughSpaceDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                if (keyCode == KeyEvent.KEYCODE_SEARCH) {
                    return true;
                } else {
                    return false;
                }
            }
        });
        if (!notEnoughSpaceDialog.isShowing() && !isFinishing()) {
            notEnoughSpaceDialog.show();
        }
    }
	
	public Resources getResources() {
		if (promptActivityParcel!=null)
			return (Resources)callPromptActivityParcel("getResources", null, null);

		return super.getResources();
	}

	private Object callPromptActivityParcel(String name, Class[] type, Object[] args) {
		try {
			Class clazz = Class.forName("com.excelliance.open.PromptActivityParcel");
			Method method = clazz.getDeclaredMethod(name, type);
			method.setAccessible(true);
			return method.invoke(promptActivityParcel, args);
		} catch (Exception e) {
			Log.d(TAG, "no PromptActivityParcel");
			e.printStackTrace();
		}

		return null;
	}

}
