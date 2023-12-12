
#include <iostream>
#include <fstream>
#include "ext4_objs.hpp"
#include <iterator>
#include <sstream>
#include <string>
#include <cstring>
#include<cmath>
// #define Name "/images_ext4/myext4image1k.img"
using namespace std;

#define BIT 1

#define BYTE 8 * BIT
#define MAX_INPUT_SIZE 255

//variaveis globais
int block_size=0;
int inode_size=0;
int itable_initial_addr;
int inode_bitmap_addr;
int block_bitmap_addr;
int total_inodes;
int total_blocks;
char img_name[100] = "myext4image2k.img";
fstream file;

//recebe um bloco de informação value e retorna o valor de um bit expecifico na posição position
bool getBit(char value[], int position){
    int index = position/8;// pega 1 byte e divide por 8
    int offset = position % 8;// pega o bit do byte encontrado
    unsigned char byte = value[index];
    return (byte >> offset) & 0x1;
}


void printHex(unsigned char byte){
    printf("%02X ", byte);// pega o byte e imprime em Hexdecimal
}
//função utilizada apenas para testes durante desenvolvimento
//imprime do arquivo img escolhido previamente da posição pos até size
void hexDump( int pos, int size){
  file.seekg(pos);
  for (int i = 0; i < size; i++) {
    char byte;
    file.read((char*)(&byte), 1 );
    printHex(byte);
  }

  printf("\n");
}

void catblock( int pos){ //percorre o tamanho de um bloco em uma posição no arquivo 
  file.seekg(pos);
  char block[block_size];
  file.read(block, block_size);
  printf("%s", block);
}

void getblock(char* block, int pos){ //le da posição pos e salva na variavel block
  file.seekg(pos);
  file.read(block, block_size);
}

void print_super_block(ext4_super_block* super_block){
  printf("imagem: %s\n",img_name);
  printf("tamanho do super_block: %ld\n",sizeof(super_block));
  printf("volume name: %s\n",super_block->s_volume_name);
  printf("last mount: %s\n",super_block->s_last_mounted);
  printf("blocks size: %d\n",super_block->s_log_block_size);
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
}

void print_ext_header(ext4_extent_header* ext_header){
  printf("ext header\n");
  printf("depth: %d \n",ext_header->eh_depth);
  printf("entries: %d \n",ext_header->eh_entries);
  printf("magic: %d \n",ext_header->eh_magic);
}

void print_ext(ext4_extent* ext){
  printf("=============[ Extend ]=============\n");
  printf("ee_block: %d\n",ext->ee_block);
  printf("ee_len: %d\n",ext->ee_len);
  printf("ee_start_lo: %d\n",ext->ee_start_lo);
  printf("ee_start_hi: %d\n",ext->ee_start_hi);
}

void print_ext_idx(ext4_extent_idx* idx){
  printf("===========[ Extend IDX ]===========\n");
  printf("ei_block: %d\n",idx->ei_block);
  printf("ei_leaf_hi: %d\n",idx->ei_leaf_hi);
  printf("ei_leaf_lo: %d\n",idx->ei_leaf_lo);
  printf("ei_unused: %d\n",idx->ei_unused);
}

//traduz e imprime as permisões do formato unsigned short mode para uma string legivel 
void print_inode_permissions(unsigned short mode){
  char permisions[]="--- --- ---";
  
  if ( (mode & 0x1))
    permisions[10] = 'x';
  if ( (mode & 0x2))
    permisions[9] = 'w';
  if ( (mode & 0x4))
    permisions[8] = 'r';

  if ( (mode & 0x8) )
    permisions[6] = 'x';
  if ( (mode & 0x10) )
    permisions[5] = 'w';
  if ( (mode & 0x20) )
    permisions[4] = 'r';
  
  
  if ( (mode & 0x40))
    permisions[2] = 'x';
  if ( (mode & 0x80))
    permisions[1] = 'w';
  if ( (mode & 0x100))
    permisions[0] = 'r';

  printf("%s \n",permisions);
}

