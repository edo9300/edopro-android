package io.github.edo9300.edopro;

import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import libwindbot.windbot.WindBot;

import android.net.wifi.WifiManager;

public class EpNativeActivity extends NativeActivity {

    private static native void putComboBoxResult(int index);
	private static native void pauseApp(boolean pause);
	static {
		System.loadLibrary("EdoproClient");
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		IntentFilter filter = new IntentFilter();
		filter.addAction("RUN_WINDBOT");
		filter.addAction("MAKE_CHOICE");
		registerReceiver(myReceiver, filter);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	@Override
	protected void onResume() {
		super.onResume();
		makeFullScreen();
		pauseApp(false);
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
	private final BroadcastReceiver myReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals("RUN_WINDBOT")) {
				String args = intent.getStringExtra("args");
				Log.i("Edoprowindbot", "Launching windbot with " + args + " as parameters.");
				WindBot.runAndroid(args);
			} else if(action.equals("MAKE_CHOICE")){
				String parameters[] = intent.getStringArrayExtra("args");
				AlertDialog.Builder builder = new AlertDialog.Builder(EpNativeActivity.this);
				// Add the buttons
				builder.setCancelable(false);
				builder.setItems(parameters, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						putComboBoxResult(id);
					}
				});
				builder.create().show();
			}
		}
	};

	public void launchWindbot(String parameters){
		Intent intent = new Intent();
		intent.putExtra("args", parameters);
		intent.setAction("RUN_WINDBOT");
		getApplicationContext().sendBroadcast(intent);
	}

	public void showComboBox(String parameters[]) {
		Intent intent = new Intent();
		intent.putExtra("args", parameters);
		intent.setAction("MAKE_CHOICE");
		getApplicationContext().sendBroadcast(intent);
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

	@Override
	public void onStop (){
		super.onStop();
		pauseApp(true);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(myReceiver);
	}
}
