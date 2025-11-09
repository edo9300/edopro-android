// Contains GPLv3-licensed code from the Termux project.
// https://github.com/termux/termux-app/blob/master/app/src/main/java/com/termux/filepicker/TermuxDocumentsProvider.java

/*
 * dirContainsFile, findDocumentPath, findDocumentPath functions taken from zomdroid
 * https://github.com/udarmolota/zomdroid/blob/9c8f64f8e2d60089bee6fc1aae3f3151f5c301d7/app/src/main/java/com/zomdroid/AppStorageProvider.java#L381-L422
 * Supposedly licensed under MIT, but the source file is itself derived from the same termux sources, hence that code is GPLv3 as well
 * */
package io.github.edo9300.edopro;

import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.graphics.Point;
import android.os.Build;
import android.os.CancellationSignal;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.provider.DocumentsContract.Document;
import android.provider.DocumentsContract.Root;
import android.provider.DocumentsProvider;
import android.webkit.MimeTypeMap;

import androidx.annotation.RequiresApi;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Objects;

@RequiresApi(api = Build.VERSION_CODES.N)
public class EdoproDocumentProvider extends DocumentsProvider {

	private static final String ALL_MIME_TYPES = "*/*";

	// The default columns to return information about a root if no specific
	// columns are requested in a query.
	private static final String[] DEFAULT_ROOT_PROJECTION = new String[]{
			Root.COLUMN_ROOT_ID,
			Root.COLUMN_MIME_TYPES,
			Root.COLUMN_FLAGS,
			Root.COLUMN_ICON,
			Root.COLUMN_TITLE,
			Root.COLUMN_SUMMARY,
			Root.COLUMN_DOCUMENT_ID,
			Root.COLUMN_AVAILABLE_BYTES
	};

	// The default columns to return information about a document if no specific
	// columns are requested in a query.
	private static final String[] DEFAULT_DOCUMENT_PROJECTION = new String[]{
			Document.COLUMN_DOCUMENT_ID,
			Document.COLUMN_MIME_TYPE,
			Document.COLUMN_DISPLAY_NAME,
			Document.COLUMN_LAST_MODIFIED,
			Document.COLUMN_FLAGS,
			Document.COLUMN_SIZE
	};

	private File getBaseDir() {
		try {
			File file = new File(getContext().getFilesDir(), "working_dir");
			String working_directory;
			if (!file.exists())
				return null;
			BufferedReader br = new BufferedReader(new FileReader(file));
			working_directory = br.readLine();
			br.close();
			if (working_directory == null)
				return null;
			return new File(working_directory);
		} catch (Exception e) {
			return null;
		}
	}

	@Override
	public Cursor queryRoots(String[] projection) throws FileNotFoundException {
		var context = getContext();
		final File BASE_DIR = getBaseDir();
		if (BASE_DIR == null)
			return null;
		final MatrixCursor result = new MatrixCursor(projection != null ? projection : DEFAULT_ROOT_PROJECTION);
		final String applicationName = context.getApplicationInfo().loadLabel(context.getPackageManager()).toString();

		final MatrixCursor.RowBuilder row = result.newRow();
		row.add(Root.COLUMN_ROOT_ID, getDocIdForFile(BASE_DIR));
		row.add(Root.COLUMN_DOCUMENT_ID, getDocIdForFile(BASE_DIR));
		row.add(Root.COLUMN_SUMMARY, null);
		row.add(Root.COLUMN_FLAGS, Root.FLAG_SUPPORTS_CREATE | Root.FLAG_SUPPORTS_SEARCH | Root.FLAG_SUPPORTS_IS_CHILD);
		row.add(Root.COLUMN_TITLE, applicationName);
		row.add(Root.COLUMN_MIME_TYPES, ALL_MIME_TYPES);
		row.add(Root.COLUMN_AVAILABLE_BYTES, BASE_DIR.getFreeSpace());
		row.add(Root.COLUMN_ICON, R.mipmap.ic_launcher);
		return result;
	}

	@Override
	public Cursor queryDocument(String documentId, String[] projection) throws FileNotFoundException {
		final MatrixCursor result = new MatrixCursor(projection != null ? projection : DEFAULT_DOCUMENT_PROJECTION);
		includeFile(result, documentId, null);
		return result;
	}


