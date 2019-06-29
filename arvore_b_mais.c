#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

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

// função que ajeita o vetor de clientes para inserção sem partição
void _adapta_vetor_sem_particao(TPizza** clientes, int inicio, int m){
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

// função que particiona a folha passada e retorna a nova folha
TNoFolha* _particiona_folha(TNoFolha* p, int d, TPizza* nova_pizza){
    TNoFolha* q = no_folha(d, d+1, p->pont_pai, p->pont_prox);
    TPizza **temp = (TPizza**) malloc(sizeof(TPizza*) * (2 * d + 1));
    int i;
    int posicionado = 0;
    for(i = 0; i < 2 * d + 1; i++){
        if(posicionado){
            temp[i] = p->clientes[i-1];
        }else{
            if(nova_pizza->cod < p->clientes[i]->cod){
                temp[i] = nova_pizza;
                posicionado = 1;
            }else{
                temp[i] = p->clientes[i];
            }
        }
    }
    for(i = 0; i < 2 * d + 1; i++){
        if(i < d){
            p->clientes[i] = temp[i];
        }else{
            q->clientes[i-d] = temp[i];
            if(i < 2 * d){
                p->clientes[i] = NULL;
            }
        }
    }
    free(temp);
    p->m = d; 
    return q;
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
    int i, j;
    // se a página não estiver cheia
    if(folha->m < 2 * d){
        // descobre a posição que o novo registro deve ser inserido dentro da página
        for(i = 0; i < folha->m && nova_pizza->cod >= folha->clientes[i]->cod; i++){
            // se o registro já existe, retorna -1
            if(folha->clientes[i]->cod == nova_pizza->cod) return -1;
        }
        // ajeita o vetor para receber o registro na posição correta
        _adapta_vetor_sem_particao(folha->clientes, i, folha->m);
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
        TNoFolha* nova_folha = _particiona_folha(folha, d, nova_pizza);
        fseek(arq_dados, 0L, SEEK_END);
        int posicao_nova_folha = ftell(arq_dados);
        nova_folha->pont_prox = folha->pont_prox;
        folha->pont_prox = posicao_nova_folha;
        fseek(arq_dados, posicao, SEEK_SET);
        salva_no_folha(d, folha, arq_dados);
        int escolhido = nova_folha->clientes[0]->cod;
        FILE* arq_indice = fopen(nome_arquivo_indice, "rb+");
        if(folha->pont_pai != -1){
            fseek(arq_indice, folha->pont_pai, SEEK_SET);
            TNoInterno* pai = le_no_interno(d, arq_indice);

            if(pai->m < 2 * d){
                for(i = 0; i < pai->m && pai->chaves[i] < escolhido; i++);
                for(j = pai->m; j >= i; j--){
                    pai->chaves[j] = pai->chaves[j - 1];
                    pai->p[j+1] = pai->p[j];
                }
                pai->chaves[i] = escolhido;
                pai->m++;
                pai->p[i+1] = posicao_nova_folha;
                nova_folha->pont_pai = folha->pont_pai;
                printf("folha_antiga=%d", posicao);
                printf("nova_folha=%d", posicao_nova_folha);
                printf("no_interno=%d", folha->pont_pai);
                fseek(arq_dados, posicao_nova_folha, SEEK_SET);
                salva_no_folha(d, nova_folha, arq_dados);
                fseek(arq_indice, folha->pont_pai, SEEK_SET);
                salva_no_interno(d, pai, arq_indice);

            }
            // printf("codigo a inserir: %d\n", cod);
            // printf("Folha 1:\n");
            // imprime_no_folha(d, folha);
            // printf("Folha 2:\n");
            // imprime_no_folha(d, nova_folha);
            // printf("Nó interno:\n");
            // imprime_no_interno(d, pai);
            fclose(arq_indice);
            fclose(arq_dados);
            return posicao;
        }else{
            printf("sem pai->");
        }
        // imprime_no_folha(d, folha);
        // imprime_no_folha(d, nova_folha);

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

// void main() {
//     printf("*********** Menu de Operações da Pizzaria ***********\n");
//     printf("Escolha uma das opções a seguir: \n");
//     printf("(1) Inserir uma pizza no cardápio;\n");
//     printf("(2) Remover uma pizza no cardápio;\n");
//     printf("(3) Buscar informações de uma pizza;\n");
//     printf("(4) Alterar informações de uma pizza;\n");
//     printf("(5) Buscar pizzas por categoria;\n");
//     printf("(6) Remover todas as pizzas por categoria;\n");
//     printf("(7) Sair do menu;\n");    
//     printf("*****************************************************\n");

//     int valor;
//     scanf("%d", &valor);

//     char arqMetadados[100] = "metadados.dat";
//     char arqIndice[100] = "indice.dat";
//     char arqDados[100] = "clientes.dat";

//     printf("Informe o valor da ordem da árvore: ");
//     int ordem;
//     scanf("%d", &ordem);
//     printf("\n");

//     switch(valor) {
//         case 1: 
//             printf("Operação: Inserir pizza no cardápio\n");
//             printf("Informe o código da pizza: ");
//             int codigo;
//             scanf("%d", &codigo);
//             printf("\n");

//             printf("Informe o nome da pizza: ");
//             char nomePizza[50];
//             scanf("%s", nomePizza);
//             printf("\n");

//             printf("Informe a categoria da pizza (Salgada, Doce ou Especial): ");
//             char categoria[20];
//             scanf("%s", categoria);
//             printf("\n");

//             printf("Informe o valor da pizza: ");
//             double preco;
//             scanf("%lf", &preco);
//             printf("\n");

//             //insere(codigo, nomePizza, categoria, preco, arqMetadados, arqIndice, arqDados, ordem);

//             break;
//         case 2:
//             printf("Operação: Remover uma pizza do cardápio\n");
//             printf("Infelizmente ainda não é possível remover itens do cardápio\n");
//             break;
//         case 3:
//             printf("Operação: Buscar informações de uma pizza\n");
//             printf("Informe o código da pizza procura: ");
//             int codigoBusca;
//             scanf("%d", codigoBusca);

//             busca(codigoBusca, arqMetadados, arqIndice, arqDados, ordem);
//             break;
//         case 4: 
//             //operação de editar infos; Só pode editar nome, categoria e preço;
//             break;
//         case 5:
//             //operação de busca por categoria;
//             break;
//         case 6:
//             //remover todas as pizzas de uma categoria
//             break;
//         case 7:
//             exit(1);
//             break;
//     }
// }