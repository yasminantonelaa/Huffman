#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    int **clauses;
    int num_vars;
    int num_clauses;
} Formula;

typedef struct {
    int *values; // 1 = true, 0 = false, -1 = indefinido
    int size;
} Interpretation;

typedef struct Node {
    int var_index;      // índice da variável atribuída neste nó
    struct Node *left;  // próxima atribuição com valor 0
    struct Node *right; // próxima atribuição com valor 1
} Node;

void free_tree(Node *node) {

    if (!node) return;

    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

void kill_execution(Node *node, int *assignment, Formula *F, Interpretation *I) // libera toda mémoria e mata a execução
{
    free(assignment);
    free_tree(node);

    for (int i = 0; i < F->num_clauses; i++) {
        free(F->clauses[i]);
    }

    free(F->clauses);
    free(I->values);
    free(F);
    free(I);

    exit(0); // mata a execução encerrando com sucesso
}

// Lê a fórmula no formato DIMACS com validações
Formula* read_formula() {

    Formula *F = malloc(sizeof(Formula));
    int vars, clauses;
    char line[256];

    // Ignorar comentários até encontrar "p cnf"
    bool header_found = false;
    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == 'p') {
            if (sscanf(line, "p cnf %d %d", &vars, &clauses) != 2) { // leitura da string e verificação de formato
                fprintf(stderr, "Erro: Cabeçalho inválido. Esperado 'p cnf <num_vars> <num_clauses>'.\n");
                exit(1);
            }
            header_found = true;
            break;
        }
    }

    if (!header_found) {
        fprintf(stderr, "Erro: Cabeçalho 'p cnf' não encontrado.\n");
        exit(1);
    }

    F->num_vars = vars;
    F->num_clauses = clauses;
    F->clauses = malloc(clauses * sizeof(int*)); // mtz dinámica aloca as "linhas"

    for (int i = 0; i < clauses; i++) { // aloca as cólunas da matriz, le as varíaveis e verificam se estão de acordo com as espécificações
        F->clauses[i] = malloc((vars + 1) * sizeof(int)); // +1 para o terminador (da clasusula) 0 
        int literal, j = 0;

        while (scanf("%d", &literal) == 1 && literal != 0) { // preeche as clausulas 
            if (abs(literal) > vars) { // abs rotorna valor absoluto
                fprintf(stderr, "Erro: Literal %d fora do intervalo permitido (1 a %d)\n", literal, vars);
                exit(1);
            }
            F->clauses[i][j++] = literal;
        }

        if (j == 0) {
            fprintf(stderr, "Erro: Cláusula %d está vazia.\n", i + 1);
            exit(1);
        }

        F->clauses[i][j] = 0; // terminador da cláusula
    }

    return F;
}

bool satisfies(Formula *F, Interpretation *I) { 
    
    for (int i = 0; i < F->num_clauses; i++) {
        bool clause_sat = false;
        for (int j = 0; F->clauses[i][j] != 0; j++) {
            int literal = F->clauses[i][j];
            int var = abs(literal);
            int val = I->values[var];

            if (val == -1) break; // força a busca por mais caminhos
            // se um literal de true toda a clasusla é saistisfeita
            if ((literal > 0 && val == 1) || (literal < 0 && val == 0)) {
                clause_sat = true;
                break;
            }
        }
        if (!clause_sat) return false; // clausula falsa não satizfaz
    }
    return true;
}

bool contradicts(Formula *F, Interpretation *I) {

    for (int i = 0; i < F->num_clauses; i++) {
        bool clause = false;

        for (int j = 0; F->clauses[i][j] != 0; j++) {
            int literal = F->clauses[i][j];
            int var = abs(literal);
            int val = I->values[var];

            if (val == -1) {
                clause = true; // ainda pode se tornar verdadeira
                continue;
            }
            // se um literal de true toda a clasusla é saistisfeita
            if ((literal > 0 && val == 1) || (literal < 0 && val == 0)) {
                clause = true;
                break;
            }
        }

        if (!clause) return true; // clausula é falsa contradiz
    }
    return false; 
}


Node* build_tree(Formula *F, Interpretation *I) {

    if (contradicts(F, I)) return NULL; // quando a formula se contradiz ocorre a "polda" do nó da contradição
    
    if (satisfies(F, I)) { // se um caminho é satisfeito

        Node *solution_node = malloc(sizeof(Node));

        solution_node->var_index = -1;
        solution_node->left = NULL;
        solution_node->right = NULL;

        return solution_node; // Nó terminal que representa o fim de uma solução válida
    }

    int var = -1; // verifica se ainda há literais indefinidos

    for (int i = 1; i <= F->num_vars; i++) { // verifica cada posição do array da interpetraçãos dos literais
        if (I->values[i] == -1) {
            var = i;
            break;
        }
    }

    if (var == -1) return NULL; // retorna nulo se todos os literais ja forma definidos

    Node *node = malloc(sizeof(Node));
    node->var_index = var;

    I->values[var] = 0; // tenta 0 no literal da posição e cria um filho a esqurda
    node->left = build_tree(F, I);

    I->values[var] = 1; // analogamente tenta 1 e cria um filho a direita
    node->right = build_tree(F, I);

    I->values[var] = -1; // backtrack:  restaurando o valor da posição para indefinido para que outro caminho seja testado
    

    if (!node->left && !node->right) { // quando solução é falsa ocorre a "polda" o nó no retono da chamda recusiva
        free(node);
        return NULL;
    }

    return node;
}

void print_solution_path(Node *node, int *assignment, Formula *F, Interpretation *I, bool kill, Node *root) {
// imprime as possibilidades em pre-ordem
    if (!node) return;

    if (node->var_index == -1) { // irá preecher o array assignment com o valor dos literais ate completar uma solução válida
        printf("SAT!\n");

        for (int i = 1; i <= F->num_vars; i++) {
            printf("%d = %d\n", i, assignment[i]);
        }

        if(kill) kill_execution(root,assignment,F,I); // mata a execução para imprimir a primeira solução
        else return; // imprime as outras soluções caso falso
    }

    if (node->left) { // se a arvore vai para a esquerda o literal repesentado pelo nó recebe 0
        assignment[node->var_index] = 0; 
        print_solution_path(node->left, assignment, F, I , kill,root);
    }
    if (node->right) { // analogamnte pela direita recebe 1
        assignment[node->var_index] = 1;
        print_solution_path(node->right, assignment, F, I, kill,root);
    }
}

int main() {

    Formula *F = read_formula(); 
    Interpretation *I = malloc(sizeof(Interpretation));
    bool EN_K = true; // (enable kill) habilita ou desabilita a impressão completa dos sats
    
    I->size = F->num_vars + 1;
    I->values = malloc(I->size * sizeof(int));
    for (int i = 0; i < I->size; i++) I->values[i] = -1; // preeche o array da interpretação como -1 (indefinido)

    int *assignment = calloc(I->size, sizeof(int)); // 0 por default no array de impressão dos literais
    Node *root = build_tree(F, I);

    if (root == NULL) printf("UNSAT!\n"); // não houve solução
    else print_solution_path(root, assignment, F,I , EN_K, root); // passa todos esse parámtros para liberar a memória dentro da função
    
    kill_execution(root,assignment, F,I); // libreção de memoria do encerramnto final: sat ou impressão completa

}