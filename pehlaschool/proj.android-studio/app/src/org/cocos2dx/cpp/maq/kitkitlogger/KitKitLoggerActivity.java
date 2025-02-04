package org.cocos2dx.cpp.maq.kitkitlogger;

import android.Manifest;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Process;
import android.support.v4.app.ActivityCompat;
import android.util.Log;

import java.util.Locale;

import static android.provider.ContactsContract.Directory.PACKAGE_NAME;

/**
 * Created by ingtellect on 8/9/17.
 */

public class KitKitLoggerActivity extends Activity {
    private static final String TAG = "KitKitLoggerActivity";
    protected String appLanguage;

    public String getAppLanguage() {
        return appLanguage;
    }

    public boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(android.Manifest.permission.READ_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                return true;
            } else {

                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
                return false;
            }
        } else { //permission is automatically granted on sdk<23 upon installation
            return true;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        isStoragePermissionGranted();

        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT) {
            ActionBar actionBar = getActionBar();
            if (actionBar != null) {
                actionBar.hide();
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void attachBaseContext(Context newBase) {
        try {
            Context launcherContext = newBase.createPackageContext(PACKAGE_NAME, 0);
            SharedPreferences pref = launcherContext.getSharedPreferences("sharedPref", Context.MODE_PRIVATE);
            appLanguage = pref.getString("appLanguage", "en-US");

            String[] splitLang = appLanguage.split("-");
            String lang = splitLang[0];
            String region = splitLang.length > 1 ? splitLang[1] : "";

            Locale newLocale = new Locale(lang, region);
            Context context = KitkitContextWrapper.wrap(newBase, newLocale);
            super.attachBaseContext(context);
        } catch (PackageManager.NameNotFoundException ne) {
            Log.e(TAG, ne.toString());
            super.attachBaseContext(newBase);
        }

    }

//    public void changeLanguage(String lang) {
//        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
//        SharedPreferences.Editor editor = preferences.edit();
//        editor.putString("appLanguage",lang);
//        editor.commit();
//        Log.d(TAG, "change language to " + lang);
//        finish();
//        //android.os.Process.killProcess(android.os.Process.myPid());
//        //System.exit(1);
//    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        try {
//            Changed the package of Gallery app to "org.cocos2dx.cpp.pehlalauncher"
            Context context = createPackageContext(PACKAGE_NAME, 0);
            //this seems working but Context.MODE_MULTI_PROCESS is deprecated since SDK 23. If it has problem, need to change to ContentProvider for sharing data.
            SharedPreferences pref = context.getSharedPreferences("sharedPref", Context.MODE_MULTI_PROCESS);
            String sharedLang = pref.getString("appLanguage", "en-US");
            if (appLanguage != null && !appLanguage.equals(sharedLang)) {
                restartApp();
            }
        } catch (PackageManager.NameNotFoundException ne) {
            Log.e(TAG, ne.toString());
        }

    }

    protected void restartApp() {
        Process.killProcess(Process.myPid());
        AlarmManager alm = (AlarmManager) this.getSystemService(Context.ALARM_SERVICE);
        alm.set(AlarmManager.RTC, System.currentTimeMillis() + 1000, PendingIntent.getActivity(this, 0, new Intent(this, this.getClass()), 0));
    }
}
