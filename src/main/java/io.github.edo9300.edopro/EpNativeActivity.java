package io.github.edo9300.edopro;

import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.BroadcastReceiver;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import libwindbot.windbot.WindBot;

import android.net.wifi.WifiManager;

import java.io.File;

import static android.content.ClipDescription.MIMETYPE_TEXT_PLAIN;
public class EpNativeActivity extends NativeActivity {

	public static native void putComboBoxResult(int index);
	public static native void putMessageBoxResult(String text, boolean isenter);
	public static native void errorDialogReturn();

	private boolean use_windbot;

	static {
		//on 4.2 libraries aren't properly loaded automatically
		//https://stackoverflow.com/questions/28806373/android-4-2-ndk-library-loading-crash-load-librarylinker-cpp750-soinfo-l/28817942
		/*if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP){
			System.loadLibrary("hidapi");
			System.loadLibrary("SDL2");
			System.loadLibrary("mpg123");
			System.loadLibrary("SDL2_mixer");
		}*/
		System.loadLibrary("EDOProClient");
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Bundle ex = getIntent().getExtras();
		use_windbot = ex.getBoolean("USE_WINDBOT",true);
		IntentFilter filter = new IntentFilter();
		filter.addAction("RUN_WINDBOT");
		filter.addAction("ATTACH_WINDBOT_DATABASE");
		filter.addAction("INPUT_TEXT");
		filter.addAction("MAKE_CHOICE");
		filter.addAction("INSTALL_UPDATE");
		filter.addAction("OPEN_SCRIPT");
		filter.addAction("SHOW_ERROR_WINDOW");
		registerReceiver(myReceiver, filter);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}

	@Override
	protected void onResume() {
		super.onResume();
		makeFullScreen();
	}

