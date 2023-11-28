
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
#define MAX_INPUT_SIZE 255

int block_size=0;
int inode_size=0;
int itable_initial_addr;
int inode_bitmap_addr;
int block_bitmap_addr;
int total_inodes;
int total_blocks;
fstream file;


// bool getBit(unsigned short value, int position){
//     return (value >> position) & 0x1;
// }
bool getBit(char value[], int position){
    int index = position/8;
    int offset = position % 8;

    unsigned char byte = value[index];
    return (byte >> offset) & 0x1;
}

void printHex(unsigned char byte){
    printf("%02X ", byte);
}

void hexDump( int pos, int size){
  file.seekg(pos);
  // unsigned int dump[size];
  for (int i = 0; i < size; i++) {
    char byte;
    file.read((char*)(&byte), 1 );
    // printf("%c ", byte);
    printHex(byte);
  }

  printf("\n");
}
void catblock( int pos){
  file.seekg(pos);
  char block[block_size];
  file.read(block, block_size );
  printf("%s\n", block);
}

void print_super_block(ext4_super_block* super_block){
  // printf("Block Count: %d / Blocks p group: %d = %f \n", super_block.s_blocks_count_lo, super_block.s_blocks_per_group, N_G_DESC);
  printf("tamanho do super_block: %ld\n",sizeof(super_block));
  printf("volume name: %s\n",super_block->s_volume_name);
  printf("last mount: %s\n",super_block->s_last_mounted);
  printf("blocks size: %d\n",super_block->s_log_block_size); //ESPERADO 4096 - RECEBIDO: 2
  printf("blocks count: %d\n",super_block->s_blocks_count_lo);
  printf("free blocks: %d\n",super_block->s_free_blocks_count_lo);
  printf("first data block: %d\n",super_block->s_first_data_block);
  printf("inodes per group: %d\n",super_block->s_inodes_per_group);
  printf("inodes count: %d\n",super_block->s_inodes_count);
  printf("free inodes: %d\n",super_block->s_free_inodes_count);
  printf("first inode: %d\n",super_block->s_first_ino);
  
  switch (super_block->s_creator_os){
  case 0:
    printf("SO: Linux\n");
    break;
  case 1:
    printf("SO: Hurd\n");
    break;
  case 2:
    printf("SO: Masix\n");
    break;
  case 3:
    printf("SO: FreeBSD\n");
    break;
  case 4:
    printf("SO: Lites\n");
    break;
  
  default:
    break;
  }

  printf("magic number: %d\n",super_block->s_magic);
  
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

void print_inode(ext4_inode* inode){
  ext4_extent_header ext_header;
  ext4_extent ext[3];

  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));
  
  memcpy(&ext, &inode->i_block[3], sizeof(ext4_extent)*3);
  //is eh_depth == 0 ? ext4_extent : ext4_extent_idx  
  // ext4_extent ext = *(ext4_extent) &inode.i_block;


  printf("====================[INODE %d]====================\n",inode->i_uid);
  printf("block count: %d\n", inode->i_blocks_lo);
  printf("i links count: %d \n", inode->i_links_count);
  printf("flags: %X \n", inode->i_flags);
  printf("file mode: %X \n", inode->i_mode);
  printf("===extend header===\n");
  print_ext_header(&ext_header);
  printf("======extends======\n");
  print_ext(&ext[0]);
  print_ext(&ext[1]);
  print_ext(&ext[2]);

  printf("==================================================\n");

  //  unsigned short b = inode->i_mode;

  //   // Print the bits from position 15 to 0
  //   for (int i = 15; i >= 0; --i) {
  //       std::cout << getBit(b, i);
  //       if (i % 4 == 0) {
  //           std::cout << ' '; // Add space every 4 bits for better readability
  //       }
  //   }
    printf("\n");
}

void print_dir(int block){
  int block_position = block * block_size;
  
  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  printf("DIR %d - %d\n",FILE_POS, block_last);
  ext4_dir_entry_2 dir;

  while (FILE_POS < block_last) {
    file.seekg(FILE_POS);
    file.read((char*)(&dir),  sizeof(ext4_dir_entry_2) );
    printf("(%d)[%d] %s\n",dir.inode, dir.file_type, dir.name);
    // printf("f-pos: %d\n", FILE_POS);
    // printf("reclen: %d\n", dir.rec_len);
    // printf("file_type: %d\n\n\n\n", dir.file_type);
    
    FILE_POS+=dir.rec_len;
  }
}