void print_inode(ext4_inode* inode){
  ext4_extent_header ext_header;
  ext4_extent ext[3];
  ext4_extent_idx ext_idx[3];


  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));// copia o header para inode
  if (ext_header.eh_depth == 0){
    // se a profundidade for zero copia em extend
    memcpy(&ext, &inode->i_block[3], sizeof(ext4_extent)*3);
  }else{
    // se a profundidade não for zero copia idx
    memcpy(&ext_idx, &inode->i_block[3], sizeof(ext4_extent_idx)*3);
  }
  printf("==========================[INODE]==========================\n");
  printf("permisoes: ");
  print_inode_permissions(inode->i_mode);
  if ((inode->i_mode & 0x1000) == 0x1000){
    printf("file type: FIFO \n");
  }else if ((inode ->i_mode & 0x2000) == 0x2000){
    printf("file type: Character device \n");
  }else if ((inode->i_mode & 0x4000) == 0x4000){
    printf("file type: Directory \n");
  }else if ((inode->i_mode & 0x6000) == 0x6000){
    printf("file type: Block device \n");
  }else if ((inode->i_mode & 0x8000) == 0x8000){
    printf("file type: Regular file \n");
  }else if ((inode->i_mode & 0xA000) == 0xA000){
    printf("file type: Symbolic link \n");
  }else if ((inode->i_mode & 0xC000) == 0xC000){
    printf("file type: Socket \n");
  }
  
  printf("block count: %d\n", inode->i_blocks_lo);
  printf("i links count: %d \n", inode->i_links_count);
  printf("flags: %X \n", inode->i_flags);
  

  printf("===================#=# Extend header #=#=================== \n");
  print_ext_header(&ext_header);
    if (ext_header.eh_depth == 0){
  printf("======================#=# Extends #=#======================\n");
      print_ext(&ext[0]);
      print_ext(&ext[1]);
      print_ext(&ext[2]);
    }else{
  printf("====================#=# Extends IDX #=#====================\n");
      print_ext_idx(&ext_idx[0]);
      print_ext_idx(&ext_idx[1]);
      print_ext_idx(&ext_idx[2]);

    }


  printf("===========================================================\n");

    printf("\n");
}

//imprime o nome dos arquivos no bloco(block)
void print_dir(int block){
  //numero do bloco para posição real no arquivo .img 
  int block_position = block * block_size; 

  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  ext4_dir_entry_2 dir; //estrutura para descrever os arquivos no diretório

  while (FILE_POS < block_last) { //imprime o conteudo enquanto não chegar ao final do bloco
    file.seekg(FILE_POS);
    file.read((char*)(&dir),  sizeof(ext4_dir_entry_2) );
    printf("(%d)[%d] %s\n",dir.inode, dir.file_type, dir.name);
    FILE_POS+=dir.rec_len;
  }
}

//procura por um arquivo de qualquer tipo com o nome(name) no bloco (block)
int find_by_name( int block, char* name){
  int inode_addr= -1;
  int block_position = block * block_size; //posição real no arquivo .img
  int block_last = block_position + block_size - 12;
  int FILE_POS = block_position;

  ext4_dir_entry_2 dir; //estrutura para descrever os arquivos no diretório

  while (FILE_POS < block_last) { // intera ate encontrar o arquivo com mesmo nome ou chegar ao final do bloco
    file.seekg(FILE_POS);
    file.read((char*)(&dir),sizeof(ext4_dir_entry_2));

    if (strncmp(name, dir.name, strlen(name)) == 0){ //compara o inicio do nome da entrada com o name 
      inode_addr = dir.inode;
      break;
    }
    
    FILE_POS+=dir.rec_len;
  }
  if (inode_addr == -1) //atribui -1 para indicar que nao foi encontrado
    printf("%s nao encontrado\n",name);
  
  return inode_addr;
}

