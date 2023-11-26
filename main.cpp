
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

int block_size=0;
int inode_size=0;
int itable_initial_addr;


bool getBit(unsigned short value, int position) // position in range 0-15 for 16 bits
{
    return (value >> position) & 0x1;
}

void printHex(unsigned char byte){
    printf("%02X ", byte);
}

void hexDump(fstream* file, int pos, int size){
  file->seekg(pos);
  // unsigned int dump[size];
  for (int i = 0; i < size; i++) {
    char byte;
    file->read((char*)(&byte), 1 );
    // printf("%c ", byte);
    printHex(byte);
  }

  printf("\n");
}
void catblock(fstream* file, int pos){
  file->seekg(pos);
  char block[block_size];
  file->read(block, block_size );
  printf("%s\n", block);
}

void print_super_block(ext4_super_block* super_block){
  // printf("Block Count: %d / Blocks p group: %d = %f \n", super_block.s_blocks_count_lo, super_block.s_blocks_per_group, N_G_DESC);
  printf("tamanho do super_block: %ld\n",sizeof(super_block));
  printf("volume name: %s\n",super_block->s_volume_name);
  printf("last mount: %s\n",super_block->s_last_mounted);
  printf("blocks count: %d\n",super_block->s_blocks_count_lo);
  printf("blocks size: %d\n",super_block->s_log_block_size); //ESPERADO 4096 - RECEBIDO: 2
  printf("inodes count: %d\n",super_block->s_inodes_count);
  printf("first data block: %d\n",super_block->s_first_data_block);
  printf("inodes per group: %d\n",super_block->s_inodes_per_group);
  printf("first inode: %d\n",super_block->s_first_ino);
  printf("\n");
}

void print_block_desc(ext4_group_desc* GDT){
  printf("bg block bitmap: %d\n", GDT->bg_block_bitmap_lo);
  printf("bg inode bitmap: %d\n", GDT->bg_inode_bitmap_lo);
  printf("bg inode table: %d\n", GDT->bg_inode_table_lo);

  // printf("Block BITMAP:%d - %d\n", GDT->bg_block_bitmap_lo, FILE_POS);
  //BLOCK BITMAP
  // FILE_POS= GDT.bg_block_bitmap_lo * block_size;
  // myfile.seekg(FILE_POS);
  // printf("FILE_POS %d :%s\n", 282624, block_bitmap);
  // printFromFile(&myfile, FILE_POS, block_size);

  //INODE BITMAP
  // FILE_POS= GDT.bg_inode_bitmap_lo * block_size;
  // myfile.seekg(FILE_POS);
  // printf("\nInode BITMAP:%d - %d\n", GDT.bg_inode_bitmap_lo, FILE_POS);
  // printf("FILE_POS %d :%s\n", 282624, block_bitmap);
  // printFromFile(&myfile, FILE_POS, block_size);
  
  //INODE TABLE
  //Inode Table Size = No. of Inodes in each Group(sb.s_inodes_per_group) * inode_size
  // printf("\nINODE table at %d:\n", GDT.bg_inode_table_lo);
}

void print_inode(ext4_inode* inode){
  printf("block count: %d\n", inode->i_blocks_lo);
  printf("i links count: %d \n", inode->i_links_count);
  printf("flags: %X \n", inode->i_flags);
  printf("file mode: %X \n", inode->i_mode);

   unsigned short b = inode->i_mode;

    // Print the bits from position 15 to 0
    for (int i = 15; i >= 0; --i) {
        std::cout << getBit(b, i);
        if (i % 4 == 0) {
            std::cout << ' '; // Add space every 4 bits for better readability
        }
    }
    printf("\n");
}

void print_ext_header(ext4_extent_header* ext_header){
  printf("ext header\n");
  printf("depth: %d \n",ext_header->eh_depth);
  printf("entries: %d \n",ext_header->eh_entries);
  printf("magic: %d \n",ext_header->eh_magic);
}

void print_ext(ext4_extent* ext){
  printf("extends:\n");
  printf("ee_block: %d\n",ext->ee_block);
  printf("ee_len: %d\n",ext->ee_len);
  printf("ee_start_lo: %d\n",ext->ee_start_lo);
}
void print_ext_idx(ext4_extent_idx* _idx){
  printf("extends:\n");
  printf("ei_block: %d\n",_idx->ei_block);
  printf("ei_leaf_hi: %d\n",_idx->ei_leaf_hi);
  printf("ei_leaf_lo: %d\n",_idx->ei_leaf_lo);
  printf("ei_unused: %d\n",_idx->ei_unused);
}

void print_dir(fstream* file, int block_position){
  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  printf("DIR %d - %d\n",FILE_POS, block_last);
  ext4_dir_entry_2 dir;

  while (FILE_POS < block_last) {
    file->seekg(FILE_POS);
    file->read((char*)(&dir),  sizeof(ext4_dir_entry_2) );
    printf("(%d)[%d] %s\n",dir.inode, dir.file_type, dir.name);
    // printf("f-pos: %d\n", FILE_POS);
    // printf("reclen: %d\n", dir.rec_len);
    // printf("file_type: %d\n\n\n\n", dir.file_type);
    
    FILE_POS+=dir.rec_len;
  }
}

