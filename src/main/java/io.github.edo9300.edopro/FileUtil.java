package io.github.edo9300.edopro;

/*
code from http://web.archive.org/web/20191227151637/https://stackoverflow.com/questions/34927748/android-5-0-documentfile-from-tree-uri
 */

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.Context;
import android.net.Uri;
import android.os.Build;
import android.os.storage.StorageManager;
import android.provider.DocumentsContract;
import android.support.annotation.Nullable;

import java.io.File;
import java.lang.reflect.Array;
import java.lang.reflect.Method;

public final class FileUtil {
	static String TAG = "TAG";
	private static final String PRIMARY_VOLUME_NAME = "primary";

	@Nullable
	public static String getFullPathFromTreeUri(@Nullable final Uri treeUri, Context con) {
		if (treeUri == null) return null;
		try {
			String volumePath = getVolumePath(getVolumeIdFromTreeUri(treeUri), con);
			if (volumePath == null) return File.separator;
			if (volumePath.endsWith(File.separator))
				volumePath = volumePath.substring(0, volumePath.length() - 1);

			String documentPath = getDocumentPathFromTreeUri(treeUri);
			if (documentPath.endsWith(File.separator))
				documentPath = documentPath.substring(0, documentPath.length() - 1);

			if (documentPath.length() > 0) {
				if (documentPath.startsWith(File.separator))
					return volumePath + documentPath;
				else
					return volumePath + File.separator + documentPath;
			} else return volumePath;
		} catch (Exception ex) {
			String message = ex.getMessage();
			if (message != null && message.startsWith("raw path")) {
				return message.split(":")[1];
			}
			return null;
		}
	}


	@SuppressLint("ObsoleteSdkInt")
	private static String getVolumePath(final String volumeId, Context context) {
		if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) return null;
		try {
			StorageManager mStorageManager =
					(StorageManager) context.getSystemService(Context.STORAGE_SERVICE);
			Class<?> storageVolumeClazz = Class.forName("android.os.storage.StorageVolume");
			Method getVolumeList = mStorageManager.getClass().getMethod("getVolumeList");
			Method getUuid = storageVolumeClazz.getMethod("getUuid");
			Method getPath = storageVolumeClazz.getMethod("getPath");
			Method isPrimary = storageVolumeClazz.getMethod("isPrimary");
			Object result = getVolumeList.invoke(mStorageManager);

			final int length = Array.getLength(result);
			for (int i = 0; i < length; i++) {
				Object storageVolumeElement = Array.get(result, i);
				String uuid = (String) getUuid.invoke(storageVolumeElement);
				Boolean primary = (Boolean) isPrimary.invoke(storageVolumeElement);

				// primary volume?
				if (primary && PRIMARY_VOLUME_NAME.equals(volumeId))
					return (String) getPath.invoke(storageVolumeElement);

				// other volumes?
				if (uuid != null && uuid.equals(volumeId))
					return (String) getPath.invoke(storageVolumeElement);
			}
			// not found.
			return null;
		} catch (Exception ex) {
			return null;
		}
	}

	@TargetApi(Build.VERSION_CODES.LOLLIPOP)
	private static String getVolumeIdFromTreeUri(final Uri treeUri) throws Exception {
		final String docId = DocumentsContract.getTreeDocumentId(treeUri);
		final String[] split = docId.split(":");
		if (split.length > 0) {
			if ("raw".equals(split[0])) {
				throw new Exception("raw path:" + split[1]);
			}
			return split[0];
		} else return null;
	}


	@TargetApi(Build.VERSION_CODES.LOLLIPOP)
	private static String getDocumentPathFromTreeUri(final Uri treeUri) {
		final String docId = DocumentsContract.getTreeDocumentId(treeUri);
		final String[] split = docId.split(":");
		if ((split.length >= 2) && (split[1] != null)) return split[1];
		else return File.separator;
	}
}