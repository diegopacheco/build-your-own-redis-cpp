### Test Heap

```
❯ ./target/test_heap
OK
```

### Result

Server

```
❯ ./target/server
new client from 127.0.0.1:47142
client closed
new client from 127.0.0.1:58290
client closed
new client from 127.0.0.1:58292
client closed

```

Client

```
❯ ./target/client set x 100
(nil)
❯ ./target/client get x
(str) 100
❯ ./target/client pexpire x 1
(int) 1
❯ ./target/client get x
(nil)
```