### Result

Server

```bash
./target/server
```

```
❯ ./target/server
new client from 127.0.0.1:52850
client says: len:6 data:hello1
client says: len:6 data:hello2
client says: len:6 data:hello3
client says: len:33554432 data:zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
client says: len:6 data:hello5
client closed
^C
```

Client

```bash
./target/client
```

```
❯ ./target/client
len:6 data:hello1
len:6 data:hello2
len:6 data:hello3
len:33554432 data:zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
len:6 data:hello5
```