#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdio.h>

#include "arvore_b_mais.h"
#include "lista_pizzas.h"
#include "metadados.h"
#include "no_folha.h"
#include "no_interno.h"
#include "pizza.h"


int busca(int cod, char *nome_arquivo_metadados, char *nome_arquivo_indice, char *nome_arquivo_dados, int d)
{
    TMetadados* meta = le_arq_metadados(nome_arquivo_metadados);
    int pont_raiz = meta->pont_raiz;

    // maior ou igual - direita
    // menor - esquerda

    if(meta->raiz_folha){
        // FILE* arq_folhas = fopen(nome_arquivo_dados, "rb");
        // fseek(arq_folhas, meta->pont_raiz, 0);
        // TNoFolha* raiz = le_no_folha(meta->d, arq_folhas);
        return pont_raiz;
    }else{
        int folha = -1;
        int no_atual = pont_raiz;
        FILE* arq_indices = fopen(nome_arquivo_indice, "rb");
        while(folha == -1){
            TNoInterno* pagina = le_no_interno(d, arq_indices);
            int i;
            for(i = 0; cod >= pagina->chaves[i] && i <= pagina->m; i++);
            if(pagina->aponta_folha){
                folha = pagina->p[i];
            }else{
                no_atual = pagina->p[i];
            }
        }
        fclose(arq_indices);
        return folha;
    }
    return INT_MAX;
}

int insere(int cod, char *nome, char *descricao, float preco, char *nome_arquivo_metadados, char *nome_arquivo_indice, char *nome_arquivo_dados, int d)
{
	//TODO: Inserir aqui o codigo do algoritmo de insercao
    return INT_MAX;
}

int exclui(int cod, char *nome_arquivo_metadados, char *nome_arquivo_indice, char *nome_arquivo_dados, int d)
{
	//TODO: Inserir aqui o codigo do algoritmo de remocao
    return INT_MAX;
}

void carrega_dados(int d, char *nome_arquivo_entrada, char *nome_arquivo_metadados, char *nome_arquivo_indice, char *nome_arquivo_dados)
{
    //TODO: Implementar essa funcao
}