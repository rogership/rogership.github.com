## Dictionaries

### Key value pair

- key is unique identifier
- value

### Sintaxe

a = {key: value}

- key pode ser string ou outra variável
- value pode ser qualquer coisa, inclusive outra dict, list, tupla etc.




```python
#Exemplo

student = {"name": "John", "age": 25, "courses": ["Math", "CompSci"]}

print(student["courses"])


```

    ['Math', 'CompSci']


## Methods

### Get Method -----------------------------

**dict.get()**

**Help on built-in function get:**

    get(key, default=None, /) method of builtins.dict instance
    Return the value for key if key is in the dictionary, else default.*

### Update Method -----------------------------

**dict.update({dict: value})** - Recebe um dictionary como argumento

    update(...)
    D.update([E, ]**F) -> None.  Update D from dict/iterable E and F.
    If E is present and has a .keys() method, then does:  for k in E: D[k] = E[k]
    If E is present and lacks a .keys() method, then does:  for k, v in E: D[k] = v
    In either case, this is followed by: for k in F:  D[k] = F[k]

    Atualiza os valores do dicionário com os valores do dicionário em argumento

    
### Pop Method -----------------------------

**dict.pop("key")**

    pop(...)
    D.pop(k[,d]) -> v, remove specified key and return the corresponding value.
    
    If key is not found, default is returned if given, otherwise KeyError is raised


### Copy Method -----------------------------

**dict.copy()**

    copy(...) method of builtins.dict instance
    D.copy() -> a shallow copy of D


    Copy key and values from one dictionary to another, position in memory are not equal

### Copy Method -----------------------------




## Loop through dictionary




```python
## Looping through dictionary

student = {'name': 'John', 'age': 25, 'courses': ['Math', 'CompSci']}

print("Dict iteration methods:")
## Method .items() retorna key and value
print(f"items() Method: {student.items()}")

## Method .keys() retorna keys
print(f"keys() Method: {student.keys()}")

## Method values()
print(f"values() Method: {student.values()}")

print("\n")

## Loop through - retorna só a key
print("Only Keys:")
for key in student:
    print(key)

print("\n")

## Loop through necessário .items() method
print("Printing key and values:")
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

