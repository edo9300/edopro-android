package io.github.edo9300.edopro;

import org.libsdl.app.SDLActivity;
import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.widget.Toast;
import android.content.Context;
import android.os.Environment;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends SDLActivity {

	private final static int PERMISSIONS = 1;
	private static final String[] REQUIRED_SDK_PERMISSIONS = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
			checkPermission();
		} else {
			copyAssets();
		}
	}

	protected void checkPermission() {
		final List<String> missingPermissions = new ArrayList<String>();
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
					.toArray(new String[missingPermissions.size()]);
			ActivityCompat.requestPermissions(this, permissions, PERMISSIONS);
		} else {
			final int[] grantResults = new int[REQUIRED_SDK_PERMISSIONS.length];
			Arrays.fill(grantResults, PackageManager.PERMISSION_GRANTED);
			onRequestPermissionsResult(PERMISSIONS, REQUIRED_SDK_PERMISSIONS,
					grantResults);
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[],
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
				copyAssets();
				break;
		}
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {

	    if (requestCode == 1) {
	        if(resultCode == Activity.RESULT_OK){
	            next();
	        }
	        if (resultCode == Activity.RESULT_CANCELED) {
	            next();
	        }
	    }
	}
	public void next() {
		/*Intent intent = new Intent(this, SDLActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(intent);*/
		Intent intent2 = new Intent(this, EpNativeActivity.class);
		intent2.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		startActivity(intent2);
	}

	public void copyAssets() {
		Intent intent = new Intent(this, MinetestAssetCopy.class);
		Bundle params = new Bundle();
		params.putString("workingDir", getExternalFilesDir(null).getAbsolutePath() + "/Edopro");
		intent.putExtras(params);
		startActivityForResult(intent, 1);
    }
}
