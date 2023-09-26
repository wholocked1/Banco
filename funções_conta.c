#include "funções.h" //para poder usar o Struct
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void debito() { // faz a função de débito
  clearBuffer();
  printf("CNPJ da conta (somente números): \n");
  char cnpj[12];
  scanf("%11s", cnpj);      // recebe o CNPJ do cliente
  while (getchar() != '\n') // essa função limpa o buffer
    ;
  DIR *dir = opendir(cnpj);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("CNPJ não tem conta nesse banco.\n");
    return;
  } else {
    printf("Erro ao tentar abrir o arquivo da conta.\n Você será redirecionado "
           "ao menu.\n Desculpe por qualquer transtorno.\n");
    return;
  }
  Cliente *cliente =
      buscarCliente(cnpj); // pega as informaçõe referentes ao cliente
  printf("senha da conta: \n");
  int senha;
  scanf("%d", &senha); // recebe a senha

  if (senha != cliente->senha) { // verifica a senha
    printf("Informações incorretas\n");
    return; // retorna caso senha incorreta
  }

  printf("Valor: \n");
  double valor;
  scanf("%lf", &valor);

  // senha e CNPJ tem que conferirem para que o deposito seja feito (ok)
  double taxa;
  if (cliente->tipo == 1) {
    taxa = 0.05 * valor;
  } else if (cliente->tipo == 2) {
    taxa = 0.03 * valor;
  }
  double valor_novo = taxa + valor;
  double saldo = cliente->saldo;
  double saldo_novo = saldo - valor_novo; // gera o valor novo da conta
  if (saldo_novo < -1000) { // verifica se é possível ter esse valor na conta
    if (cliente->tipo == 1) {
      printf("Valor limite atingido");
    } else if ((cliente->tipo == 1) && (saldo_novo < -4000)) {
      printf("Valor de limite atingido");
    }
  }

  cliente->saldo = saldo_novo;

  char debito[200];
  sprintf(debito, "Debito de %.2lf, taxa de %.2lf, total debitado de %.2lf\n",
          valor, taxa,
          valor_novo);             // cria o texto a ser colocado no extrato
  atualizaExtrato(debito, cnpj);   // atualiza o extrato
  atualizarCliente(cnpj, cliente); // atualiza as infos do cliente
  clearBuffer();
}

void debito_trans(
    char cnpj[12],
    double valor) { // usando a mesma ideia da função a cima, ela foi adaptada
                    // para ser usada na função transferência iniciada abaixo
  Cliente *cliente = buscarCliente(cnpj);

  double tax;
  if (cliente->tipo == 1) {
    tax = valor * 0.05;
  } else if (cliente->tipo == 2) {
    tax = valor * 0.03;
  }
  double valor_novo = tax + valor;
  double saldo = cliente->saldo;
  double saldo_novo = saldo - valor;
  if (saldo_novo < -1000) {
    if (cliente->tipo == 1) {
      printf("Limite excedido!\n");
      return;
    }
    if ((cliente->tipo == 2) && (saldo_novo < -5000)) {
      printf("Limite excedido!\n");
    }
  }
  cliente->saldo = saldo_novo;

  char debito[100];
  sprintf(debito,
          "Transferêcia no valor: %.2lf para o CNPJ: %s, taxa: "
          "%.2lf, valor final: %.2lf\n",
          valor, cnpj, valor, valor);
  atualizaExtrato(debito, cnpj);
  atualizarCliente(cnpj, cliente);
  clearBuffer();
}

void deposito() { // gera a função depósito
  clearBuffer();
  printf(
      "Qual o CNPJ da conta que deseja ser depositado (somente números): \n");
  char cnpj[12];
  scanf("%s", cnpj); // recebe o CNPJ
  DIR *dir = opendir(cnpj);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("CNPJ não tem conta nesse banco.\n");
    return;
  } else {
    printf("Erro ao tentar abrir o arquivo da conta.\n Você será redirecionado "
           "ao menu.\n Desculpe por qualquer transtorno.\n");
    return;
  }
  Cliente *cliente =
      buscarCliente(cnpj); // encontrar a conta com o CNPJ referente a ele

  printf("Qual valor que deseja ser depositado: \n");
  double valor_add;
  scanf("%lf", &valor_add); // valor a ser adicionado
  double saldo = cliente->saldo;
  double saldo_novo = saldo + valor_add;
  cliente->saldo = saldo_novo;
  atualizarCliente(cnpj, cliente); // atualiza as informações do cliente

  char ext_deposito[50];
  sprintf(ext_deposito, "Depósito de %.2lf\n",
          valor_add);                  // gera o que será colocado no extrato
  atualizaExtrato(ext_deposito, cnpj); // adiciona no extrato
  clearBuffer();
}

