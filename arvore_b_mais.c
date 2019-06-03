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

    // se o noh raiz ja for uma folha
    if(meta->raiz_folha){ 
        return pont_raiz;
    }else{
        // declara folha como -1(nulo)
        int folha = -1; 
        int no_atual = pont_raiz;
        FILE* arq_indices = fopen(nome_arquivo_indice, "rb");
        // enquanto nao apontarmos para uma folha
        while(folha == -1){ 
            // posiciona o ponteiro do arquivo de indices no noh atual
            fseek(arq_indices, no_atual, 0); 
            TNoInterno* pagina = le_no_interno(d, arq_indices);
            int i;
            // descobre onde deve descer para a posicao onde o elemento buscado deveria estar
            for(i = 0; cod >= pagina->chaves[i] && i < pagina->m; i++); 
            // se essa proxima pagina for uma folha, muda o valor de folha e sai do loop
            if(pagina->aponta_folha){
                folha = pagina->p[i];
            }else{ // senao, atualiza no atual
                no_atual = pagina->p[i];
            }
        }
        // fecha o arquivo e retorna folha
        fclose(arq_indices);
        return folha;
    }
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