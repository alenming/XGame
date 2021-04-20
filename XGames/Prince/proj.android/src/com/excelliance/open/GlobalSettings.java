package com.excelliance.open;

public class GlobalSettings {
    public final static int F_SHOW_NEVER = 0;
    public final static int F_SHOW_OLD_VERSION = 0x01;
    public final static int F_SHOW_NEW_VERSION = 0x02;
    public final static int F_SHOW_NEW_VERSION_FIRSTTIME_ONLY = 0x04;
    public final static int F_SHOW_ALWAYS = (F_SHOW_OLD_VERSION|F_SHOW_NEW_VERSION);

    // this setting ONLY affects splash visibility during start
    public static int SHOW_BACKGROUND_POLICY = F_SHOW_NEVER;

    public static boolean INSTALL_SHORTCUT = false;
    public static boolean USE_LEBIAN = true;
    public static boolean DOWNLOAD_AFTER_QUIT = true;
    public static boolean AUTO_CHECK_NEWVER_ONSTART = true;
    public static void refreshState() {
        //USE_LEBIAN = !(new java.io.File("/sdcard/disable_LB_flag").exists());
        android.util.Log.d("GlobalSettings", "USE_LEBIAN="+USE_LEBIAN);
    }
}

