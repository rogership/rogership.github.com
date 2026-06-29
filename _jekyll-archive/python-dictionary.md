---
title: "Python Dictionary"
date: 2023-12-21
categories: [Python]
tags: [python, dictionary, data-structures]
---

## Dictionaries

### Key value pair

- key is unique identifier
- value

### Sintaxe

```python
a = {key: value}
```

- key pode ser string ou outra variável
- value pode ser qualquer coisa, inclusive outra dict, list, tupla etc.

```python
# Exemplo
student = {"name": "John", "age": 25, "courses": ["Math", "CompSci"]}
print(student["courses"])
```

    ['Math', 'CompSci']


## Methods

### Get Method

**dict.get()**

```
get(key, default=None, /) method of builtins.dict instance
Return the value for key if key is in the dictionary, else default.
```

### Update Method

**dict.update({dict: value})** - Recebe um dictionary como argumento

```
D.update([E, ]**F) -> None. Update D from dict/iterable E and F.
If E is present and has a .keys() method, then does: for k in E: D[k] = E[k]
If E is present and lacks a .keys() method, then does: for k, v in E: D[k] = v
In either case, this is followed by: for k in F: D[k] = F[k]
```

Atualiza os valores do dicionário com os valores do dicionário em argumento.

### Pop Method

**dict.pop("key")**

```
D.pop(k[,d]) -> v, remove specified key and return the corresponding value.
If key is not found, default is returned if given, otherwise KeyError is raised
```

### Copy Method

**dict.copy()**

```
copy(...) method of builtins.dict instance
D.copy() -> a shallow copy of D
```

Copy key and values from one dictionary to another, position in memory are not equal.

## Loop through dictionary

```python
student = {'name': 'John', 'age': 25, 'courses': ['Math', 'CompSci']}

print("Dict iteration methods:")
print(f"items() Method: {student.items()}")
print(f"keys() Method: {student.keys()}")
print(f"values() Method: {student.values()}")

# Loop through - retorna só a key
for key in student:
    print(key)

# Loop through necessário .items() method
for key, value in student.items():
    print("Key and value: ", key, value)
```

    Dict iteration methods:
    items() Method: dict_items([('name', 'John'), ('age', 25), ('courses', ['Math', 'CompSci'])])
    keys() Method: dict_keys(['name', 'age', 'courses'])
    values() Method: dict_values(['John', 25, ['Math', 'CompSci']])

    Only Keys:
    name
    age
    courses

    Printing key and values:
    Key and value:  name John
    Key and value:  age 25
    Key and value:  courses ['Math', 'CompSci']
