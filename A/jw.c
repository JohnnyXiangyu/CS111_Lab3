#include "lab3a.h"
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

extern struct ext2_super_block super_block;
extern unsigned int block_size;
extern int img_fd;

extern int dir_logic_offset;

unsigned int group_count; /* total number of groups */


unsigned int getBlockOffst(unsigned int block_id) {
    unsigned int my_offset = block_id * block_size;
    return my_offset;
}


void getBlockSize() {
    block_size= 1024 << super_block.s_log_block_size;
}


void readSuperBlock() {
    /* read super block into global structure */
    m_pread(img_fd, &super_block, 1024, SUPERBLOCK_OFFSET);
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
    group_count = ceil((float) super_block.s_blocks_count / (float) super_block.s_blocks_per_group);
    
    int group_tab_offset = SUPERBLOCK_OFFSET + block_size; /* first group descriptor offset (from super block) */

    struct ext2_group_desc group_desc; /* container of iterated group descriptor */

    /* iterate through all group descriptor table entries */
    unsigned int group_id = 0; /* index into the table */
    for (group_id = 0; group_id < group_count; group_id ++) {
        /* read group table entry */
        m_pread(img_fd, &group_desc, 32, group_tab_offset + 32 * group_id);

        /* # of blocks in this group
         * full/mod
         * local to each iteration
         */
        int blocks_in_group = super_block.s_blocks_per_group;
        if (group_id == group_count - 1) {
            blocks_in_group = super_block.s_blocks_count - (group_count-1) * super_block.s_blocks_per_group;
        }

        /* # of inodes in this group
         * full/mod
         * local to iteration
         */
        int inodes_in_group = super_block.s_inodes_per_group;
        if (group_id == group_count - 1) {
            inodes_in_group = super_block.s_inodes_count - (group_count-1) * super_block.s_inodes_per_group;
        }

        /* print group table entry info */
        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
            group_id, // group number (decimal, starting from zero)
            blocks_in_group, // total number of blocks in this group (decimal)
            inodes_in_group, // total number of i-nodes in this group (decimal)
            group_desc.bg_free_blocks_count, // number of free blocks (decimal)
            group_desc.bg_free_inodes_count, // number of free i-nodes (decimal)
            group_desc.bg_block_bitmap, // block number of free block bitmap for this group (decimal)
            group_desc.bg_inode_bitmap, // block number of free i-node bitmap for this group (decimal)
            group_desc.bg_inode_table // block number of first block of i-nodes in this group (decimal)
        );

        /* process inodes */
        readInodeInfo(group_id, group_desc);
        readBlockInfo(group_id, group_desc);
    }
}


void readBlockInfo(unsigned int group_id, struct ext2_group_desc cur_group) {
    /* number of blocks in the given group */
    unsigned int my_block_count = super_block.s_blocks_per_group;
    if (group_id == (unsigned int) group_count - 1) {
        my_block_count = super_block.s_blocks_count - (group_count-1) * super_block.s_blocks_per_group;
    }

    unsigned int bitmap_block_id = cur_group.bg_block_bitmap; /* block id of first block of bitmap */
    unsigned int bitmap_offset = getBlockOffst(bitmap_block_id); /* byte offset of first bitmap block*/
    char* bitmap = (char*) malloc(my_block_count * 1); /* variable of bitmap, always size 1 block */
    
    /* read free block bitmap */
    m_pread(img_fd, bitmap, my_block_count, bitmap_offset);

    /* first block id in this group */
    unsigned int first_block_id = super_block.s_first_data_block + super_block.s_blocks_per_group*group_id;
    
    /* block id each bit corresponds to */
    unsigned int cur_block_id = first_block_id;

    /* loop each byte and bit in the block */
    unsigned int byte_id, bit_id;
    for (byte_id = 0; byte_id < my_block_count; byte_id++) {
        char cur_byte = bitmap[byte_id];
        
        /* loop all 8 bits in the byte */
        for (bit_id = 0; bit_id < 8; bit_id ++) {
            char cur_bit = (cur_byte>>bit_id) & 1; /* current bit in bitmap */
            if (!cur_bit) { /* if allocated */
                printf("BFREE,%d\n", cur_block_id);
            }
            cur_block_id ++;
        }
    }

    free(bitmap);
}


void readDirectories(unsigned int parent_inode, unsigned int block_id) {
    int my_actual_offset = getBlockOffst(block_id); /* first byte of this block */

    /* loop linked list of directory entries */
    unsigned int cur_offset = 0; /* temporary offset to entry (in this block) */
    for (cur_offset = 0; cur_offset < block_size;) {
        /* read directory entry without name section */
        struct ext2_dir_entry cur_entry; /* entry in this iteration */
        m_pread(img_fd, &cur_entry, sizeof(struct ext2_dir_entry), my_actual_offset + cur_offset);

        if (cur_entry.inode != 0) {
            unsigned int my_full_length = cur_entry.rec_len; 
            /* print information */
            printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",
                parent_inode, // parent inode number
                dir_logic_offset, // logical byte offset of this entry within the directory
                cur_entry.inode, // inode number of the referenced file
                my_full_length, // entry length
                cur_entry.name_len, // name length
                cur_entry.name // name (string, surrounded by single-quotes)
            );
        }

        cur_offset += cur_entry.rec_len;
        dir_logic_offset += cur_entry.rec_len;
    }
}
