### Result

Server

```
❯ ./target/server
new client from 127.0.0.1:46950
client closed
new client from 127.0.0.1:46954
client closed
new client from 127.0.0.1:34420
client closed
new client from 127.0.0.1:34432
client closed
new client from 127.0.0.1:34192
client closed
```

Client

```
❯ ./target/client set x 10
(nil)
❯ ./target/client set z 20
(nil)
❯ ./target/client get x
(str) 10
❯ ./target/client get z
(str) 20
❯ ./target/client sss xxx
(err) 1 unknown command.
```