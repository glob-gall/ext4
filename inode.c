enum inode_type {
  // 1	List of defective blocks.
  LIST_DEFECTIVE_BLOCKS = 1,

  // 2	Root directory.
  ROOT_DIR=2,

  // 3	User quota.
  USER_QUOTA=3,

  // 4	Group quota.
  GROUP_QUOTA=4,

  // 5	Boot loader.
  BOOT_LOADER=5,

  // 6	Undelete directory.
  UNDELETE_DIR=6,

  // 7	Reserved group descriptors inode. ("resize inode")
  INODE_DESCRIPTOR=7,

  // 8	Journal inode.
  JOURNAL_INODE=8,

  // 9	The "exclude" inode, for snapshots(?)
  EXCLUDE_INODE=9,

  // 10	Replica inode, used for some non-upstream feature?
  REPLICA_INODE=10,

  // 11	Traditional first non-reserved inode. Usually this is the lost+found directory. See s_first_ino in the superblock.
  LOST_FOUND_DIR=11
};

