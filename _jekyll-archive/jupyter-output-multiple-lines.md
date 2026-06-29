---
title: "Jupyter Notebook - Multiple lines output configuration"
date: 2024-10-20
categories: [Python]
tags: [python, jupyter, ipython, configuration]
---

### Introduction

Flow

1 - Create Ipython config file

```bash
ipython profile create <profile-name>
# leave it blank to use the default profile
```

2 - Adjust the file

```
/home/$user/.ipython/profile_<profile-name>/ipython_config.py
```

The files typically start by getting the root config object:

```python
c = get_config()

# add the line
c.InteractiveShell.ast_node_interactivity = "all"
```

You need to restart the Jupyter Kernel server, do it by closing all Jupyter instances and opening again.

Make sure that your ipython is running the right profile:

```
~/.ipython/profile_default  ipython --show-config

Python 3.12.6 (main, Sep  8 2024, 13:18:56) [GCC 14.2.1 20240805]
IPython 8.27.0 -- An enhanced Interactive Python.
Loaded config files:
/home/roger/.ipython/profile_default/ipython_config.py

InteractiveShell.ast_node_interactivity = 'all'
```

## References

- [IPython configuration intro](https://ipython.org/ipython-doc/3/config/intro.html)
- [Why does the last line in a cell generate output but preceding lines do not](https://stackoverflow.com/questions/75186036/why-does-the-last-line-in-a-cell-generate-output-but-preceding-lines-do-not)
- [InteractiveShell.ast_node_interactivity](https://ipython.readthedocs.io/en/stable/config/options/terminal.html#configtrait-InteractiveShell.ast_node_interactivity)
