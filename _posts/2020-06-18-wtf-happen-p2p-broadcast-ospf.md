---
layout: post
title: "WTF really happens quando se tenta fechar adjacência entre dois roteadores com as interaces setadas em point-to-point e o outro lado broadcast?"
categories: [OSPF, IGP, Redes, WTF]
---
- [1 - WTF REALLY HAPPENS](#1---wtf-really-happens)
- [2 - WTF do I need to know?](#2---wtf-do-i-need-to-know)
- [3 - O LAB](#3---o-lab)
  - [3.1 - Configurações e verificações](#31---configurações-e-verificações)
  - [3.2 - P2P e BROADCAST fecham adjacência???](#32---p2p-e-broadcast-fecham-adjacência)
- [4 - The Fucking LSA](#4---the-fucking-lsa)
  - [4.1 - The Router LSA](#41---the-router-lsa)
- [5 - WTF os routers se anunciam então?](#5---wtf-os-routers-se-anunciam-então)
    - [5.1 - Captura de pacotes e Interpetação ###](#51---captura-de-pacotes-e-interpetação-)

# 1 - WTF REALLY HAPPENS

**WTF really happens quando se tenta fechar adjacência entre dois roteadores vizinhos com a interface network type em point-to-point e o outro lado broadcast?**


![Thinking](https://media.giphy.com/media/DfSXiR60W9MVq/giphy.gif)


# 2 - WTF do I need to know? ##

O OSPF é um IGP *distribuído*, isto é, todos os nós participantes agem em conjunto para o protocolo funcionar, portanto, é necessário que os nós participantes do protocolo se conheçam, troquem informações entre si sobre suas interfaces e redes, saiba como operar com incidentes, atualizações na topologia e finalmente calcular o menor caminho através do algoritmo de busca de menor caminho em um GRAFO - o video ilustra como funciona -> [DJIKSTRA SPF Algorithm]¹(link de como funciona o djkstra).

> ¹Há outros algoritmos de busca em grafos, [CORMEN]() é uma boa referência para tais algoritmos.

O OSPF pode ser dividido em 3 processos e 

[Diagrama com os 3 processos](imagem-diagrama.png)

- **OSPF Hello Subprotocol**

  - Descoberta de vizinhos;
  - Assegura uma comunicação two-way (bidirecional) entre os dois vizinhos, isto é os pacotes do router alcançam o router vizinho e vice-versa. 
  - Keepalive - É usado um tempo para que cada router informe aos seus vizinhos que o mesmo se encontra; ativo e operante. Caso contrário é necessário reformulação da topologia;
  - Elege o DR e BDR em redes broadcast e non-boradcast;
  - Faz a validação se os roteadores vizinhos concordam em fechar vizinhança²;
  - Utiliza o OSPF packet type - Hello Protocol



- **Database Syncronization**
  
  - Faz toda a troca de informação da topologia entre os roteadores do domínio
  - Envolve flooding via **IP multicast 224.0.0.5 e 226.0.0.6 para DROTHERs** ou unicast

  - Utiliza os 4 Pacotes dos 5 Pacotes totais do OSPF. 
    1. Database Descriptor Packet
    2. Link State Request Packet
    3. Link State Update Packet
    4. Link State Ack

- **Shortest Path First - DIJKSTRA Algorithm**
  - Após toda a rede tiver com os mesmos pacotes LSA's, isto é, os headers LSA forem identicos, roda-se o algoritmo de DIJKSTRA
  - Cada roteador executa o algoritmo de DIJKSTRA e calcula o menor caminho para a cada REDE anunciada em todo o domínio OSPF utilizando-se das métricas.
    
# 3 - O LAB

![LAB](https://media.giphy.com/media/iNQ2cIve8rUqI/giphy.gif)
Fiz um laboratório bem complexo/s para descobrir isso

![Topologia](/images/topologia1.png)
<small> *O funcionamento normal do protocolo determina que as rotas 172.16.10.0/24 e 172.16.20.0/24 serão aprendidas via OSPF entre R1 e R2* </small>

## 3.1 - Configurações e verificações

**Configuração R1 - (brodcast)**



    R1#show run | sec interface
      interface Loopback0
       ip address 1.1.1.1 255.255.255.255
       ip ospf 1 area 0
      interface GigabitEthernet0/0
       ip address 192.168.0.1 255.255.255.252
       duplex auto
       speed auto
       media-type rj45
 .
 .
 .


**Configuração R2 (point-to-point)**

    
    R2#show run | sec interface
      interface Loopback0
        ip address 2.2.2.2 255.255.255.255
        ip ospf 1 area 0
      interface GigabitEthernet0/0
        ip address 192.168.0.2 255.255.255.252
        ip ospf network point-to-point
        duplex auto
        speed auto
        media-type rj45
   .
   .
   .


**Vamos ver a tabela de vizinhança de R1**


    R1#show ip ospf nei

    Neighbor ID     Pri   State           Dead Time   Address         Interface
    2.2.2.2           1   FULL/DR         00:00:31    192.168.0.2     GigabitEthernet0/0


**HOHOHOHO, full?**

![shaqille](https://media.giphy.com/media/go3X4svFhKdzi/giphy.gif)

**e a de R2?**


    R2#show ip ospf nei

    Neighbor ID     Pri   State           Dead Time   Address         Interface
    1.1.1.1           0   FULL/  -        00:00:33    192.168.0.1     GigabitEthernet0/0


Aqui vamos prestar atenção em alguns elementos - O campo **priority** é 1, este é um campo diretamente proporcional, isto é, a preferência maior é do valor mais alto entre as interfaces do domínio de broadcast. A interface foi eleita como
 DR</code> pelo processo de elelição, a adjacência foi formada pelo processo de sincronização da database, isto é ambos os Routers tem a mesma tabela [Database Summary List](https://tools.ietf.org/html/rfc2328#section-10)


Então a princípio é pra haver troca de rotas pois há adjacência.

## 3.2 - P2P e BROADCAST fecham adjacência??? ##

![WTF](https://media.giphy.com/media/ukGm72ZLZvYfS/giphy.gif)

**Vamos ver a tabela de rotas**


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


![Vish](https://media.giphy.com/media/SDogLD4FOZMM8/giphy.gif)

**E agora??**


# 4 - The Fucking LSA #

**OSPF** é um protocolo distribuído, isso quer dizer que todos os nós do domínio OSPF interagem entre si, o protocolo envolve todos os routers para seu funcionamento.

O **LSA** aka **Link State Advertisement** é a menor unidade de informação da topologia que é trocada entre os roteadores para informar ***links/redes*** que cada roteador poussui, esse LSA é redistribuído através de ***flooding*** **(multicast)** por todo o domínio OSPF, assim todos os routers recebem a mesma informação.

> Imagine uma casa no qual é necessário transportá-la para o outro lado do país, podemos diminuir a casa em sua menor unidade, o tijolo, enviar pelo correios para onde a casa deve ser montada e lá remontá-la tijolo por tijolo, bem, o LSA é isso. Se eu tiver todos os tijolos então consigo remontar a mesma casa. 
>
> O paradigma é o mesmo, cada roteador recebe todas as unidades de informação, o LSA, para montar o mesmo grafo e assim calcular o menor caminho para o destino. 

Todo **LSA** possui um cabeçalho no qual indica parâmetros importantes para distinguir entre outros LSAs geradas pelo próprio roteador e também para distinguir LSAs geradas dos outros roteadores.

![LSA OSPF Header](../images/wtf-really-happens/lsa-header.png)
<small> O header tem função de identificar cada instância de LSA emitida, Sequence Number, originador do LSA e a quanto tempo esse LSA existe dentro do domínio.
O campo ***Link State ID*** difere para cada ***TYPE*** de LSA, a [RFC 2328](rfc)  define 11 tipos de LSAs </small>

![LSAs definidios](../images/wtf-really-happens/lsa-types.png)

## 4.1 - The Router LSA ##

Estamos interessado no **Router LSA**.

Todo Roteador gera **Router LSAs** onde descreve as redes no qual o roteador está diretamente conectado, o tipo de rede e o custo (métrica) para chegar a esta rede.
Isto é, as informações da rede 192.168.0.0/30 estão contidas neste LSA.

O formato do Router LSA está na figurax, lembrar que todo LSA possui Header.

![Router LSA](../images/wtf-really-happens/router-lsa.png)
<small>Router Link State Advertisement </small>

Vou falar apenas dos campos ***Link type, Link ID e Link Data***, os campos ***Link ID*** e ***Link Data*** diferem de acordo com o Link Type, a **tabela 1**  descreve os valores de cada um.

- **Link State ID**: Apresenta o Router-ID do originador do pacote LSA, observe que é o mesmo valor para o campo **Advertising Router**
- **Link Type**: Link type é basicamente o tipo de rede a qual o roteador está conectado, esta podendo ser uma rede point-to-point, transit network, stub ou um virtual link.

<small> Tabela 1 - Tipos de redes especificados para o Router-LSA</small>
| Link Type  | Link ID  | Link Data
|:---:|:---:|:---:|
|**1 - Point-to-Point**|Router ID do vizinho|IP da interface local da rede p2p|
|**2 - Connection to a transit Network**|DR IP Adress|IP da interface local da rede|
|**3 - Connection to a stub Network**|Endereço de Rede da rede stub|Máscara de rede
|**4 - virtual-link**|Router-ID do vizinho| MiB Index da interface

> Obs: Uma rede **stub** é uma rede no qual não há nenhum outro roteador conectado

# 5 - WTF os routers se anunciam então? #

![WTF3](https://media.giphy.com/media/WRAaC4EGVIzcb7Io34/giphy.gif)

Vamos ver o que acontece a nível de pacotes, primeiro vamos verificar quais LSAs estão presentes no LSA database 

**R1**

    R1#show ip ospf database

            OSPF Router with ID (1.1.1.1) (Process ID 1)

		Router Link States (Area 0)

    Link ID         ADV Router      Age         Seq#       Checksum Link count
    1.1.1.1         1.1.1.1         148         0x80000002 0x00241B 2
    2.2.2.2         2.2.2.2         149         0x80000002 0x006BD0 2
    R1#


Vamos ver alguns campos, aqui o output chama de **Link ID** o campo ***State Link ID*** do cabeçalho do LSA, esse campo vem com o valor do Router-ID do router que originou o LSA, lembre-se que o campo ***State Link ID*** varia com o tipo de LSA.

***Advertising Router*** é o mesmo valor, o Router-ID.

Observe aqui o campo ***Sequence Number*** do LSA Header, esse campo difere LSAs mais novas das antigas, O Sequence Number é incrementado a cada mudança na topologia ou quando o LSA Expira (***Age*** maior que 30 min)


**R2**


    R2#show ip ospf database

            OSPF Router with ID (2.2.2.2) (Process ID 1)

		Router Link States (Area 0)

    Link ID         ADV Router      Age         Seq#       Checksum Link count
    1.1.1.1         1.1.1.1         287         0x80000002 0x00241B 2
    2.2.2.2         2.2.2.2         286         0x80000002 0x006BD0 2



![Letsgo](https://media.giphy.com/media/XDSBGwnjvTpoZGJhxY/giphy.gif)


Boris para o Wireshark então


### 5.1 - Captura de pacotes e Interpetação ### 

![Captura](../images/wtf-really-happens/ospf-msg-lsupdate.png)
<small> Aqui to filtrando os pacotes LS Update.</small>

Estamos interessado nos LSAs com **Sequence Number == 0x80000002**, no qual está presente na Link State database dos routers, vou deixar a Captura [**AQUI**](../images/wtf-really-happens/captura-wtf.pcap) para uso do leitor.

**LSA R1(Broadcast) para R2(Point-to-Point)**

![ROUTER-LSA-R1-PARA-R2](../images/wtf-really-happens/routerlsa-r1-para-r2.png)

- Observe que o número de LSAs é 1
- ***LS Type*** é o Router-LSA(1)
- Os campos ***Link State ID*** e ***Advertising Router*** são iguais, como discutido antes
- O campo ***Sequence Number*** é o esperado
- E o número de Links nesse LSA, 2
  - Anúncio de **loopback** é como rede **stub** com **máscara /32**
  - R1(Interface em Broadcast) anuncia que a rede 192.168.0.0/30 é uma rede de trânsito, isto é, há pelo menos um outro roteador conectado e que o **Designated Router** dessa rede é o ***Link ID*** **192.168.0.2** (Interface do R2) e o campo ***Data Link*** informa qual é o endereço da minha interface na rede.


**LSA R2(P2P) para R1(Broadcast)**

![ROUTER-LSA-R2-PARA-R1](../images/wtf-really-happens/router-lsa-r2-para-r1.png)











