"""
algorigthms for block and directories
"""
import utils


# Inode Consistency
def inconsistent_inode(inode_alloc={}, inode_freelist=[]):
    for allocated_inode in inode_alloc.values():
        if allocated_inode.id in inode_freelist:
            toprint = "ALLOCATED INODE " + str(allocated_inode.id) + " ON FREELIST"
            print(toprint)
    
    for id in range(utils.FIRST_INODE_NUM, utils.MAX_INODE_NUM+1):
        if inode_alloc.get(str(id)) == None and id not in inode_freelist:
            toprint = "UNALLOCATED INODE " + str(id) + " NOT ON FREELIST"
            print(toprint)
            toprint = "INODE " + str(id) + " HAS 0 LINKS BUT LINKCOUNT IS 1"


# Block Consistency

# checking invalid blocks - block id smaller than 0 and greater than max
def invalid_block_number(blocks_invalid=[]):
    for b in blocks_invalid:
        # b is a dictionary
        toprint = "INVALID " + b["indirection"] + " " + str(b["id"]) + " IN INODE " + str(b["inode"]) + " AT OFFSET " + str(b["offset"])
        print(toprint)


# checking whether the block is reserved - reserved block has id from 1 to RESERVED_BLOCK_ID
def reserved_block(blocks={}):
    for b in blocks.values():
        if b.id < utils.RESERVED_BLOCK_ID and b.id > 0:
            for t in b.inode_refs:
                toprint = "RESERVED " + t["indirection"] + " " + str(
                    b.id) + " IN INODE " + str(t["inode"]) + " AT OFFSET " + str(t["offset"])
                print(toprint)


# checking unreferenced blocks
def unreferenced_block(blocks={}):
    for b in blocks.values():
        reference_count = len(b.inode_refs)
        if reference_count == 0 and b.allocated and b.id != 0 and b.id >= utils.RESERVED_BLOCK_ID:
            # if not in free list and not being referenced
            toprint = "UNREFERENCED BLOCK " + str(b.id)
            print(toprint)

# checking free blocks being referenced


def free_block_referenced(block_freelist=[], blocks={}):
    for b_id in block_freelist:
        b = blocks[str(b_id)]
        reference_count = len(b.inode_refs)
        if reference_count > 0:
            toprint = "ALLOCATED BLOCK " + str(b.id) + " ON FREELIST"
            print(toprint)

# checking if the block is referenced more than once


def duplicate_block(blocks={}):
    for b in blocks.values():
        if b.id == 0:
            continue
        reference_count = len(b.inode_refs)
        if reference_count > 1:
            for t in b.inode_refs:
                toprint = "DUPLICATE " + t["indirection"] + " " + str(
                    b.id) + " IN INODE " + str(t["inode"]) + " AT OFFSET " + str(t["offset"])
                print(toprint)


# check directory consistency


def invalid_inode(directory=[]):
    for d in directory:
        if d.inode_number < 1 or d.inode_number > utils.MAX_INODE_NUM:
            toprint = "DIRECTORY INODE " + str(d.parent_inode_number) + \
                " NAME " + d.name + " INVALID INODE " + str(d.inode_number)
            print(toprint)


def free_inode_referenced(directory=[], inode_freelist=[], inode_alloc={}):
    for d in directory:
        if d.inode_number in inode_freelist and str(d.inode_number) not in inode_alloc.keys():
            toprint = "DIRECTORY INODE " + \
                str(d.parent_inode_number) + " NAME " + d.name + \
                " UNALLOCATED INODE " + str(d.inode_number)
            print(toprint)


def inconsistent_link_count(inode_alloc={}):
    # for d in directory:
    #     parent = directory.get(str(d.parent_inode_number))
    #     if parent is None:
    #         continue
    #         #what if the parent directory is invalid ?
    #     else:
    #         count = parent.dir_link_count
    #         parent.dir_link_count = count + 1
    for i in inode_alloc.values():
        if i.link_count != i.dir_link_count:
            toprint = "INODE " + str(i.id) + " HAS " + str(i.dir_link_count) + \
                " LINKS BUT LINKCOUNT IS " + str(i.link_count)
            print(toprint)


def dot_dot(directory=[]):
    for d in directory:
        if d.name == "'.'":
            if d.inode_number != d.parent_inode_number:
                toprint = "DIRECTORY INODE " + str(d.parent_inode_number) + " NAME " + d.name + " LINK TO INODE " + str(
                    d.inode_number) + " SHOULD BE " + str(d.parent_inode_number)
                print(toprint)
        if d.name == "'..'":
            if d.parent_inode_number == 2:
                if d.parent_inode_number != d.inode_number:
                    toprint = "DIRECTORY INODE " + \
                        str(d.parent_inode_number) + " NAME " + d.name + \
                        " LINK TO INODE " + \
                        str(d.inode_number) + " SHOULD BE 2"
                    print(toprint)
