package com.excelliance.open;

import java.lang.reflect.Field;

import android.app.Application;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.res.Configuration;

import com.excelliance.kxqp.sdk.GameSdk;
import com.u8.sdk.U8Application;

/* 请将YourApplicationClassName替换为你们自己的Application类名。
 * 如果在使用我们SDK之前没有自己的Application，强烈建议增加自己的Application类。
 * 特别注意：该文件除了替换这一个地方，别的地方不要改动。如需修改，请修改你们自己的Application类。
 *
 * Please replace YourApplicationClassName with your Application Class name.
 * It's strongly recommended to add your own Application Class if you don't have one yet.
 * NOTE:
 * Please remain this file untouched except replacing YourApplicationClassName.
 * Should you modify your own Application implementation in any case you want to.
 */
public class LBApplication extends U8Application {
    private String pName = null;
    private boolean exec = true;

    @Override
    public void onCreate() {
        // 切勿修改此方法！！请修改你们自己的Application类，如有疑问请和卓盟联系
        // Please modify your own Application class instead of this one. Any
        // question, ask LEBIAN SDK provider
        GlobalSettings.refreshState();
        if (GlobalSettings.USE_LEBIAN) {
            GameSdk.appOnCreate(this, exec, pName);
            if (exec) {
                super.onCreate();
            }
        } else {
            super.onCreate();
        }
    }

    @Override
    protected void attachBaseContext(Context base) {
        // 切勿修改此方法！！请修改你们自己的Application类，如有疑问请和卓盟联系
        // Please modify your own Application class instead of this one. Any
        // question, ask LEBIAN SDK provider
        try {
            Field mBase = ContextWrapper.class.getDeclaredField("mBase");
            mBase.setAccessible(true);
            mBase.set(this, base);
        } catch (Exception ignored) {
            ignored.printStackTrace();
        }

        pName = SimpleUtil.getCurProcessName(base);
        GlobalSettings.refreshState();
        if (GlobalSettings.USE_LEBIAN) {
            exec = SimpleUtil.execOldOnCreate(base, pName);
            if (exec) {
                try {
                    Field mBase = ContextWrapper.class.getDeclaredField("mBase");
                    mBase.setAccessible(true);
                    mBase.set(this, null);

                    super.attachBaseContext(base);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } else {
            try {
                Field mBase = ContextWrapper.class.getDeclaredField("mBase");
                mBase.setAccessible(true);
                mBase.set(this, null);

                super.attachBaseContext(base);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        // 切勿修改此方法！！请修改你们自己的Application类，如有疑问请和卓盟联系
        if (!GlobalSettings.USE_LEBIAN || exec) {
            super.onConfigurationChanged(newConfig);
        }
    }

    @Override
    public void onLowMemory() {
        // 切勿修改此方法！！请修改你们自己的Application类，如有疑问请和卓盟联系
        if (!GlobalSettings.USE_LEBIAN || exec) {
            super.onLowMemory();
        }
    }

    @Override
    public void onTerminate() {
        // 切勿修改此方法！！请修改你们自己的Application类，如有疑问请和卓盟联系
        if (!GlobalSettings.USE_LEBIAN || exec) {
            super.onTerminate();
        }
    }
}
