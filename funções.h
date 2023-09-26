#include <stdio.h>

void criar_conta();
void apagar_cliete();
void clearBuffer();

typedef struct Cliente {
  double saldo;
  int senha;
  int tipo;
  char nome[30];
} Cliente;

Cliente *buscarCliente(char cnpj[12]);
void *atualizarCliente(char cnpj[12], Cliente *c);
char *path(char cnpj[12]);
void *atualizaExtrato(char ext[100], char cnpj[12]);
void *geraExtrato(char cnpj[12]);
FILE *lerExtrato(char cnpj[12]);
