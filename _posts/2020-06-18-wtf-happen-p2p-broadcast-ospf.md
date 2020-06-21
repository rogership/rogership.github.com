---
layout: post
title: "WTF happens between P2P and BROADCAST neighbors on OSPF??"
categories: [OSPF, IGP, Redes, WTF]
---


Já imaginou o que realmente acontece quando se tenta formar adjacência entre 2 roteadores com *OSPF INTERFACE TYPE* point-to-point de um lado e broadcast do outro lado?

Fecha vizinhança entre os 2? E adjacência? Funciona?

Bem, vamos ver...

Um colega de trabalho chamado Bruno passou por esse troubleshoot e isso me motivou a escrever sobre.

## WTF do I need to know? ##

O OSPF é um IGP distribuído, isto é, todos os nós participantes agem em conjunto para o protocolo funcionar, portanto, é necessário que os nós participantes do protocolo se conheçam, troquem informações entre si sobre suas interfaces e redes, saiba como operar com incidentes, atualizações na topologia e finalmente calcular o menor caminho através do algoritmo de busca em grafos, que no caso do OSPF, é usado algoritmo de DJIKSTRA categorizado como **SHORTEST PATH FIRST**, vem daí o nome do protocolo.

> Há outros algoritmos de busca em grafos, [CORMEN]() é uma boa referência para tais algoritmos.

> OSPF é encapsulado pelo pacote IP diretamente, utiliza-se o código 89 no campo Protocolo do cabeçalho IPV4 para identificação

### Hello Subprotocol ###

O *Hello Subprotocol* do OSPF é um dos processos que compõe a operação do OSPF.
 Database syncronization e Djkistra SPF Algorithm são os outros 'processos' necessários para o protocolo operar completamente.

A vizinhança é feita apenas com o uso do OSPF Hello Packet entre os [5 outros pacotes que existem](https://tools.ietf.org/html/rfc2328#appendix-A.3).

A RFC especifica que para fechar vizinhança os seguintes campos precisam ser iguais:

- **Área** - Ambas interfaces devem pertencer a mesma área.

- **HelloInterval, RouterDeadInterval**

- **Network Mask** - Ding Ding Ding

- **Options** - As capacidades dos routers determinam se eles devem fechar vizinhança ou não. Felizmente pra esse post o campo Options não tem referência com o tipo de rede da interface.

A imagem 1 mostra o formato do OSPF type 1 packet
Obs: O campo Área faz parte do OSPF Packet Header e está no OSPF Packet Header comum a todos os pacotes OSPF.

<img src="/images/ospf-hello-packet.png" alt="Ospf Hello Packet">  
<span class="caption">OSPF HELLO PACKET - É acrescentado ao pacote o RID de cada vizinho no qual o Hello Packet foi recebido, portanto é adcionado 4 bytes ao Header do OSPF.</span>

Beleza, já temos o que precisamos, vamos ver o comportamento do protocolo.

O recebimento de um pacote Hello é especificada seção [RFC 2328](https://tools.ietf.org/html/rfc2328#page-96) e diz que...


```
"...Next, the values of the Network Mask, HelloInterval,
        and RouterDeadInterval fields in the received Hello packet must
        be checked against the values configured for the receiving
        interface.  Any mismatch causes processing to stop and the
        packet to be dropped"

```

  ...após um Router receber um Hello Packet de um vizinho são verificado os campos Network Mask, Hello Interval, RouterDeadInterval e comparados com seu próprio, se forem iguais o pacote é aceito, caso não, o pacote é descartado.



```
"However,
        there is one exception to the above rule: on point-to-point
        networks and on virtual links, the Network Mask in the received
        Hello Packet should be ignored."
```


Porém, para redes point-to-point e virtual links os valores do campo *Network Mask*
devem ser ignorados ao serem recebidos em um Hello OSPF Packet.

Para envio de Hello Packets a [RFC 2328](https://tools.ietf.org/html/rfc2328#page-130) diz

```
"The Hello packet also contains the IP address
        mask of the attached network (Network Mask).  On unnumbered
        point-to-point networks and on virtual links this field should
        be set to 0.0.0.0."
```
Como assim unnumbered?

Na real é simples, point-to-point networks são redes no qual há somente dois participantes, todo pacote que sai de uma interface point-to-point sempre terá somente um destino, não há necessidade de identificação alguma (IP) para envio de pacotes em redes point-to-point.


### Pode isso produção ?###

Fiz um laboratório bem complexo para descobrir isso

<img src="/images/topologia1.png" alt="Topologia em laboratório">  
<span class="caption"> O comportamento normal do OSPF é que ao final do processo R1 tenha em sua tabela de rota a rede 172.16.20.0/24 e R2 a rede 172.16.10.0/24 </span>

![Topologia](/images/topologia1.png)

**Configuração R1 - (point-to-point)**

```
R1(config-router)#do show run | sec interface
interface Loopback0
 ip address 1.1.1.1 255.255.255.255
interface Ethernet0/0
 ip address 192.168.0.1 255.255.255.0
 ip ospf network point-to-point
 no keepalive
interface Ethernet0/1
 ip address 172.16.10.1 255.255.255.0

```

**Configuração R2 (broadcast)**
```
  R2(config)#do show run | sec interface
  interface Loopback0
   ip address 2.2.2.2 255.255.255.255
  interface Ethernet0/0
   ip address 192.168.0.2 255.255.255.0
   no keepalive
  interface Ethernet0/1
   ip address 172.16.20.1 255.255.255.0

```

Vamos ver a tabela de vizinhança

```

```

WTF, full?

![shaqille](https://media.giphy.com/media/go3X4svFhKdzi/giphy.gif)
