#!/usr/bin/env python3
"""Fetches Goodreads RSS shelves and generates livros/index.qmd."""

import urllib.request
import xml.etree.ElementTree as ET
import re
from pathlib import Path

GOODREADS_USER_ID = "85180846"
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

def fetch_shelf(shelf: str) -> list[dict]:
    url = f"https://www.goodreads.com/review/list_rss/{GOODREADS_USER_ID}?shelf={shelf}"
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
    with urllib.request.urlopen(req, timeout=15) as resp:
        xml = resp.read()

    root = ET.fromstring(xml)
    ns = {"gr": "https://www.goodreads.com"}
    books = []

    for item in root.findall(".//item"):
        def tag(name):
            el = item.find(name)
            return el.text.strip() if el is not None and el.text else ""

        title = tag("title")
        # Remove series info from title: "Title (Series, #N)"
        title = re.sub(r"\s*\(.*?\)\s*$", "", title).strip()

        img = tag("book_image_url")
        # Upgrade thumbnail to medium cover
        img = re.sub(r"\._S[XY]\d+_", "", img)

        books.append({
            "title": title,
            "author": tag("author_name"),
            "image": img,
            "rating": tag("average_rating"),
            "shelf": shelf,
        })

    return books


def card_html(book: dict, label: str, color: str) -> str:
    img = book["image"] or ""
    return f"""
  <div class="book-card" data-status="{book['shelf']}">
    <div class="book-status" style="background:{color}">{label}</div>
    <img src="{img}" alt="{book['title']}" loading="lazy" onerror="this.style.display='none'">
    <div class="book-info">
      <h4>{book['title']}</h4>
      <p class="book-author">{book['author']}</p>
    </div>
  </div>"""


def generate_page(all_books: dict[str, list[dict]]) -> str:
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

Lista sincronizada automaticamente com o [Goodreads](https://www.goodreads.com/user/show/{GOODREADS_USER_ID}-rogership) · {total} livros

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
    print("Fetching Goodreads shelves...")
    all_books = {}
    for shelf, label in SHELVES.items():
        books = fetch_shelf(shelf)
        all_books[shelf] = books
        print(f"  {label}: {len(books)} livros")

    out = Path("livros/index.qmd")
    out.write_text(generate_page(all_books), encoding="utf-8")
    print(f"Generated {out} ({sum(len(v) for v in all_books.values())} livros total)")


if __name__ == "__main__":
    main()
