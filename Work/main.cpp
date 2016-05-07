#include "Merge_Sort.h"

int main()
{
    int inicio,fim,resultado;
    inicio = clock();
    Arquivo_Test("dados.txt");// aqui estou gerando meus arquivos 
    Merge_Sort_Externo("dados.txt");//aqui estou chamando minha função mergesortesxterno que vai ordenar o arquivo
    Verificar("dados.txt");
    fim = clock();
    resultado = fim-inicio;
    printf("%d",resultado);
    system("pause>nul");
    return 0;
}
