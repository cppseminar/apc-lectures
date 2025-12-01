# C++

## Multithreading

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Performance of current processors
* Processes and threads
* Memory model
* Synchronization
* Patterns
* Async and futures

---

# CPU

---

## MHz vs cores

![CPU Clock speed vs core count over time](./lectures/threads/50-years-processor-trend.png)<!-- .element: style="max-width: 50%;" -->

<https://github.com/karlrupp/microprocessor-trend-data/tree/master>

* CPU clock speed hit its peak around 2005 and raw performance is currently achieved through other techniques


## Modern CPU

* Small portion of CPU is actually computing stuff as in good old days of i486
* Nowadays large portion of CPU is dedicated to other tasks (branch prediction, out of order execution, caching, pipelining, etc.)
* To truly exploit performance of modern CPUs, parallelism must be exploited
    * Using vector instructions (SIMD)
    * Using multiple cores (threads)
    * Keeping the pipeline full and make branch prediction work
    * Understanding memory hierarchy and latency


## Architecture

![Cache Hierarchy](./lectures/threads/cache.jpg)<!-- .element: style="max-width: 50%;" -->

<https://www.linkedin.com/posts/anis-hassen_how-do-cpu-caches-work-cpu-caches-are-a-activity-7218663324123598848-Tmgi>


## What about GPUs?

* Outside the scope of these lectures
* GPUs are even more parallel than CPUs and require different programming model (CUDA, OpenCL, Vulkan, etc.)
* There are some libraries to use GPU from C++ (e.g. Thrust, C++ AMP, boost::compute etc.)

---

# Exploiting parallelism

---

## Parallelism in data

