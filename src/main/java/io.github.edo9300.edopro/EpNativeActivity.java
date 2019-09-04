package io.github.edo9300.edopro;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

import java.net.URL;
import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class EpNativeActivity extends NativeActivity {

	static {
		System.loadLibrary("EdoproClient");
	}

	private int m_MessagReturnCode;
	private String m_MessageReturnValue;

	public static native void putMessageBoxResult(String text);

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		m_MessagReturnCode = -1;
		m_MessageReturnValue = "";
	}

	@Override
	protected void onResume() {
		super.onResume();
		makeFullScreen();
	}

	public void makeFullScreen() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			this.getWindow().getDecorView().setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
			);
		}
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			makeFullScreen();
		}
	}

	public void copyAssets() {
		Intent intent = new Intent(this, MinetestAssetCopy.class);
		startActivity(intent);
	}

	public void showDialog(String acceptButton, String hint, String current,
						   int editType) {

		Intent intent = new Intent(this, MinetestTextEntry.class);
		Bundle params = new Bundle();
		params.putString("acceptButton", acceptButton);
		params.putString("hint", hint);
		params.putString("current", current);
		params.putInt("editType", editType);
		intent.putExtras(params);
		startActivityForResult(intent, 101);
		m_MessageReturnValue = "";
		m_MessagReturnCode = -1;
	}

	/* ugly code to workaround putMessageBoxResult not beeing found */
	public int getDialogState() {
		return m_MessagReturnCode;
	}

	public String getDialogValue() {
		m_MessagReturnCode = -1;
		return m_MessageReturnValue;
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

	@Override
	protected void onActivityResult(int requestCode, int resultCode,
									Intent data) {
		if (requestCode == 101) {
			if (resultCode == RESULT_OK) {
				String text = data.getStringExtra("text");
				m_MessagReturnCode = 0;
				m_MessageReturnValue = text;
			} else {
				m_MessagReturnCode = 1;
			}
		}
	}

	public static int memcmp(byte b1[], byte b2[], int sz){
		for(int i = 0; i < sz; i++){
			if(b1[i] != b2[i]){
				if((b1[i] >= 0 && b2[i] >= 0)||(b1[i] < 0 && b2[i] < 0))
					return b1[i] - b2[i];
				if(b1[i] < 0 && b2[i] >= 0)
					return 1;
				if(b2[i] < 0 && b1[i] >=0)
					return -1;
			}
		}
		return 0;
	}

	public int checkHeader(byte data[]){
		byte jpg_header[] = {(byte)0xff, (byte)0xd8, (byte)0xff};
		byte png_header[] = {(byte)0x89, (byte)0x50, (byte)0x4e, (byte)0x47, (byte)0x0d, (byte)0x0a, (byte)0x1a, (byte)0x0a};
		if(memcmp(png_header, data, 8) == 0)
			return 1;
		else if(memcmp(jpg_header, data, 3) == 0)
			return 2;
		return 0;
	}

	public int downloadFile(String url, String path) {
		int extension = 0;
		try (BufferedInputStream inputStream = new BufferedInputStream(new URL(url).openStream());
			 FileOutputStream fileOS = new FileOutputStream(path)) {
			byte data[] = new byte[1024];
			int byteContent;
			/*if((byteContent = inputStream.read(data, 0, 8)) != -1){
				if((byteContent >= 8) && ((extension = checkHeader(data)) != 0)) {
					fileOS.write(data, 0, byteContent);*/
					while ((byteContent = inputStream.read(data, 0, 1024)) != -1) {
						fileOS.write(data, 0, byteContent);
					}
			extension = 1;
				//}
			//}
		} catch (IOException e) {
			return 0;
			// handles IO exceptions
		}
		return extension;
	}
}
