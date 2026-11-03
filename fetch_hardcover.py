#!/usr/bin/env python3
"""Fetches Hardcover shelves via GraphQL API and generates livros/index.qmd."""

import html
import json
import os
import time
import urllib.error
import urllib.request
from pathlib import Path

API_URL = "https://api.hardcover.app/v1/graphql"
SHELVES = {
    "currently-reading": "Lendo",
    "read": "Lido",
    "to-read": "Quero ler",
}
STATUS_COLORS = {
    "currently-reading": "#0d6efd",
    "read": "#198754",
    "to-read": "#6c757d",
}
# status_id do Hardcover -> estante do site (4 = abandonado, fica de fora)
STATUS_IDS = {1: "to-read", 2: "currently-reading", 3: "read"}

# Tokens sem o escopo read:me: user_books já vem restrito ao dono do token.
# Sem filtro explícito por user_id, porém, a consulta passa de ~15s e o servidor
# responde 408; por isso descobrimos o user_id primeiro e filtramos por ele.
WHOAMI_QUERY = "{ user_books(limit: 1) { user_id user { username } } }"
BOOKS_QUERY = """
query ($uid: Int!) {
  user_books(where: {user_id: {_eq: $uid}}, order_by: {updated_at: desc}) {
    status_id
    book { title cached_image cached_contributors }
  }
}
"""


def graphql(token: str, query: str, variables: dict | None = None, attempts: int = 3) -> dict:
    # O site do Hardcover exibe o token já com o prefixo "Bearer "
    auth = token if token.lower().startswith("bearer ") else f"Bearer {token}"
    req = urllib.request.Request(
        API_URL,
        data=json.dumps({"query": query, "variables": variables or {}}).encode(),
        headers={
            "Content-Type": "application/json",
            "Authorization": auth,
            "User-Agent": "rogership.github.com livros sync",
        },
    )
    for attempt in range(1, attempts + 1):
        try:
            with urllib.request.urlopen(req, timeout=30) as resp:
                payload = json.load(resp)
            break
        except (urllib.error.URLError, TimeoutError) as e:
            retryable = not isinstance(e, urllib.error.HTTPError) or e.code in (408, 429) or e.code >= 500
            if not retryable or attempt == attempts:
                raise
            print(f"  tentativa {attempt} falhou ({e}); tentando de novo...")
            time.sleep(5 * attempt)
    if payload.get("errors"):
        raise RuntimeError(payload["errors"][0].get("message", payload["errors"]))
    return payload["data"]


def fetch_books(token: str) -> tuple[str, dict[str, list[dict]]]:
    whoami = graphql(token, WHOAMI_QUERY)["user_books"]
    if not whoami:
        raise RuntimeError("nenhum livro retornado")
    uid, username = whoami[0]["user_id"], whoami[0]["user"]["username"]

    all_books = {shelf: [] for shelf in SHELVES}
    for ub in graphql(token, BOOKS_QUERY, {"uid": uid})["user_books"]:
        shelf = STATUS_IDS.get(ub["status_id"])
        if not shelf:
            continue
        book = ub["book"]
        contributors = book.get("cached_contributors") or []
        # Prefere autores; tradutores, ilustradores etc. só se não houver autor
        authors = [c for c in contributors if c.get("contribution") in (None, "Author")] or contributors
        all_books[shelf].append({
            "title": book["title"],
            "author": ", ".join(c["author"]["name"] for c in authors[:2]),
            "image": (book.get("cached_image") or {}).get("url") or "",
            "shelf": shelf,
        })
    return username, all_books


def card_html(book: dict, label: str, color: str) -> str:
    img = html.escape(book["image"])
    title = html.escape(book["title"])
    author = html.escape(book["author"])
    return f"""
  <div class="book-card" data-status="{book['shelf']}">
    <div class="book-status" style="background:{color}">{label}</div>
    <img src="{img}" alt="{title}" loading="lazy" onerror="this.style.display='none'">
    <div class="book-info">
      <h4>{title}</h4>
      <p class="book-author">{author}</p>
    </div>
  </div>"""


def generate_page(username: str, all_books: dict[str, list[dict]]) -> str:
    total = sum(len(v) for v in all_books.values())
    counts = {s: len(b) for s, b in all_books.items()}

    cards = ""
    for shelf, books in all_books.items():
        label = SHELVES[shelf]
        color = STATUS_COLORS[shelf]
        for book in books:
            cards += card_html(book, label, color)

    return f"""---
title: "Livros"
toc: false
page-layout: full
---

Lista sincronizada automaticamente com o [Hardcover](https://hardcover.app/@{username}) · {total} livros

```{{=html}}
<div class="book-filters">
  <button class="filter-btn active" onclick="filterBooks('todos', this)">Todos ({total})</button>
  <button class="filter-btn" onclick="filterBooks('currently-reading', this)">Lendo ({counts['currently-reading']})</button>
  <button class="filter-btn" onclick="filterBooks('read', this)">Lidos ({counts['read']})</button>
  <button class="filter-btn" onclick="filterBooks('to-read', this)">Quero ler ({counts['to-read']})</button>
</div>

<div class="books-grid">
{cards}
</div>

<script>
function filterBooks(status, btn) {{
  document.querySelectorAll('.filter-btn').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  document.querySelectorAll('.book-card').forEach(card => {{
    card.style.display = (status === 'todos' || card.dataset.status === status) ? 'flex' : 'none';
  }});
}}
</script>
```
"""


def main():
    out = Path("livros/index.qmd")
    token = os.environ.get("HARDCOVER_TOKEN", "").strip()
    if not token:
        print(f"::warning::HARDCOVER_TOKEN não definido; mantendo {out} existente")
        return

    print("Fetching Hardcover shelves...")
    try:
        username, all_books = fetch_books(token)
    except urllib.error.HTTPError as e:
        hint = " (token expirado ou inválido? gere outro em hardcover.app/account/api)" if e.code in (401, 403) else ""
        body = e.read()[:200].decode(errors="replace").replace("\n", " ")
        print(f"::warning::Falha ao buscar Hardcover (HTTP {e.code}: {body}){hint}; mantendo {out} existente")
        return
    except Exception as e:
        # Hardcover fora do ar não deve bloquear o deploy: mantém a página versionada
        print(f"::warning::Falha ao buscar Hardcover ({e}); mantendo {out} existente")
        return

    for shelf, label in SHELVES.items():
        print(f"  {label}: {len(all_books[shelf])} livros")
    out.write_text(generate_page(username, all_books), encoding="utf-8")
    print(f"Generated {out} ({sum(len(v) for v in all_books.values())} livros total)")


if __name__ == "__main__":
    main()
