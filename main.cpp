
// basic file operations
#include <iostream>
#include <fstream>
#include "ext4_objs.hpp"
#include <iterator>
#include <sstream>
#include <string>
#include <cstring>
#include<cmath>

using namespace std;

#define BIT 1
#define BYTE 8 * BIT

void printHex(unsigned char byte){
    printf("%02X ", byte);
}

void printFromFile(fstream& file, int pos, int size){
  file.seekg(pos);
  // unsigned int dump[size];
  for (int i = 0; i < size; i++) {
    unsigned char byte;
    file.read((char*)(&byte), 1 );
    // printf("%02X ", byte);
    printHex(byte);
  }

  printf("\n");
}
void printFromFileToChar(fstream& file, int pos, int size){
  file.seekg(pos);
  // unsigned int dump[size];
  for (int i = 0; i < size; i++) {
    char byte;
    file.read((char*)(&byte), 1 );
    printf("%d ", byte);
    // printHex(byte);
  }

  printf("\n");
}

int main () {
  int FILE_POS=0;
  int block_size;
  int inode_size;

  //abrir o .img
  fstream myfile;
  myfile.open("myext4image4k.img", fstream::in | fstream::binary);
  FILE_POS= 1024;
  myfile.seekg(FILE_POS);

  //SUPERBLOCK
  ext4_super_block super_block;
  myfile.read((char*)(&super_block),  sizeof(super_block) );//1024

  printf("tamanho do super_block: %ld\n",sizeof(super_block));
  printf("volume name: %s\n",super_block.s_volume_name);
  printf("last mount: %s\n",super_block.s_last_mounted);
  printf("blocks count: %d\n",super_block.s_blocks_count_lo);
  printf("blocks size: %d\n",super_block.s_log_block_size); //ESPERADO 4096 - RECEBIDO: 2
  printf("inodes count: %d\n",super_block.s_inodes_count);
  printf("first data block: %d\n",super_block.s_first_data_block);
  printf("inodes per group: %d\n",super_block.s_inodes_per_group);
  printf("first inode: %d\n",super_block.s_first_ino);

  printf("\n@@@@@@@@@@\n");
  // Block count / Blocks per group
  float N_G_DESC = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;
  printf("Block Count: %d / Blocks p group: %d = %f \n", super_block.s_blocks_count_lo, super_block.s_blocks_per_group, N_G_DESC);
  printf("\n@@@@@@@@@@\n");

  int x = 10 + super_block.s_log_block_size;
  block_size = pow(2, x);

  inode_size = super_block.s_inode_size;

  //ext4_group_desc
  // myfile.seekg(1024 + sizeof(super_block));

  //número de block groups
  int N_GDT = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;

  printf("BLOCK GROUP 1:\n");
  //BLOCK GROUP
  FILE_POS=block_size;
  myfile.seekg(FILE_POS);
  ext4_group_desc GDT;
  myfile.read((char*)(&GDT),  sizeof(GDT) );//64

  printf("bg block bitmap: %d\n", GDT.bg_block_bitmap_lo);
  printf("bg inode bitmap: %d\n", GDT.bg_inode_bitmap_lo);
  printf("bg inode table: %d\n", GDT.bg_inode_table_lo);

  //BLOCK BITMAP
  FILE_POS= GDT.bg_block_bitmap_lo * block_size;
  // myfile.seekg(FILE_POS);
  printf("Block BITMAP:%d - %d\n", GDT.bg_block_bitmap_lo, FILE_POS);
  // printf("FILE_POS %d :%s\n", 282624, block_bitmap);
  // printFromFile(myfile, FILE_POS, block_size);

  //INODE BITMAP
  FILE_POS= GDT.bg_inode_bitmap_lo * block_size;
  // myfile.seekg(FILE_POS);
  printf("\nInode BITMAP:%d - %d\n", GDT.bg_inode_bitmap_lo, FILE_POS);
  // printf("FILE_POS %d :%s\n", 282624, block_bitmap);
  // printFromFile(myfile, FILE_POS, block_size);
  
  //INODE TABLE
  //Inode Table Size = No. of Inodes in each Group(sb.s_inodes_per_group) * inode_size
  
  printf("\nINODE table at %d:\n", GDT.bg_inode_table_lo);


  FILE_POS = (GDT.bg_inode_table_lo * block_size) + inode_size;// *block_size;
  printf("(%d,* %d) + %d\n", GDT.bg_inode_table_lo, block_size, inode_size);
  printf("INODE 2 POS: %d\n", FILE_POS);
  
  ext4_inode inode;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&inode),  sizeof(ext4_inode) );




  printf("block count: %d \n", inode.i_blocks_lo);
  printf("i links count: %d \n", inode.i_links_count);
  printf("flags: %X \n", inode.i_flags);
  printf("file mode: %X \n", inode.i_mode);

  // ext_header =(ext4_extent*) &inode.i_block;
  ext4_extent_header ext_header;
  memcpy(&ext_header, inode.i_block, sizeof(ext4_extent_header));
  printf("ext header\n");
  printf("depth: %d \n",ext_header.eh_depth);
  printf("entries: %d \n",ext_header.eh_entries);
  printf("magic: %d \n",ext_header.eh_magic);
  
  ext4_extent ext;
  memcpy(&ext, &inode.i_block[3], sizeof(ext4_extent));
  //is eh_depth == 0 ? ext4_extent : ext4_extent_idxs  
  // ext4_extent ext = *(ext4_extent) &inode.i_block;
  printf("extends:\n");
  printf("ee_block: %d\n",ext.ee_block);
  printf("ee_len: %d\n",ext.ee_len);
  printf("ee_start_hi: %d\n",ext.ee_start_hi);
  printf("ee_start_lo: %d\n",ext.ee_start_lo);

  //  block;

  ext4_dir_entry_2 root;
  FILE_POS = ext.ee_start_lo * block_size;


  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );

  printf("!!ROOT!! %d - %d\n",FILE_POS, FILE_POS+block_size);
  

  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);

  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);

  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);

  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);
  
  
  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);
  
  
  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);
  
  FILE_POS+=root.rec_len;
  myfile.seekg(FILE_POS);
  myfile.read((char*)(&root),  sizeof(ext4_dir_entry_2) );
  printf("f-pos: %d\n", FILE_POS);
  printf("reclen: %d\n", root.rec_len);
  printf("name_len: %d\n", root.name_len);
  printf("name: %s\n", root.name);
  printf("inode: %d\n", root.inode);
  printf("file_type: %d\n\n\n\n", root.file_type);
  
  FILE_POS+=root.rec_len;
  printf("f-pos: %d\n", FILE_POS);
  

  myfile.close();
  return 0;
}