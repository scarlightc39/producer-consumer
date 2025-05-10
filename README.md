
# 🧵 Producer-Consumer Using Shared Memory and Semaphores

This project implements the classic **Producer-Consumer Problem** using **POSIX shared memory** and **semaphores** in C. It demonstrates inter-process communication and synchronization between a producer and a consumer accessing a shared buffer.

---

## 🛠️ Features

- Shared memory segment using `shmget`, `shmat`, and `shmctl`
- Semaphores (`sem_t`) for:
  - Mutual exclusion (`mutex`)
  - Tracking empty buffer slots (`empty`)
- Two-item buffer (`SIZE = 2`)
- Each process runs for 10 iterations (`ITERATIONS = 10`)
- Producer generates random integers and stores them in the shared table
- Consumer reads and consumes items from the table
- Simple output logs to trace item production and consumption

---

## 🧪 Environment

- **Language**: C
- **Platform**: Ubuntu 22.04 (running inside VirtualBox)
- **Build Tools**: GCC, pthreads
- **IPC Mechanisms Used**:
  - Shared memory (`sys/shm.h`)
  - POSIX semaphores (`semaphore.h`)

---

## 🔧 How to Compile & Run

```bash
# Compile producer and consumer
gcc producer.c -pthread -lrt -o producer
gcc consumer.c -pthread -lrt -o consumer

# Run both processes in the background
./producer & ./consumer &
```

> ✅ Make sure both programs are run on the **same system/VM** so they can access the shared memory segment properly.

---

## 📝 Notes

- The producer initializes the shared memory and semaphores.
- The consumer reads from the same shared segment.
- Use `ipcs` and `ipcrm` to check or manually clean shared memory if needed.

---

## 📷 Sample Output (Trimmed)

```
Producer thread created.
Producer entered...
Produced item 1
Item 1 produced = 103928
...
Consumer thread created.
Consumer entered...
Consumed item 1
Item 1 consumed = 103928
...
```

---

## 🧹 Cleanup

If shared memory doesn't release automatically, use:

```bash
ipcs -m     # List shared memory segments
ipcrm -m <shmid>  # Replace <shmid> with the actual ID
```

---

## 📚 References

- [Linux Man Pages - shmget, shmat, sem_init](https://man7.org/linux/man-pages/)
- Operating Systems: Three Easy Pieces — Chapter on Synchronization

---

## 👨‍💻 Author

Pratyush Barik  
Developed as part of a systems programming experiment on Ubuntu (VirtualBox).