int find_by_name(fstream* file, int block, char* name){
  int inode_addr= -1;
  int block_position = block * block_size;
  // print_dir(file, block_position);
  // printf("block:%d, name:%s\n", block_position, name);
  // return inode_addr;

  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  // printf("DIR %d - %d\n",FILE_POS, block_last);
  ext4_dir_entry_2 dir;

  while (FILE_POS < block_last) {
    file->seekg(FILE_POS);
    file->read((char*)(&dir),  sizeof(ext4_dir_entry_2) );

    if (strcmp(dir.name, name) == 0){
      inode_addr = dir.inode;
      break;
    }
    
    // printf("(%d)[%d] %s\n",dir.inode, dir.file_type, dir.name);
    FILE_POS+=dir.rec_len;
  }
  return inode_addr;
}

void change_dir(fstream* file, int itable_addr, char* inode_name, ext4_inode* cur_dir){
  ext4_extent_header cur_header;
  memcpy(&cur_header, cur_dir->i_block, sizeof(ext4_extent_header));
  
  ext4_extent cur_ext;
  memcpy(&cur_ext, &cur_dir->i_block[3], sizeof(ext4_extent));

  int inode_addr = find_by_name(file, cur_ext.ee_start_lo, inode_name);  
  if (inode_addr == -1){
    printf("DIRETÓRIO NÃO ENCONTRADO!\n");
    return;
  }
  

  int FILE_POS = itable_addr + (inode_addr * inode_size) - inode_size;
  ext4_inode dir;
  file->seekg(FILE_POS);
  file->read((char*)(&dir),  sizeof(ext4_inode) );
  // print_inode(&inode);

  ext4_extent_header dir_header;
  memcpy(&dir_header, dir.i_block, sizeof(ext4_extent_header));
  
  ext4_extent ext_dir;
  memcpy(&ext_dir, &dir.i_block[3], sizeof(ext4_extent));
  FILE_POS = ext_dir.ee_start_lo * block_size;

  print_dir(file, FILE_POS);
}

void cat_file(fstream* file, ext4_inode* inode){

  print_inode(inode);
  ext4_extent_header ext_header;
  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));
  ext4_extent cur_ext;
  memcpy(&cur_ext, &inode->i_block[3], sizeof(ext4_extent));

  ext4_extent_header f_header;
  memcpy(&f_header, inode->i_block, sizeof(ext4_extent_header));
  print_ext_header(&f_header);
  
  ext4_extent f_ext;
  memcpy(&f_ext, &inode->i_block[3], sizeof(ext4_extent));
  print_ext(&f_ext);
  
  catblock(file, f_ext.ee_start_lo * block_size);
}

void init_ext4(fstream* file, ext4_super_block* super_block, ext4_inode* root_dir, ext4_extent_header* ext_header,  ext4_extent* ext){
  int FILE_POS;
  //abrir o .img
  file->open("myext4image4k.img", fstream::in | fstream::binary);
  FILE_POS= 1024;

  file->seekg(FILE_POS);
  //SUPERBLOCK
  file->read((char*)(super_block),  sizeof(ext4_super_block) );//1024
  // print_super_block(&super_block);

  // Block count / Blocks per group
  // float N_G_DESC = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;
  
  int x = 10 + super_block->s_log_block_size;
  block_size = pow(2, x);

  inode_size = super_block->s_inode_size;

  //número de block groups
  // int N_GDT = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;
  
  //ext4_group_desc
  FILE_POS=block_size;
  file->seekg(FILE_POS);
  ext4_group_desc GDT;
  file->read((char*)(&GDT),  sizeof(ext4_group_desc) );
  // print_block_desc(&GDT);  

  // printf("(%d,* %d) + %d\n", GDT.bg_inode_table_lo, block_size, inode_size);

  itable_initial_addr=GDT.bg_inode_table_lo * block_size;

  //inode <2>
  FILE_POS = itable_initial_addr + inode_size;

  file->seekg(FILE_POS);
  file->read((char*)(root_dir),  sizeof(ext4_inode) );
  // print_inode(&ROOT);

  memcpy(ext_header, root_dir->i_block, sizeof(ext4_extent_header));
  // print_ext_header(&ext_header);
  
  memcpy(ext, &root_dir->i_block[3], sizeof(ext4_extent));
  //is eh_depth == 0 ? ext4_extent : ext4_extent_idx  
  // ext4_extent ext = *(ext4_extent) &inode.i_block;
  // print_ext(&ext);

}

int main () {
  int FILE_POS=0;

  fstream file;
  ext4_super_block super_block;
  ext4_inode root_dir;
  ext4_extent_header root_header;
  ext4_extent root_extend;
  
  //0 = ext, 1 = ext_idx
  // int is_extended = 0;
  // ext4_extent_idx root_extend_idx;

  init_ext4(&file, &super_block, &root_dir, &root_header, &root_extend);

  // print_super_block(&super_block);

  FILE_POS = root_extend.ee_start_lo * block_size;
  file.seekg(FILE_POS);

  char* hello = "hello.txt";
  int hello_i_addr = find_by_name(&file, root_extend.ee_start_lo, hello);
  printf("hello_i_addr: %d\n", hello_i_addr);
  if (hello_i_addr != -1){
    // printf("hello.txt found, [%d]inode\n",hello_i_addr);
    FILE_POS = itable_initial_addr + (hello_i_addr * inode_size) - inode_size;
    
    file.seekg(FILE_POS);
    ext4_inode f;
    file.read((char*)(&f),  sizeof(ext4_inode) );
    cat_file(&file,&f);
  }

  char* dir_name = "documentos";
  change_dir(&file, itable_initial_addr, dir_name  , &root_dir);  

  file.close();
  return 0;
}