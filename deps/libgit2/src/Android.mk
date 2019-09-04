LOCAL_PATH:= $(call my-dir)

local_c_includes := \
	$(LOCAL_PATH)/ \
	$(LOCAL_PATH)/hash \
	$(LOCAL_PATH)/streams \
	$(LOCAL_PATH)/transports \
	$(LOCAL_PATH)/unix \
	$(LOCAL_PATH)/xdiff \
	$(LOCAL_PATH)/../../openssl/include \
	$(LOCAL_PATH)/../deps/http-parser \
	$(LOCAL_PATH)/../deps/zlib \
	$(LOCAL_PATH)/../include

local_src_files:= \
	alloc.c \
	annotated_commit.c \
	apply.c \
	attr.c \
	attrcache.c \
	attr_file.c \
	blame.c \
	blame_git.c \
	blob.c \
	branch.c \
	buffer.c \
	buf_text.c \
	cache.c \
	checkout.c \
	cherrypick.c \
	clone.c \
	commit.c \
	commit_list.c \
	config.c \
	config_cache.c \
	config_entries.c \
	config_file.c \
	config_mem.c \
	config_parse.c \
	crlf.c \
	date.c \
	delta.c \
	describe.c \
	diff.c \
	diff_driver.c \
	diff_file.c \
	diff_generate.c \
	diff_parse.c \
	diff_print.c \
	diff_stats.c \
	diff_tform.c \
	diff_xdiff.c \
	errors.c \
	fetch.c \
	fetchhead.c \
	filebuf.c \
	fileops.c \
	filter.c \
	fnmatch.c \
	global.c \
	graph.c \
	hash.c \
	hashsig.c \
	ident.c \
	idxmap.c \
	ignore.c \
	index.c \
	indexer.c \
	iterator.c \
	mailmap.c \
	merge.c \
	merge_driver.c \
	merge_file.c \
	message.c \
	mwindow.c \
	netops.c \
	notes.c \
	object_api.c \
	object.c \
	odb.c \
	odb_loose.c \
	odb_mempack.c \
	odb_pack.c \
	offmap.c \
	oidarray.c \
	oid.c \
	oidmap.c \
	pack.c \
	pack-objects.c \
	parse.c \
	patch.c \
	patch_generate.c \
	patch_parse.c \
	path.c \
	pathspec.c \
	pool.c \
	posix.c \
	pqueue.c \
	proxy.c \
	push.c \
	reader.c \
	rebase.c \
	refdb.c \
	refdb_fs.c \
	reflog.c \
	refs.c \
	refspec.c \
	remote.c \
	repository.c \
	reset.c \
	revert.c \
	revparse.c \
	revwalk.c \
	settings.c \
	sha1_lookup.c \
	signature.c \
	sortedcache.c \
	stash.c \
	status.c \
	stdalloc.c \
	strmap.c \
	submodule.c \
	sysdir.c \
	tag.c \
	thread-utils.c \
	trace.c \
	trailer.c \
	transaction.c \
	transport.c \
	tree.c \
	tree-cache.c \
	tsort.c \
	util.c \
	varint.c \
	vector.c \
	worktree.c \
	zstream.c \
	streams/mbedtls.c \
	streams/openssl.c \
	streams/registry.c \
	streams/socket.c \
	streams/stransport.c \
	streams/tls.c \
	transports/auth.c \
	transports/auth_negotiate.c \
	transports/cred.c \
	transports/cred_helpers.c \
	transports/git.c \
	transports/http.c \
	transports/local.c \
	transports/smart.c \
	transports/smart_pkt.c \
	transports/smart_protocol.c \
	transports/ssh.c \
	transports/winhttp.c \
	unix/map.c \
	unix/realpath.c \
	xdiff/xdiffi.c \
	xdiff/xemit.c \
	xdiff/xhistogram.c \
	xdiff/xmerge.c \
	xdiff/xpatience.c \
	xdiff/xprepare.c \
	xdiff/xutils.c \
	../deps/http-parser/http_parser.c \
	../deps/zlib/adler32.c \
	../deps/zlib/crc32.c \
	../deps/zlib/deflate.c \
	../deps/zlib/infback.c \
	../deps/zlib/inffast.c \
	../deps/zlib/inflate.c \
	../deps/zlib/inftrees.c \
	../deps/zlib/trees.c \
	../deps/zlib/zutil.c


include $(CLEAR_VARS)
LOCAL_MODULE := git2
LOCAL_CFLAGS    := -frtti -DGIT_OPENSSL=1 -DGIT_HTTPS=1 -DGIT_SHA1_OPENSSL=1 -DGIT_SSH=1 -DGIT_THREADS=1 -D_FILE_OFFSET_BITS=64
ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_CFLAGS +=-DGIT_ARCH_32=1
endif
ifeq ($(TARGET_ARCH_ABI), x86)
LOCAL_CFLAGS +=-DGIT_ARCH_32=1
endif
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
LOCAL_CFLAGS +=-DGIT_ARCH_64=1
endif
ifeq ($(TARGET_ARCH_ABI), x86_64)
LOCAL_CFLAGS +=-DGIT_ARCH_64=1
endif
LOCAL_STATIC_LIBRARIES :=  ssh2
LOCAL_SRC_FILES += $(local_src_files)
LOCAL_C_INCLUDES += $(local_c_includes)
include $(BUILD_STATIC_LIBRARY)
