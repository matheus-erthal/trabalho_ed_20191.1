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

// função que ajeita o vetor de clientes para inserção
void _adapta_vetor(TPizza** clientes, int inicio, int m){
    int i;
    TPizza* escolhido = NULL;
    TPizza* temp;
    for(i = inicio; i <= m; i++){
        temp = clientes[i];
        clientes[i] = escolhido;
        escolhido = temp;
    }
    return;
}

int insere(int cod, char *nome, char *categoria, float preco, char *nome_arquivo_metadados, char *nome_arquivo_indice, char *nome_arquivo_dados, int d)
{   
    // busca a posição onde o dado deve ser inserido
	int posicao = busca(cod, nome_arquivo_metadados, nome_arquivo_indice, nome_arquivo_dados, d);
    // abre o arquivo de dados
    FILE* arq_dados = fopen(nome_arquivo_dados, "rb+");
    // vai para a posição onde o dado deve ser inserido
    fseek(arq_dados, posicao, SEEK_SET);
    // lê a página da memória
    TNoFolha* folha = le_no_folha(d, arq_dados);
    // cria o novo registro
    TPizza* nova_pizza = pizza(cod, nome, categoria, preco);
    // se a página não estiver cheia
    if(folha->m < 2 * d){
        int i;
        // descobre a posição que o novo registro deve ser inserido dentro da página
        for(i = 0; i < folha->m && nova_pizza->cod >= folha->clientes[i]->cod; i++){
            // se o registro já existe, retorna -1
            if(folha->clientes[i]->cod == nova_pizza->cod) return -1;
        }
        // ajeita o vetor para receber o registro na posição correta
        _adapta_vetor(folha->clientes, i, folha->m);
        // atribui o novo registro a posição
        folha->clientes[i] = nova_pizza;
        // atualiza o valor de m da página
        folha->m++;
        // volta para a posição inicial da página no arquivo
        fseek(arq_dados, posicao, SEEK_SET);
        // salva a página no arquivo
        salva_no_folha(d, folha, arq_dados);
        // fecha o arquivo
        fclose(arq_dados);
        // retorna a posição da página
        return posicao;
    // se a página estiver cheia
    }else{
        printf("%d->", -1);
        fclose(arq_dados);
        return INT_MAX;
    }
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