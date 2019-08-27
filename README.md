# introduce-operating-system
---
## 1. Readers and writers problem
- Multireader and multiwriter
- More than one reader can read the same time
- Just one writer can write
- Reader and writer can not read write at the same time
---
## 2. Chat group console socket
- Server can serve multiclient using thread
`gcc -pthread server.c -o server`
`gcc client.c -o client`