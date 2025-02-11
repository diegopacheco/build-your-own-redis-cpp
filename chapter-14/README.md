### Result

Server

```
❯ ./target/server
EOF
EOF
EOF
EOF
EOF
^C
```

Client

```
❯ ./target/client set x 100
(nil)
❯ ./target/client set y 200
(nil)
❯ ./target/client keys
(arr) len=2
(str) y
(str) x
(arr) end
❯ ./target/client get x
(str) 100
❯ ./target/client get y
(str) 200
```
