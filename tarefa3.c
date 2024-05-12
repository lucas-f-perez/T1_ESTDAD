#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define SIZE 7963
#define N_MUNICIPIOS 5570


typedef struct municipio {
  int cod_ibge;
  char nome[33];
  float x;
  float y;
  int capital;
  int cod_uf;
  int id_siafi;
  int DDD;
  char fuso[33];
} municipio;

typedef struct node {
  municipio m;
  struct node* left;
  struct node* right;
} node;



void print_municipio(municipio* p){
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



municipio *ler_novo1(FILE *f){
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


void ler_novo2(FILE *f, municipio *m){
  char buffer[40];
  char c;


  fscanf(f, " %[^0-9]", buffer);
  fscanf(f, "%d", &m->cod_ibge);

  fscanf(f, " %[^:]", buffer);
  fscanf(f, " %[^\"]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, " %[^\"]", m->nome);
  fscanf(f, " %[^\n]", buffer);

  fscanf(f, " %[^ ]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%f", &m->y);

  fscanf(f, " %[^:]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%f", &m->x);
 
  fscanf(f, " %[^:]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%d", &m->capital);
  
  fscanf(f, " %[^:]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%d", &m->cod_uf);

  fscanf(f, " %[^:]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%d", &m->id_siafi);

  fscanf(f, " %[^:]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, "%d", &m->DDD);

  fscanf(f, " %[^:]", buffer);
  fscanf(f, " %[^\"]", buffer);
  fscanf(f, "%c", &c);
  fscanf(f, " %[^\"]", m->fuso);
  fscanf(f, " %[^\n]", buffer);
}




unsigned long hash_str(char *str){
  unsigned long hash = 5381;
  int c;

  while ((c = *str++)){
    hash = ((hash << 5) + hash) + c;
  }

  return hash % SIZE;
}



int h1(int key){
  return key % SIZE;
}

int h2(int key){
  return (key % SIZE) + 1;
}

int double_hash(int key, int i){
  return (h1(key) + (i * h2(key))) % SIZE;
}


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


void insert_node2(municipio *T[], municipio *n){
  int pos;
  int i;
  int count;
  unsigned long hash;

  i = 0;
  count = 0;
  hash = hash_str(n->nome);
  pos = double_hash(hash, i);
  
  while (T[pos] != NULL){
    if (count != SIZE){
      count++;
      i++;
      pos = double_hash(hash, i);
    } else {
      return;
    }
  }
  T[pos] = n;
  
  return;
}


municipio *search_table_cod(municipio *T[], int key){
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


int search_table_nome(municipio *T[], char *nome){
  int pos;
  int i, j;
  unsigned long hash;
  int n;
  municipio *results[10];
  int cod;

  pos = 0;
  i = 0;
  n = 0;
  hash = hash_str(nome);
  while (T[pos]!= NULL) {
    if (strcmp(T[pos]->nome, nome) == 0) {
      //printf("Found match: %s [%d]\n", T[pos]->nome, T[pos]->cod_ibge);
      results[n] = T[pos];
      n++;
    }
    pos = double_hash(hash, i++);
  }

  if (n > 1){
    printf("\nEncontrados %d resultados\n\n", n);
    for (j = 0; j < n; j++){
      printf("%s [%d]\n", results[j]->nome, results[j]->cod_ibge);
    }
    printf("\n\nCódigo IBGE: ");
    scanf("%d", &cod);
    return cod;
  }
    
  return results[0]->cod_ibge;
}





municipio *search_array_nome(municipio *v, char *nome){
  int i;

  for (i = 0; i < SIZE; i++){
    if (strcmp(v[i].nome, nome) == 0){
      return &v[i];
    }
  }
  return NULL;
}


municipio *search_array_cod(municipio *v, int codigo_ibge){
  int i;

  for (i = 0; i < SIZE; i++){
    if (v[i].cod_ibge == codigo_ibge){
      return &v[i];
    }
  }
  return NULL;
}


double distance(municipio a, municipio b) {
  return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
int cmp_x(const void *a, const void *b){
  const municipio *p1 = (municipio *) a;
  const municipio *p2 = (municipio *) b;
  
  if (p1->x < p2->x){
    return -1;
  } else if (p1->x == p2->x){
    return 0;
  } else {
    return 1;
  }
}

int cmp_y(const void *a, const void *b){
  const municipio *p1 = (municipio *) a;
  const municipio *p2 = (municipio *) b;
  
  if (p1->y < p2->y){
    return -1;
  } else if (p1->y == p2->y){
    return 0;
  } else {
    return 1;
  }
}


node *build_kdtree(municipio *v, int n, int lvl){
  int axis;
  int median;
  node *novo;
  
  if (n <= 0){
    return NULL;
  }

  axis = lvl % 2;

  
  if (axis == 0){
    qsort(v, n, sizeof(municipio), cmp_x);
  } else {
    qsort(v, n, sizeof(municipio), cmp_y);
  }

  median = (n / 2);

  novo = (node *) malloc (sizeof(node));
  novo->m = v[median];
  novo->left = NULL;
  novo->right = NULL;

  novo->left = build_kdtree(v, median, (lvl + 1));
  novo->right = build_kdtree((v + median + 1), (n - median - 1), (lvl + 1));

  return novo;
}


void nns(node *root, municipio target, municipio *nearest, double *best, double *limit, int depth){
  int axis;
  double dist;
 
  if (root == NULL){
    return;
  }

  if (root->m.x != target.x || root->m.y != target.y){
    dist = distance(root->m, target);
    if (dist < *best && dist > *limit){
      *best = dist;
      *nearest = root->m;
    }
  }

  axis = depth % 2;
  
  if (axis == 0){
    if (target.x < root->m.x || (target.x == root->m.x && target.y < root->m.y)){
      nns(root->left, target, nearest, best, limit, depth+1);
      if (abs(target.x - root->m.x) < *best){
	nns(root->right, target, nearest, best, limit, depth+1);
      }
    } else {
      nns(root->right, target, nearest, best, limit, depth+1);
      if (abs(target.x - root->m.x) < *best){
	nns(root->left, target, nearest, best, limit, depth+1);
      }
    }
  } else {
    if (target.y < root->m.y || (target.y == root->m.y && target.x < root->m.x)){
      nns(root->left, target, nearest, best, limit, depth+1);
      if (abs(target.y - root->m.y) < *best){
	nns(root->right, target, nearest, best, limit, depth+1);
      }
    } else {
      nns(root->right, target, nearest, best, limit, depth+1);
      if (abs(target.y - root->m.y) < *best){
	nns(root->left, target, nearest, best, limit, depth+1);
      }
    }    
  }
  

  return;
}


void k_nns(node *root, municipio target, municipio *k_proximos, int K){
  double best;
  double limit;
  int i;

  best = 100000;
  limit = 0;
  for (i = 0; i < K; i++){
    best = 100000;
    nns(root, target, &k_proximos[i], &best, &limit, 0);
    limit = best;
  };
}





int main(){
  FILE *f;
  municipio *TABLE1[SIZE] = {NULL};
  municipio *TABLE2[SIZE] = {NULL};
  municipio v[SIZE];
  node *T = NULL;
  municipio *novo;
  municipio *m;
  municipio target;
  municipio *target_pointer;
  municipio *proximos;
  char nome[40];
  int cod;
  int i;
  int key;
  int K;


  f = fopen("municipios.json", "r");
  for (i = 0; i < N_MUNICIPIOS; i++){
    novo = ler_novo1(f);
    insert_node(TABLE1, novo);
  }
  fclose(f);

  f = fopen("municipios.json", "r");
  for (i = 0; i < N_MUNICIPIOS; i++){
    ler_novo2(f, &v[i]);
  }
  fclose(f);

  f = fopen("municipios.json", "r");
  for (i = 0; i < N_MUNICIPIOS; i++){
    novo = ler_novo1(f);
    insert_node2(TABLE2, novo);
  }

  
  
  T = build_kdtree(v, SIZE, 0);

    
  system("clear");
  printf("Municipio: ");
  scanf(" %[^\n]", nome);
  cod = search_table_nome(TABLE2, nome);
  target_pointer = search_array_cod(v, cod);
  if (target_pointer == NULL){
    printf("Município não encontrado\n");
  } else {
    target = *target_pointer;
  }
  
  
  system("clear");
  printf("Buscar K municípios mais próximos de %s\n", target.nome);
  printf("K = ");
  scanf("%d", &K);
  proximos = calloc (K, sizeof(municipio));
  k_nns(T, target, proximos, K);

  printf("\nOs %d municípios mais próximos de %s são:\n\n", K, target.nome);
  for (i = 0; i < K; i++){
    printf("[%d] ", i);
    print_municipio(&proximos[i]);
    printf("\n\n");
  }

  

  return 0;
}