* SIMD (single instruction multiple data)
* SSE, AVX, etc.
* One instruction operates on multiple data points simultaneously
* Poor C++ and compiler support (getting better), hard to get right
   * [MSVC Support](https://learn.microsoft.com/en-us/cpp/parallel/auto-parallelization-and-auto-vectorization?view=msvc-170)
   * [GCC Support](https://gcc.gnu.org/projects/tree-ssa/vectorization.html#using)
* There are libraries to help
   * [NSIMD](https://github.com/agenium-scale/nsimd)
   * C++26 `#include <simd>`
   * `simdjson` for extremely fast JSON parsing


## Parallelism in algorithm

* Multi Core (threads)
* Poor compiler support (getting better), hard to get right
* Idea is simple, more CPUs result in more power
   * If one CPU compute one task X seconds, then four CPUs will compute one task in X/4 seconds, right?
* `std::execution_policy` in C++17 and later
   * `std::for_each(std::execution::par, ...)`
   * `std::transform(std::execution::par, ...)`
   * etc.


## Problems with parallel algorithms

* Designing algorithms is hard
* Designing scalable parallel algorithms is (almost) “mission impossible”
* Code tends to be longer, harder to read and reason about
* “Nondeterministic bugs”

![Abstract colorful magic box](./lectures/threads/open_box_full_of_junk.png)<!-- .element: style="max-width: 30%;" -->

---

# Processes

---

## Process

* Every process has its own memory
* Identified by unique PID (process id)
* “Inside” every process can run 1..n threads

![Threads inside a process](./lectures/threads/process-structure.png)<!-- .element: style="max-width: 50%;" -->

---

## Creating new processes

### Windows

```cpp
#include <windows.h>

int main() {
    STARTUPINFO si{ sizeof(si) };
    PROCESS_INFORMATION pi{};

    // Full path to Notepad (safer than relying on PATH)
    const wchar_t* appName = LR"(C:\Windows\System32\notepad.exe)";
    if (CreateProcess(
        appName, // executable to run
        NULL,    // command line arguments (NULL = none)
        NULL, NULL,    // security attributes for process and primary thread (NULL = default)
        FALSE,   // should child inherit handles? (FALSE = no)
        0,       // process creation flags (0 = default)
        NULL, NULL,   // environment block and working directory (NULL = parent's values)
        &si,     // startup settings for the process
        &pi      // returns process and thread handles
    )) {
        CloseHandle(pi.hProcess); // handle to process
        CloseHandle(pi.hThread);  // handle to primary thread
    } else {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "Notepad launched successfully!" << std::endl;
}
```


### Linux

```cpp
#include <unistd.h>
#include <iostream>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed!\n";
        return 1;
    } else if (pid == 0) {
        std::cout << "Child process.\n";

        // Arguments must include the command name first, and end with nullptr.
        char *const argv[] = {(char*)"/usr/bin/touch", (char*)"file.txt", nullptr};
        
        // execve requires the full path to the executable.
        execve("/usr/bin/touch", argv, nullptr); 
        
        _exit(1); // Exit if execve fails
    } else {
        std::cout << "Parent process.\n";
    }
    return 0;
}
```

---

## Waiting for process

* In previous examples we just started new process and exited immediately
* It is not ideal, because child process may outlive parent process
   * On Linux it will become child of init process (PID 1)
* There are more ways to wait for process to finish, but only one correct way


## Sleep is evil

### Windows

```cpp
Sleep(1000); // Sleep for 1 second
```

### Linux

```cpp
sleep(1); // Sleep for 1 second
```

* Even if we are sure that process will finish in given time, it may not
* Sleep is not synchronization


## Polling

### Windows

```cpp
DWORD exitCode;
do {
    GetExitCodeProcess(pi.hProcess, &exitCode);
    Sleep(100); // Sleep for 100 milliseconds
} while (exitCode == STILL_ACTIVE);
```

### Linux

```cpp
pid_t result;
do {
    int status;
    result = waitpid(pid, &status, WNOHANG);
    usleep(100000); // Sleep for 100 milliseconds
} while (result == 0);
```

* Slightly better than sleep
* Waste of CPU cycles, since if the child process is long running, we are just polling uselessly
* Busy waiting


## Proper waiting for process

### Windows

```cpp
WaitForSingleObject(pi.hProcess, INFINITE);
```

### Linux

```cpp
int status;
waitpid(pid, &status, 0);
```

* Proper synchronization
* OS will suspend parent process until child process finishes

---

## Communication between processes

* Processes do not share memory (no pointers)
* Sharing information between processes is more complicated than sharing information between threads
* Means of communication
   * Files
   * Named pipes
   * RPC / LPC
   * COM objects

---

## Processes and C++

* C++ standard doesn’t recognize concept of a process
* `boost::process`/`boost::interprocess` is more or less portable
   * Manages process creation
   * Shared memory
   * Named mutexes
   * Offset ptr


## `boost::process` example

```cpp
#include <boost/process.hpp>

int main() {
    namespace bp = boost::process;

    // Start a new process (e.g., Notepad on Windows)
    bp::child c("notepad.exe");

    // Wait for the process to finish
    c.wait();

    return 0;
}
```

---

# Threads

---

## Main thread

* Our main function starts its execution in newly created thread (system will automatically do this for us)
* This is the same thread as returned from `CreateProcess` on windows
* Identified by TID (thread id) – from same enumeration as PID on Windows
* Code is only executed in threads

![Some threads to illustrate](./lectures/threads/random-threads.jpg)

---

## Starting new thread

### Windows

```cpp
#include <windows.h>

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    return 0; // Thread exit code here
}

int main() {
    HANDLE hThread;
    DWORD threadId;

    hThread = CreateThread(
        NULL,             // default security attributes
        0,                // default stack size
        ThreadFunc,       // thread function name
        NULL,             // argument to thread function
        0,                // default creation flags
        &threadId);       // returns the thread identifier

    // Wait for the thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Close the thread handle
    CloseHandle(hThread);

    return 0;
}
```


### Linux

```cpp
#include <pthread.h>

void* thread_func(void* arg) {
    return (void*)0; // exit code is number as void*
}

int main() {
    // Create a new thread
    pthread_t thread;
    if (pthread_create(&thread, nullptr, thread_func, nullptr) != 0) {
        // Handle error
        return 1;
    }

    // Wait for the thread to finish
    pthread_join(thread, nullptr);
}
```

---

## OS Scheduler

* Application has limited control over scheduling of its threads
* OS scheduler is neither predictable, nor fair
* You should not rely on any specific scheduling behavior, however some OS provide APIs to set thread priorities or affinities

![Round robin -ish](./lectures/threads/A_bright_little_robin.jpg)<!-- .element: style="max-width: 50%;" -->

<https://commons.wikimedia.org/wiki/File:A_bright_little_robin!_%288420315404%29.jpg>

---

## Multitasking

* Cooperative multitasking
   * Threads must explicitly yield
   * OS will never interrupt a thread
   * DOS era
* Preemptive multitasking
   * OS schedules the threads
   * When the time slice for a particular thread ends, the OS suspends it and starts another ready thread
   * Threads can also yield
   * Default on modern OSes

---

## Passing parameters to thread

* Let's try Linux pthread example with parameters, Windows implementation is similar

```cpp
struct data {
    int number;
    std::string text;
};

// Function executed by the new thread
void* thread_func(void* arg) {
    data* data_ptr = static_cast<data*>(arg);

    if (data_ptr) {
        std::cout << "Child Thread: Number: " << data_ptr->number << ", Text: " << data_ptr->text << std::endl;
        
        // **CRITICAL STEP: CONSUMPTION / CLEANUP**, beware of exceptions
        delete data_ptr;
    } else {
        std::cerr << "Child Thread: Error - Received null data pointer.\n";
    }

    return (void*)0;
}
```


```cpp
int main() {
    // DYNAMICALLY ALLOCATE the data object
    data* data_ptr = new data{42, "Hello, World!"};

    pthread_t thread;
    if (pthread_create(&thread, nullptr, thread_func, data_ptr) != 0) {
        std::cerr << "Error creating thread.\n";
        delete data_ptr; // do not leak
        return 1;
    }

    if (pthread_join(thread, nullptr) != 0) {
        std::cerr << "Error joining thread.\n";
        return 1;
    }

    std::cout << "Main Thread: Thread finished and object consumed.\n";
    return 0; // no need to delete data_ptr here
}
```

---

## C++ and threads

* Until C++11: What threads?
* Since C++11:
   * Standard threads are called execution agents (`std::thread`)
   * Memory model is clearly and portably defined
   * Advanced thread support and synchronization primitives
   * `std::jthread` (C++20)
   * `std::async`

---

# `std::thread`

---

## Simple `std::thread` example

```cpp
#include <thread>

void thread_func(std::string message) {
    try {
        // Thread code here
        std::cout << "Thread says: " << message << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

int main() {
    try {
        // Start a new thread
        std::thread t(thread_func, "Hello from std::thread!");

        // Wait for the thread to finish
        t.join();
    } catch (const std::system_error& e) {
        std::cerr << "Error creating thread: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## `std::thread` and exceptions

* If thread function throws an exception and it is not caught, `std::terminate` is called
* Always catch exceptions inside thread function, at least to log them
* Constructor of `std::thread` may throw `std::system_error` if thread cannot be created
* To test if thread is associated with execution agent, use `joinable()` method
* `join()` throws `std::system_error` if
   * Thread is not joinable
   * Deadlock is detected (joining itself)
   * If the underlying OS call fails

---

## What to do in destructor?

* Join or not to join?
* If `std::thread` has associated execution agent in destructor (`joinable() == true`), `std::terminate()` will be executed.
* Generally bad stuff
* Always join or detach the thread before destructor is called

```cpp
std::thread t(thread_func);

t.detach(); // or t.join();
```

---

## Multiple parameters

* We can do it as in posix threads with `struct`
* From now on, `try ... catch` blocks are omitted for clarity

```cpp
struct data {
    int number;
    std::string text;
};

void thread_func(data* d) {
    std::unique_ptr<data> data_ptr(d); // take ownership (ensure cleanup)
    std::cout << "Thread: Number: " << d->number << ", Text: " << d->text << std::endl;
}

int main() {
    auto data = std::make_unique<data>(42, "Hello, World!");
    std::thread t(thread_func, data.release()); // release ownership
    t.join();
}
```

---

## Type safe parameters

* Parameters are forwarded to thread object

```cpp
void thread_func(int number, const std::string& text) {
    std::cout << "Thread: Number: " << number << ", Text: " << text << std::endl;
}

int main() {
    int num = 42;
    std::string str = "Hello, World!";
    std::thread t(thread_func, num, str); // copies of num and str are passed
    t.join();
}
```


## Rvalue references?

```cpp
void thread_func(std::string&& text) {
    std::cout << "Thread: Text: " << text << std::endl;
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, std::move(str)); // str is moved
    t.join();
}
```

* String is moved to thread function


### But this also works...

```cpp
void thread_func(std::string&& text) {
    std::cout << "Thread: Text: " << text << std::endl;
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, str); // str is copied
    t.join();
}
```


### Parameters are always forwarded as temporaries

* `std::thread` always keep copies of parameters
* Thread entry point is called as if the parameters are temporaries
* So you can move or copy values to thread object (this is done in `std::thread` constructor)
* Your thread function will always be called with temporaries, so 
   * rvalue references and 
   * const lvalue references and 
   * values 
  will bind correctly
* But lvalue references will not bind!

```cpp
void thread_func(std::string& text) {
    std::cout << "Thread: Text: " << text << std::endl;
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, str); // ERROR: cannot bind temporary to lval reference
    t.join();
}
```


### Use `std::ref` to pass lvalue references

```cpp
#include <functional> // for std::ref

