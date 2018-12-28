/**
 * xattr.h
 *
 * Many parts of codes are copied from Linux kernel/fs/f2fs.
 *
 * Copyright (C) 2015 Huawei Ltd.
 * Witten by:
 *   Hou Pengyang <houpengyang@huawei.com>
 *   Liu Shuoran <liushuoran@huawei.com>
 *   Jaegeuk Kim <jaegeuk@kernel.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _XATTR_H_
#define _XATTR_H_

#include <f2fs_fs.h>
#include "f2fs.h"

struct f2fs_xattr_header {
	__le32 h_magic;		/* magic number for identification */
	__le32 h_refcount;	/* reference count */
	__le32 h_ctx_crc;	/* crc of fscrypt context */
	__u32 h_sloadd[3];	/* zero right now */
};

struct f2fs_xattr_entry {
	__u8 e_name_index;
	__u8 e_name_len;
	__le16 e_value_size;	/* size of attribute value */
	char e_name[0];		/* attribute name */
};

#define FS_KEY_DESCRIPTOR_SIZE 8
#define FS_KEY_DERIVATION_NONCE_SIZE 16
#define FS_KEY_DERIVATION_IV_SIZE 16
#define FS_KEY_DERIVATION_CIPHER_SIZE (64 + 16) /* nonce + tag */

struct fscrypt_context {
	u8 format;
	u8 contents_encryption_mode;
	u8 filenames_encryption_mode;
	u8 flags;
	u8 master_key_descriptor[FS_KEY_DESCRIPTOR_SIZE];
	u8 nonce[FS_KEY_DERIVATION_CIPHER_SIZE];
	u8 iv[FS_KEY_DERIVATION_IV_SIZE];
} __attribute__((packed));
#define FSCRYPT_CTX_SIZE sizeof(struct fscrypt_context)

#define XATTR_ROUND	(3)

#define XATTR_SELINUX_SUFFIX "selinux"
#define F2FS_XATTR_INDEX_USER			1
#define F2FS_XATTR_INDEX_POSIX_ACL_ACCESS	2
#define F2FS_XATTR_INDEX_POSIX_ACL_DEFAULT	3
#define F2FS_XATTR_INDEX_TRUSTED		4
#define F2FS_XATTR_INDEX_LUSTRE			5
#define F2FS_XATTR_INDEX_SECURITY		6
#define F2FS_XATTR_INDEX_ADVISE			7
#define F2FS_XATTR_INDEX_ENCRYPTION		9

#define F2FS_XATTR_NAME_ENCRYPTION_CONTEXT		"c"

#define IS_XATTR_LAST_ENTRY(entry) (*(__u32 *)(entry) == 0)

#define XATTR_HDR(ptr)		((struct f2fs_xattr_header *)(ptr))
#define XATTR_ENTRY(ptr) 	((struct f2fs_xattr_entry *)(ptr))
#define F2FS_XATTR_MAGIC	0xF2F52011

#define XATTR_NEXT_ENTRY(entry) ((struct f2fs_xattr_entry *) ((char *)(entry) +\
					ENTRY_SIZE(entry)))
#define XATTR_FIRST_ENTRY(ptr)	(XATTR_ENTRY(XATTR_HDR(ptr) + 1))

#define XATTR_ALIGN(size)	(((size) + XATTR_ROUND) & ~XATTR_ROUND)

#define ENTRY_SIZE(entry) (XATTR_ALIGN(sizeof(struct f2fs_xattr_entry) + \
			entry->e_name_len + le16_to_cpu(entry->e_value_size)))

#define F2FS_XATTR_ENCRYPTION_SIZE (XATTR_ALIGN( \
	sizeof(struct f2fs_xattr_entry) + sizeof(struct fscrypt_context) + \
	strlen(F2FS_XATTR_NAME_ENCRYPTION_CONTEXT)))

#define list_for_each_xattr(entry, addr) \
	for (entry = XATTR_FIRST_ENTRY(addr); \
			!IS_XATTR_LAST_ENTRY(entry); \
			entry = XATTR_NEXT_ENTRY(entry))

#define MIN_OFFSET	XATTR_ALIGN(PAGE_SIZE -			\
		sizeof(struct node_footer) - sizeof(__u32))

#define MAX_VALUE_LEN	(MIN_OFFSET -				\
		sizeof(struct f2fs_xattr_header) -		\
		sizeof(struct f2fs_xattr_entry))

#endif
