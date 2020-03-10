"""
algorigthms for block and directories
"""
import utils


#Inode Consistency
def inconsistent_inode():
    for id in range(1,MAX_INODE_NUM+1):
        allocated_inode = inode_alloc.get(str(id))
        free_inode = inode_freelist.get(str(id))
        if allocated_inode is not None and free_inode is not None:
            toprint = "ALLOCATED INODE " + str(id) + " ON FREELIST"
            print(toprint)
        elif allocated_inode is None and free_inode is None:
            toprint = "UNALLOCATED INODE " + str(id) + " NOT ON FREELIST"
            print(toprint)
            toprint = "INODE " + str(id) + " HAS 0 LINKS BUT LINKCOUNT IS 1"
            




#Block Consistency

#checking invalid blocks - block id smaller than 0 and greater than max
def invalid_block_number():
    for b in blocks.values():
        if b.id < 0 or b.id > MAX_BLOCK_NUM:
            for t in b.inode_refs:
                # t is a dictionary
                toprint = "INVALID "+ t["indirection"] + " " + str(b.id) + " IN INODE " + t["inode"] + " AT OFFSET "+ t["offset"]                
                print(toprint)



#checking whether the block is reserved - reserved block has id from 1 to RESERVED_BLOCK_ID
def reserved_block():
    for b in blocks.values():
        if b.id < RESERVED_BLOCK_ID:
            for t in b.inode_refs:
                toprint = "RESERVED "+ t["indirection"] + " " + str(b.id) + " IN INODE " + t["inode"] + " AT OFFSET "+ t["offset"]
                print(toprint)



#checking unreferenced blocks
def unreferenced_block():
    for b in blocks.values():
        reference_count = len(b.inode_refs)
        if reference_count == 0 and b.allocated:
            #if not in free list and not being referenced
            toprint = "UNREFERENCED BLOCK " + str(b.id)
            print(toprint)

#checking free blocks being referenced
def free_block_referenced():
    for b in block_freelist:
        reference_count = len(b.inode_refs)
        if reference_count > 0:
            toprint = "ALLOCATED BLOCK " + str(b.id) + " ON FREELIST"

#checking if the block is referenced more than once
def duplicate_block():
    for b in blocks.values():
        reference_count = len(b.inode_refs)
        if reference_count > 1:
            for t in b.inode_refs:
                toprint = "DUPLICATED "+ t["indirection"] + " " + str(b.id) + " IN INODE " + t["inode"] + " AT OFFSET "+ t["offset"]
                print(toprint)   


#check directory consistency


def invalid_inode():
    for d in directory:
        if d.inode_number < 1 and d.inode_number > MAX_INODE_NUM:
            toprint = "DIRECTORY INODE" + d.parent_inode_number + " NAME " + d.name + " INVALIDE INODE " + d.inode_number
            print(toprint)

def free_inode_referenced():
    for d in directory: 
        # d.inode number could be invalid             
        referenced_inode = directory.get(str(d.inode_number))
        if referenced_inode is None:
            toprint = "DIRECTORY INODE " + str(d.parent_inode_number) + " NAME " + d.name + " UNALLOCATED INODE " + str(d.inode_number)
            print(toprint)

def inconsistent_link_count():
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
            toprint = "INODE " + str(i.id) + " HAS " + str(i.dir_link_count) + " LINKS BUT LINKCOUNT IS " + str(i.link_count)
            print(toprint)

def dot_dot():
    for d in directory:
        if d.name == '.':
            if d.inode_number != d.parent_inode_number:
                toprint = "DIRECTORY INODE " + str(d.parent_inode_number) + " NAME " + d.name + " LINK TO INODE " + str(d.inode_number) + " SHOULD BE " + str(d.parent_inode_number)
                print(toprint)
        if d.name == '..':
            if d.inode_number == 2:
                if d.parent_inode_number != d.inode_number:
                    toprint = "DIRECTORY INODE " + str(d.parent_inode_number) + " NAME " + d.name + " LINK TO INODE " + str(d.inode_number) + " SHOULD BE 2"
                    print(toprint)


        
        