void thread_func(std::string& text) {
    std::cout << "Thread: Text: " << text << std::endl;
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, std::ref(str)); // OK: pass lval reference
    t.join();
}
```

* Here `std::ref` creates a wrapper that tells `std::thread` to pass the reference instead of making a copy
* Make sure that the referenced object outlives the thread, otherwise you will have dangling reference and undefined behavior
* Or obviously you can use pointers and manage lifetime manually

---

## Interthread communication

* Threads share memory!
* Pointers can be passed around, so no problem to share data between threads
* Thread can communicate through shared objects
* C++ has no special support for interthread communication (unlike for example channels in Go)
* Third-party libraries exist (e.g. `boost::asio`)

---

## `this_thread` namespace

* Namespace with functions to operate on current thread (sleep, yield)

```cpp
#include <thread>

void thread_func() {
    // Sleep for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Yield execution to another thread, hint to scheduler
    std::this_thread::yield();

    // Get current thread id
    std::thread::id this_id = std::this_thread::get_id();
}

int main() {
    std::thread t(thread_func);
    t.join();
}
```

---

# Memory model

---

## Accessing memory

* Race condition
* Accessing one resource from multiple thread without proper locking
* Results in undefined behavior
* Strange, hard to debug problems

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter; // race condition
    }
}

int main() {
    std::thread t1(increment), t2(increment);

    t1.join();
    t2.join();

    std::cout << "Final counter value: " << counter << std::endl;
}
```


## HW point of view

* The result of the previous code is undefined, in practice it can be anywhere between 100000 and 200000.
* Write instruction can be non atomic
   * Instructions are broken down to microcode and then are executed
   * Interleaving writes can modify memory inconsistently
* Every core has its own cache
   * Greatly speed up things
   * Changes in one thread are not propagated to another cores

---

## Example of race

```cpp
int a, b; // global, so zero initialized

void func() {
    std::cout << a; // ok
    b += 1; // not OK, must be synchronized
}

int main() {
    std::thread t1(func), t2(func);

    t1.join();
    t2.join();
}
```

---

## How to avoid?

* We can only use one (main) thread, or just use threads for parallel algorithms (e.g. `std::execution_policy`)
* Even if threads only work on disjoint data we are safe
* If threads share data, we must synchronize access to shared data

---

## Synchronization

* C++ provides synchronization primitives to avoid race conditions
* `std::mutex`
* `std::lock_guard`
* `std::barrier`
* `std::condition_variable`
* ...

---

## Contract with the C++ standard

* C++11 standard defines memory model for multithreaded programs
* Defines what is and what is not allowed
* The rule is simple: 

**One addressable memory location (no bit arrays) can be accessed by two or more threads simultaneously only if all accesses are read-only.**

* Basically either one writer or multiple readers are allowed on any given memory location
* We must ensure this, otherwise our program has undefined behavior

