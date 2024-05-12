#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define SIZE 5570


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



void ler_novo(FILE *f, municipio *m){
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




//Busca pelo município na array dado nome;
municipio *search_array_nome(municipio *v, char *nome){
  int i;

  for (i = 0; i < SIZE; i++){
    if (strcmp(v[i].nome, nome) == 0){
      return &v[i];
    }
  }
  return NULL;
}


//Busca por município na array dado código IBGE;
municipio *search_array_cod(municipio *v, int codigo_ibge){
  int i;

  for (i = 0; i < SIZE; i++){
    if (v[i].cod_ibge == codigo_ibge){
      return &v[i];
    }
  }
  return NULL;
}



//Distância euclidiana entre dois pontos;
double distance(municipio a, municipio b) {
  return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}


//Funções de comparação para usar na qsort;
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



//Constrói uma árvore k-d dado um vetor de municípios de tamanho n;
node *build_kdtree(municipio *v, int n, int lvl){
  int axis;
  int median;
  node *novo;
  
  if (n <= 0){
    return NULL;
  }

  //Determinação do eixo de divisão;
  axis = lvl % 2;

  
  if (axis == 0){
    qsort(v, n, sizeof(municipio), cmp_x);  //Elementos ordenados por longitude;
  } else {
    qsort(v, n, sizeof(municipio), cmp_y);  //Elementos ordenados por latitude;
  }

  //Índice do elemento mediano após ordenações;
  median = (n / 2);

  //Criação de novo node para o mediano;
  novo = (node *) malloc (sizeof(node));
  novo->m = v[median];
  novo->left = NULL;
  novo->right = NULL;

  //Constrói o ramo esquerdo com os elementos à esquerda do mediano no próximo nível;
  novo->left = build_kdtree(v, median, (lvl + 1));
  //Constrói o ramo direito com os elementos à direita do mediano no próximo nível;
  novo->right = build_kdtree((v + median + 1), (n - median - 1), (lvl + 1));

  return novo;
}


/*
  nns = nearest neighbor search

  Busca pelo município nearest, de menor distância em relação ao município target;
  A função desce recursivamente a árvore calculando e atualizando as menores distâncias;

  O parâmetro limit serve pra controlar a atualização do nearest. Se limit valer 100, a função
  vai encontrar o município mais próximo do target cuja distância seja maior que 100;
  Se a função for chamada com limit 0, vai atribuir a nearest o município mais próximo do target;
*/

void nns(node *root, municipio target, municipio *nearest, double *best, double *limit, int depth){
  int axis;
  double dist;
 

  //Caso base para terminar as chamadas recursivas em uma folha;
  if (root == NULL){
    return;
  }


  /*
    Quando a função for chamada sendo root = target, a distância vai resultar em 0.
    Sem essa condicional, a função atribui o próprio target como o município mais próximo
    uma vez que nenhum outro município vai ter uma distância menor que 0;
  */
  if (root->m.x != target.x || root->m.y != target.y){
    dist = distance(root->m, target);
    if (dist < *best && dist > *limit){
      *best = dist;
      *nearest = root->m;
      //Atualização do nearest caso encontrada uma distância melhor que a atual;
    }
  }

  //Determinação do eixo, axis vai sempre variar entre 0 e 1;
  axis = depth % 2;


  /*
    Determinação do próximo ramo da árvore a ser explorado a partir
    das coordenadas do nodo atual e do target num dado eixo;

    Se o eixo for 0, comparam-se as longitudes;
    Se o eixo for 1, comparam-se as latitudes;

    Caso 1: uma longitude menor do target em relação ao nodo atual
    leva a busca para o ramo esquerdo do nodo atual;
    Exemplo: atual = (20, y1), target = (10, y2);
    
    Caso 2: uma longitude maior do target em relação ao nodo atual
    leva a busca para o ramo direito do nodo atual;
    Exemplo: atual = (20, y1), target = (30, y2);

    Caso 3: uma latitude menor do target em relação ao nodo atual
    leva a busca para o ramo esquerdo do nodo atual;
    Exemplo: atual = (x1, 20), target = (x2, 10);

    Caso 4: uma latitude maior do target em relação ao nodo atual
    leva a busca para o ramo direito do nodo atual;
    Exemplo: atual = (x1, 20), target = (x2, 30);

    Se as coordenadas comparadas forem iguais mas os pontos forem
    diferentes, o outro eixo é usado para determinar o próximo ramo;

    Quando as chamadas chegam nas folhas, a recursão começa a voltar.
    Nas voltas, é preciso checar se a melhor distância encontrada não
    é maior do que a distância entre o ponto atual e o target no dado eixo;

    O ponto target e a distância best determinam uma circunferência de raio
    best com centro no target. O eixo e nodos atuais determinam um eixo
    de divisão no plano x-y. Se esse eixo cortar a circunferência, é preciso
    checar o outro lado do eixo de divisão pois podem existir pontos
    no outro lado mais próximos do target que não foram checados.

    Exemplo: se axis = 0 e ponto atual = (12, 20), o eixo de divisão é (x = 12);
    se target = (10, 10), a busca continua no ramo esquerdo da árvore onde
    todos os pontos estão à esquerda do ponto atual

    se nessa busca o ponto mais próximo encontrado for nearest = (7, 12),
    a melhor distância será de aproximadamente 3.60
    sendo que a distância do target pro eixo de divisão é |(10 - 12)| = 2
    a circunferência vai ter equação = (x - 10)² + (y - 10)² = 3.60²
    essa circunferência vai ser interceptada pelo eixo de divisão (x = 12)
    nos pontos (12, 13) e (12, 7)
    indicando que existe uma região da circunferência do outro
    lado do eixo de divisão
    é preciso checar então o ramo direito do ponto atual em busca
    de possíveis pontos melhores que o nearest atual
    se, por exemplo, existir o ponto (13, 11), no ramo
    direito de (12, 20), a distância será de aproximadamente 3.16
    até o target, menor que o best

    //https://www.desmos.com/calculator/m74xevhcdr
    
    
   */
  
  if (axis == 0){
    if (target.x < root->m.x || (target.x == root->m.x && target.y < root->m.y)){
      nns(root->left, target, nearest, best, limit, depth+1);  //Caso 1
      if (abs(target.x - root->m.x) < *best){
	nns(root->right, target, nearest, best, limit, depth+1);
      }
    } else {
      nns(root->right, target, nearest, best, limit, depth+1); //Caso 2
      if (abs(target.x - root->m.x) < *best){
	nns(root->left, target, nearest, best, limit, depth+1);
      }
    }
  } else {
    if (target.y < root->m.y || (target.y == root->m.y && target.x < root->m.x)){
      nns(root->left, target, nearest, best, limit, depth+1);  //Caso 3
      if (abs(target.y - root->m.y) < *best){
	nns(root->right, target, nearest, best, limit, depth+1);
      }
    } else {
      nns(root->right, target, nearest, best, limit, depth+1);  //Caso 4
      if (abs(target.y - root->m.y) < *best){
	nns(root->left, target, nearest, best, limit, depth+1);
      }
    }    
  }
  

  return;
}



/*
  Função para encontrar os K municípios mais próximos de target usando nns() em loop
  
  Parâmetros:
  root = ponteiro para a raíz da árvore k-d
  target = município do qual se buscam os mais próximos
  k_proximos = array com K municípios mais próximos de target
  K = quantos se buscam

  Essa função chama sequencialmente nns() usando limit como o best do anterior;

  A primeira chamada é com limit = 0, então a função nns() encontra
  o primeiro município mais próximo de target, à uma distância d1.
  Então, o loop atualiza limit com d1 e chama novamente a função nns(),
  dessa vez encontrando o município mais próximo de target cuja distância
  seja maior que d1. Esse é o segundo município mais próximo de target,
  com distância d2. O loop atualiza limit para d2 e chama novamente
  nns() para encontrar o terceiro município mais próximo de target,
  que é o município mais próximo de target cuja distância é maior que
  d2. O loop continua até que os K municípios mais próximos sejam encontrados;
*/

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



int height(node *root){
  int left;
  int right;
  
  if (root == NULL){
    return 0;
  } else {
    left = height(root->left);
    right = height(root->right);

    if (left > right){
      return (left + 1);
    } else {
      return (right + 1);
    }
  }
}



int main(){
  municipio v[SIZE];
  node *T;
  int i;
  FILE *f_municipios;
  municipio target;
  municipio *target_pointer;
  municipio *proximos;
  int K;
  int cod;

  f_municipios = fopen("municipios.json", "r");
  
  srand(time(NULL));
  for (i = 0; i < SIZE; i++){
    ler_novo(f_municipios, &v[i]);
  }
  
  T = NULL;
  T = build_kdtree(v, SIZE, 0);

  
  system("clear");
  printf("Municipio: ");
  scanf("%d", &cod);
  target_pointer = search_array_cod(v, cod);
  if (target_pointer == NULL){
    printf("Município não encontrado\n");
  } else {
    target = *target_pointer;
  }
  

  printf("K = ");
  scanf("%d", &K);
  proximos = calloc (K, sizeof(municipio));
  k_nns(T, target, proximos, K);

  printf("\n\nOs %d municípios mais próximos de %s são:\n\n", K, target.nome);
  for (i = 0; i < K; i++){
    printf("[%d] %s\n", i, proximos[i].nome);
  }
  


  return 0;
}
