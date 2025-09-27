// Contains GPLv2 or later code from the PPSSPP project.
// https://github.com/hrydgard/ppsspp/blob/ebfa66b0dab6b39bc8a33c4b34ab868fccdf85ba/android/src/org/ppsspp/ppsspp/PpssppActivity.java

package io.github.edo9300.edopro;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.provider.DocumentsContract;
import android.util.Log;

import androidx.annotation.RequiresApi;
import androidx.documentfile.provider.DocumentFile;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;

public class StorageOperations {

	final private String scoped_storage_dir;
	final private Context context;

	StorageOperations(Context parent_context, String root) {
		scoped_storage_dir = root;
		context = parent_context;
	}

	public String normalizeUri(String input) {
		return scoped_storage_dir + Uri.encode(input.replace(scoped_storage_dir, ""));
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	public boolean contentUriRemoveFile(String uriString) {
		try {
			var uri = Uri.parse(normalizeUri(uriString));
			var documentFile = DocumentFile.fromSingleUri(context, uri);
			if (documentFile != null) {
				return documentFile.delete();
			} else {
				return false;
			}
		} catch (Exception e) {
			Log.e("EDOPro", "contentUriRemoveFile exception: " + e);
			return false;
		}
	}

	// Probably slightly faster than contentUriGetFileInfo.
	// Smaller difference now than before I changed that one to a query...
	public enum EXISTS_TYPE {
		NONE,
		FOLDER,
		FILE,
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	private static void closeQuietly(AutoCloseable closeable) {
		if (closeable != null) {
			try {
				closeable.close();
			} catch (RuntimeException rethrown) {
				throw rethrown;
			} catch (Exception ignored) {
			}
		}
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	public EXISTS_TYPE contentUriElementExists(String elementUri) {
		Cursor c = null;
		try {
			var uri = Uri.parse(elementUri);
			c = context.getContentResolver().query(uri, new String[]{
					DocumentsContract.Document.COLUMN_MIME_TYPE,
					DocumentsContract.Document.COLUMN_DOCUMENT_ID,
			}, null, null, null);
			if (c != null) {
				if (c.moveToNext()) {
					final String mimeType = c.getString(0);
					return DocumentsContract.Document.MIME_TYPE_DIR.equals(mimeType) ? EXISTS_TYPE.FOLDER : EXISTS_TYPE.FILE;
				}
			}
			return EXISTS_TYPE.NONE;
		} catch (Exception e) {
			Log.e("EDOPro", "Failed query: " + e);
			return EXISTS_TYPE.NONE;
		} finally {
			closeQuietly(c);
		}
	}

	static private String[] getPathAndFilenameFromUri(String uri_string) {
		if (uri_string.endsWith("%2F")) {
			uri_string = uri_string.substring(0, uri_string.length() - 3);
		}
		int i = uri_string.lastIndexOf("%2F");
		String[] ret = new String[2];
		ret[0] = uri_string.substring(0, i);
		ret[1] = uri_string.substring(i + 3);
		return ret;
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	public boolean contentUriCreateDirectory(String dirUri) {
		try {
			var normalized = normalizeUri(dirUri);
			switch (contentUriElementExists(normalized)) {
				case FOLDER -> {
					return true;
				}
				case NONE -> {
					var pathComponents = getPathAndFilenameFromUri(Uri.parse(normalized).toString());
					var rootTreeUri = Uri.parse(pathComponents[0]);
					var dirName = Uri.decode(pathComponents[1]);
					var documentFile = DocumentFile.fromTreeUri(context, rootTreeUri);
					if (documentFile != null) {
						var createdDir = documentFile.createDirectory(dirName);
						return createdDir != null;
					} else {
						Log.e("EDOPro", "contentUriCreateDirectory: fromTreeUri returned null");
						return false;
					}
				}
				default -> {
					return false;
				}
			}
		} catch (Exception e) {
			Log.e("EDOPro", "contentUriCreateDirectory exception: " + e);
			return false;
		}
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	private boolean contentUriCreateFile(String rootTreeUri, String fileName) {
		try {
			var uri = Uri.parse(rootTreeUri);
			var documentFile = DocumentFile.fromTreeUri(context, uri);
			if (documentFile != null) {
				var createdFile = documentFile.createFile("application/octet-stream", Uri.decode(fileName));
				return createdFile != null;
			} else {
				Log.e("EDOPro", "contentUriCreateFile: fromTreeUri returned null");
				return false;
			}
		} catch (Exception e) {
			Log.e("EDOPro", "contentUriCreateFile exception: " + e);
			return false;
		}
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	public int openContentUri(String uriString, String mode) {
		try {
			if (mode.isEmpty()) {
				return -1;
			}
			var uri = Uri.parse(normalizeUri(uriString));
			var uri_string = uri.toString();
			var exists = contentUriElementExists(uri_string);
			if (exists == EXISTS_TYPE.FOLDER) {
				return -1;
			}
			if (exists == EXISTS_TYPE.NONE) {
				if (!("r".equals(mode) || "rw".equals(mode))) {
					var pathComponents = getPathAndFilenameFromUri(uri_string);
					if (!contentUriCreateFile(pathComponents[0], pathComponents[1]))
						return -1;
				} else {
					return -1;
				}
			}
			try (var filePfd = context.getContentResolver().openFileDescriptor(uri, mode)) {
				if (filePfd == null) {
					Log.e("EDOPro", "Failed to get file descriptor for " + uriString);
					return -1;
				}
				return filePfd.detachFd();  // Take ownership of the fd.
			}
		} catch (IllegalArgumentException e) {
			// This exception is long and ugly and really just means file not found.
			Log.d("EDOPro", "openFileDescriptor: File not found." + e);
			return -1;
		} catch (Exception e) {
			Log.e("EDOPro", "Unexpected openContentUri exception: " + e);
			return -1;
		}
	}

	@RequiresApi(Build.VERSION_CODES.O_MR1)
	public String[] listFolderUri(String uriString) {
		try {
			ArrayList<String> filenames = new ArrayList<>();
			var uri = Uri.parse(normalizeUri(uriString));
			final String[] columns = new String[]{
					DocumentsContract.Document.COLUMN_DISPLAY_NAME,
					DocumentsContract.Document.COLUMN_MIME_TYPE,
					DocumentsContract.Document.COLUMN_DOCUMENT_ID,
			};
			final var resolver = context.getContentResolver();
			final var documentId = DocumentsContract.getDocumentId(uri);
			final var childrenUri = DocumentsContract.buildChildDocumentsUriUsingTree(uri, documentId);
			var c = resolver.query(childrenUri, columns, null, null, null);
			if (c != null) {
				while (c.moveToNext()) {
					final var mimeType = c.getString(1);
					final var isFile = !DocumentsContract.Document.MIME_TYPE_DIR.equals(mimeType);
					var filename = c.getString(0);
					if (isFile) {
						filenames.add(filename);
					} else {
						filenames.add(filename + "/");
					}
				}
				c.close();
			}
			return filenames.toArray(new String[0]);
		} catch (IllegalArgumentException e) {
			// This exception is long and ugly and really just means file not found.
			Log.d("EDOPro", "openFileDescriptor: File not found." + e);
			return new String[0];
		} catch (Exception e) {
			Log.e("EDOPro", "Unexpected openContentUri exception: " + e);
			return new String[0];
		}
	}

	// NOTE:
	// The destination is the parent directory! This means that contentUriCopyFile
	// cannot rename things as part of the operation.
	@RequiresApi(Build.VERSION_CODES.R)
	public void contentUriCopyFile(File src, String dstParentDirUri) {
		var normalizedOutputUri = normalizeUri(dstParentDirUri + "/" + src.getName());
		if (contentUriElementExists(normalizedOutputUri) != EXISTS_TYPE.NONE)
			return;
		try {
			{
				var pathComponents = getPathAndFilenameFromUri(normalizedOutputUri);
				if (!contentUriCreateFile(pathComponents[0], pathComponents[1])) {
					return;
				}
			}
			var inputStream = new FileInputStream(src);
			var outputStream = new FileOutputStream(context.getContentResolver().openFileDescriptor(Uri.parse(normalizedOutputUri), "w").getFileDescriptor());

			byte[] buf = new byte[1024];
			int len;
			while ((len = inputStream.read(buf)) > 0) {
				outputStream.write(buf, 0, len);
			}
			inputStream.close();
			outputStream.close();
		} catch (Exception e) {
			Log.e("EDOPro", "Unexpected copyDocument exception: " + e);
		}
	}

	@RequiresApi(Build.VERSION_CODES.R)
	public boolean hasAccess() {
		var uri = Uri.parse(normalizeUri(scoped_storage_dir));
		final String[] columns = new String[]{
				DocumentsContract.Document.COLUMN_DOCUMENT_ID,
		};
		try {
			final var resolver = context.getContentResolver();
			final var documentId = DocumentsContract.getDocumentId(uri);
			final var childrenUri = DocumentsContract.buildChildDocumentsUriUsingTree(uri, documentId);
			var c = resolver.query(childrenUri, columns, null, null, null);
			if (c != null) {
				c.close();
			}
			return true;
		} catch (SecurityException e) {
			return false;
		} catch (Exception e) {
			Log.e("EDOPro", "Unkonwn exception: " + e);
			return false;
		}
	}
}
