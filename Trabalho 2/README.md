# Trabalho 2
Unifesp - ICT - BCT/BCC/EC
Programação Concorrente e Distribuída - PCD
Profs. Álvaro e Denise


Atividades de Programação -  2

OBS: Aguarde instruções de entrega. Os grupos podem ser os mesmos da Atividade 1.
Atividade  1 - Seção Crítica por espera ocupada

## Atividade  1 - Seção Crítica por espera ocupada

Implemente o algoritmo de Manna-Pnueli que implementa entrada em SC por algoritmo Cliente-Servidor. Escolher uma linguagem/biblioteca entre: linguagem C com PThreads ou OpenMP, JavaThreads ou threads em Python. Demonstre o funcionamento do código para 2 e 4 threads como processos clientes, realizando o correto incremento em uma determinada variável global ou ainda através de "prints" que demonstrem o funcionamento correto da Exclusão Mútua para a seção crítica.

OBS: Para provar que a seção crítica no código implementado de fato funciona, pode-se fazer um teste em dois códigos, onde o primeiro não implementa controle de seção crítica nos processos clientes (comentar o pré-protocolo, por exemplo) e o segundo faz a correta implementação da seção crítica nos processos clientes, e comparar as duas versões. Nestes testes faça com que a seção crítica seja executada inúmeras vezes pelos processos clientes (normalmente da ordem de bilhão de vezes). Segue abaixo um exemplo de seção crítica que pode ser usada no problema.

Considerando a variável SOMA como global/compartilhada, tem-se o seguinte trecho de código:

{
  int local = SOMA;
  sleep(rand()%2);
  SOMA = local + 1;
}

## Atividade 2 - Somatórias, seção crítica e reduções em OpenMP

A partir do programa Jogo da Vida já desenvolvido em atividade de programação anterior, a partir da versão desenvolvida em linguagem C/C++ e OpenMP, modifique o  procedimento/função (ou trecho de código) que realiza a somatória de todas as posições da última geração do tabuleiro (soma a quantidade total de células vivas no tabuleiro), desenvolvendo três  versões:

Utilizar a diretiva #pragma omp critical para realizar a operação entre as threads, totalizando os resultados em uma variável global ao final;

Utilizar uma operação de redução através da diretiva #pragma omp for reduction(???) para realizar a mesma operação.

Posteriormente, verifique o desempenho apenas desse trecho do código para as três versões, avaliando tempo de processamento do trecho e speedup ao se variar a quantidade de threads em 1, 2, 4 e 8 threads.


## Atividade 3 - Seção crítica em Java

Considere o programa anexo em Java que implementa uma simulação de uma via de mão dupla, onde carros vêm em ambos os sentidos (direita e esquerda). Os carros vindos das duas direções têm que passar por uma ponte, onde somente entra um carro por vez (tanto vindo da direita ou esquerda). 

Implemente um mecanismo de seção crítica que permita evitar colisões dos carros sobre a ponte de via única. 

O código implementa uma interface gráfica facilmente manipulável que permite inserir carros em ambas as direções, onde é possível identificar as colisões sobre a ponte.

Link para download do programa em Java:

https://drive.google.com/file/d/1gjP3E2F0ppLvgItm-PmW8xRe9ObWDrF2/view?usp=sharing
