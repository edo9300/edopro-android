package io.github.edo9300.edopro;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

import android.net.wifi.WifiManager;
import android.text.format.Formatter;

public class EpNativeActivity extends NativeActivity {

	static {
		System.loadLibrary("EdoproClient");
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	@Override
	protected void onResume() {
		super.onResume();
		makeFullScreen();
	}

    private void makeFullScreen() {
		if (Build.VERSION.SDK_INT >= 19) {
			this.getWindow().getDecorView().setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		}
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus)
			makeFullScreen();
	}

	public void copyAssets() {
		Intent intent = new Intent(this, MinetestAssetCopy.class);
		startActivity(intent);
	}

	public void showDialog(String acceptButton, String hint, String current,
						   int editType) {

		Intent intent = new Intent(this, TextEntry.class);
		Bundle params = new Bundle();
		params.putString("acceptButton", acceptButton);
		params.putString("hint", hint);
		params.putString("current", current);
		params.putInt("editType", editType);
		intent.putExtras(params);
		startActivity(intent);
	}

	public float getDensity() {
		return getResources().getDisplayMetrics().density;
	}

	public int getDisplayWidth() {
		return getResources().getDisplayMetrics().widthPixels;
	}

	public int getDisplayHeight() {
		return getResources().getDisplayMetrics().heightPixels;
	}

	public int getLocalIpAddress() {
		WifiManager wm = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
		return wm.getConnectionInfo().getIpAddress();
	}
}
