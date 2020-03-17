package io.github.edo9300.edopro;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import libwindbot.windbot.WindBot;
import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends Activity {

	private final static int PERMISSIONS = 1;
	private static final String[] REQUIRED_SDK_PERMISSIONS = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};
	public static String working_directory;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			checkPermission();
		} else {
			getworkingDirectory();
		}
	}

	protected void checkPermission() {
		final List<String> missingPermissions = new ArrayList<>();
		// check required permission
		for (final String permission : REQUIRED_SDK_PERMISSIONS) {
			final int result = ContextCompat.checkSelfPermission(this, permission);
			if (result != PackageManager.PERMISSION_GRANTED) {
				missingPermissions.add(permission);
			}
		}
		if (!missingPermissions.isEmpty()) {
			// request permission
			final String[] permissions = missingPermissions
					.toArray(new String[0]);
			ActivityCompat.requestPermissions(this, permissions, PERMISSIONS);
		} else {
			final int[] grantResults = new int[REQUIRED_SDK_PERMISSIONS.length];
			Arrays.fill(grantResults, PackageManager.PERMISSION_GRANTED);
			onRequestPermissionsResult(PERMISSIONS, REQUIRED_SDK_PERMISSIONS,
					grantResults);
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
										   @NonNull int[] grantResults) {
		switch (requestCode) {
			case PERMISSIONS:
				for (int index = 0; index < permissions.length; index++) {
					if (grantResults[index] != PackageManager.PERMISSION_GRANTED) {
						// permission not granted - toast and exit
						Toast.makeText(this, R.string.not_granted, Toast.LENGTH_LONG).show();
						finish();
						return;
					}
				}
				// permission were granted - run
				getworkingDirectory();
				break;
		}
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch(requestCode) {
			case 1: {
				try {
					File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
					file.createNewFile();
				} catch (Exception e){
					Log.e("Edopro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
				break;
			}
			case 2: {
				if (resultCode == Activity.RESULT_CANCELED) {
					break;
				}
				Uri uri = data.getData();
				Log.i("Edopro", "Result URI " + uri);
				String dest_dir = FileUtil.getFullPathFromTreeUri(uri,this);
				Log.i("Edopro", "Parsed result URI " + dest_dir);
				if(dest_dir.startsWith("/storage/emulated/0"))
					setWorkingDir(dest_dir);
				else {
					File[] paths = getApplicationContext().getExternalFilesDirs("Edopro");
					String storage = dest_dir.split("/")[2];
					boolean found = false;
					for(int i = 0; i < paths.length; i++) {
						Log.i("Edopro", "Path " + i + " is: " + paths[i]);
						if(storage.equals(paths[i].getAbsolutePath().split("/")[2])){
							Log.i("Edopro", "path matching with " + dest_dir + " is: " + paths[i].getAbsolutePath());
							dest_dir = paths[i].getAbsolutePath();
							if (!paths[i].exists()){
								paths[i].mkdirs();
							}
							found = true;
							break;
						}

					}
					if(found) {
						Toast.makeText(getApplicationContext(), "Using " + dest_dir + " as working directory", Toast.LENGTH_LONG).show();
						setWorkingDir(dest_dir);
					} else {
						Log.e("Edopro", "couldn't find matching storage");
						finish();
					}
				}
				break;
			}
	    }
	}

	public void next() {
		WindBot.initAndroid(working_directory + "/Windbot");
		Intent intent = new Intent(this, SdlLauncher.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(intent);
	}

	public void getworkingDirectory() {
		File file = new File(getApplicationContext().getFilesDir(),"working_dir");
		if(file.exists()) {
			try {
				BufferedReader br = new BufferedReader(new FileReader(file));
				working_directory = br.readLine();
				br.close();
				if(working_directory != null) {
					copyAssetsPrompt(working_directory);
					return;
				}
			}
			catch (IOException e) {
				Log.e("Edopro", "working directory file found but not read: " + e.getMessage());
			}
		}
		chooseWorkingDir();
	}

	public void chooseWorkingDir(){
		Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
		i.addCategory(Intent.CATEGORY_DEFAULT);
		startActivityForResult(Intent.createChooser(i, "Choose directory"), 2);
	}

	public void setWorkingDir(String dest_dir){
		File file = new File(getApplicationContext().getFilesDir(),"working_dir");
		try {
			FileOutputStream fOut = new FileOutputStream(file);
			OutputStreamWriter myOutWriter = new OutputStreamWriter(fOut);
			myOutWriter.append(dest_dir);
			myOutWriter.close();
			fOut.close();
		} catch(Exception e) {
			Log.e("Edopro", "cannot write to working directory file: " + e.getMessage());
			finish();
			return;
		}
		copyAssetsPrompt(dest_dir);
	}

	public void copyAssetsPrompt(final String working_dir) {
		File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
		if(file.exists()){
			next();
			return;
		}
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Copy internal assets?").setNegativeButton("No", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				try {
					File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
					file.createNewFile();
				} catch (Exception e){
					Log.e("Edopro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
			}
		}).setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				copyAssets(working_dir);
			}
		}).setCancelable(false).show();
    }

    public void copyAssets(String working_dir){
		Intent intent = new Intent(this, AssetCopy.class);
		Bundle params = new Bundle();
		params.putString("workingDir", working_dir);
		intent.putExtras(params);
		startActivityForResult(intent, 1);
    }
}