int find_by_name( int block, char* name){
  int inode_addr= -1;
  int block_position = block * block_size;
  // print_dir(file, block);
  // printf("block:%d, name:%s\n", block_position, name);
  // return inode_addr;

  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  // printf("DIR %d - %d\n",FILE_POS, block_last);
  ext4_dir_entry_2 dir;

  while (FILE_POS < block_last) {
    file.seekg(FILE_POS);
    file.read((char*)(&dir),  sizeof(ext4_dir_entry_2) );

    // printf("[%ld]:%s   [%d]:%s\n",strlen(name), name, dir.name_len, dir.name);

    if (strncmp(name, dir.name, strlen(name)) == 0){
      inode_addr = dir.inode;
      break;
    }
    
    // printf("(%d)[%d] %s\n",dir.inode, dir.file_type, dir.name);
    FILE_POS+=dir.rec_len;
  }
  return inode_addr;
}

int change_dir( char* inode_name, ext4_inode* cur_inode, ext4_extent_header* cur_header ,ext4_extent* cur_ext){

  int inode_addr = find_by_name(cur_ext->ee_start_lo, inode_name);  
  if (inode_addr == -1){
    printf("DIRETÓRIO NÃO ENCONTRADO!\n");
    return -1;
  }
  

  int FILE_POS = itable_initial_addr + (inode_addr * inode_size) - inode_size;
  
  file.seekg(FILE_POS);
  file.read((char*)(cur_inode),  sizeof(ext4_inode) );
  // print_inode(cur_inode);

  memcpy(cur_header, cur_inode->i_block, sizeof(ext4_extent_header));
  
  // ext4_extent* ext_dir ;
  memcpy(cur_ext, &cur_inode->i_block[3], sizeof(ext4_extent)*3);
  
  // print_dir(cur_ext->ee_start_lo);
  return 0;
}

void cat_file(fstream* file, ext4_inode* inode){

  // print_inode(inode);
  ext4_extent_header ext_header;
  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));
  ext4_extent cur_ext;
  memcpy(&cur_ext, &inode->i_block[3], sizeof(ext4_extent)*3);

  ext4_extent_header f_header;
  memcpy(&f_header, inode->i_block, sizeof(ext4_extent_header));
  // print_ext_header(&f_header);
  
  ext4_extent f_ext;
  memcpy(&f_ext, &inode->i_block[3], sizeof(ext4_extent)*3);
  // print_ext(&f_ext);
  
  catblock( f_ext.ee_start_lo * block_size);
}

void init_ext4(ext4_super_block* super_block, ext4_inode* root_dir, ext4_extent_header* ext_header,  ext4_extent* ext){
  int FILE_POS;
  //abrir o .img
  file.open("myext4image4k.img", fstream::in | fstream::binary);
  FILE_POS= 1024;

  file.seekg(FILE_POS);
  //SUPERBLOCK
  file.read((char*)(super_block),  sizeof(ext4_super_block) );//1024
  // print_super_block(&super_block);

  // Block count / Blocks per group
  // float N_G_DESC = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;
  
  int x = 10 + super_block->s_log_block_size;
  block_size = pow(2, x);

  inode_size = super_block->s_inode_size;

  total_inodes = super_block->s_inodes_count;
  total_blocks = super_block->s_blocks_count_lo;

  //número de block groups
  // int N_GDT = super_block.s_blocks_count_lo / super_block.s_blocks_per_group;
  
  //ext4_group_desc
  FILE_POS=block_size;
  file.seekg(FILE_POS);
  ext4_group_desc GDT;
  file.read((char*)(&GDT),  sizeof(ext4_group_desc) );
  // print_block_desc(&GDT);  
  inode_bitmap_addr = GDT.bg_inode_bitmap_lo * block_size;
  block_bitmap_addr = GDT.bg_block_bitmap_lo * block_size;

  itable_initial_addr=GDT.bg_inode_table_lo * block_size;
  // printf("itable = %d * %d = %d\n", itable_initial_addr, GDT.bg_inode_bitmap_lo, block_size);

  //inode <2>
  FILE_POS = itable_initial_addr + inode_size;
  // printf("inode 2 = %d\n", FILE_POS);

  file.seekg(FILE_POS);
  file.read((char*)(root_dir),  sizeof(ext4_inode) );
  // print_inode(&ROOT);

  memcpy(ext_header, root_dir->i_block, sizeof(ext4_extent_header));
  // print_ext_header(&ext_header);
  
  memcpy(ext, &root_dir->i_block[3], sizeof(ext4_extent)*3);
  //is eh_depth == 0 ? ext4_extent : ext4_extent_idx  
  // ext4_extent ext = *(ext4_extent) &inode.i_block;
  // print_ext(&ext);

}


void change_pathname(char** current_path, int* path_size,char *dir_name){
  if (strcmp(dir_name,".") == 0)
    return;
  if ((strcmp(dir_name,"..") == 0)){
    if ( (*path_size) > 1 ){
      free(current_path[*path_size - 1]);
      (*path_size)--;
    }
    
    return;
  }

  current_path[*path_size] = strdup(dir_name);
  (*path_size)++;
  
}

