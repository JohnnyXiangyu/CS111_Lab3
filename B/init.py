import utils
import csv
from optparse import OptionParser
import sys
import dl 
import math

filename = ""
report = []

# stringify indirection level
indirection_str = ["INDIRECT", "DOUBLE INDIRECT", "TRIPLE INDIRECT"]

# gobal data structures
inode_alloc = {}
inode_freelist = []
blocks = {}
block_freelist = []
block_invalid = []
directory = []

super_block = None


if __name__ == '__main__':
    # parse argument
    version_msg = "%prog 114.514"
    usage_msg = """%prog FILE

Output inconsistencies from a given report file."""

    parser = OptionParser(version=version_msg,
                          usage=usage_msg)
    (opts, args) = parser.parse_args(sys.argv[1:])

    if len(args) != 1:
        sys.stderr.write("Error multiple file names given!\n")
        exit(1)
    
    # open file
    filename = args[0]
    try:
        with open(filename, newline='') as f:
            report_obj = csv.reader(f, delimiter=',')
            for i in report_obj:
                report.append(i)
    except:
        sys.stderr.write("Error can't open given file!\n")
        exit(1)

    # loop for superblock entry
    for entry in report:
        entry_type = entry[0]
        if entry_type == 'SUPERBLOCK':
            super_block = utils.SuperBlock(entry)
            break
    
    if super_block == None:
        exit(2)
    
    utils.MAX_BLOCK_NUM = super_block.block_count
    utils.MAX_INODE_NUM = super_block.inode_count
    
    # reserve some blocks and inodes
    for i in range(0, super_block.block_count):
        new_id = str(i)
        blocks.update({new_id: utils.Block()})
    
    # construct raw structures
    for entry in report:
        entry_type = entry[0]
        if entry_type == "BFREE":
            blocks[entry[1]].allocated = False
            block_freelist.append(int(entry[1]))
        elif entry_type == "IFREE":
            inode_freelist.append(int(entry[1]))
        elif entry_type == "INODE":
            inode_alloc.update({entry[1]: utils.Inode(entry)})
        elif entry_type == "DIRENT":
            directory.append(utils.Directory(entry))
        elif entry_type == "INDIRECT":
            offset = int(entry[3])
            bid = entry[5]
            inode = int(entry[1])
            indirection = indirection_str[int(entry[2]) - 1]
            try:
                temp_block = blocks[bid]
                temp_block.inode_refs.append({"inode": inode, "offset": offset, "indirection": indirection})
            except KeyError:
                block_invalid.append({"id": int(bid), "inode": inode, "offset": offset, "indirection": indirection})
        elif entry_type == "GROUP":
            inode_table_id = int(entry[8])
            inode_num_in_group = int(entry[3])
            util.RESERVED_BLOCK_ID = inode_table_id + math.ceil(inode_num_in_group * super_block.inode_size / super_block.block_size)


    # update metadata in blocks
    for inode in inode_alloc.values():
        for i in range(0, 12):
            block_id = inode.blocks[i]
            try:
                temp_block = blocks[str(block_id)]
                temp_block.inode_refs.append({"inode": inode.id, "offset": i, "indirection": "DIRECT"})
            except KeyError:
                block_invalid.append({"id": block_id, "inode": inode.id, "offset": i, "indirection": "DIRECT"})
            
    
    # update reference count in inode
    for dire in directory:
        try:
            temp_inode = inode_alloc[str(dire.inode_number)]
            temp_inode.dir_link_count += 1
        except KeyError:
            pass
    
    dl.dot_dot(directory=directory)
    dl.duplicate_block(blocks=blocks)
    dl.free_block_referenced(block_freelist=block_freelist)
    dl.free_inode_referenced(directory=directory)
    dl.inconsistent_inode(inode_alloc, inode_freelist)
    dl.inconsistent_link_count(inode_alloc=inode_alloc)
    dl.invalid_block_number(blocks=blocks)
    dl.invalid_inode(directory=directory)
    dl.reserved_block(blocks=blocks)
    dl.unreferenced_block(blocks=blocks)
