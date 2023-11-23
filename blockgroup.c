
struct block_group {
  // Group 0 Padding	
  g0padding;	//1 block	

  // ext4 Super Block	
  ext4_super_block;	//1024 bytes	

  // Group Descriptors	
  g_descriptors;	//many blocks	
  
  // Reserved GDT Blocks	
  reserved_GDT_blocks; //many blocks	

  // Data Block Bitmap	
  data_block_bitmap;	//1 block	

  // inode Bitmap	
  inode_bitmap;	//1 block	

  // inode Table	
  inode_table;	//many blocks	

  // Data Blocks
  data_blocks; //many more blocks
};