//troca o inode, header e extends do diretório atual (current_dir, cur_header e cur_ext) pelo diretório com nome new_dir_name, caso exista.
int change_dir( char* new_dir_name, ext4_inode* current_dir, ext4_extent_header* cur_header ,ext4_extent* cur_ext){
  ext4_inode* new_directory = new ext4_inode;

  int inode_addr = find_by_name(cur_ext->ee_start_lo, new_dir_name); //verifica se existe diretório com o nome procurado 
  if (inode_addr == -1){
    printf("DIRETÓRIO NÃO ENCONTRADO!\n");
    return -1;
  }

  int FILE_POS = itable_initial_addr + (inode_addr * inode_size) - inode_size; //posicao do byte no arquivo .img
  file.seekg(FILE_POS);
  file.read((char*)(new_directory),  sizeof(ext4_inode) );
  if ((new_directory->i_mode & 0x4000) != 0x4000){//verifica se é um diretório
    printf("%s nao e um diretorio.\n",new_dir_name);
    return -1;
  }
  current_dir = new_directory;

  //troca o diretório atual pelo novo
  memcpy(cur_header, current_dir->i_block, sizeof(ext4_extent_header));
  memcpy(cur_ext, &current_dir->i_block[3], sizeof(ext4_extent)*3);
  return 0;
}

//imprime os dados do arquivo relacionado ao inode
int cat_file(ext4_inode* inode){
  if ((inode->i_mode & 0x8000) != 0x8000) return -1;
  
  ext4_extent_header ext_header;
  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));
  ext4_extent cur_ext;
  memcpy(&cur_ext, &inode->i_block[3], sizeof(ext4_extent)*3);

  ext4_extent_header f_header;
  memcpy(&f_header, inode->i_block, sizeof(ext4_extent_header));

  ext4_extent f_ext;
  memcpy(&f_ext, &inode->i_block[3], sizeof(ext4_extent));

  if (f_header.eh_depth == 0){
    for (int i = 0; i < f_ext.ee_len; i++){
      catblock( f_ext.ee_start_lo * block_size + (block_size*i) );
    }
    //verifica se o diretório utiliza o segundo extend 
    if (ext_header.eh_entries>1){
      memcpy(&f_ext, &inode->i_block[6], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        catblock( f_ext.ee_start_lo * block_size + (block_size*i) );
      }
    }
    //verifica se o diretório utiliza o terceiro extend 
    if (ext_header.eh_entries > 2){
      memcpy(&f_ext, &inode->i_block[9], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        catblock( f_ext.ee_start_lo * block_size + (block_size*i) );
      }
    }
    //verifica se o diretório utiliza o ultimo extend 
    if (ext_header.eh_entries > 3){
      memcpy(&f_ext, &inode->i_block[11], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        catblock( f_ext.ee_start_lo * block_size + (block_size*i) );
      }
    }
  }
  return 0;
}

//inicializa o super_block, root_dir, ext_header, ext
//salva o endereço inicial do inode bitmap, block bitmap, tamanho do inode, tamanho do bloco, posicao da tabela de inodes.
void init_ext4(ext4_super_block* super_block, ext4_inode* root_dir, ext4_extent_header* ext_header,  ext4_extent* ext){
  int FILE_POS;
  //abrir o .img
  file.open(img_name, fstream::in | fstream::binary);
  FILE_POS= 1024;

  file.seekg(FILE_POS);
  //SUPERBLOCK
  file.read((char*)(super_block),  sizeof(ext4_super_block) );

  int x = 10 + super_block->s_log_block_size;
  //tamanho do bloco
  block_size = pow(2, x);

  inode_size = super_block->s_inode_size;
  //total de inodes e blocos
  total_inodes = super_block->s_inodes_count;
  total_blocks = super_block->s_blocks_count_lo;

  FILE_POS=block_size;
  file.seekg(FILE_POS);
  ext4_group_desc GDT;
  file.read((char*)(&GDT),  sizeof(ext4_group_desc));

  //inode e block bitmap
  inode_bitmap_addr = GDT.bg_inode_bitmap_lo * block_size;
  block_bitmap_addr = GDT.bg_block_bitmap_lo * block_size;

  //endereço inicial da tabela de inodes
  itable_initial_addr=GDT.bg_inode_table_lo * block_size;
  FILE_POS = itable_initial_addr + inode_size;

  //diretório raiz(/)
  file.seekg(FILE_POS);
  file.read((char*)(root_dir),  sizeof(ext4_inode) );

  memcpy(ext_header, root_dir->i_block, sizeof(ext4_extent_header));
  memcpy(ext, &root_dir->i_block[3], sizeof(ext4_extent)*3);
}

