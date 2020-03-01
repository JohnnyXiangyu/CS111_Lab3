#include "lab3a.h"

#include <unistd.h>
#include <math.h>

extern struct ext2_super_block super_block;
extern unsigned int block_size;
extern int img_fd;

int group_count; /* total number of groups */


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
    group_count = ceil(super_block.s_blocks_count / super_block.s_blocks_per_group);
    
    int group_tab_offset = SUPERBLOCK_OFFSET + block_size; /* first group descriptor offset (from super block) */

    struct ext2_group_desc group_desc; /* container of iterated group descriptor */

    /* iterate through all group descriptor table entries */
    int group_desc_id = 0; /* index into the table */
    for (group_desc_id = 0; group_desc_id < group_count; group_desc_id ++) {
        /* read group table entry */
        pread(img_fd, &group_desc, 32, group_tab_offset + 32 * group_desc_id);

        /* # of blocks in this group
         * case1 full
         * case2 mod
         * local to each iteration
         */
        int blocks_in_group = super_block.s_blocks_per_group;
        if (group_desc_id == group_count - 1) {
            blocks_in_group = super_block.s_blocks_count - (group_count-1) * super_block.s_blocks_per_group;
        }

        /* # of inodes in this group
         * case1 case 2
         * local to iteration
         */
        int inodes_in_group = super_block.s_inodes_per_group;
        if (group_desc_id == group_count - 1) {
            blocks_in_group = super_block.s_inodes_count - (group_count-1) * super_block.s_inodes_per_group;
        }

        /* print group table entry info */
        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
            group_desc_id, // group number (decimal, starting from zero)
            blocks_in_group, // total number of blocks in this group (decimal)
            inodes_in_group, // total number of i-nodes in this group (decimal)
            group_desc.bg_free_blocks_count, // number of free blocks (decimal)
            group_desc.bg_free_inodes_count, // number of free i-nodes (decimal)
            group_desc.bg_block_bitmap, // block number of free block bitmap for this group (decimal)
            group_desc.bg_inode_bitmap, // block number of free i-node bitmap for this group (decimal)
            group_desc.bg_inode_table // block number of first block of i-nodes in this group (decimal)
        );

        /* process inodes */
        readInodeInfo(group_desc_id, group_desc);
        readBlockInfo(group_desc_id, group_desc);
    }
}


void readBlockInfo(unsigned int group_id, struct ext2_group_desc cur_group) {
    /* number of blocks in the given group */
    int total_blocks = super_block.s_blocks_per_group;
    if (group_id == group_count - 1) {
        total_blocks = super_block.s_blocks_count - (group_count-1) * super_block.s_blocks_per_group;
    }

    /* block id of first block of bitmap */
    int bitmap_block_id = cur_group.bg_block_bitmap;
    /* byte offset of first bitmap block*/
    int bitmap_offset = getBlockOffst(bitmap_block_id);
    /* variable of bitmap, always size 1 block */
    char* bitmap = (char*) malloc(block_size * 1);
    /* read free block bitmap */
    pread(img_fd, bitmap, block_size, bitmap_offset);

    /* block id each bit corresponds to */
    int cur_block_id = super_block.s_first_data_block + super_block.s_blocks_per_group*group_id;
    
    int byte_id, bit_id; /* indexes to bit map (each byte, each bit) */
    /* loop each byte in the block */
    for (byte_id = 0; byte_id < block_size; byte_id++) {
        char cur_byte = bitmap[byte_id]; /* current byte in bitmap */
        /* loop all 8 bits in the byte */
        for (bit_id = 0; bit_id < 8; bit_id ++) {
            char cur_bit = (cur_byte>>bit_id) & 1; /* current bit in bitmap */
            if (cur_bit) {
                printf("BFREE,%d", cur_block_id);
            }
            cur_block_id ++;
        }
    }

    free(bitmap);
}


void readDirectories(unsigned int parent_inode, unsigned int block_id) {
    int my_block_offset = getBlockOffst(block_id);
    
    /* read directory entry without name section */
    struct dir_entry_no_name nameless_entry; /* nameless entry structure (for finding name length) */
    pread(img_fd, &nameless_entry, 8, my_block_offset);

    int my_full_length = 8 + nameless_entry.name_len; /* full length of current directory entry */

    /* read full directory entry */
    struct ext2_dir_entry my_dir_entry; /* current directory entry */
    pread(img_fd, &my_dir_entry, 8, my_block_offset);

    /* print information */
    printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",
           parent_inode, // parent inode number
           block_id, // logical byte offset of this entry within the directory
           my_dir_entry.inode, // inode number of the referenced file
           my_full_length, // entry length
           my_dir_entry.name_len, // name length
           my_dir_entry.name // name (string, surrounded by single-quotes)
    );
}
