#include <stdio.h>// bibliotecas 
#include <stdlib.h>
#include <time.h>

#define N 1310720 // essaa vai ser a quantidade de valores que eu posso ter na memória
//#define K 10

// struct para gerenciar os buffers
struct arquivo{
    FILE *f;
    int pos, MAX;
    int *buffer;
};

int Comparar(const void *a, const void *b){
    if(*(int*)a == *(int*)b)
        return 0;//iguais
    else
        if(*(int*)a < *(int*)b)
            return -1; //vem antes
        else
            return 1; //vem depois
}

//procedimento responsavél por salvar o arquivo de memória 
void Salvar(char *nome, int *V, int tam, int mudaLinhaFinal){
// recebe o nome, o vetor o tamanho do vetor é uma flag que vou dizer se vou mudar de linha ou não
    int i;
    FILE *f = fopen(nome,"a");
    for(i=0; i < tam-1; i++)// percorrer o vetor 
        fprintf(f,"%d\n",V[i]);// salvando em disco
    if(mudaLinhaFinal == 0)
        fprintf(f,"%d",V[tam-1]); //controla a mudança de linha  no final do arquivo
    else
        fprintf(f,"%d\n",V[tam-1]);
    fclose(f);
}

void Arquivo_Test(char *nome){ 
    int i;
    FILE *f = fopen(nome,"w");// gerador de arquivo
    srand(time(NULL));
    for(i=1; i < 57666666; i++)  // este arquivo está gerando de 1 até 1000000 numeros apesar de nesse caso só posso carregar de acordo com tamanho do N//
        fprintf(f,"%d\n",rand());//deixa os numeros um debaixo do outro
    fprintf(f,"%d",rand());
    fclose(f);//fecha oa arquivo
}
// procedimentos para criar os arquivos ordenaados, passei o arquivo que não está ordenado
int criaArquivosOrdenados(char *nome){
    int V[N];//um vetor ed tamanho N elementos que cabem na memoria
    char novo[20];
    int cont = 0, total = 0;
    FILE *f = fopen(nome,"r");
    while(!feof(f)){//enquanto não acabar o arquivo o que eu faço
        fscanf(f,"%d",&V[total]);
        total++;
        if(total == N){// se o total de valores lidos é igual a N
            cont++;
            sprintf(novo,"Temp%d.txt",cont);// buffer cheio salva em disco
            qsort(V,total,sizeof(int),Comparar);//ordeno ele
            Salvar(novo, V, total,0);//salva o arquivo
            total = 0;//zero o total e começa um novo processo
        }
    }

    if(total > 0){// verifico o vetor se o total for maior que 0
        cont++;
        sprintf(novo,"Temp%d.txt",cont);//sobraram dados no buffer salva em disco
        qsort(V,total,sizeof(int),Comparar);
        Salvar(novo, V, total,0);
    }
    fclose(f);
    return cont;
}

//procedimento verifica buffer 
void Preencher_Buffer(struct arquivo* arq, int T){ // recebe o arquivo 
    int i;
    if(arq->f == NULL) // se o arquivo está aberto ou não se o ponteiro f for nulo significa que o arquivo já foi fechado
        return;// termina a função

    arq->pos = 0;
    arq->MAX = 0;
    for(i=0; i<T; i++){// se tamanho for k 
        if(!feof(arq->f)){ // vou ler o arquivo
            fscanf(arq->f,"%d",&arq->buffer[arq->MAX]);// lendo dados e guardando a posição Max pra frente
            arq->MAX++;
        }else{
            fclose(arq->f);
            arq->f = NULL;
            break;
        }
    }
}
// função procura menor
int Menor(struct arquivo* arq,int K,int T,int* menor){
    int i, idx = -1;
    for(i=0; i<K; i++){// vai procurar entre todos os meus arquivos vai procurar o menor elemento
        if(arq[i].pos < arq[i].MAX){// tenho um tratamento aqui marcada po pos 
            if(idx == -1)
                idx = i;
            else{
                if(arq[i].buffer[arq[i].pos] < arq[idx].buffer[arq[idx].pos])
                    idx = i;
            }
        }
    }
    if(idx != -1){// se menor valor achado é diferente de -1, atualiza a posição do buffer
        *menor = arq[idx].buffer[arq[idx].pos];
        arq[idx].pos++;
        if(arq[idx].pos == arq[idx].MAX)//sempre que idx posição for igual o valor de max, significa que cheguei na minha posição buffer
            Preencher_Buffer(&arq[idx],T);// prenncher o buffer
        return 1;
    }else
        return 0;

}

void Merge_Sort(char *nome, int K, int T){
    char novo[20];
    int i;
    int *buffer = (int*)malloc(T*sizeof(int));// buffer de saida

    struct arquivo* arq;
    arq = (struct arquivo*)malloc(K*sizeof(struct arquivo));// vetor de struct saida
    for(i=0; i < K; i++){
        sprintf(novo,"Temp%d.txt",i+1);//esipulei o nome dele
        arq[i].f = fopen(novo,"r");// abrindo arquivo
        arq[i].MAX = 0;// defini o max igual a zero
        arq[i].pos = 0;//pos igual a zero
        arq[i].buffer = (int*)malloc(T*sizeof(int)); //definir que o valor dele de tamanho K
        Preencher_Buffer(&arq[i],T);// onde preenche buffer 
    }

    //enquanto houver arquivos para processar
    int menor, qtdBuffer = 0; // função procura menor
    while(Menor(arq,K,T,&menor) == 1){// dentro dos meus arquivos abertos partes deles carregados na memoria
        buffer[qtdBuffer] = menor;//guardo a proxima posição 
        qtdBuffer++;
        if(qtdBuffer == T){// se quantidade do buffer for igual a K o buffer de saida está cheio 
            Salvar(nome, buffer, T,1);//salva em arquivo no buffer de saida 
            qtdBuffer = 0; //e zera ele pra começar e encher novamente 
        }
    }

    if(qtdBuffer != 0)// verificando se há algo no buffer de saída 
        Salvar(nome, buffer, qtdBuffer,1);// salvo esse restante  mo meu buffer de saída 

    for(i=0; i<K; i++)
        free(arq[i].buffer);// e libero todos os meus buffer de entrada 
    free(arq);
    free(buffer);

}

void Merge_Sort_Externo(char *nome){ // procedimento merge sort externo recebe um arquivo
    char novo[20];
    int K = criaArquivosOrdenados(nome);// chama a função criarArquivosOrdenados
    int i, T = N / (K + 1);// aqui faço o calculo dos meus buffer que no final vai o tamanho do meu K
    printf("Nro de arquivos:%d\n",K);
    printf("K:%d\n",T);

    remove(nome);// apago o arquivo original
    Merge_Sort(nome,K,T);//ordenar o arquivo original onde cada um vai ter K elementos
    //segue abaixo onde eu apago todos os arquivos que gerei
    for(i=0; i<K; i++){
        sprintf(novo,"Temp%d.txt",i+1);
        remove(novo);
    }
    printf("Fim!\n");

}
//procedimento de verificação
void Verificar(char *nome){
    int v1,v2, erro = 0;
    FILE *f = fopen(nome,"r");// abri o arquivo
    fscanf(f,"%d",&v1);
    while(!feof(f)){
        fscanf(f,"%d",&v2);
        if (v2 < v1){
            erro = 1;
            break;
        }
    }
    fclose(f);
    if(erro)
        printf("Valores fora de ordem!\n");
    else
        printf("Arquivo corretamente ordenado!\n");
}
