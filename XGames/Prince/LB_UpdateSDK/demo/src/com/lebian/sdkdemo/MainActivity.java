package com.lebian.sdkdemo;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;

import com.excelliance.kxqp.sdk.GameSdk;
import com.excelliance.kxqp.sdk.IQueryUpdateCallback;

public class MainActivity extends Activity {
	private boolean queried = false;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

	@Override
	protected void onResume() {
		Log.d("MainActivity", "onResume");
		super.onResume();
		
		if (!queried) {
			queried = true;
			final IQueryUpdateCallback callBack = new IQueryUpdateCallback() {
				public void onUpdateResult(int result) {
					Log.d("MainActivity", "onResume result="+result);
				}
			};
			GameSdk.queryUpdate(this, callBack);
		}
		
	}
}
