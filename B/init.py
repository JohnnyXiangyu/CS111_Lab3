import utils
import csv
from optparse import OptionParser
import sys

filename = ""
report = []

# stringify indirection level
indirection_str = ["INDIRECT", "DOUBLE INDIRECT", "TRIPLE INDIRECT"]

# gobal data structures
inode_alloc = {}
inode_freelist = {}
blocks = {}
block_freelist = []
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
        blocks.update({new_id: utils.Block})
    
    # construct raw structures
    for entry in report:
        entry_type = entry[0]
        if entry_type == "BFREE":
            blocks[str(i)].allocated = False
            block_freelist.append(i)
        elif entry_type == "IFREE":
            inode_freelist.update({str(i): utils.Inode(entry)})
        elif entry_type == "INODE":
            inode_alloc.update({str(i): utils.Inode(entry)})
        elif entry_type == "DIRENT":
            directory.append(utils.Directory(entry))
        elif entry_type == "INDIRECT":
            offset = entry[3]
            bid = entry[5]
            inode = int(entry[1])
            indirection = indirection_str[int(entry[2]) - 1]
            temp_block = blocks[bid]
            temp_block.inode_refs.append({"inode": inode, "offset": offset, "indirection": indirection})


    # update metadata in blocks
    for inode in inode_alloc:
        for i in range(0, 16):
            block_id = inode.blocks[i]
            temp_block = blocks[str(block_id)]
            temp_block.inode_refs.append({"inode": inode.id, "offset": i, "indirection": "DIRECT"})
    
    # update reference count in inode
    for dire in directory:
        try:
            temp_inode = inode_alloc[str(dire.inode_number)]
            temp_inode.dir_link_count += 1
        except KeyError:
            pass


