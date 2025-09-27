package io.github.edo9300.edopro;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.provider.DocumentsContract;

import androidx.annotation.RequiresApi;

public class ManageFilesActivity extends Activity {
	@RequiresApi(Build.VERSION_CODES.R)
	private String getDocumentsUiPackage() {
		var packageInfos = getPackageManager().getPackagesHoldingPermissions(new String[]{android.Manifest.permission.MANAGE_DOCUMENTS}, 0);
		for (final var packageinfo : packageInfos) {
			if (packageinfo.packageName.endsWith(".documentsui"))
				return packageinfo.packageName;
		}
		return null;
	}

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
			var documentPackage = getDocumentsUiPackage();
			if (documentPackage == null)
				return;
			final var EXTERNAL_STORAGE_PROVIDER_AUTHORITY = "com.android.externalstorage.documents";
			final var DOCUMENT_ID_PRIMARY = "primary";
			final var DOCUMENT_ID_PRIMARY_ANDROID_DATA = "primary:Android/data/" + getApplicationContext().getPackageName() + "/files/EDOPro";
			final var TREE_URI_PRIMARY_ANDROID = DocumentsContract.buildTreeDocumentUri(EXTERNAL_STORAGE_PROVIDER_AUTHORITY, DOCUMENT_ID_PRIMARY);
			final var DOCUMENT_URI_ANDROID_DATA = DocumentsContract.buildDocumentUriUsingTree(TREE_URI_PRIMARY_ANDROID, DOCUMENT_ID_PRIMARY_ANDROID_DATA);
			Intent documentViewerIntent = new Intent(Intent.ACTION_VIEW)
					.setDataAndType(DOCUMENT_URI_ANDROID_DATA, DocumentsContract.Document.MIME_TYPE_DIR)
					.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION).setPackage(documentPackage);
			if (documentViewerIntent.resolveActivity(getPackageManager()) != null)
				startActivity(documentViewerIntent);
		}
		finish();
	}
}
