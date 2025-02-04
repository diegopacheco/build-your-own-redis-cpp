### Result

Server

```bash
./target/server
```

```
❯ ./target/server
new client from 127.0.0.1:3360030720
client closed
new client from 127.0.0.1:3360161792
client closed
new client from 127.0.0.1:3360555008
client closed
new client from 127.0.0.1:2832465920
client closed
new client from 127.0.0.1:3963879424
client closed
new client from 127.0.0.1:3964796928
client closed
```

Client

```bash
❯ ./target/client set k 10
server says: [0] 
❯ ./target/client set x 20
server says: [0] 
❯ ./target/client get x
server says: [0] 20
❯ ./target/client get k
server says: [0] 10
❯ ./target/client get z
server says: [2] 
❯ ./target/client zzz cccc
server says: [1] 
```