void test_inode(int inode){ //ARRUMAR
  if (inode > total_inodes){
    printf("inode <%d> nao existe a acontagem de inodes vai ate %d\n",inode, total_inodes);
    return; 
  }

  //BLOCK BITMAP
  // FILE_POS= GDT.bg_block_bitmap_lo * block_(*path_size);
  // myfile.seekg(FILE_POS);
  // printf("FILE_POS %d :%s\n", 282624, block_bitmap);
  // printFromFile(&myfile, FILE_POS, block_size);

  //INODE BITMAP
  file.seekg(inode_bitmap_addr);
  char block[block_size];
  file.read(block, block_size );
  bool result = getBit(block, inode);
    
  // printf("\nInode<%d>\n",inode);
  // printf("inode bitmap addr:%d\n",inode_bitmap_addr);
  // catblock(inode_bitmap_addr);
  // hexDump(inode_bitmap_addr, block_size);
  
  if (!result){
    printf("Inode disponivel\n");
    return;
  }
    printf("Inode nao disponivel\n");

  
  

}

void print_path(char** path,int path_size){

  for (int i = 0; i < path_size; i++){
    printf("%s/", path[i]);
  }
}

int main () {
  int FILE_POS=0;

  ext4_super_block super_block;
  ext4_inode root_dir;
  ext4_extent_header root_header;
  
  ext4_inode current_dir;
  ext4_extent_header current_header;
  ext4_extent current_extend[3];
  
  //0 = ext, 1 = ext_idx
  // int is_extended = 0;
  // ext4_extent_idx root_extend_idx;

  ext4_extent root_extend[3];
  init_ext4( &super_block, &root_dir, &root_header, root_extend );
  current_dir = root_dir;
  current_header = root_header;
  current_extend[0]= root_extend[0];
  current_extend[1]= root_extend[1];
  current_extend[2]= root_extend[2];

  
  
  // print_super_block(&super_block);
  char* cmd[MAX_INPUT_SIZE];
  char input[MAX_INPUT_SIZE];
  int cmd_size;

  int path_size=1;
  char* current_path[50];
  current_path[0] = strdup(" ");
  

  while (1){
    // print_inode(&current_dir);
    // print_ext_header(&current_header);
    // print_ext(&current_extend);
    printf("[%d]",path_size);
    printf("[");
    print_path( current_path,path_size );
    printf("]");
    
    cmd_size=0;
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1]='\0';
    if (strcmp(input, "exit") == 0) break;

    if (input[strlen(input)-1] == '\n'){
      input[strlen(input)-1] = '\0';
    }
    char * token = strtok(input, " ");

    while( token != NULL ) {
      cmd[cmd_size] = token;
      token = strtok(NULL, " ");
      cmd_size++;
    }
    // for (int i = 0; i < cmd_size; i++)
    //   printf("%s, ",cmd[i]);
    // printf("\n");
    // printf("cmd[0]: %s\n",cmd[0]);
    
    
    if (strcmp(cmd[0],"clear") == 0){
      printf("\e[1;1H\e[2J");
    
    }else if (strcmp(cmd[0],"ls") == 0){
      print_dir(current_extend[0].ee_start_lo);
    
    }else if (strcmp(cmd[0],"cd") == 0){
      char dir_name[255];
      strcpy(dir_name,cmd[1]);
      int result = change_dir( dir_name, &current_dir, &current_header, current_extend); 
        
      if (result == 0){
        change_pathname(current_path,&path_size, dir_name);
      }
       
      
    }else if(strcmp(cmd[0],"cat") == 0){
      char* file_name = cmd[1];
      printf("procurando arquivo %s\n",file_name);
      int file_addrs = find_by_name( current_extend[0].ee_start_lo, file_name);

      printf("file_addrs: %d\n\n", file_addrs);
      if (file_addrs != -1){
        // printf("hello.txt found, [%d]inode\n",file_addrs);
        FILE_POS = itable_initial_addr + (file_addrs * inode_size) - inode_size;
        
        file.seekg(FILE_POS);
        ext4_inode f;
        file.read((char*)(&f),  sizeof(ext4_inode) );
        cat_file(&file,&f);
      }
      
    }else if (strcmp(cmd[0],"testi") == 0){
      test_inode(atoi(cmd[1]));
    
    }else if(strcmp(cmd[0],"pwd") == 0){
      // printf("current path: ");
      print_path(current_path,path_size);
      printf("\n");
    
    }else if (strcmp(cmd[0],"info") == 0){
      
      print_super_block(&super_block);
    }
    
    
  }
  

  // FILE_POS = root_extend.ee_start_lo * block_size;
  // file.seekg(FILE_POS);





  file.close();
  return 0;
}