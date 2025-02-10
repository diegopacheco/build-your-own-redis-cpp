### Build

```bash
./build.sh
```

### Test Heap

```
❯ ./target/test_heap
OK
```

### Result

Server

```
❯ ./target/server
new client from 127.0.0.1:57146
client closed
new client from 127.0.0.1:57160
client closed
new client from 127.0.0.1:40836
client closed
new client from 127.0.0.1:42024
client closed
^C
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