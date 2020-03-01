#include "lab3a.h"

#include <unistd.h>

extern struct ext2_super_block super_block;
extern unsigned int block_size;
extern int img_fd;

unsigned int getBlockOffst(unsigned int block_id) {
    return SUPERBLOCK_OFFSET + (block_id - 1) * block_size;
}

void getBlockSize() {
    block_size= 1024 << super_block.s_log_block_size;
}

void readSuperBlock() {
    /* read super block into global structure */
    pread(img_fd, &super_block, block_size, SUPERBLOCK_OFFSET);
    getBlockSize();

    /* print super block summary */
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
        super_block.s_blocks_count,
        super_block.s_inodes_count,
        block_size,
        super_block.s_inode_size,
        super_block.s_blocks_per_group,
        super_block.s_inodes_per_group,
        super_block.s_first_ino
    );    
}

void readGroupDescriptor() {
    int group_desc_id = 0; /* index into the table */
    /* get first group descriptor offset from super block */
    
}

void readBlockInfo(unsigned int group_num, struct ext2_group_desc cur_group) {

}