void deposito_trans(
    double valor_add,
    char cnpj[12]) { // usando a mesma ideia da função a cima, é feita as
                     // alterações necessárias para que seja usada na função
                     // transferência iniciada a baixo.
  Cliente *cliente = buscarCliente(cnpj);

  double saldo = cliente->saldo;
  double saldo_novo = saldo + valor_add;
  cliente->saldo = saldo_novo;
  atualizarCliente(cnpj, cliente);

  char ext_deposito[80];
  sprintf(ext_deposito, "Transferência recebida no valor de %.2lf do CNPJ %s",
          valor_add, cnpj);
  atualizaExtrato(ext_deposito, cnpj);
  // adicionar a conta referente ao CNPJ (ok)
}

void extrato() { // faz a leitura do extrato do cliente
  clearBuffer();
  printf("CNPJ da conta (somente números): \n");
  char CNPJ[12];
  scanf("%s", CNPJ);
  DIR *dir = opendir(CNPJ);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("CNPJ não tem conta nesse banco.\n");
    return;
  } else {
    printf("Erro ao tentar abrir o arquivo da conta.\n Você será redirecionado "
           "ao menu.\n Desculpe por qualquer transtorno.\n");
    return;
  }
  Cliente *cliente = buscarCliente(CNPJ);
  // int cnpje;
  // sscanf(CNPJ, "%d", &cnpje);

  printf("Senha da conta: \n");
  int senha;
  scanf("%d", &senha);

  if (cliente->senha != senha) { // verifica se as informações estão corretas
    printf("Senha incorreta. \n");
    return;
  }

  // senha e CNPJ devem estar de acordo com o cadastro original (ok)
  FILE *ext2 = lerExtrato(CNPJ); // recebe as informações do extrato
  char *result;
  char Linha[100];

  while (!feof(ext2)) {
    // Lê uma linha (inclusive com o '\n')
    result =
        fgets(Linha, 100, ext2); // o 'fgets' lê até 99 caracteres ou até o '\n'
    if (result)                  // Se foi possível ler
      printf("%s", Linha);
  }
  clearBuffer();
  // imprime o arquivo com todas as transações feitas pelo usuário (ok)
}

void transferencia() { // função transferência
  clearBuffer();
  printf("Qual o CNPJ de origem (somente números): \n");
  char CNPJ[12];
  scanf("%s", CNPJ); // recebe o CNPJ
  DIR *dir = opendir(CNPJ);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("CNPJ não tem conta nesse banco.\n");
    return;
  } else {
    printf("Erro ao tentar abrir o arquivo da conta.\n Você será redirecionado "
           "ao menu.\n Desculpe por qualquer transtorno.\n");
    return;
  }
  Cliente *cliente = buscarCliente(CNPJ); // recebe as informações do cliente

  printf("Senha da conta: \n");
  int senha;
  scanf("%d", &senha);

  if (cliente->senha != senha) { // verifica a senha
    printf("Senha incorreta. \n");
    return;
  }

  // CNPJ e senha devem condizentes para que o programa continue
  // colocar um modo de ver se o arquivo existe (verifica as informações para
  // vertificar a veracidade das informações) encontrar a conta de origem

  printf("CNPJ de destino (somente números): \n");
  char CNPJ_dest[12];
  scanf("%s", CNPJ_dest);

  // encontrar a conta de destino (verifica a existencia de uma conta com esse
  // CNPJ)

  printf("Valor a ser transfirido: \n");
  double valor_trans;
  scanf("%lf", &valor_trans);

  debito_trans(CNPJ,
               valor_trans); // faz a função de débito para o CNPJ de origem

  deposito_trans(valor_trans,
                 CNPJ_dest); // faz a função de deposito para o CNPJ de destino

  // subtrair da conta de origem e somar na conta de destino (ok)
}

void saldo() { // verifica o saldo do clinte
  clearBuffer();
  printf("CNPJ: (somente números)\n");
  char CNPJ[12];
  scanf("%s", CNPJ); // recebe o CNPJ
  DIR *dir = opendir(CNPJ);
  if (dir) {
    closedir(dir);
  } else if (ENOENT == errno) {
    printf("CNPJ não tem conta nesse banco.\n");
    return;
  } else {
    printf("Erro ao tentar abrir o arquivo da conta.\n Você será redirecionado "
           "ao menu.\n Desculpe por qualquer transtorno.\n");
    return;
  }

  Cliente *cliente = buscarCliente(CNPJ); // recebe as infos do cliente

  printf("Senha: \n");
  int senha;
  scanf("%d", &senha);

  if (senha != cliente->senha) { // verifica a senha
    printf("senha incorreta.\n");
    return;
  }
  printf("Saldo no valor de: %lf\n",
         cliente->saldo); // imprime o saldo do cliente
  clearBuffer();
}
