package io.github.edo9300.edopro;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.content.Intent;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Vector;

public class AssetCopy extends Activity {
	static {
		System.loadLibrary("assetcopier");
	}
	public static native boolean makeDirectory(String path);
	public static native boolean fileDelete(String path);
	public static native boolean copyAssetToDestination(AssetManager assetManager, String source, String destination);
	private ProgressBar m_ProgressBar;
	private TextView m_Filename;
	private copyAssetTask m_AssetCopy;
	private String workingDir;
	private boolean isUpdate;
	//private static native void assetsMutexUnlock();

	public boolean copyAsset(String source, String destination) {
		return copyAssetToDestination(getAssets(), source, destination);
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.e("AssetCopy", "AssetCopy on create");
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		Bundle b = getIntent().getExtras();
		String _workingDir = "_workingDir";
		boolean _isUpdate = false;
		if(b != null){
			_workingDir = b.getString("workingDir");
			_isUpdate = b.getBoolean("isUpdate");
		}
		setContentView(R.layout.assetcopy);
		m_ProgressBar = findViewById(R.id.progressBar1);
		m_Filename = findViewById(R.id.textView1);
		DisplayMetrics displaymetrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
		m_ProgressBar.getLayoutParams().width = (int) (displaymetrics.widthPixels * 0.8);
		m_ProgressBar.invalidate();

		/* check if there's already a copy in progress and reuse in case it is*/
		AssetCopy prevActivity =
				(AssetCopy) getLastNonConfigurationInstance();
		if (prevActivity != null) {
			m_AssetCopy = prevActivity.m_AssetCopy;
		} else {
			workingDir = _workingDir;
			isUpdate = _isUpdate;
			m_AssetCopy = new copyAssetTask();
			m_AssetCopy.execute();
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		makeFullScreen();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (m_AssetCopy != null) {
			m_AssetCopy.cancel(true);
		}
	}

	@SuppressWarnings("ObsoleteSdkInt")
	private void makeFullScreen() {
		if (Build.VERSION.SDK_INT >= 19)
			this.getWindow().getDecorView().setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus)
			makeFullScreen();
	}

	/* preserve asset copy background task to prevent restart of copying */
	/* this way of doing it is not recommended for latest android version */
	/* but the recommended way isn't available on android 2.x */
	public Object onRetainNonConfigurationInstance() {
		return this;
	}

	private class copyAssetTask extends AsyncTask<String, Integer, String> {
		boolean m_copy_started = false;
		String m_Foldername = "media";
		Vector<String> m_foldernames;
		Vector<String> m_filenames;
		Vector<String> m_tocopy;

		@Override
		protected String doInBackground(String... files) {
			Log.e("AssetCopy", "doInBackground");
			m_foldernames = new Vector<>();
			m_filenames = new Vector<>();
			m_tocopy = new Vector<>();

			// build lists from prepared data
			BuildFolderList();
			BuildFileList();

			// scan filelist
			ProcessFileList();

			// doing work
			m_copy_started = m_tocopy.size() > 0;
			m_ProgressBar.setMax(m_tocopy.size());

			for (int i = 0; i < m_tocopy.size(); i++) {
				String filename = m_tocopy.get(i);
				String full_source_filename = isUpdate ? "update/" + filename : "defaults/" + filename;
				publishProgress(i);
				if(copyAsset(full_source_filename, workingDir + "/" + filename))
					Log.v("AssetCopy", "Copied file: " +	m_tocopy.get(i));
				else
					Log.e("AssetCopy", "Copying file: " + m_tocopy.get(i));
			}
			return "";
		}

		/**
		 * update progress bar
		 */
		protected void onProgressUpdate(Integer... progress) {
			if (m_copy_started) {
				String todisplay = m_tocopy.get(progress[0]);
				m_ProgressBar.setProgress(progress[0]);
				m_Filename.setText(todisplay);
			} else {
				String todisplay = m_Foldername;
				String full_text = "scanning " + todisplay + " ...";
				m_Filename.setText(full_text);
			}
		}

		/**
		 * check all files and folders in filelist
		 */
		void ProcessFileList() {

			for (String current_path : m_filenames) {
				String FlashPath = workingDir + "/" + current_path;
				if (isAssetFolder(current_path)) {
					if(current_path.isEmpty()){
						FlashPath=workingDir;
					}
					/* store information and update gui */
					if(isUpdate)
						m_Foldername = "update/" + current_path;
					else
						m_Foldername = "defaults/" + current_path;
					publishProgress(0);
					/* open file in order to check if it's a folder */
					makeDirectory(FlashPath);
					if (!makeDirectory(FlashPath)) {
						Log.e("AssetCopy", "\t failed create folder: " +
								FlashPath);
					} else {
						Log.v("AssetCopy", "\t created folder: " +
								FlashPath);
					}
					continue;
				}
				if(isUpdate)
					fileDelete(FlashPath);
				m_tocopy.add(current_path);
			}
		}

		/**
		 * read list of folders prepared on package build
		 */
		void BuildFolderList() {
			try {
				InputStream is = getAssets().open(isUpdate ? "indexu.txt" : "index.txt");
				BufferedReader reader = new BufferedReader(new InputStreamReader(is));

				String line = reader.readLine();
				while (line != null) {
					m_foldernames.add(line);
					line = reader.readLine();
				}
				is.close();
			} catch (IOException e) {
				Log.e("AssetCopy", "Error on processing index.txt " + e.getMessage());
				e.printStackTrace();
			}
		}

		/**
		 * read list of asset files prepared on package build
		 */
		@SuppressWarnings("unused")
		void BuildFileList() {
			long entrycount = 0;
			try {
				InputStream is = getAssets().open(isUpdate ? "filelistu.txt": "filelist.txt");
				BufferedReader reader = new BufferedReader(new InputStreamReader(is));

				String line = reader.readLine();
				while (line != null) {
					m_filenames.add(line);
					line = reader.readLine();
					entrycount++;
				}
				is.close();
			} catch (IOException e) {
				Log.e("AssetCopy", "Error on processing filelist.txt " + e.getMessage());
				e.printStackTrace();
			}
		}

		protected void onPostExecute(String result) {
			Intent returnIntent = new Intent();
			setResult(Activity.RESULT_CANCELED, returnIntent);
			finish();
		}

		boolean isAssetFolder(String path) {
			return m_foldernames.contains(path);
		}
	}
}