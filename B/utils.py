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
class Inode:
    def __init__(self):
        self.id = 0
        self.blocks = []
        self.link_count = 0
        self.dir_link_count = 0

class Block:
    def __init__(self):
        self.id = 0
        self.inode_refs = []
        self.indirection = ''
        self.allocated = True

class Directory:
    def __init__(self):
        self.inode_number = 0
        self.parent_inode_number = 0
        self.name = ""

class SuperBlock:
    def __init__(self, entry):
        self.block_count = entry[1]
        self.inode_count = entry[2]
        self.block_size = entry[3]
        self.inode_size = entry[4]
        self.blocks_per_group = entry[5]
        self.inodes_per_group = entry[6]
        self.first_nonres_inode = entry[7]