---

## `std::mutex`

* Mutual exclusion primitive
* Only one thread can own the mutex at a time
* Other threads trying to lock the mutex will be blocked until it is unlocked
* Under the hood, OS synchronization primitives are used (e.g. critical sections on Windows, futexes on Linux)

```cpp
std::mutex mtx;
int a;

void func() {
    mtx.lock();
    a += 1; // safe to run from multiple threads
    mtx.unlock();
}
```


## `try_lock`

* Attempts to lock the mutex without blocking
* Returns true if the lock was acquired, false otherwise

```cpp
std::mutex mtx;

int a;

void func() {
    if (mtx.try_lock()) {
        a += 1; // safe to run from multiple threads
        mtx.unlock();
    } else {
        // could not acquire lock, do something else
    }
}
```


## RAII with `std::lock_guard`

* Manually locking and unlocking mutex is error prone
* If an exception is thrown between lock and unlock, mutex will remain locked, causing deadlock

```cpp
std::mutex mtx;
int a;

void func() {
    std::lock_guard<std::mutex> lock(mtx); // locks mutex
    a += 1; // safe to run from multiple threads
} // mutex is automatically unlocked when lock goes out of scope
```


## or with `std::unique_lock`

* More flexible than `std::lock_guard`
* Can be locked and unlocked multiple times

```cpp
std::mutex mtx;
int a;

void func() {
    std::unique_lock<std::mutex> lock(mtx); // locks mutex
    a += 1; // safe to run from multiple threads
    lock.unlock(); // unlocks mutex

    // do some other work

    lock.lock(); // locks mutex again
    a += 1; // safe to run from multiple threads
} // mutex is automatically unlocked when lock goes out of scope
```

---

## Deadlock

* Happens when two or more threads are waiting for each other to release resources
* Results in all threads being blocked indefinitely

```cpp
std::mutex mtx1, mtx2;

void func1() {
    std::lock_guard<std::mutex> lock1(mtx1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work
    std::lock_guard<std::mutex> lock2(mtx2); // deadlock here if func2 holds mtx2
}

void func2() {
    std::lock_guard<std::mutex> lock2(mtx2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work
    std::lock_guard<std::mutex> lock1(mtx1); // deadlock here if func1 holds mtx1
}
```


## Avoiding deadlock

* Always lock multiple mutexes in the same order
* Use `std::lock` to lock multiple mutexes at once

```cpp
std::mutex mtx1, mtx2;

void func1() {
    std::lock(mtx1, mtx2);
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // do work
}
```

* Using `std::lock` avoids deadlock by locking both mutexes atomically, it will try to lock both and if it cannot lock both, it will unlock any locked mutex and try again (with backoff)
* `std::adopt_lock` tells `std::lock_guard` that the mutex is already locked, so it should not try to lock it again just unlock it when going out of scope

---

## Other solutions to `int` synchronization

### Atomic operations

* `std::atomic` types provide atomic operations on variables
* No need for mutexes for simple operations
* Under the hood, CPU atomic instructions are used, they are usually faster than mutexes and ensure proper memory visibility between threads

```cpp
#include <atomic>

std::atomic<int> a(0);

void func() {
    a += 1; // atomic increment
}
```


### `volatile` keyword

* `volatile` tells the compiler that the variable can be changed by external factors (e.g. hardware, other threads)
* Prevents compiler optimizations that assume variable does not change unexpectedly
* Does NOT provide any synchronization or atomicity
* Rarely useful in multithreaded programming, prefer `std::atomic` instead


### Transactional memory

* C++ introduces technical specification for support for transactional memory (not widely supported yet)
* Allows grouping multiple memory operations into a single atomic transaction
* If transaction cannot be completed (e.g. due to conflict with another thread), it can be retried

---

## `thread_local` storage

* Variables declared with `thread_local` specifier have separate instances for each thread

```cpp
thread_local std::string thread_id = "0";

void func() {
    thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::cout << "Thread ID: " << pthread_self() << " hash " << thread_id << std::endl;
}
```

---

# Other primitives

---

## `std::call_once`

* Ensures that a piece of code is executed only once, even in the presence of multiple threads

```cpp
#include <mutex>

std::once_flag flag;

void init(int i, std::string s) {
    // initialization code here
}

void func() {
    std::call_once(flag, init, 42, "example"); // init will be called only once
}
```

* If multiple threads call `func`, only one will execute `init`, others will wait until initialization is complete


## Exceptions and `std::call_once`

* If exception is thrown during initialization, subsequent calls to `std::call_once` will retry the initialization (if another thread is waiting, it will be unblocked and will try to call the function again)
* On one flag multiple different functions can be called, but only one will succeed, others will be ignored

```cpp
std::once_flag flag;

void init1() {
    // initialization code here
}

void init2() {
    // different initialization code here
    throw std::runtime_error("Initialization failed");
}

void func1() {
    std::call_once(flag, init1); //  since it doesn't throw, it will succeed
}

void func2() {
    std::call_once(flag, init2); // subsequent calls will retry
}
```

---

## `hardware_concurrency()`

* Returns the number of hardware threads available on the system
* It is only a hint, the actual number of threads that can run concurrently may be different
* On my system it returns 14 (2 performance cores with hyperthreading + 8 efficiency cores + 2 low power cores)

```cpp
unsigned int n = std::thread::hardware_concurrency();
std::cout << "Number of hardware threads: " << n << std::endl;

std::vector<std::thread> threads;
for (unsigned int i = 0; i < n; ++i) {
    threads.emplace_back([]() {
        // thread work here
    });
}

for (auto& t : threads) {
    t.join();
}
```

