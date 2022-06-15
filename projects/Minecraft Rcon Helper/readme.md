# Minecraft Rcon Helper

**Status** » In progress
**Language** » C++
**Dependency** » https://github.com/Tiiffi/mcrcon

*I will redo this project on a scripting language like bash or python*

---

### About this RCON Helper

This code works like a centralized console for the minecraft servers I manage.
On startup asks the user to choose a server from the list. After making that selection, the terminal turns into a RCON for that server.

As the code is when I'm writing this readme, the server info is hardcoded into the `.cpp` file. Instead of fixing this, I will write from scratch this on python, C, shell or any other executable script.


### Usage

```
1. Download mcrcon
2. Edit Line 6 of rcon_helper.cpp and set the path to your saved file
3. Edit populate_servers() function. Line 37. Each entry should use
    the following format: <name>, <version>, <ip>, <rcon_port>, <rcon_pwd>
4. Build the source code using -std=c++11

5. [Optional] Instalation
    For ease of use, I copied my rcon_helper binary to /usr/sbin/. 
    This way I can just write in the console rcon_helper and it works
```


<div style="display: flex; justify-content: center;">
    <img src="images/rcon helper example.png" alt="Gear image as logo" >
</div>