	public static boolean dirContainsFile(File dir, File file) {
		if (dir == null || file == null) return false;
		String dirPath = dir.getAbsolutePath();
		String filePath = file.getAbsolutePath();
		if (dirPath.equals(filePath)) {
			return true;
		}
		if (!dirPath.endsWith("/")) {
			dirPath += "/";
		}
		return filePath.startsWith(dirPath);
	}

	protected final List<String> findDocumentPath(File parent, File doc)
			throws FileNotFoundException {
		if (!doc.exists()) {
			throw new FileNotFoundException(doc + " is not found.");
		}
		if (!dirContainsFile(parent, doc)) {
			throw new FileNotFoundException(doc + " is not found under " + parent);
		}
		List<String> path = new ArrayList<>();
		while (doc != null && dirContainsFile(parent, doc)) {
			path.add(0, getDocIdForFile(doc));

			doc = doc.getParentFile();
		}
		return path;
	}

	@Override
	@RequiresApi(api = Build.VERSION_CODES.O)
	public DocumentsContract.Path findDocumentPath(String parentDocumentId, String childDocumentId) throws FileNotFoundException {
		final var baseDir = getBaseDir();
		if (baseDir == null)
			throw new FileNotFoundException();
		final String rootId = (parentDocumentId == null) ? getDocIdForFile(baseDir) : null;
		if (parentDocumentId == null) {
			parentDocumentId = getDocIdForFile(baseDir);
		}
		final File parent = getFileForDocId(parentDocumentId);
		final File doc = getFileForDocId(childDocumentId);
		return new DocumentsContract.Path(rootId, findDocumentPath(parent, doc));
	}

	@Override
	public Cursor queryChildDocuments(String parentDocumentId, String[] projection, String sortOrder) throws FileNotFoundException {
		final MatrixCursor result = new MatrixCursor(projection != null ? projection : DEFAULT_DOCUMENT_PROJECTION);
		final File parent = getFileForDocId(parentDocumentId);
		for (File file : parent.listFiles()) {
			includeFile(result, null, file);
		}
		return result;
	}

	@Override
	public ParcelFileDescriptor openDocument(final String documentId, String mode, CancellationSignal signal) throws FileNotFoundException {
		final File file = getFileForDocId(documentId);
		final int accessMode = ParcelFileDescriptor.parseMode(mode);
		return ParcelFileDescriptor.open(file, accessMode);
	}