---

## `std::shared_mutex`

* Allows multiple threads to read shared data simultaneously, but only one thread can write to the data at a time
* Two new methods: `lock_shared()` and `unlock_shared()` for shared (read) access
* Basic `lock()` and `unlock()` are for exclusive (write) access
* Useful for read-heavy scenarios where reads are more frequent than writes
* New helper classes: `std::shared_lock` for shared locking (`std::unique_lock` is for exclusive locking)


```cpp
#include <shared_mutex>

std::shared_mutex rw_mutex;
int shared_data;

void reader() {
    std::shared_lock<std::shared_mutex> lock(rw_mutex); // shared lock for reading
    std::cout << "Read data: " << shared_data << std::endl;
}

void writer() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex); // exclusive lock for writing
    shared_data += 1;
}
```

---

## Other flavors of mutexes

* `std::timed_mutex`
   * Mutex with timeout capabilities
   * `try_lock_for()`, `try_lock_until()` methods
* `std::recursive_mutex`
   * Mutex that can be locked multiple times by the same thread
   * It must be unlocked the same number of times it was locked
* `std::recursive_timed_mutex`
   * Combination of recursive and timed mutex
* `std::shared_timed_mutex`
   * Shared mutex with timeout capabilities

---

## `std::semaphore` (C++20)

* Counting semaphore or binary semaphore
* Controls access to a shared resource through a counter
* `acquire()` decreases the counter, blocking if the counter is zero
* `try_acquire()` tries to decrease the counter without blocking
* `release()` increases the counter, potentially unblocking waiting threads

```cpp
#include <semaphore>

std::counting_semaphore<5> sem(5); // max 5 concurrent accesses

void access_resource() {
    sem.acquire(); // wait for access
    // access shared resource here, max 5 threads can be here simultaneously
    sem.release(); // release access
}
```


## `std::binary_semaphore` (C++20)

* `using binary_semaphore = std::counting_semaphore<1>;`
* Similar to mutex, but with semaphore semantics
* Mutex can only be owned by one thread at a time, while binary semaphore can be released by a different thread than the one that acquired it
* So binary semaphore is useful for signaling between threads

```cpp
#include <semaphore>

std::binary_semaphore sem(0); // initially unavailable

void worker() {
    // do some work
    sem.release(); // signal that work is done
}

void waiter() {
    sem.acquire(); // wait for signal
    // proceed after work is done
}
```

---

## `std::latch` and `std::barrier` (C++20)

* Synchronization primitives for coordinating multiple threads
* Can be implemented using semaphores or condition variables, these are just higher level abstractions
* `std::latch`
   * A synchronization primitive that allows one or more threads to wait until a set of operations being performed in other threads completes
   * Once the counter reaches zero, it cannot be reset
* `std::barrier`
   * A synchronization primitive that allows a set of threads to wait for each other to reach a common barrier point
   * The barrier can be reused multiple times


## `std::latch`

* Constructed with a counter
* Threads call `count_down()` to decrement the counter
* Threads call `wait()` to block until the counter reaches zero
* Once the counter reaches zero, all waiting threads are unblocked
* There is `try_wait()` method to check if the counter is zero without blocking
* And `arrive_and_wait()` method that combines `count_down()` and `wait()`


## `std::latch` example

```cpp
constexpr int NUM_WORKERS = 5; // Define the number of worker tasks
std::latch worker_latch(NUM_WORKERS);

void worker_task(int id) {
    // Simulate initialization with varying durations
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (NUM_WORKERS - id + 1))); 

    // The worker signals that one task is complete and continues
    worker_latch.count_down();
    
    // The worker is now free to continue its independent work
    std::cout << "Worker " << id << ": Resuming background job.\n";
}

int main() {
    std::vector<std::thread> workers;
    for (int i = 1; i <= NUM_WORKERS; ++i) {
        workers.emplace_back(worker_task, i); 
    }

    worker_latch.wait(); // Blocks until the latch counter reaches zero.
    
    std::cout << "Every worker finished setup and called count_down().\n";
    
    for (auto& t : workers) {
            t.join();
    }
}
```

---

## `std::barrier`

* Constructed with a count and an optional completion function
* Threads call `arrive_and_wait()` to signal they have reached the barrier and wait for others
* Once the specified number of threads have called `arrive_and_wait()`, the optional completion function is executed, and all threads are unblocked
* Arrive and wait can be split to `arrive()` and `wait()` calls, where arrive signals reaching the barrier and wait blocks until all threads arrive (output of `arrive` must be passed to `wait`), however this is rarely used
* Call to `arrive_and_drop()` decrements the counter and removes the calling thread from the barrier (it will not wait for it anymore), this is useful for dynamic number of threads


## `std::barrier` example

```cpp
#include <barrier>

constexpr int WORKERS = 4; // 4 workers

auto on_completion = [phase_count = 0]() mutable noexcept {
    phase_count++;
    std::cout << "Starting Phase " << phase_count << "\n";
};

std::barrier sync_point(WORKERS, on_completion);

void worker(int id) {
    std::cout << "Data Worker " << id << ": Performing initial setup.\n";
    if (id % 2 == 0) {
        sync_point.arrive_and_wait();
    } else {
        sync_point.arrive_and_drop(); // odd workers drop out after first phase
        return; // if we keep thread participate, strange things happen
    }

    std::cout << "Data Worker " << id << ": Starting work.\n";
    sync_point.arrive_and_wait(); 

    std::cout << "Data Worker " << id << ": All work done.\n";
}
```


