package io.github.edo9300.edopro;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
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

import static android.content.Intent.URI_INTENT_SCHEME;

public class MainActivity extends Activity {

	private final static int PERMISSIONS = 1;
	private static final String[] REQUIRED_SDK_PERMISSIONS = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};
	public static String working_directory;
	public static ArrayList<String> parameter;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Uri data = getIntent().getData();
		parameter = new ArrayList<String>();
		if(data!=null) {
			getIntent().setData(null);
			try {
				Log.e("Edopro", data.getPath());
				String path = FileUtil.getFullPathFromTreeUri(data,this);
				parameter.add(path);
				Log.e("Edopro", "parsed path: " + parameter);
			} catch (Exception e) {
				String path = data.getPath();
				parameter.add(path);
				Log.e("Edopro", "It was already a path: " + data.getPath());
			}
		}
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			checkPermission();
		} else {
			getWorkingDirectory();
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
	@SuppressWarnings("SwitchStatementWithTooFewBranches")
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
				getWorkingDirectory();
				break;
		}
	}

	@SuppressWarnings("ResultOfMethodCallIgnored")
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch(requestCode) {
			case 1: {
				try {
					File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
					file.createNewFile();
				} catch (Exception e){
					Log.e("EDOPro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
				break;
			}
			case 2: {
				if (resultCode == Activity.RESULT_CANCELED) {
					break;
				}
				Uri uri = data.getData();
				Log.i("EDOPro", "Result URI " + uri);
				String dest_dir = FileUtil.getFullPathFromTreeUri(uri,this);
				if(dest_dir == null){
					Log.e("EDOPro", "returned URI is null");
					finish();
					break;
				}
				Log.i("EDOPro", "Parsed result URI " + dest_dir);
				if(dest_dir.startsWith("/storage/emulated/0"))
					setWorkingDir(dest_dir);
				else {
					File[] paths = getApplicationContext().getExternalFilesDirs("EDOPro");
					String storage = dest_dir.split("/")[2];
					boolean found = false;
					for(int i = 0; i < paths.length; i++) {
						Log.i("EDOPro", "Path " + i + " is: " + paths[i]);
						if(storage.equals(paths[i].getAbsolutePath().split("/")[2])){
							Log.i("EDOPro", "path matching with " + dest_dir + " is: " + paths[i].getAbsolutePath());
							dest_dir = paths[i].getAbsolutePath();
							if (!paths[i].exists()){
								paths[i].mkdirs();
							}
							found = true;
							break;
						}

					}
					if(found) {
						Toast.makeText(getApplicationContext(),  String.format(getResources().getString(R.string.toast_dir), dest_dir), Toast.LENGTH_LONG).show();
						final String cbdir = dest_dir;
						AlertDialog.Builder builder = new AlertDialog.Builder(this);
						builder.setMessage(String.format(getResources().getString(R.string.default_path), dest_dir))
								.setCancelable(false)
								.setPositiveButton("OK", new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog, int id) {
										setWorkingDir(cbdir);
									}
								});
						AlertDialog alert = builder.create();
						alert.show();
					} else {
						Log.e("EDOPro", "couldn't find matching storage");
						finish();
					}
				}
				break;
			}
	    }
	}

	public void next() {
		WindBot.initAndroid(working_directory + "/WindBot");
		Intent intent = new Intent(this, SdlLauncher.class);
		intent.putStringArrayListExtra("ARGUMENTS", parameter);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(intent);
	}

	public void getWorkingDirectory() {
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
				Log.e("EDOPro", "working directory file found but not read: " + e.getMessage());
			}
		}
		chooseWorkingDir();
	}

	public void chooseWorkingDir(){
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(R.string.folder_prompt)
				.setCancelable(false)
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
						i.addCategory(Intent.CATEGORY_DEFAULT);
						startActivityForResult(Intent.createChooser(i, "Choose directory"), 2);
					}
				});
		AlertDialog alert = builder.create();
		alert.show();
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
			Log.e("EDOPro", "cannot write to working directory file: " + e.getMessage());
			finish();
			return;
		}
		copyAssetsPrompt(dest_dir);
	}

	@SuppressWarnings("ResultOfMethodCallIgnored")
	public void copyAssetsPrompt(final String working_dir) {
		File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
		if(file.exists()){
			next();
			return;
		}
		copyCertificate();
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(R.string.assets_prompt).setNegativeButton("No", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				try {
					File file = new File(getApplicationContext().getFilesDir(),"assets_copied");
					file.createNewFile();
				} catch (Exception e){
					Log.e("EDOPro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
			}
		}).setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				copyAssets(working_dir);
			}
		}).setCancelable(false).show();
    }

    public void copyCertificate(){
		try {
			File certout = new File(getApplicationContext().getFilesDir(),"cacert.cer");
			if(certout.exists()){
				Log.i("EDOPro", "Certificate file already copied");
			} else {
				InputStream certin = getAssets().open("cacert.cer");
				try {
					FileOutputStream fOut = new FileOutputStream(certout);
					byte[] buffer = new byte[1024];
					int length;
					while ((length = certin.read(buffer)) > 0) {
						fOut.write(buffer, 0, length);
					}
					fOut.close();
				} catch (Exception e) {
					Log.e("EDOPro", "cannot copy certificate file: " + e.getMessage());
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

    public void copyAssets(String working_dir){
		Intent intent = new Intent(this, AssetCopy.class);
		Bundle params = new Bundle();
		params.putString("workingDir", working_dir);
		intent.putExtras(params);
		startActivityForResult(intent, 1);
    }
}
