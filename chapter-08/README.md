### Result

Server

```bashg
❯ ./target/server
new client from 127.0.0.1:57794
client closed
new client from 127.0.0.1:57798
client closed
new client from 127.0.0.1:52128
client closed
new client from 127.0.0.1:52136
client closed
new client from 127.0.0.1:49732
client closed
new client from 127.0.0.1:49748
client closed
```

Client

```
❯ ./target/client set x 20
server says: [0] 
❯ ./target/client get x 20
server says: [1] 
❯ ./target/client get x
server says: [0] 20
❯ ./target/client set y 30
server says: [0] 
❯ ./target/client get y
server says: [0] 30
❯ ./target/client xxx rrr
server says: [1] 

```