```cpp
int main() {
    std::vector<std::thread> threads;
    std::generate_n(std::back_inserter(threads), WORKERS, [n = 0]() mutable {
        return std::thread(worker, n++);
    });
    
    std::for_each(threads.begin(), threads.end(), [](std::thread& t) {
        t.join();
    });
}
```

---

## `std::jthread` (C++20)

* Joining thread (or Josuttis thread 😂)
* Similar semantics as `std::thread`, but with some caveats
   * Automatically joins in destructor (no need to call `join()` manually)
   * Supports cooperative cancellation through `std::stop_token`

```cpp
#include <thread>

int main() {
    std::jthread jt([]() {
        // thread work here
    }); // automatically joined when jt goes out of scope
}
```


## `request_stop`

* `std::jthread` supports cooperative cancellation
* There is even a way to use one request stop token for multiple threads
* If callback passed to `std::jthread` takes `std::stop_token` as first, it will be provided automatically

```cpp
#include <thread>

int main() {
    std::jthread jt([i = 0](std::stop_token st) mutable {
        while (!st.stop_requested()) {
           std::cout << i++ << "\n";

           std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Request the thread to stop
    jt.request_stop(); // will even be called automatically by destructor alongside join
}
```

---

## `std::condition_variable`

* Synchronization primitive that allows threads to wait for certain conditions to be met
* Used in conjunction with `std::mutex` to protect shared data
* `wait()` method blocks the calling thread until notified
* `notify_one()` wakes up one waiting thread
* `notify_all()` wakes up all waiting threads
* Typically used for producer-consumer scenarios


## Producer-consumer example

```cpp
// Shared resources
std::queue<int> buffer; // where produced items are stored
std::mutex mtx; // mutex to protect shared buffer
std::condition_variable cv; // condition variable for synchronization

// we will produce and consume fixed number of items
constexpr int MAX_BUFFER_SIZE = 10;
constexpr int ITEMS_TO_PRODUCE = 20;

int main() {
    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    std::cout << "Main: All threads finished.\n";
    return 0;
}
```


### Producer thread

```cpp
void producer() {
    for (int i = 1; i <= ITEMS_TO_PRODUCE; ++i) {
        {
            // Lock the mutex before checking the condition (buffer size)
            std::unique_lock<std::mutex> lock(mtx);
            
            // Wait while the buffer is full. 
            // The wait() function releases the lock while sleeping and reacquires it on wakeup.
            cv.wait(lock, []{ return buffer.size() < MAX_BUFFER_SIZE; });

            // Produce an item
            buffer.push(i);
            std::cout << "P: Produced item " << i << ". Buffer size: " << buffer.size() << std::endl;
        } // Lock is automatically released here

        // simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100) * i % 3);

        // Notify one waiting consumer that new data is available
        cv.notify_one(); 
    }
    std::cout << "P: Finished producing all items.\n";
}
```


### Consumer thread

```cpp
void consumer() {
    int consumed_count = 0;
    while (consumed_count < ITEMS_TO_PRODUCE) {
        int data = 0;
        {
            // Lock the mutex before checking the condition (buffer empty)
            std::unique_lock<std::mutex> lock(mtx);

            // Wait while the buffer is empty. 
            // The wait() function releases the lock while sleeping and reacquires it on wakeup.
            cv.wait(lock, []{ return !buffer.empty(); });
            
            // Consume an item
            data = buffer.front();
            buffer.pop();
            consumed_count++;

            std::cout << "C: Consumed item " << data << ". Buffer size: " << buffer.size() << std::endl;
        } // Lock is automatically released here

        // Notify the producer that space is now available
        cv.notify_one(); 

        // simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100) * data % 2);
    }
    std::cout << "C: Finished consuming all items.\n";
}
```


### `wait` without predicate

* If `wait` is called without a predicate, it may wake up even if the condition is not met (spurious wakeup)
* Always use the predicate version of `wait` or check the condition in a loop

```cpp
cv.wait(lock); // may wake up spuriously

// Always recheck the condition in a loop
while (buffer.size() >= MAX_BUFFER_SIZE) {
    cv.wait(lock); // recheck condition
}
```

---

# Patterns

---

## Double checked locking

* Problem: We want to initialize a singleton object in a thread-safe manner.
* Something like following code, but with lazy initialization and without compiler error 😏

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        return instance;
    }
private:
    Singleton() = default;
    static Singleton instance;
};
```


## Lazy initialization

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        if (instance == nullptr) // first check 
            instance = new Singleton();

        return *instance;
    }
private:
    Singleton() = default;
    static Singleton* instance;
};
```

<ul>
<li class="fragment"> Problem: multiple threads may call <code>get_instance</code> simultaneously, resulting in multiple instances being created</li>
<li class="fragment"> Problem: When to delete the instance?</li>
</ul>


### Deleting

* We can use `std::atexit` to register a cleanup function that will delete the instance when the program exits
* In next slides we will omit cleanup for clarity

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        if (instance == nullptr) {
            instance = new Singleton();
            std::atexit(cleanup);
        }
        return *instance;
    }
