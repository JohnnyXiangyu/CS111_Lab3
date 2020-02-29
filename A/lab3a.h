#include "ext2_fs.h"
#include <stdio.h>
#include <stdint.h>
#define SUPERBLOCK_OFFSET 1024 /* superblock always starts at 1024 */

/* utility function, calculate block offset given block number */
unsigned int getBlockOffst(unsigned int block_id);
/* utility function, calculate block size through information provided by superblock */
void getBlockSize();

/* read superblock and output superblock summary */
void readSuperBlock();

/* process group table, output group summary, then call readInodeInfo() and readBlockInfo() */
void readGroupDescriptor();

/* process inode table, output inodes' information, inode summary, file types and indirect blocks' info  */
void readInodeInfo(struct ext2_group_desc cur_group);
/* process block bitmat, output free block information */
void readBlockInfo(struct ext2_group_desc cur_group);
