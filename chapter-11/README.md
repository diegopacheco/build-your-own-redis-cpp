### Build

```bash
./build.sh
```

### Tests

```
❯ ./target/test_offset
OK
```

### Results

```bash
./test_cmds.py
```

```
❯ ./test_cmds.py
PASS  ./target/client zscore asdf n1
PASS  ./target/client zquery xxx 1 asdf 1 10
PASS  ./target/client zadd zset 1 n1
PASS  ./target/client zadd zset 2 n2
PASS  ./target/client zadd zset 1.1 n1
PASS  ./target/client zscore zset n1
PASS  ./target/client zquery zset 1 "" 0 10
PASS  ./target/client zquery zset 1.1 "" 1 10
PASS  ./target/client zquery zset 1.1 "" 2 10
PASS  ./target/client zrem zset adsf
PASS  ./target/client zrem zset n1
PASS  ./target/client zquery zset 1 "" 0 10
```

```bash
./target/server
```

```
❯ ./target/server
new client from 127.0.0.1:50920
client closed
new client from 127.0.0.1:50330
client closed
new client from 127.0.0.1:50342
client closed
new client from 127.0.0.1:50350
client closed
new client from 127.0.0.1:50356
client closed
new client from 127.0.0.1:50360
client closed
new client from 127.0.0.1:50370
client closed
new client from 127.0.0.1:50380
client closed
new client from 127.0.0.1:50382
client closed
new client from 127.0.0.1:50388
client closed
new client from 127.0.0.1:50402
client closed
new client from 127.0.0.1:50416
client closed
new client from 127.0.0.1:50428
client closed


```