private:
    static void cleanup() {
        delete instance;
        instance = nullptr;
    }
    Singleton() = default;
    static Singleton* instance;
};
```


## Using mutex to synchronize

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        std::lock_guard<std::mutex> lock(mtx); // lock mutex
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return *instance;
    }
private:
    Singleton() = default;
    static Singleton* instance;
    static std::mutex mtx;
};
```

* This works, but mutex is locked every time `get_instance` is called, even if the instance is already created

<!-- .element: class="fragment" -->


## Double checked locking

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        if (instance == nullptr) { // first check (no lock)
            std::lock_guard<std::mutex> lock(mtx); // lock mutex
            if (instance == nullptr) { // second check (with lock)
                instance = new Singleton();
            }
        }
        return *instance;
    }
private:
    Singleton() = default;
    static Singleton* instance;
    static std::mutex mtx;
};
```

* Mutex is only locked when instance is not yet created
<!-- .element: class="fragment" -->
* This is almost correct, but still has problems due to compiler optimizations and CPU reordering
<!-- .element: class="fragment" -->
* Instance pointer may be assigned before the constructor is finished, so another thread may see a non-nullptr instance that is not fully constructed
<!-- .element: class="fragment" -->


## Using `std::atomic`

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        Singleton* temp = instance.load(std::memory_order_acquire); // atomic load
        if (temp == nullptr) { // first check (no lock)
            std::lock_guard<std::mutex> lock(mtx); // lock mutex
            temp = instance.load(std::memory_order_relaxed);
            if (temp == nullptr) { // second check (with lock)
                temp = new Singleton();
                instance.store(temp, std::memory_order_release); // atomic store
            }
        }
        return *temp;
    }
private:
    Singleton() = default;
    static std::atomic<Singleton*> instance;
    static std::mutex mtx;
};
```


* Using `std::atomic` for the instance pointer ensures proper memory visibility between threads
* `memory_order_acquire` ensures that subsequent reads are not reordered before the load
* `memory_order_release` ensures that previous writes are not reordered after the store
* This is the correct implementation of double checked locking in C++ (if it would compile, since the static member definitions are missing from source files)


## Using `static` local variable

* Forget everything, just use `static` local variable

```cpp
class Singleton {
public:
    static Singleton& get_instance() {
        // initialized only once in a thread-safe manner
        // it is up to compiler to synchronize it with different threads
        // as a bonus, it is destroyed automatically at program exit
        static Singleton instance;
        return instance;
    }
private:
    Singleton() = default;
};
```

---

## Thread safe cache

* Imagine we have a function that is expensive to compute and it needs to be called multiple times with the same parameters from different threads
* It would be nice to cache the results of the function calls to avoid recomputing them

```cpp
class ThreadSafeCache {
public:
    int compute(int x) { // obviously not thread safe
        if (cache.find(x) != cache.end()) {
            return cache[x]; // return cached result
        }

        int result = expensive_function(x);

        cache[x] = result; // store result in cache
        return result;
    }
private:   
    std::map<int, int> cache;
    std::mutex mtx;
};
```


## Making it thread safe

```cpp
class ThreadSafeCache {
public:
    int compute(int x) {
        std::unique_lock<std::mutex> lock(mtx);
        if (cache.find(x) != cache.end()) {
            return cache[x]; // return cached result
        }

        lock.unlock(); // unlock mutex before expensive computation
        int result = expensive_function(x);

        lock.lock(); // lock mutex again before updating cache
        cache[x] = result; // store result in cache, do not care it overwrites

        return result;
    }
private:
    std::map<int, int> cache;
    std::mutex mtx;
}; 
```


## If there are way more reads than writes

```cpp
class ThreadSafeCache {
public:
    int compute(int x) {
        {
            std::shared_lock<std::shared_mutex> lock(rw_mutex); // shared lock for reading
            if (cache.find(x) != cache.end()) {
                return cache[x]; // return cached result
            }
        } // release shared lock

        int result = expensive_function(x);

        {
            std::unique_lock<std::shared_mutex> lock(rw_mutex); // exclusive lock for writing
            cache[x] = result; // store result in cache, do not care it overwrites
        } // release exclusive lock

        return result;
    }
private:
    std::map<int, int> cache;
    std::shared_mutex rw_mutex;
}; 
```


## If values are expensive to copy

```cpp
class ThreadSafeCache {
public:
    std::shared_ptr<std::string> compute(int key) {
        {
            std::shared_lock<std::shared_mutex> lock(rw_mutex); // shared lock for reading
            auto it = cache.find(key);
            if (it != cache.end()) {
                return it->second; // return cached result
            }
        } // release shared lock

        auto result = std::make_shared<ValueType>(expensive_function(key));

        {
            std::unique_lock<std::shared_mutex> lock(rw_mutex); // exclusive lock for writing
            cache[key] = result; // store result in cache, do not care it overwrites
        } // release exclusive lock

        return result;
    }
private:
    std::map<int, std::shared_ptr<std::string>> cache;
    std::shared_mutex rw_mutex;
};
```


## Making it `const`?

* We can make `compute` method `const` by marking the mutex and cache as `mutable`
* This allows us to modify them even in `const` methods
* This is useful for caching, as the logical state of the object does not change from the perspective of the caller
* Also this is one way of thinking about `const` correctness in multithreaded scenarios
   * Either the object is logically `const` (read-only) and can be accessed from multiple threads simultaneously
   * Or not and it must be synchronized externally
* The Standard library uses this approach, so every `const` method of standard containers is thread safe for reading from multiple threads
* Non-`const` methods are not thread safe and must be synchronized externally

---

## Channel

