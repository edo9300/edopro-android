package io.github.edo9300.edopro;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import libwindbot.windbot.WindBot;

public class MainActivity extends Activity {

	private final static int PERMISSIONS = 1;
	private static final String[] REQUIRED_SDK_PERMISSIONS = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};
	private static String working_directory;
	private static boolean changelog;
	private static ArrayList<String> parameter;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
		if (!isTaskRoot()) {
			if (intent.hasCategory(Intent.CATEGORY_LAUNCHER)
					&& intent.getAction() != null
					&& intent.getAction().equals(Intent.ACTION_MAIN)) {
				finish();
				return;
			}
		}
		parameter = new ArrayList<String>();
		if (intent.getAction() != null
				&& intent.getAction().equals(Intent.ACTION_VIEW)) {
			Log.e("Edopro open file", "aa");
			if (!isTaskRoot()) {
				Log.e("Edopro open file", "bb");
				/* TODO: Send drop event */
				finish();
				return;
			}
			Uri data = intent.getData();
			if (data != null) {
				intent.setData(null);
				try {
					Log.e("Edopro", data.getPath());
					String path = FileUtil.getFullPathFromTreeUri(data, this);
					parameter.add(path);
					Log.e("Edopro", "parsed path: " + parameter);
				} catch (Exception e) {
					String path = data.getPath();
					parameter.add(path);
					Log.e("Edopro", "It was already a path: " + data.getPath());
				}
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
		switch (requestCode) {
			case 1: {
				try {
					File file = new File(getFilesDir(), "assets_copied");
					if (file.exists() || file.createNewFile()) {
						FileWriter wr = (new FileWriter(file));
						wr.write("" + BuildConfig.VERSION_CODE);
						wr.flush();
					}
				} catch (Exception e) {
					Log.e("EDOPro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
				break;
			}
			case 2: {
				if (resultCode == Activity.RESULT_CANCELED) {
					break;
				}
				if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
					return;
				}
				Uri uri = data.getData();
				Log.i("EDOPro", "Result URI " + uri);
				String dest_dir = FileUtil.getFullPathFromTreeUri(uri, this);
				if (dest_dir == null) {
					Log.e("EDOPro", "returned URI is null");
					finish();
					break;
				}
				Log.i("EDOPro", "Parsed result URI " + dest_dir);
				if (dest_dir.startsWith("/storage/emulated/0"))
					setWorkingDir(dest_dir);
				else {
					File[] paths = getExternalFilesDirs("EDOPro");
					String[] dirs = dest_dir.split("/");
					boolean found = false;
					if (dirs.length > 2) {
						String storage = dirs[2];
						for (int i = 0; i < paths.length; i++) {
							Log.i("EDOPro", "Path " + i + " is: " + paths[i]);
							if (storage.equals(paths[i].getAbsolutePath().split("/")[2])) {
								Log.i("EDOPro", "path matching with " + dest_dir + " is: " + paths[i].getAbsolutePath());
								dest_dir = paths[i].getAbsolutePath();
								if (!paths[i].exists()) {
									paths[i].mkdirs();
								}
								found = true;
								break;
							}
						}
					}
					if (found) {
						Toast.makeText(this, String.format(getResources().getString(R.string.default_dir), dest_dir), Toast.LENGTH_LONG).show();
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
						Toast.makeText(this, getResources().getString(R.string.no_matching), Toast.LENGTH_LONG).show();
						Log.e("EDOPro", "couldn't find matching storage");
						finish();
					}
				}
				break;
			}
		}
	}

	@SuppressWarnings("ConstantConditions")
	public void next() {
		boolean use_windbot = true;
		try {
			/*
			 * windbot loading might fail, for whatever reason,
			 * disable it if that's the case
			 */
			WindBot.initAndroid(working_directory + "/WindBot");
		} catch (Exception e) {
			use_windbot = false;
		}
		/*
			pass the working directory via parameters, rather than making
			the app read the working_dir file
		*/
		if (changelog)
			parameter.add(0, "-l");
		parameter.add(0, working_directory + "/");
		parameter.add(0, "-C");
		Object[] array = parameter.toArray();
		String[] strArr = new String[array.length];
		for (int i = 0; i < array.length; i++) {
			strArr[i] = array[i].toString();
		}
		Intent intent = new Intent(this, EpNativeActivity.class);
		intent.putExtra("ARGUMENTS", strArr);
		intent.putExtra("USE_WINDBOT", use_windbot);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(intent);
	}

	public void getWorkingDirectory() {
		File file = new File(getFilesDir(), "working_dir");
		if (file.exists()) {
			try {
				BufferedReader br = new BufferedReader(new FileReader(file));
				working_directory = br.readLine();
				br.close();
				if (working_directory != null) {
					copyAssetsPrompt(working_directory);
					return;
				}
			} catch (IOException e) {
				Log.e("EDOPro", "working directory file found but not read: " + e.getMessage());
			}
		}
		getDefaultPath();
	}

	public void getDefaultPath() {
		final File path = new File(Environment.getExternalStorageDirectory() + "/EDOPro");
		final String dest_dir = path.getAbsolutePath();
		if (!"".equals(dest_dir)) {
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
				Toast.makeText(this, String.format(getResources().getString(R.string.default_dir), dest_dir), Toast.LENGTH_LONG).show();
				builder.setMessage(String.format(getResources().getString(R.string.default_dir), dest_dir))
						.setCancelable(false)
						.setPositiveButton("OK", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								if (!path.exists()) {
									path.mkdirs();
								}
								setWorkingDir(dest_dir);
							}
						});
			} else {
				builder.setMessage(String.format(getResources().getString(R.string.default_dir_changeable), dest_dir))
						.setCancelable(false)
						.setPositiveButton(R.string.keep_game_folder, new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								if (!path.exists()) {
									path.mkdirs();
								}
								setWorkingDir(dest_dir);
							}
						})
						.setNeutralButton(R.string.change_game_folder, new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								chooseWorkingDir();
							}
						});
			}
			AlertDialog alert = builder.create();
			alert.show();
		}
	}

	@TargetApi(Build.VERSION_CODES.LOLLIPOP)
	public void chooseWorkingDir() {
		Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
		i.addCategory(Intent.CATEGORY_DEFAULT);
		startActivityForResult(Intent.createChooser(i, "Choose directory"), 2);
	}

	public void setWorkingDir(String dest_dir) {
		working_directory = dest_dir;
		File file = new File(getFilesDir(), "working_dir");
		try {
			FileOutputStream fOut = new FileOutputStream(file);
			OutputStreamWriter myOutWriter = new OutputStreamWriter(fOut);
			myOutWriter.append(dest_dir);
			myOutWriter.close();
			fOut.close();
		} catch (Exception e) {
			Log.e("EDOPro", "cannot write to working directory file: " + e.getMessage());
			finish();
			return;
		}
		copyAssetsPrompt(dest_dir);
	}

	public void copyAssetsPrompt(final String working_dir) {
		changelog = false;
		File file = new File(getFilesDir(), "assets_copied");
		if (file.exists()) {
			try {
				BufferedReader fileReader = new BufferedReader(new FileReader(file));
				String line = fileReader.readLine();
				int prevversion = Integer.parseInt(line);
				if (prevversion < BuildConfig.VERSION_CODE) {
					try {
						copyCertificate();
						PrintWriter pw = new PrintWriter(file);
						pw.close();
						Toast.makeText(this, getResources().getString(R.string.copying_update), Toast.LENGTH_LONG).show();
						copyAssets(working_dir, true);
					} catch (Exception e) {
					}
				} else
					next();
				return;
			} catch (Exception e) {
			}
		}
		copyCertificate();
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(R.string.assets_prompt).setNegativeButton("No", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				try {
					File file = new File(getFilesDir(), "assets_copied");
					if (!file.createNewFile()) {
						Log.e("EDOPro", "error when creating assets_copied file");
					} else {
						FileWriter wr = (new FileWriter(file));
						wr.write("" + BuildConfig.VERSION_CODE);
						wr.flush();
					}
				} catch (Exception e) {
					Log.e("EDOPro", "error when creating assets_copied file: " + e.getMessage());
				}
				next();
			}
		}).setPositiveButton("Yes", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				copyAssets(working_dir, false);
			}
		}).setCancelable(false).show();
	}

	public void copyCertificate() {
		try {
			File certout = new File(getFilesDir(), "cacert.pem");
			if (certout.exists()) {
				Log.i("EDOPro", "Certificate file already copied");
			} else {
				InputStream certin = getAssets().open("cacert.pem");
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

	public void copyAssets(String working_dir, boolean isUpdate) {
		changelog = true;
		Intent intent = new Intent(this, AssetCopy.class);
		Bundle params = new Bundle();
		params.putString("workingDir", working_dir);
		params.putBoolean("isUpdate", isUpdate);
		intent.putExtras(params);
		startActivityForResult(intent, 1);
	}
}