	@SuppressWarnings("ObsoleteSdkInt")
	private void makeFullScreen() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
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

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event){
		return (keyCode == KeyEvent.KEYCODE_BACK || super.onKeyDown(keyCode, event));
	}

	private final BroadcastReceiver myReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if ("RUN_WINDBOT".equals(action)) {
				String args = intent.getStringExtra("args");
				Log.i("EDOProWindBotIgnite", "Launching WindBot Ignite with " + args + " as parameters.");
				WindBot.runAndroid(args);
			} else if ("ATTACH_WINDBOT_DATABASE".equals(action)) {
				String args = intent.getStringExtra("args");
				Log.i("EDOProWindBotIgnite", "Loading database: " +args+".");
				WindBot.addDatabase(args);
			} else if("INPUT_TEXT".equals(action)){
				new TextEntry().Show(context, intent.getStringExtra("current"));
			} else if("MAKE_CHOICE".equals(action)){
				String[] parameters = intent.getStringArrayExtra("args");
				AlertDialog.Builder builder = new AlertDialog.Builder(EpNativeActivity.this);
				// Add the buttons
				//builder.setCancelable(false);
				builder.setItems(parameters, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						putComboBoxResult(id);
					}
				});
				builder.create().show();
			} else if("INSTALL_UPDATE".equals(action)){
				String path = intent.getStringExtra("args");
				Log.i("EDOProUpdater", "Installing update from: \"" + path + "\".");
				Intent _intent = new Intent(Intent.ACTION_VIEW);
				_intent.setDataAndType(FileProvider.getUriForFile(context, context.getApplicationContext().getPackageName() + ".provider", new File(path)), "application/vnd.android.package-archive");
				_intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				_intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
				startActivity(_intent);
			} else if("OPEN_SCRIPT".equals(action)){
				String path = intent.getStringExtra("args");
				Log.i("EDOPro", "opening script from: "+path);
				Intent fileIntent = new Intent(Intent.ACTION_VIEW);
				Uri uri = FileProvider.getUriForFile(context, context.getApplicationContext().getPackageName() + ".provider", new File(path));
				fileIntent.setDataAndType(uri, "text/*");
				fileIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				fileIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION | Intent.FLAG_GRANT_READ_URI_PERMISSION);
				startActivity(fileIntent);
			} else if("SHOW_ERROR_WINDOW".equals(action)){
				String message_context = intent.getStringExtra("context");
				String message = intent.getStringExtra("message");
				Log.i("EDOPro", "Received show error dialog " + message_context + " "+message);
				AlertDialog.Builder builder = new AlertDialog.Builder(EpNativeActivity.this);
				builder.setTitle(message_context);
				builder.setMessage(message);
				builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						errorDialogReturn();
					}
				});
				builder.setCancelable(false);
				AlertDialog dialog = builder.create();
				dialog.show();
			}
		}
	};

	@SuppressWarnings("unused")
	public void launchWindbot(String parameters) {
		if(!use_windbot)
			return;
		Intent intent = new Intent();
		intent.putExtra("args", parameters);
		intent.setAction("RUN_WINDBOT");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void addWindbotDatabase(String database) {
		if(!use_windbot)
			return;
		Intent intent = new Intent();
		intent.putExtra("args", database);
		intent.setAction("ATTACH_WINDBOT_DATABASE");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void showDialog(String current) {
		Intent intent = new Intent();
		intent.putExtra("current", current);
		intent.setAction("INPUT_TEXT");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void showComboBox(String[] parameters) {
		Intent intent = new Intent();
		intent.putExtra("args", parameters);
		intent.setAction("MAKE_CHOICE");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void installUpdate(String path) {
		try {
			File file = new File(getFilesDir(),"should_copy_update");
			if(!file.createNewFile()){
				Log.e("EDOPro", "error when creating should_copy_update file:");
			}
		} catch (Exception e){
			Log.e("EDOPro", "error when creating should_copy_update file: " + e.getMessage());
		}
		Intent intent = new Intent();
		intent.putExtra("args", path);
		intent.setAction("INSTALL_UPDATE");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void openUrl(String url) {
		Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		startActivity(browserIntent);
	}

	@SuppressWarnings("unused")
	public void openFile(String path) {
		Intent intent = new Intent();
		intent.putExtra("args", path);
		intent.setAction("OPEN_SCRIPT");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public void showErrorDialog(String context, String message) {
		Intent intent = new Intent();
		intent.putExtra("context", context);
		intent.putExtra("message", message);
		intent.setAction("SHOW_ERROR_WINDOW");
		getApplicationContext().sendBroadcast(intent);
	}

	@SuppressWarnings("unused")
	public float getDensity() {
		return getResources().getDisplayMetrics().density;
	}

	@SuppressWarnings("unused")
	public int getDisplayWidth() {
		return getResources().getDisplayMetrics().widthPixels;
	}

	@SuppressWarnings("unused")
	public int getDisplayHeight() {
		return getResources().getDisplayMetrics().heightPixels;
	}

	@SuppressWarnings("unused")
	public int getLocalIpAddress() {
		WifiManager wm = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
		return wm.getConnectionInfo().getIpAddress();
	}

	public void setClipboard(final String text) {
		EpNativeActivity.this.runOnUiThread(new Runnable() {
			@Override
			public void run(){
				ClipboardManager clipboard = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
				ClipData clip = ClipData.newPlainText("", text);
				clipboard.setPrimaryClip(clip);
			}
		});
	}

	class RunnableObject implements Runnable {
		public String result = "";
		public void run () {
			ClipboardManager clipboard = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
			if(!(clipboard.hasPrimaryClip()) || clipboard.getPrimaryClip() == null || (clipboard.getPrimaryClipDescription() == null)
					|| !(clipboard.getPrimaryClipDescription().hasMimeType(MIMETYPE_TEXT_PLAIN))) {
				result = "";
			} else {
				ClipData clip = clipboard.getPrimaryClip();
				result = clip.getItemAt(0).getText().toString();
			}
			synchronized(this)
			{
				this.notify();
			}
		}
	}

	public String getClipboard() {
		RunnableObject myRunnable = new RunnableObject();
		synchronized( myRunnable ) {
			EpNativeActivity.this.runOnUiThread(myRunnable);

			try {
				myRunnable.wait() ; // unlocks myRunable while waiting
			} catch (InterruptedException e) {
				return "";
			}
		}
		return myRunnable.result;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(myReceiver);
	}
}