```cpp
class Channel {
public:
    // Constructor for a potentially bounded buffer
    Channel(size_t capacity = 0) : capacity_(capacity), closed_(false) {}

    // Sends a value to the channel. Blocks if the buffer is full.
    // value The string value to send.
    // throws std::runtime_error if the channel is closed.
    void send(std::string value);

    // Receives a value from the channel. Blocks if the buffer is empty.
    // return An optional string containing the received value, 
    //        or std::nullopt if the channel is closed and empty.
    std::optional<std::string> receive();

    // Closes the channel.
    void close();

private:
    std::queue<std::string> buffer_; // internal buffer
    std::mutex mtx_;
    std::condition_variable cv_producer_; 
    std::condition_variable cv_consumer_; 
    const size_t capacity_; // max buffer size, 0 for unbounded
    bool closed_; // indicates if the channel is closed
};
```

note: Two condition variables are used to separately notify producers and consumers, improving efficiency. There may be more than one waiting producer or consumer at a time, so if item is consumed only producers should be notified and vice versa.


```cpp
void send(std::string value) {
    {
        std::unique_lock<std::mutex> lock(mtx_);

        // Block if the buffer is full (for bounded channels)
        cv_producer_.wait(lock, [this] {
            return closed_ || capacity_ == 0 || buffer_.size() < capacity_;
        });

        // we do not allow sending to a closed channel
        if (closed_) {
            throw std::runtime_error("Channel closed while waiting to send.");
        }
        buffer_.push(std::move(value));
    }

    // Notify one waiting consumer that new data is available
    cv_consumer_.notify_one();
}
```


```cpp
std::optional<std::string> receive() {
    std::optional<std::string> result; // Default-constructed, initially empty
    {
        std::unique_lock<std::mutex> lock(mtx_);

        // Block if the buffer is empty AND the channel is not closed
        cv_consumer_.wait(lock, [this] {
            return closed_ || !buffer_.empty();
        });

        if (buffer_.empty()) {
            // If buffer is empty and channel is closed, return the empty optional
            return std::nullopt; 
        }

        // Consume the item: move it directly into the optional
        result.emplace(std::move(buffer_.front()));
        buffer_.pop();
    }

    cv_producer_.notify_one();

    return result; // Return the optional containing the received string
}
```


```cpp
void close() {
    std::unique_lock<std::mutex> lock(mtx_);
    closed_ = true;
    lock.unlock();

    cv_producer_.notify_all();
    cv_consumer_.notify_all();
}
```


```cpp
int main() {
    Channel chan(5); // bounded channel with capacity 5

    // Producer thread
    std::jthread producer([&chan]() {
        for (int i = 0; i < 10; ++i) {
            chan.send("Message " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        chan.close(); // close channel after sending all messages
    });

    // Consumer thread
    std::jthread consumer([&chan]() {
        while (true) {
            auto msg = chan.receive();
            if (!msg.has_value()) {
                break; // channel is closed and empty
            }
            std::cout << "Received: " << *msg << std::endl;
        }
    });

    // Threads will be joined automatically when going out of scope
}
```

---

# `std::async` and futures

---

## `std::async`

* Launches a function asynchronously (potentially in a new thread) and returns a `std::future` representing the result of the function

```cpp
#include <future>

int compute_factorial(int n) {
    if (n <= 1) return 1;
    return n * compute_factorial(n - 1);
}

std::future<int> fut = std::async(compute_factorial, 15);

int result = fut.get(); // blocks until the result is available
```


## Exceptions in `std::async`

* If the function passed to `std::async` throws an exception, it is stored in the `std::future`
* Calling `get()` on the future will rethrow the exception in the calling thread

```cpp

std::future<int> fut = std::async([]() {
    throw std::runtime_error("Error in async task");
    return 42;
});

try {
    int result = fut.get(); // will rethrow the exception
} catch (const std::exception& e) {
    std::cout << "Caught exception: " << e.what() << std::endl;
}
```


## Execution policies

* `std::async` supports different launch policies
   * `std::launch::async` - always run in a new thread
   * `std::launch::deferred` - run only when `get()` or `wait()` is called, in the calling thread
   * Default is `std::launch::async | std::launch::deferred`, allowing the implementation to choose

```cpp
std::future<int> fut = std::async(std::launch::deferred, compute_factorial, 15);

int result = fut.get(); // compute_factorial runs here (in the calling thread) and now
```


## Manually using `std::promise` and `std::future`

* `std::promise` allows setting a value or exception that can be retrieved through a `std::future`

```cpp
#include <future>

std::promise<int> prom;
std::future<int> fut = prom.get_future();

std::jthread worker([&prom]() {
    try {
        int result = compute_factorial(15);
        prom.set_value(result); // set the result
    } catch (...) {
        prom.set_exception(std::current_exception()); // set the exception
    }
});

int result = fut.get(); // blocks until the result is available
```


## Future destructor

* If a `std::future` is destroyed before `get()` or `wait()` is called, and the associated asynchronous task has not completed, the destructor will block **only** if the task was launched with `std::async`
* Otherwise, it will neither block nor terminate, so it may lead to subtle bugs
* Try to always call `get()` or `wait()` before destroying the future 

```cpp
{
    std::future<int> fut = std::async(compute_factorial, 15);
    // do some other work
} // destructor of fut will block until compute_factorial finishes
```

```cpp
{
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    // do some other work
} // destructor of fut will NOT block, may lead to bugs
```

---

# ĎAKUJEM

## Otázky?