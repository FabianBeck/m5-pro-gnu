/**
 * xattr.c
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
#include "fsck.h"
#include "node.h"
#include "xattr.h"

/*#define XATTR_CREATE 0x1PCLINT*/
/*#define XATTR_REPLACE 0x2PCLINT*/

void *read_all_xattrs(struct f2fs_sb_info *sbi, struct f2fs_node *inode)
{
	struct f2fs_xattr_header *header;
	void *txattr_addr;
	size_t inline_size = (size_t)inline_xattr_size(&inode->i);

	txattr_addr = calloc(inline_size + BLOCK_SZ, 1);
	ASSERT(txattr_addr);

	if (inline_size)
		memcpy(txattr_addr, inline_xattr_addr(&inode->i), inline_size);

	/* Read from xattr node block. */
	if (inode->i.i_xattr_nid) {
		struct node_info ni;
		int ret;

		get_node_info(sbi, le32_to_cpu(inode->i.i_xattr_nid), &ni);
		ret = dev_read_block((char*)txattr_addr + inline_size, ni.blk_addr);/*lint !e747*/
		ASSERT(ret >= 0);
	}

	header = XATTR_HDR(txattr_addr);

	/* Never been allocated xattrs */
	if (le32_to_cpu(header->h_magic) != F2FS_XATTR_MAGIC) {
		header->h_magic = cpu_to_le32(F2FS_XATTR_MAGIC);
		header->h_refcount = cpu_to_le32(1);
	}
	return txattr_addr;
}

struct f2fs_xattr_entry *find_xattr(void *base_addr, int index,
		size_t len, const char *name)
{
	struct f2fs_xattr_entry *entry;
	/*lint -save -e826*/
	list_for_each_xattr(entry, base_addr) {
		if (entry->e_name_index != index)
			continue;
		if (entry->e_name_len != len)
			continue;
		if (!memcmp(entry->e_name, name, len))
			break;
	}
	/*lint -restore*/
	return entry;
}
