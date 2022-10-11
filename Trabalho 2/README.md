# Trabalho 2

## Enunciado 1
Unifesp - ICT - BCT/BCC/EC
Programação Concorrente e Distribuída - PCD
Profs. Álvaro e Denise


Atividades de Programação -  2

OBS: Aguarde instruções de entrega. Os grupos podem ser os mesmos da Atividade 1.
Atividade  1 - Seção Crítica por espera ocupada

Implemente o algoritmo de Manna-Pnueli que implementa entrada em SC por algoritmo Cliente-Servidor. Escolher uma linguagem/biblioteca entre: linguagem C com PThreads ou OpenMP, JavaThreads ou threads em Python. Demonstre o funcionamento do código para 2 e 4 threads como processos clientes, realizando o correto incremento em uma determinada variável global ou ainda através de "prints" que demonstrem o funcionamento correto da Exclusão Mútua para a seção crítica.

OBS: Para provar que a seção crítica no código implementado de fato funciona, pode-se fazer um teste em dois códigos, onde o primeiro não implementa controle de seção crítica nos processos clientes (comentar o pré-protocolo, por exemplo) e o segundo faz a correta implementação da seção crítica nos processos clientes, e comparar as duas versões. Nestes testes faça com que a seção crítica seja executada inúmeras vezes pelos processos clientes (normalmente da ordem de bilhão de vezes). Segue abaixo um exemplo de seção crítica que pode ser usada no problema.

Considerando a variável SOMA como global/compartilhada, tem-se o seguinte trecho de código:

{
  int local = SOMA;
  sleep(rand()%2);
  SOMA = local + 1;
}
