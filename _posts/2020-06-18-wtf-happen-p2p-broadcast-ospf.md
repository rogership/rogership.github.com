---
layout: post
title: "WTF happens between **P2P** and **BROADCAST** neighbors on OSPF??"
categories: [OSPF, IGP, Redes, WTF]
---


Já imaginou o que de fato acontece quando se tenta formar adjacência entre 2 roteadores com **OSPF** interface point-to-point de um lado e OSPF interface broadcast do outro lado?

Em qual etapa do processo aparece o problema??

Será mesmo que aparece problema??

Fecha vizinhança? Fecha adjacência??

Bem, vamos ver...

Um colega de trabalho chamado Bruno passou por esse troubleshoot e isso me motivou a escrever sobre.

## WTF do I need to know? ##

O OSPF é um IGP distribuído, isto é, todos os nós participantes agem em conjunto para o protocolo funcionar, portanto, é necessário que os nós participantes do protocolo se conheçam, troquem informações entre si sobre suas interfaces e redes, saiba como operar com incidentes e finalmente calcular o menor caminho através do algoritmo de busca em grafos, que no caso do OSPF, é usado um algoritmo categorizado como **SHORTEST PATH FIRST**, vem daí o nome do protocolo.

Há outros algorimtos de busca em grafos, [CORMEN] é uma boa referência para tais algoritmos.


### Hello Subprotocol ###

O *Hello Subprotocol* do OSPF é um dos processos que compõe a operação do OSPF.
 Database syncronization e Djkistra SPF Algorithm são os outros 'processos' necessários para OSPF operar completamente.

A vizinhaça é feita apenas com o uso do OSPF Hello Packet entre os 5 outros pacotes que existem.

A RFC especifica que para fechar vizinhança os seguintes campos precisam ser iguais:

- Área - Ambas interfaces deve esta na mesma área

- HelloInterval -

- RouterdeadInterval - [John T Moy] explica muito bem as consequências de não ter RouterDeadInterval e HelloInterval iguais entre os vizinhos.

- Network Mask - Ding Ding Ding

- Options - As capacidades dos routers determinam se eles devem fechar vizinhança ou não. Felizmente pra esse post o campo Options não tem referência com o tipo de rede da interface.

A imagem 1 mostra o formato do OSPF type 1 packet
Obs: O campo Área faz parte do OSPF Packet Header e está no OSPF Packet Header comum a todos os pacotes OSPF.

![OSPF HELLO PACKET](/images/ospf-hello-packet.png)

Beleza, já temos o que precisamos, vamos ver o comportamento do protocolo.

O recebimento de um pacote Hello é especificada seção [RFC 2328](https://tools.ietf.org/html/rfc2328#page-96) e diz,

```
"...Next, the values of the Network Mask, HelloInterval,
        and RouterDeadInterval fields in the received Hello packet must
        be checked against the values configured for the receiving
        interface.  Any mismatch causes processing to stop and the
        packet to be dropped"

```

Não sendo igual os valores nos campos mencionados, o processo de vizinhança é interrompido e o pacote descartado.

Mas também diz,

```
"However,
        there is one exception to the above rule: on point-to-point
        networks and on virtual links, the Network Mask in the received
        Hello Packet should be ignored."
```

Ou seja, para redes point-to-point e virtual links os valores do campo *Network Mask*
devem ser ignorados ao serem recebidos em um Hello OSPF Packet.

Para envio de Hello Packets a [RFC 2328](https://tools.ietf.org/html/rfc2328#page-130) diz

```
"The Hello packet also contains the IP address
        mask of the attached network (Network Mask).  On unnumbered
        point-to-point networks and on virtual links this field should
        be set to 0.0.0.0."
```
Como assim unnumbered?



### Pode isso produção ?###

Fiz um laboratório bem complexo para descobrir isso


![Topologia](/images/topologia1.png)



Vamos ver a tabela de vizinhança

```

```

WTF, full?

![shaqille](https://media.giphy.com/media/go3X4svFhKdzi/giphy.gif)
