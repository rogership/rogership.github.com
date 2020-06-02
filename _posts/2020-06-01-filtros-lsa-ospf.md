---
layout: post
title: "OSPF - Filtro de LSAs"
categories: [OSPF, IGP, Redes]
---

## Porquê filtrar pacotes LSAs? ##

Imagine o cenário onde o OSPF é usado para troca de rotas entre entidades.
Existe protocolos melhores para isso, certo? BGP permite a troca de rotas sem que as entidades compartilhem o mesmo domínio por exemplo. MPLS Layer3 também seria uma solução entre diversas outras tecnologias.

Porém implementações de projetos envolve inúmeros fatores e acabei tratando de um cenário onde o domínio OSPF, ou melhor dizendo, o sistema autônomo OSPF é compartilhado entre 3 entidade.

![Topologia](images/topologia-ospf-filtro-lsa.jpeg)
