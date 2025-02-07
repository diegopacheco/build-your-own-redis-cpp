# build-your-own-redis-cpp

Build Your Own Redis in C/C++ book
by James Smith

## site

https://build-your-own.org/redis/

## Notes

- socket() -> fd syscall
- bind() syscall associate an address to a fd
- listen() syscall allow connections from address of the fd
- accept() and connect() creates the TCP connection
- read() reads data from syscall from socket
- write() syscall write data into socket
- SO_REUSEADDR so the server can reuse same address when restarted
- 3 ways to deal with concurrent connections:
	- forking
	- multi-threading
	- event loop (pooling and non-blocking io)
- process and threads have overheads so event loop is preferred
- 3 syscalls blocks:
	- read(): when no data in the kernel
	- write(): when the write buffer is full
	- accept(): when there are no new connections in the kernel queue
- poll is the sole blocking operation in an event loop
- fcntl is the syscall to set fd to nonblocking
- besides poll linux has epoll and select
- select is the same as poll but ancient and limit of fd is small
- epoll API:
	- epoll_create, epoll_wait, epoll_ctl
- epoll is prefered
- 2 Types of hashtables - chaining and open addressing
	- Open Addressing: seeks another slot in event collision
	- Chaining: Groups conflicting keys under a linked list
- Hash functions:
	- Cryptographic hash functions, such as MD5, SHA1.
	- Checksum hash functions, such as CRC32, Adler32.
	- Hash functions for hashtables, such as FNV, Murmur. This is what to use!
- Redis can query ordered data by rank. 
	- If you put 20M records in a sorted set
	- You can get the recorded that ranked at 10M without going trough the first 10M records
	- This feat cannot be emulated by current SQL databases
	- SortedSet super power.	