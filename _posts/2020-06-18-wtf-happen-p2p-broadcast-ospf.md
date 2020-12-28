---
layout: post
title: "WTF really happens quando se tenta fechar adjacência entre dois roteadores com as interaces setadas em point-to-point e o outro lado broadcast?"
categories: [OSPF, IGP, Redes, WTF]
---


Um brother de trabalho chegou no escritório falando -

"Vei, passei a noite toda resolvendo um problema de OSPF, acabou que no final uma interface estava em point-to-point e a outra em broadcast."

Aconteceu esse troubleshoot justamente enquanto eu estava estudando OSPF para prova de CCNP e eu na hora comecei a raciocinar, **WTF really happens quando se tenta fechar adjacência entre dois roteadores com as interaces setadas em point-to-point e o outro lado broadcast?**

![Thinking](https://media.giphy.com/media/DfSXiR60W9MVq/giphy.gif)


## WTF do I need to know? ##

O OSPF é um IGP *distribuído*, isto é, todos os nós participantes agem em conjunto para o protocolo funcionar, portanto, é necessário que os nós participantes do protocolo se conheçam, troquem informações entre si sobre suas interfaces e redes, saiba como operar com incidentes, atualizações na topologia e finalmente calcular o menor caminho através [DJIKSTRA SPF Algorithm]¹(link de como funciona o djkstra).

Todos essas funções podem ser divididas em 3 processos para melhor entendimento do protocolo.

- **OSPF Hello Subprotocol**

 - Descoberta de vizinhos;
 - Assegura uma comunicação two-way (bidirecional) entre os dois vizinhos, isto é os pacotes de um; router alcançam o router vizinho e vice-versa. O OSPF é um IGP que trabalha com endereços de multicast e unicast;
 - Keepalive - É usado um tempo para que cada router informe aos seus vizinhos que o mesmo se encontra; ativo e operante. Caso contrário é necessário reformulação da topologia;
 - Elege o DR e BDR em redes broadcast e non-boradcast;
 - Faz a validação se os roteadores vizinhos concordam em fechar vizinhança²;

> ¹Há outros algoritmos de busca em grafos, [CORMEN]() é uma boa referência para tais algoritmos.

> ²Dos campos OSPF Header e OSPF Hello Header - Area, Network Mask, Options, Hello Interval e RouterDeadInterval tem que ser iguais para haver vizinhança.


- **Database Syncronization**
 - Utiliza os 4 Pacotes dos 5 Pacotes especificado para o protocolo OSPF. Database Descriptor Packet, Link State Request Packet, Link State Update Packet e Link State Ack;
 - Troca de Pacotes carregando apenas o cabeçalho dos pacotes Link State informando quais LSAs cada roteador tem³;
 - É feito um Link State Request quando um router identifica que não tem determinada LSA ou tem uma versão antiga do LSA
 -
 - Flooding de Pacotes Link State Update na rede através do endereço **multicast 224.0.0.5**;
 -

 > ³

- **DJIKSTRA SPF Algorithm**
 -
 
### Pode isso produção? ###

**Fiz um laboratório bem complexo para descobrir isso**

![Topologia](/images/topologia1.png)
<small> *O funcionamento normal do protocolo determina que as rotas 172.16.10.0/24 e 172.16.20.0/24 serão aprendidas via OSPF entre R1 e R2* </small>

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

**Vamos ver a tabela de vizinhança de R1**

```
  R1(config-router)#do show ip ospf nei

  Neighbor ID     Pri   State           Dead Time   Address         Interface
  2.2.2.2           0   FULL/  -        00:00:31    192.168.0.2     Ethernet0/0

```

**HOHOHOHO, full?**

![shaqille](https://media.giphy.com/media/go3X4svFhKdzi/giphy.gif)

**e a de R2?**

```
  R2(config)#do show ip ospf nei

  Neighbor ID     Pri   State           Dead Time   Address         Interface
  1.1.1.1           1   FULL/BDR        00:00:34    192.168.0.1     Ethernet0/0

```
Além de terem sido eleitos o DR e BDR no estado exstart, a adjacência foi formada pelo processo de sincronização da database, isto é ambos os Routers tem a mesma tabela [Database Summary List](https://tools.ietf.org/html/rfc2328#section-10)

**R1**
```
R1#show ip ospf database

            OSPF Router with ID (1.1.1.1) (Process ID 1)

		Router Link States (Area 0)

Link ID         ADV Router      Age         Seq#       Checksum Link count
1.1.1.1         1.1.1.1         709         0x80000072 0x00EAD0 2
2.2.2.2         2.2.2.2         598         0x80000072 0x00FDD3 1

		Net Link States (Area 0)

Link ID         ADV Router      Age         Seq#       Checksum
192.168.0.2     2.2.2.2         598         0x8000006E 0x003914
R1#
```
**R2**
```
R2#show ip ospf database

            OSPF Router with ID (2.2.2.2) (Process ID 1)

		Router Link States (Area 0)

Link ID         ADV Router      Age         Seq#       Checksum Link count
1.1.1.1         1.1.1.1         908         0x80000072 0x00EAD0 2
2.2.2.2         2.2.2.2         795         0x80000072 0x00FDD3 1

		Net Link States (Area 0)

Link ID         ADV Router      Age         Seq#       Checksum
192.168.0.2     2.2.2.2         795         0x8000006E 0x003914
R2#

```
**P2P e BROADCAST fecham adjacência???**

![WTF](https://media.giphy.com/media/ukGm72ZLZvYfS/giphy.gif)

**Vamos ver a tabela de rotas**

**R1#**
```
R1#show ip route ospf
Codes: L - local, C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2
       i - IS-IS, su - IS-IS summary, L1 - IS-IS level-1, L2 - IS-IS level-2
       ia - IS-IS inter area, * - candidate default, U - per-user static route
       o - ODR, P - periodic downloaded static route, H - NHRP, l - LISP
       a - application route
       + - replicated route, % - next hop override

Gateway of last resort is not set

R1#
```
**R2#**
```
R2#show ip route ospf
Codes: L - local, C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2
       i - IS-IS, su - IS-IS summary, L1 - IS-IS level-1, L2 - IS-IS level-2
       ia - IS-IS inter area, * - candidate default, U - per-user static route
       o - ODR, P - periodic downloaded static route, H - NHRP, l - LISP
       a - application route
       + - replicated route, % - next hop override

Gateway of last resort is not set

R2#

```

![Vish](https://media.giphy.com/media/SDogLD4FOZMM8/giphy.gif)


**E agora??**

### Captura de pacotes ###

![Captura](/images/captura-topologia1.png)
<small> Captura de pacotes de todo o processo OSPF.</small>

Vamos analisar o processo de database syncronization e troca de LSAs.