	@Override
	public AssetFileDescriptor openDocumentThumbnail(String documentId, Point sizeHint, CancellationSignal signal) throws FileNotFoundException {
		final File file = getFileForDocId(documentId);
		final ParcelFileDescriptor pfd = ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_ONLY);
		return new AssetFileDescriptor(pfd, 0, file.length());
	}

	@Override
	public boolean onCreate() {
		return true;
	}

	@Override
	public String createDocument(String parentDocumentId, String mimeType, String displayName) throws FileNotFoundException {
		File newFile = new File(parentDocumentId, displayName);
		int noConflictId = 2;
		while (newFile.exists()) {
			newFile = new File(parentDocumentId, displayName + " (" + noConflictId++ + ")");
		}
		try {
			boolean succeeded;
			if (Document.MIME_TYPE_DIR.equals(mimeType)) {
				succeeded = newFile.mkdir();
			} else {
				succeeded = newFile.createNewFile();
			}
			if (!succeeded) {
				throw new FileNotFoundException("Failed to create document with id " + newFile.getPath());
			}
		} catch (IOException e) {
			throw new FileNotFoundException("Failed to create document with id " + newFile.getPath());
		}
		return newFile.getPath();
	}

	private static void deleteRecursive(File fileOrDirectory) {
		if (fileOrDirectory.isDirectory())
			for (File child : Objects.requireNonNull(fileOrDirectory.listFiles()))
				deleteRecursive(child);

		fileOrDirectory.delete();
	}

	@Override
	public void deleteDocument(String documentId) throws FileNotFoundException {
		File file = getFileForDocId(documentId);
		if (file.isDirectory()) {
			deleteRecursive(file);
		} else if (!file.delete()) {
			throw new FileNotFoundException("Failed to delete document with id " + documentId);
		}
	}

	@Override
	public String renameDocument(String documentId, String displayName) throws FileNotFoundException {
		File file = getFileForDocId(documentId);
		File newFile = new File(file.getParentFile(), displayName);
		if (newFile.exists() || !file.renameTo(newFile)) {
			getDocIdForFile(file);
		}
		return getDocIdForFile(newFile);
	}

	@Override
	public String getDocumentType(String documentId) throws FileNotFoundException {
		File file = getFileForDocId(documentId);
		return getMimeType(file);
	}

	@Override
	public Cursor querySearchDocuments(String rootId, String query, String[] projection) throws FileNotFoundException {
		final MatrixCursor result = new MatrixCursor(projection != null ? projection : DEFAULT_DOCUMENT_PROJECTION);
		final File parent = getFileForDocId(rootId);

		// This example implementation searches file names for the query and doesn't rank search
		// results, so we can stop as soon as we find a sufficient number of matches.  Other
		// implementations might rank results and use other data about files, rather than the file
		// name, to produce a match.
		final LinkedList<File> pending = new LinkedList<>();
		pending.add(parent);

		final int MAX_SEARCH_RESULTS = 50;
		while (!pending.isEmpty() && result.getCount() < MAX_SEARCH_RESULTS) {
			final File file = pending.removeFirst();
			// Avoid folders outside the $HOME folders linked in to symlinks (to avoid e.g. search
			// through the whole SD card).
			boolean isInsideHome;
			try {
				isInsideHome = file.getCanonicalPath().startsWith(getContext().getFilesDir().getParent());
			} catch (IOException e) {
				isInsideHome = true;
			}
			if (isInsideHome) {
				if (file.isDirectory()) {
					Collections.addAll(pending, file.listFiles());
				} else {
					if (file.getName().toLowerCase().contains(query)) {
						includeFile(result, null, file);
					}
				}
			}
		}

		return result;
	}

	@Override
	public boolean isChildDocument(String parentDocumentId, String documentId) {
		return documentId.startsWith(parentDocumentId);
	}

	/**
	 * Get the document id given a file. This document id must be consistent across time as other
	 * applications may save the ID and use it to reference documents later.
	 * <p/>
	 * The reverse of @{link #getFileForDocId}.
	 */
	private static String getDocIdForFile(File file) {
		return file.getAbsolutePath();
	}

	/**
	 * Get the file given a document id (the reverse of {@link #getDocIdForFile(File)}).
	 */
	private static File getFileForDocId(String docId) throws FileNotFoundException {
		final File f = new File(docId);
		if (!f.exists()) throw new FileNotFoundException(f.getAbsolutePath() + " not found");
		return f;
	}

	private static String getMimeType(File file) {
		if (file.isDirectory()) {
			return Document.MIME_TYPE_DIR;
		} else {
			final String name = file.getName();
			final int lastDot = name.lastIndexOf('.');
			if (lastDot >= 0) {
				final String extension = name.substring(lastDot + 1).toLowerCase();
				final String mime = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
				if (mime != null) return mime;
				if ("lua".equals(extension) || "md".equals(extension) || "log".equals(extension))
					return "text/plain";
			}
			return "application/octet-stream";
		}
	}

	/**
	 * Add a representation of a file to a cursor.
	 *
	 * @param result the cursor to modify
	 * @param docId  the document ID representing the desired file (may be null if given file)
	 * @param file   the File object representing the desired file (may be null if given docID)
	 */
	private void includeFile(MatrixCursor result, String docId, File file)
			throws FileNotFoundException {
		if (docId == null) {
			docId = getDocIdForFile(file);
		} else {
			file = getFileForDocId(docId);
		}

		int flags = 0;
		if (file.isDirectory()) {
			if (file.canWrite()) flags |= Document.FLAG_DIR_SUPPORTS_CREATE;
		} else if (file.canWrite()) {
			flags |= Document.FLAG_SUPPORTS_WRITE;
		}
		if (file.getParentFile().canWrite())
			flags |= Document.FLAG_SUPPORTS_DELETE | Document.FLAG_SUPPORTS_RENAME;

		final String displayName = file.getName();
		final String mimeType = getMimeType(file);
		if (mimeType.startsWith("image/")) flags |= Document.FLAG_SUPPORTS_THUMBNAIL;

		final MatrixCursor.RowBuilder row = result.newRow();
		row.add(Document.COLUMN_DOCUMENT_ID, docId);
		row.add(Document.COLUMN_DISPLAY_NAME, displayName);
		row.add(Document.COLUMN_SIZE, file.length());
		row.add(Document.COLUMN_MIME_TYPE, mimeType);
		row.add(Document.COLUMN_LAST_MODIFIED, file.lastModified());
		row.add(Document.COLUMN_FLAGS, flags);
		row.add(Document.COLUMN_ICON, R.mipmap.ic_launcher);
	}
}
