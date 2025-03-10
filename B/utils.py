"""
classes declared here
inode 
    id 
    blocks list 
    link_count int 
    dir_link_count int 
block 
    id
    inode_refs list of dictionary: {"inode": , "offset": ,"indirection": }
    indirection string (NONE, INDIRECT, DOUBLE INDIRECT, TRIPLE INDIRECT)
    allocated boolean
directory
    inode_number int 
    parent_inode_number int
    name string  
"""


# maximum blocks in the fs
MAX_BLOCK_NUM = 0
# block number of the first non-reserved block
RESERVED_BLOCK_ID = 0
# maximum of inodes in the fs
MAX_INODE_NUM = 0 
# first valid inode number
FIRST_INODE_NUM = 0


class Inode:
    def __init__(self, entry):
        self.id = int(entry[1])
        self.blocks = []
        self.link_count = int(entry[6])
        self.dir_link_count = 0
        for i in range(12, 27):
            self.blocks.append(entry[i])


class Block:
    def __init__(self, id):
        self.id = id
        self.inode_refs = []
        self.allocated = True


class Directory:
    def __init__(self, entry):
        self.inode_number = int(entry[3])
        self.parent_inode_number = int(entry[1])
        self.name = entry[6]


class SuperBlock:
    def __init__(self, entry):
        self.block_count = int(entry[1])
        self.inode_count = int(entry[2])
        self.block_size = int(entry[3])
        self.inode_size = int(entry[4])
        self.blocks_per_group = int(entry[5])
        self.inodes_per_group = int(entry[6])
        self.first_nonres_inode = int(entry[7])