//muda o current_path de acordo com o dir_name, atualiza o tamanho do caminho(path_size)
void change_pathname(char** current_path, int* path_size,char *dir_name){
  if (strcmp(dir_name,".") == 0)
    return;// caso utilize um ponto mantem no atual
  if ((strcmp(dir_name,"..") == 0)){//retira o nome do diretório atual do path
    if ( (*path_size) > 1 ){
      free(current_path[*path_size - 1]);// desaloca a estrutura no path
      (*path_size)--; //diminui em 1
    }
    
    return;
  }

  current_path[*path_size] = strdup(dir_name);// copia o nome do diretório 
  (*path_size)++;// incremente 1 o caminho relativo
}

void test_inode(int inode){
  if (inode > total_inodes){ //trata o limite superior de inodes validos
    printf("inode <%d> nao existe a acontagem de inodes vai ate %d\n",inode, total_inodes);
    return; 
  }
  if (inode < 1) return; //trata o limite inferior de inodes validos
  inode= inode-1;

  file.seekg(inode_bitmap_addr);
  char block[block_size];
  file.read(block, block_size);
  bool result=getBit(&block[inode/8],inode%8); //recebe o bit da posição determinada
  if (result){
    printf("Inode nao disponivel\n");
    return;
  }
  printf("Inode disponivel\n");
}

void test_block(int bloco){
  if (bloco > total_blocks){ //trata o limite superior de blocos validos
    printf("bloco <%d> nao existe a acontagem de blocos vai ate %d\n",bloco, total_blocks);
    return; 
  }
  if (bloco < 1) return; //trata o limite inferior de blocos validos
  bloco= bloco-1;

  file.seekg(block_bitmap_addr);
  char block[block_size];
  file.read(block, block_size);


  bool result=getBit(&block[bloco/8],bloco%8);
  if (result){
    printf("bloco nao disponivel\n");
    return;
  }
  printf("bloco disponivel\n");
}

void print_path(char** path,int path_size){
  for (int i = 0; i < path_size; i++){
    printf("%s/", path[i]);
  }
}

//exporta o inode para um arquivo export File na maquina real
int write_to_file(fstream* exportFile, ext4_inode* inode){
  ext4_extent_header ext_header;
  memcpy(&ext_header, inode->i_block, sizeof(ext4_extent_header));
  ext4_extent cur_ext;
  memcpy(&cur_ext, &inode->i_block[3], sizeof(ext4_extent)*3);

  ext4_extent_header f_header;
  memcpy(&f_header, inode->i_block, sizeof(ext4_extent_header));

  ext4_extent f_ext;
  memcpy(&f_ext, &inode->i_block[3], sizeof(ext4_extent));

  char aux[block_size];//buffer temporario para armazenar os blocos

  if (f_header.eh_depth == 0){ //verifica se utiliza extend
    for (int i = 0; i < f_ext.ee_len; i++){
      getblock(aux, f_ext.ee_start_lo * block_size + (block_size*i) );
      exportFile->write( aux, block_size );
    }
    
    if (ext_header.eh_entries>1){
      memcpy(&f_ext, &inode->i_block[6], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        getblock(aux, f_ext.ee_start_lo * block_size + (block_size*i) );
        exportFile->write( aux, block_size );
      }
    }
    if (ext_header.eh_entries > 2){
      memcpy(&f_ext, &inode->i_block[9], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        getblock(aux, f_ext.ee_start_lo * block_size + (block_size*i) );
        exportFile->write( aux, block_size );
      }
    }
    if (ext_header.eh_entries > 3){
      memcpy(&f_ext, &inode->i_block[11], sizeof(ext4_extent));
      for (int i = 0; i < f_ext.ee_len; i++){
        getblock(aux, f_ext.ee_start_lo * block_size + (block_size*i) );
        exportFile->write( aux, block_size );
      }
    }
  }
  return 0;
}

