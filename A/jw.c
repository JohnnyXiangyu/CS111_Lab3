#include "lab3a.h"

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

}

void readGroupDescriptor() {
    
}

void readBlockInfo(unsigned int group_num, struct ext2_group_desc cur_group) {

}
