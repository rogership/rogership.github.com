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

### Hello Subprotocol ###
O *Hello Subprotocol* do OSPF é um dos processos que compõe a operação do OSPF, Database syncronization e Djkistra SPF Algorithm são os outros 'processos' necessários para OSPF operar completamente.

O OSPF é um IGP distribuído, isto é todos os nós participantes agem de alguma forma para o protocolo funcionar, então é necessário que os nós participantes do protocolo se conheçam, troquem informações entre si sobre suas interfaces e redes, saiba como operar com incidentes,e calcular o menor caminho através do algoritmo de busca em grafos, que no caso do OSPF, o algoritmo de DIJKSTRA é usado e categorizado como um algoritmo SHORTEST PATH FIRST.

Há outros algorimtos de busca em grafos, [CORMEN] é uma boa referência para tais algoritmos.


A vizinhaça é feita apenas com o uso do OSPF Hello packet, type 1 entre os 5 OSPF packets.

Para fechar vizinhança os seguintes campos precisam ser iguais:

- Area - Ambas interfaces deve esta na mesma area

- Hello HelloInterval

- Router dead HelloInterval

- Network Mask

- Options

A imagem 1 mostra o formato do OSPF type 1 packet
Obs: O campo Área faz parte do OSPF Packet Header e está no header acima do Hello OSPF Packet

![OSPF HELLO PACKET](/images/ospf-hello-packet.png)

Por tanto vamos analisar,

A [RFC 2328](https://tools.ietf.org/html/rfc2328#page-96) fala assim

```
"...Next, the values of the Network Mask, HelloInterval,
        and RouterDeadInterval fields in the received Hello packet must
        be checked against the values configured for the receiving
        interface.  Any mismatch causes processing to stop and the
        packet to be dropped"
```

mas também diz

```
"However,
        there is one exception to the above rule: on point-to-point
        networks and on virtual links, the Network Mask in the received
        Hello Packet should be ignored."
```

Daí já pode-se concluir que **ACABOU, GG, EZ**

Vamos ver a tabela de vizinhança



WTF, full?

![shaqille](https://gph.is/2dnGSbp)

###Pode isso produção?###

Fiz um laboratório bem complexo para descobrir isso



## Da topologia da rede ##

![Topologia](images/topologia-ospf-filtro-lsa.jpeg)
