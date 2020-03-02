#include "lab3a.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFDIR 0x4000

extern struct ext2_super_block super_block;
extern unsigned int block_size;
extern int img_fd;

void readIndirectInfo(unsigned int indirect_table_id, unsigned int level, unsigned int inode_number, unsigned int is_dir){
    unsigned int offset = getBlockOffst(indirect_table_id);
    unsigned int table_size = block_size / sizeof(__u32);
    unsigned int *ids = malloc(sizeof(__u32) * table_size);
    pread(img_fd, ids, block_size, offset);

    unsigned int i;
    for(i = 0; i < table_size; i++){
        if( ids[i] != 0 ){
            if(is_dir && level == 1){
                readDirectories(inode_number, ids[i]);
            }
            unsigned int logical_offset = level * i;
            printf("INDIRECT,%d,%d,%d,%d,%d\n",inode_number,level,logical_offset,indirect_table_id,ids[i]);

        }
    }
    if(level != 1){
        for(i = 0; i < table_size; i++){         
                readIndirectInfo(ids[i], level-1, inode_number, is_dir);
        }
    }


}


void inode_summary(unsigned int inode_number, unsigned int inode_table, unsigned int table_index)
{
    printf("INODE,%d,", inode_number);

    unsigned int offset = getBlockOffst(inode_table) + (table_index - 1) * super_block.s_inode_size;

    struct ext2_inode inode;

    pread(img_fd, &inode, super_block.s_inode_size, offset);

    int is_link = inode.i_mode & EXT2_S_IFLNK;
    int is_reg = inode.i_mode & EXT2_S_IFREG;
    int is_dir = inode.i_mode & EXT2_S_IFDIR;

    if (is_link)
    {
        printf("s,");
    }
    else if (is_reg)
    {
        printf("f,");
    }
    else if (is_dir)
    {
        printf("d,");
    }
    else
    {
        printf("?,");
    }

    unsigned int mode = 0xFFFF & inode.i_mode;
    unsigned int owner = inode.i_uid;
    unsigned int group = inode.i_gid;
    unsigned int link_count = inode.i_links_count;

    time_t inode_ctime = inode.i_ctime;
    struct tm *gmt_ctime = gmtime(&inode_ctime);
    char ctime[20];
    strftime(ctime, 20, "%m/%d/%Y %H:%M:%S", gmt_ctime);

    time_t inode_mtime = inode.i_mtime;
    struct tm *gmt_mtime = gmtime(&inode_mtime);
    char mtime[20];
    strftime(mtime, 20, "%m/%d/%Y %H:%M:%S", gmt_mtime);

    time_t inode_atime = inode.i_atime;
    struct tm *gmt_atime = gmtime(&inode_atime);
    char atime[20];
    strftime(atime, 20, "%m/%d/%Y %H:%M:%S", gmt_atime);

    unsigned int file_size = inode.i_size;
    unsigned int block_num = inode.i_blocks;

    printf("%o,%d,%d,%d,%s,%s,%s,%d,%d", mode, owner, group, link_count, ctime, mtime, atime, file_size, block_num);
    int k;
    if (!is_link || (file_size < 60))
    {
        
        for (k = 0; k < 15; k++)
        {
            printf(",%d", inode.i_block[k]);
        }
    }
    else
    {
        printf(",");
    }
    printf("\n");

    if (is_dir)
    {
        for (k = 0; k < 12; k++)
        {
            if (inode.i_block[k] != 0)
            {
                readDirectories(inode_number, inode.i_block[k]);
            }
        }
        return;
    }
    
    if( !is_link ){
        unsigned int first_indir = inode.i_block[12];
        if(first_indir != 0){
            readIndirectInfo(first_indir, 1, inode_number, is_dir);
        }
        unsigned int second_indir = inode.i_block[13];
        if(second_indir != 0){
            readIndirectInfo(second_indir, 2, inode_number, is_dir);
        }
        unsigned int third_indir = inode.i_block[14];
        if(third_indir != 0){
            readIndirectInfo(third_indir, 3, inode_number, is_dir);
        }        
    }
}

void readInodeInfo(unsigned int group_num, struct ext2_group_desc cur_group)
{
    //need an extra group_num here

    unsigned int bitmap_id = cur_group.bg_inode_bitmap;
    unsigned int table_id = cur_group.bg_inode_table;
    unsigned int offset = getBlockOffst(bitmap_id);

    int map_size = super_block.s_inodes_per_group / 8;
    char *map = malloc(map_size * sizeof(char));

    pread(img_fd, map, map_size, offset);

    int mask = 1;
    int i;
    for (i = 0; i < map_size; i++)
    {
        char cur = map[i];
        int j;
        for (j = 0; j < 8; j++)
        {
            unsigned int is_used = cur & mask;
            unsigned int inode_index = group_num * super_block.s_inodes_per_group + i * 8 + j + 1;

            if (is_used)
            {
                inode_summary(inode_index, table_id, i * 8 + j + 1);
            }
            else
            {
                printf("IFREE,%d\n", inode_index);
            }
            mask <<= 1;
        }
    }
}
