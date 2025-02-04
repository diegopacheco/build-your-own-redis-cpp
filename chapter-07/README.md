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
❯ ./target/client get k 10
server says: [1] 
❯ ./target/client get k 10
server says: [1] 
❯ ./target/client set x 20
server says: [0] 
❯ ./target/client get x get y
server says: [1] 
❯ ./target/client xxx ttt
server says: [1] 
```