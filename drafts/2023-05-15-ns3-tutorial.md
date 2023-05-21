---
layout: post
title: "Ns3 Tutorial"
categories: [Simuladores, Redes]
---

<nav class="toc-fixed" markdown="1">
* TOC
{:toc}
</nav>

## Introdução

Ns3 é uma ferramenta open source sob a licença GNU GPLv2 de eventos discretos de sistemas de redes.

Os cenários em estudo são montados através de códigos **C++** ou **Python** e através de módulos compilados em kernel. Portanto é altamente recomendado rodar o NS3 em um ambiente virtual.

A [documentação](https://www.nsnam.org/documentation/) apresenta um introdutório dos requisitos para rodar o código e tutorial para início do uso da ferramenta.

Este documento apresenta um breve introdutório com aplicação e expicação do código, ferramentas, módulos ...

## Teoria

Analisa-se o efeito de bottleneck ou gargalo em um fluxo UDP entre cliente e servidor, a respectiva topologia e capacidade de taxa de dados é apresentada na figura 1, onde trata-se de 3 links point-to-point com delay default de 5 ms e taxa de dados para os links de acesso de 100 kbps.


                   access link 1       access link 2
                      100 kbps           100 kbps
                        |                  |
                        🡻                  🡻
                    n0 === n1 ======== n2 === n3
                (cliente)        | |       (server)
                                 | |
                                  🡻
                            bottleneck link
                            taxa < 100 kbps




## Desenvolvimento

## Resultados

## Conclusão

## Bibliografia