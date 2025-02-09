### Server

Server

```
❯ ./build.sh
❯ ./target/server
new client from 127.0.0.1:40436
client closed
new client from 127.0.0.1:40440
client closed
^C
```

Client

```
❯ ./target/client set x 10
(nil)
❯ ./target/client get x
(str) 10
```