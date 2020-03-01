#include <stdio.h>
#include <stdint.h>
#include "ext2_fs.h"
#define SUPERBLOCK_OFFSET 1024 /* superblock always starts at 1024 */

struct dir_entry_no_name {
	__u32	inode;			/* Inode number */
	__u16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* name length	*/
	__u8	file_type;		/* file type */
};


/* utility function, calculate block offset given block number */
unsigned int getBlockOffst(unsigned int block_id);

/* utility function, calculate block size through information provided by superblock */
void getBlockSize();

/* read superblock and output superblock summary */
void readSuperBlock();

/* process group table, output group summary, then call readInodeInfo() and readBlockInfo() */
void readGroupDescriptor();

/* process inode table, output inodes' information, inode summary, file types and indirect blocks' info  */
void readInodeInfo(unsigned int group_num, struct ext2_group_desc cur_group);

/* process block bitmat, output free block information */
void readBlockInfo(unsigned int group_num, struct ext2_group_desc cur_group);

/* process each dir_entry, output dir's parent inode number, logical byte offset, inode num of referenced file, entry length, name length and name
 * param: parent_inode inode number of parent inode
 *        block_id block id of current directory entry
 */
void readDirectories(unsigned int parent_inode, unsigned int block_id);
