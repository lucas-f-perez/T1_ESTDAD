#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 7963
#define N_MUNICIPIOS 5570
#define MENOR_COD 1100015
#define MAIOR_COD 5300108


/*
  A tarefa pede o armazenamento de 5570 elementos em uma tabela hash.
  Para que os 5570 elementos ocupem 70% da tabela, seu tamanho deve ser 7958.
  O número 7963 é o primeiro número primo após 7958, então ele vai ser usado como SIZE para a tabela,
  como recomendado no livro do Sedgewick.
*/



typedef struct m {
  int cod_ibge;
  char nome[33];
  double x;
  double y;
  int capital;
  int cod_uf;
  int id_siafi;
  int DDD;
  char fuso[33];
} municipio;
//x = longitude
//y = latitude



//Impressão dos dados de um município.
void print_municipio(municipio *p){
  printf("Nome: %s\n", p->nome);
  printf("Código IBGE: %d\n", p->cod_ibge);
  printf("Longitude: %.4f\n", p->x);
  printf("Latitude: %.4f\n", p->y);
  printf("Estado: %d\n", p->cod_uf);
  printf("ID Siafi: %d\n", p->id_siafi);
  printf("DDD: %d\n", p->DDD);
  printf("Fuso horário: %s\n", p->fuso);

  printf("Capital: ");
  if (p->capital == 1){
    printf("Sim\n");
  } else {
    printf("Não\n");
  }
}


municipio *ler_novo(FILE *f){
  municipio *novo;
  char buffer[40];
  char c;

  novo = malloc (sizeof(municipio));

  if (novo != NULL){
    fscanf(f, " %[^0-9]", buffer);
    fscanf(f, "%d", &novo->cod_ibge);

    fscanf(f, " %[^:]", buffer);
    fscanf(f, " %[^\"]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, " %[^\"]", novo->nome);
    fscanf(f, " %[^\n]", buffer);

    fscanf(f, " %[^ ]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%f", &novo->y);

    fscanf(f, " %[^:]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%f", &novo->x);
 
    fscanf(f, " %[^:]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%d", &novo->capital);
  
    fscanf(f, " %[^:]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%d", &novo->cod_uf);

    fscanf(f, " %[^:]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%d", &novo->id_siafi);

    fscanf(f, " %[^:]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, "%d", &novo->DDD);

    fscanf(f, " %[^:]", buffer);
    fscanf(f, " %[^\"]", buffer);
    fscanf(f, "%c", &c);
    fscanf(f, " %[^\"]", novo->fuso);
    fscanf(f, " %[^\n]", buffer);


    return novo;
  }
}


//Hash duplo com hash modular, págs. 594/595 Sedgewick
int h1(int key){
  return key % SIZE;
}

int h2(int key){
  return (key % SIZE) + 1;
}

int double_hash(int key, int i){
  return (h1(key) + (i * h2(key))) % SIZE;
}



//Inserção de um novo município n na tabela T.
void insert_node(municipio *T[], municipio *n){
  int pos;
  int i;
  int count;

  i = 0;
  count = 0;
  pos = double_hash(n->cod_ibge, i);
  
  while (T[pos] != NULL){
    if (count != SIZE){
      count++;
      i++;
      pos = double_hash(n->cod_ibge, i);
    } else {
      return;
    }
  }
  T[pos] = n;
  
  return;
}



//Busca de um município na tabela T dado seu código_ibge
municipio *search(municipio *T[], int key){
  int pos;
  int i;
  int count;


  i = 0;
  count = 0;
  pos = double_hash(key, i);
  while (T[pos] == NULL || T[pos]->cod_ibge != key){
    if (count != SIZE){
      i++;
      count++;
      pos = double_hash(key, i);
    } else {
      return NULL;
    }
  }

  return T[pos];
}





int main(){
  FILE *f;
  municipio *table[SIZE] = {NULL};
  municipio *novo;
  municipio *m;
  int i;
  int pos;
  int key;

  f = fopen("municipios.json", "r");

  for (i = 0; i < N_MUNICIPIOS; i++){
    novo = ler_novo(f);
    insert_node(table, novo);
  }

  
  printf("Buscar município.\n");
  printf("Código IBGE: ");
  scanf("%d", &key);
  m = search(table, key);

  if (m != NULL){
    print_municipio(m);
  } else {
    printf("Não encontrado\n");
  }
  
  
  return 0;
}
