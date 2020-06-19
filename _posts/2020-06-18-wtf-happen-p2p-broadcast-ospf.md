---
layout: post
title: "What the fuck really happens trying to form adjacency between p2p and broadcast neighbor on OSPF??"
categories: [OSPF, IGP, Redes, WTF]
---


Já imaginou o que de fato acontece quando se tenta formar adjacência entre 2 roteadores com ospf interface point-to-point e broadcast respectivamente?

Em qual etapa do processo aparece o problema e será que aparece?
Vizinhança é fechada e o estado 2-way é alcançado?

Bem, vamo ver...

Um colega de trabalho chamado Bruno passou por esse troubleshoot e isso me motivou a escrever sobre.

## WTF do I need to know? ##

O Hello Subprotocol do OSPF é um dos processos que compõe a operação do OSPF, Database syncronization e Djkistra SPF Algorithm são os outros 'processos' necessários para OSPF operar completamente.

O OSPF é um IGP distribuído, isto é todos os nós participantes agem de alguma forma para o protocolo funcionar, então é necessário que os nós participantes do protocolo se conheçam (Hello SubProtocol), troquem informações de suas interfaces e redes e a partir daí cada roteador executa o algoritmo de busca por menor caminho em grafos, que no caso do OSPF, o algoritmo é categorizado como SHORTEST PATH FIRST.
OPEN SHORTEST PATH e se chama algoritmo de DIJKSTRA.


A vizinhaça é feita só com o uso do OSPF Hello packet, type 1 entre os 5 OSPF packets.
E já adiantando, para fechar vizinhança os seguintes campos precisam da match

- Area - Ambas interfaces deve esta na mesma area

-Hello HelloInterval

-Router dead HelloInterval

-Network Mask

-Options

A imagem 1 mostra o formato do OSPF type 1 packet
Obs: O campo Area faz parte do OSPF Packet Header e está no header acima do Hello OSPF Packet

[OSPF HELLO PACKET]/images/ospf-hello-packet.png

Por tanto vamos analisar

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





Vamo começar pelo Hello Sub Subprotocol então


RFC[2328] é a ultima atualização do OSPFv4  

## Da topologia da rede ##

![Topologia](images/topologia-ospf-filtro-lsa.jpeg)