// ./main myext4.img
int main ( int argc, char *argv[] ) {
  if (argc == 2){
    strcpy(img_name,argv[1]);
  }else{
    printf("imagem não informada, utilizando a imagem padrão (%s). \n\n\n",img_name);
  }
  int FILE_POS=0;


  ext4_super_block super_block;// super block
  ext4_inode root_dir;
  ext4_extent_header root_header;
  ext4_extent root_extend[3];
  
  ext4_inode current_dir;
  ext4_extent_header current_header;
  ext4_extent current_extend[3];

  //inicializa o super bloco e o diretório raiz
  init_ext4( &super_block, &root_dir, &root_header, root_extend );
  current_dir = root_dir;
  current_header = root_header;
  current_extend[0]= root_extend[0];
  current_extend[1]= root_extend[1];
  current_extend[2]= root_extend[2];

  char* cmd[MAX_INPUT_SIZE];
  char input[MAX_INPUT_SIZE];
  int cmd_size;

  //variaveis para controlar o path de diretório atual.
  int path_size=1;
  char* current_path[50];
  current_path[0] = strdup(" ");

  while (1){
    printf("[%d]",path_size); //imprime o diretório atual
    printf("[");
    print_path( current_path,path_size );
    printf("]");    //
    cmd_size=0;
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1]='\0';
    if (strcmp(input, "exit") == 0) break;

    if (input[strlen(input)-1] == '\n'){
      input[strlen(input)-1] = '\0';
    }
    char * token = strtok(input, " "); //separa a entrada por palavras

    while( token != NULL ) {
      cmd[cmd_size] = token;
      token = strtok(NULL, " ");
      cmd_size++;
    }
    
    if (strcmp(cmd[0],"clear") == 0){
      printf("\e[1;1H\e[2J"); //limpa o terminal
    
    }else if (strcmp(cmd[0],"ls") == 0){
      print_dir(current_extend[0].ee_start_lo);
    
    }else if (strcmp(cmd[0],"cd") == 0){
      if (cmd_size <2)
        continue; //se nao tiver um diretório ignora o comando
      
      char dir_name[255];
      strcpy(dir_name,cmd[1]);
      int result = change_dir( dir_name, &current_dir, &current_header, current_extend); 
        
      if (result == 0){
        change_pathname(current_path,&path_size, dir_name);
      }
       
      
    }else if(strcmp(cmd[0],"cat") == 0){
      char* file_name = cmd[1];
      int file_addrs = find_by_name( current_extend[0].ee_start_lo, file_name);

      if (file_addrs != -1){
        FILE_POS = itable_initial_addr + (file_addrs * inode_size) - inode_size;
        
        file.seekg(FILE_POS);
        ext4_inode f;
        file.read((char*)(&f),  sizeof(ext4_inode) );
        if (cat_file(&f) == -1)
          printf("%s nao e um arquivo.\n", file_name);
        
      }
      
    }else if (strcmp(cmd[0],"testi") == 0){
      test_inode(atoi(cmd[1]));
    
    }else if (strcmp(cmd[0],"testb") == 0){
      test_block(atoi(cmd[1]));
    
    }else if(strcmp(cmd[0],"pwd") == 0){
      print_path(current_path,path_size);
      printf("\n");
    
    }else if (strcmp(cmd[0],"info") == 0){
      print_super_block(&super_block);
      
    }else if (strcmp(cmd[0],"attr") == 0){
      char* file_name = cmd[1];
      int inode_addrs = find_by_name( current_extend[0].ee_start_lo, file_name);
      if (inode_addrs != -1){
        FILE_POS = itable_initial_addr + (inode_addrs * inode_size) - inode_size; 
        file.seekg(FILE_POS);
        ext4_inode inode;
        file.read((char*)(&inode),  sizeof(ext4_inode) );
        print_inode(&inode);
      }

    }else if (strcmp(cmd[0],"export") == 0){
      if (cmd_size < 3){
        printf("informe os parametros no formato: export <source_path> <target_path>\n");
        continue;
      }
      char* file_name = cmd[1];
      char* output_name = cmd[2];
      int inode_addrs = find_by_name( current_extend[0].ee_start_lo, file_name);
      if (inode_addrs == -1)
        continue;
      
      FILE_POS = itable_initial_addr + (inode_addrs * inode_size) - inode_size; 
      file.seekg(FILE_POS);
      ext4_inode inode;
      file.read((char*)(&inode),  sizeof(ext4_inode) );
      fstream exportFile (output_name, ios::out  | ios::binary | ios::app);
      write_to_file(&exportFile, &inode);

      exportFile.close();
    }
  }
  
  file.close();
  return 0;
}