# C++

## Viacvláknové programovanie

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Výkon súčasných procesorov
* Procesy a vlákna
* Pamäťový model
* Synchronizácia
* Návrhové vzory
* Asynchrónne výpočty a futures

---

# CPU

---

## MHz verzus jadrá

![Vývoj taktovacej frekvencie CPU a počtu jadier v čase](./lectures/threads/50-years-processor-trend.png)<!-- .element: style="max-width: 50%;" -->

<https://github.com/karlrupp/microprocessor-trend-data/tree/master>

* Taktovacia frekvencia CPU dosiahla vrchol okolo roku 2005 a ďalší rast výkonu sa dnes dosahuje inými technikami


## Moderné CPU

* Len malá časť CPU skutočne vykonáva výpočty tak ako za starých dobrých čias i486
* Dnes je veľká časť CPU vyhradená na iné úlohy (predikcia vetvenia, vykonávanie mimo poradia, cache, zreťazené spracovanie atď.)
* Ak chceme naplno využiť výkon moderných CPU, musíme využiť paralelizmus
    * Používať vektorové inštrukcie (SIMD)
    * Využívať viacero jadier (vlákien)
    * Udržiavať pipeline plnú a pomáhať predikcii vetvenia
    * Rozumieť hierarchii pamäte a latencii


## Architektúra

![Hierarchia cache](./lectures/threads/cache.jpg)<!-- .element: style="max-width: 50%;" -->

<https://www.linkedin.com/posts/anis-hassen_how-do-cpu-caches-work-cpu-caches-are-a-activity-7218663324123598848-Tmgi>


## A čo GPU?

* Mimo rozsahu týchto prednášok
* GPU sú ešte paralelnejšie ako CPU a vyžadujú iný programovací model (CUDA, OpenCL, Vulkan atď.)
* Existujú knižnice na využitie GPU z C++ (napr. Thrust, C++ AMP, boost::compute atď.)

---

# Využitie paralelizmu

---

## Dátový paralelizmus

* SIMD (single instruction multiple data, jedna inštrukcia nad viacerými údajmi)
* SSE, AVX atď.
* Jedna inštrukcia spracúva viacero údajov súčasne
* Slabá podpora v C++ a kompilátoroch (zlepšuje sa), správne použitie je náročné
    * [Podpora v MSVC](https://learn.microsoft.com/en-us/cpp/parallel/auto-parallelization-and-auto-vectorization?view=msvc-170)
    * [Podpora v GCC](https://gcc.gnu.org/projects/tree-ssa/vectorization.html#using)
* Pomôcť môžu knižnice
   * [NSIMD](https://github.com/agenium-scale/nsimd)
   * C++26 `#include <simd>`
    * `simdjson` na mimoriadne rýchle parsovanie JSON


## Algoritmický paralelizmus

* Viacero jadier (vlákna)
* Slabá podpora v kompilátoroch (zlepšuje sa), správne použitie je náročné
* Myšlienka je jednoduchá: viac CPU znamená vyšší výkon
    * Ak jedno CPU vypočíta úlohu za X sekúnd, štyri CPU ju vypočítajú za X/4 sekúnd, však?
* `std::execution_policy` od C++17
   * `std::for_each(std::execution::par, ...)`
   * `std::transform(std::execution::par, ...)`
    * atď.


## Problémy paralelných algoritmov

* Navrhovať algoritmy je náročné
* Navrhovať škálovateľné paralelné algoritmy je (takmer) „nemožná misia“
* Kód býva dlhší, horšie sa číta a ťažšie sa uvažuje o jeho správaní
* „Nedeterministické chyby“

![Abstraktná farebná čarovná škatuľa](./lectures/threads/open_box_full_of_junk.png)<!-- .element: style="max-width: 30%;" -->

---

# Procesy

---

## Proces

* Každý proces má vlastnú pamäť
* Je identifikovaný jedinečným PID (identifikátorom procesu)
* „Vnútri“ každého procesu môže bežať 1..n vlákien

![Vlákna v procese](./lectures/threads/process-structure.png)<!-- .element: style="max-width: 50%;" -->

---

## Vytváranie nových procesov

### Windows

```cpp
#include <windows.h>

int main() {
    STARTUPINFOW si{ sizeof(si) };
    PROCESS_INFORMATION pi{};

    // Full path to Notepad (safer than relying on PATH)
    const wchar_t* appName = LR"(C:\Windows\System32\notepad.exe)";
    if (CreateProcessW(
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
        std::println(stderr, "CreateProcess failed. Error: {}", GetLastError());
        return 1;
    }
    std::println("Notepad launched successfully!");
}
```


### Linux

```cpp
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::println(stderr, "Fork failed!");
        return 1;
    } else if (pid == 0) {
        std::println("Child process.");

        // Arguments must include the command name first, and end with nullptr.
        char *const argv[] = {(char*)"/usr/bin/touch", (char*)"file.txt", nullptr};
        
        // execve requires the full path to the executable.
        execve("/usr/bin/touch", argv, nullptr); 
        
        _exit(1); // Exit if execve fails
    } else {
        std::println("Parent process.");
    }
    return 0;
}
```

---

## Čakanie na proces

* V predchádzajúcich príkladoch sme len spustili nový proces a hneď skončili
* Nie je to ideálne, pretože potomok môže prežiť rodičovský proces
    * Na Linuxe sa stane potomkom procesu init (PID 1)
* Existuje viacero spôsobov, ako čakať na skončenie procesu, ale iba jeden je správny


## Sleep je zlo

### Windows

```cpp
Sleep(1000); // Sleep for 1 second
```

### Linux

```cpp
sleep(1); // Sleep for 1 second
```

* Aj keď sme si istí, že proces skončí v danom čase, nemusí to tak byť
* Sleep nie je synchronizácia


## Opakované zisťovanie stavu (polling)

### Windows

```cpp
DWORD exitCode;
do {
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        std::println(stderr, "GetExitCodeProcess failed. Error: {}", GetLastError());
        return 1;
    }
    Sleep(100); // Sleep for 100 milliseconds
} while (exitCode == STILL_ACTIVE);
```


### Linux

```cpp
#include <cerrno>

pid_t result;
do {
    int status;
    result = waitpid(pid, &status, WNOHANG);
    if (result == -1) {
        if (errno == EINTR) {
            continue;
        }
        std::println(stderr, "waitpid failed. Error: {}", errno);
        return 1;
    }
    usleep(100000); // Sleep for 100 milliseconds
} while (result <= 0);
```

* O niečo lepšie ako sleep
* Plytvanie cyklami CPU: ak potomok beží dlho, iba zbytočne opakovane zisťujeme jeho stav
* Aktívne čakanie (busy waiting)


## Správne čakanie na proces

### Windows

```cpp
if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
    std::println(stderr, "WaitForSingleObject failed. Error: {}", GetLastError());
    return 1;
}
```

### Linux

```cpp
#include <cerrno>

int status;
pid_t result;
do {
    result = waitpid(pid, &status, 0);
} while (result == -1 && errno == EINTR);

if (result == -1) {
    std::println(stderr, "waitpid failed. Error: {}", errno);
    return 1;
}
```

* Správna synchronizácia
* OS pozastaví rodičovský proces, kým potomok neskončí

---

## Komunikácia medzi procesmi

* Procesy nezdieľajú pamäť (smerníky sa nedajú priamo použiť)
* Zdieľanie informácií medzi procesmi je zložitejšie ako medzi vláknami
* Spôsoby komunikácie
    * Súbory
    * Pomenované rúry (named pipes)
   * RPC / LPC
    * COM objekty

---

## Procesy a C++

* Štandard C++ nepozná pojem proces
* `boost::process`/`boost::interprocess` sú viac-menej prenositeľné
    * Správa vytvárania procesov
    * Zdieľaná pamäť
    * Pomenované mutexy
    * Smerníky s relatívnym posunom (offset pointers)


## Príklad s `boost::process`

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

# Vlákna

---

## Hlavné vlákno

* Funkcia main sa začne vykonávať v novovytvorenom vlákne (systém ho pre nás vytvorí automaticky)
* Na Windows je to to isté vlákno, ktoré vráti `CreateProcess`
* Je identifikované TID (identifikátorom vlákna), na Windows z rovnakého číselného priestoru ako PID
* Kód sa vykonáva iba vo vláknach

![Nite na ilustráciu vlákien](./lectures/threads/random-threads.jpg)

---

## Spustenie nového vlákna

### Windows

```cpp
DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    return 0; // Thread exit code here
}

int main() {
    DWORD threadId;
    HANDLE hThread = CreateThread(
        NULL, 0,        // default security attributes, default stack size
        ThreadFunc,     // thread function name
        NULL, 0,        // argument to thread function, default creation flags
        &threadId);     // returns the thread identifier

    if (hThread == nullptr) {
        std::println(stderr, "CreateThread failed. Error: {}", GetLastError());
        return 1;
    }

    // Wait for the thread to finish
    if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED) {
        std::println(stderr, "WaitForSingleObject failed. Error: {}", GetLastError());
        CloseHandle(hThread);
        return 1;
    }

    CloseHandle(hThread); // Close the thread handle

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
    if (pthread_join(thread, nullptr) != 0) {
        return 1;
    }
}
```

---

## Plánovač OS

* Aplikácia má obmedzenú kontrolu nad plánovaním svojich vlákien
* Plánovač OS nie je predvídateľný ani spravodlivý
* Nemali by sme sa spoliehať na konkrétne správanie plánovača, hoci niektoré OS poskytujú API na nastavenie priorít alebo afinity vlákien

![Červienka ako slovná hračka k round-robin plánovaniu](./lectures/threads/A_bright_little_robin.jpg)<!-- .element: style="max-width: 50%;" -->

<https://commons.wikimedia.org/wiki/File:A_bright_little_robin!_%288420315404%29.jpg>

---

## Multitasking

* Kooperatívny multitasking
    * Vlákna musia explicitne odovzdať riadenie
    * OS vlákno nikdy nepreruší
    * Éra DOS-u
* Preemptívny multitasking
    * Vlákna plánuje OS
    * Keď vláknu uplynie časový úsek, OS ho pozastaví a spustí iné pripravené vlákno
    * Vlákna môžu riadenie odovzdať aj samy
    * Predvolený spôsob v moderných OS

---

## Odovzdávanie parametrov vláknu

* Vyskúšajme príklad s pthread na Linuxe s parametrami, implementácia na Windows je podobná

```cpp
struct data {
    int number;
    std::string text;
};

// Function executed by the new thread
void* thread_func(void* arg) {
    data* data_ptr = static_cast<data*>(arg);

    if (data_ptr) {
        std::println("Child Thread: Number: {}, Text: {}", data_ptr->number, data_ptr->text);
        
        // **CRITICAL STEP: CONSUMPTION / CLEANUP**, beware of exceptions
        delete data_ptr;
    } else {
        std::println(stderr, "Child Thread: Error - Received null data pointer.");
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
        std::println(stderr, "Error creating thread.");
        delete data_ptr; // do not leak
        return 1;
    }

    if (pthread_join(thread, nullptr) != 0) {
        std::println(stderr, "Error joining thread.");
        return 1;
    }

    std::println("Main Thread: Thread finished and object consumed.");
    return 0; // no need to delete data_ptr here
}
```

---

## C++ a vlákna

* Do C++11: Aké vlákna?
* Od C++11:
    * Štandard označuje vlákna ako vykonávacích agentov (execution agents, `std::thread`)
    * Pamäťový model je jasne a prenositeľne definovaný
    * Pokročilá podpora vlákien a synchronizačných primitív
   * `std::jthread` (C++20)
   * `std::async`

---

# `std::thread`

---

## Jednoduchý príklad s `std::thread`

```cpp
#include <thread>

void thread_func(std::string message) {
    try {
        // Thread code here
        std::println("Thread says: {}", message);
    } catch (const std::exception& e) {
        std::println(stderr, "Exception in thread: {}", e.what());
    }
}

int main() {
    try {
        // Start a new thread
        std::thread t(thread_func, "Hello from std::thread!");

        // Wait for the thread to finish
        t.join();
    } catch (const std::system_error& e) {
        std::println(stderr, "Error creating thread: {}", e.what());
        return 1;
    }
}
```

---

## `std::thread` a výnimky

* Ak funkcia vlákna vyhodí výnimku a tá sa nezachytí, zavolá sa `std::terminate`
* Výnimky vždy zachytávame vo funkcii vlákna, aspoň kvôli ich zalogovaniu
* Konštruktor `std::thread` môže vyhodiť `std::system_error`, ak sa vlákno nedá vytvoriť
* Metódou `joinable()` zistíme, či je objekt vlákna spojený s vykonávacím agentom
* `join()` vyhodí `std::system_error`, ak
    * Vlákno sa nedá joinúť (`joinable() == false`)
    * Zistí sa uviaznutie (vlákno čaká samo na seba)
    * Zlyhá príslušné volanie OS

---

## Čo robiť v deštruktore?

* Joinúť či nejoinúť?
* Ak má `std::thread` pri volaní deštruktora priradeného vykonávacieho agenta (`joinable() == true`), zavolá sa `std::terminate()`
* To spravidla nechceme
* Pred zavolaním deštruktora vždy zavoláme join alebo detach

```cpp
std::thread t(thread_func);

t.detach(); // or t.join();
```

---

## Viacero parametrov

* Môžeme postupovať ako pri POSIX vláknach a použiť `struct`
* Odteraz pre prehľadnosť vynechávame bloky `try ... catch`

```cpp
struct data {
    int number;
    std::string text;
};

void thread_func(std::unique_ptr<data> data_ptr) {
    std::println("Thread: Number: {}, Text: {}", data_ptr->number, data_ptr->text);
}

int main() {
    auto data_ptr = std::make_unique<data>(42, "Hello, World!");
    std::thread t(thread_func, std::move(data_ptr)); // transfer ownership
    t.join();
}
```

---

## Typovo bezpečné parametre

* Parametre sa preposielajú objektu vlákna

```cpp
void thread_func(int number, const std::string& text) {
    std::println("Thread: Number: {}, Text: {}", number, text);
}

int main() {
    int num = 42;
    std::string str = "Hello, World!";
    std::thread t(thread_func, num, str); // copies of num and str are passed
    t.join();
}
```


## Rvalue referencie?

```cpp
void thread_func(std::string&& text) {
    std::println("Thread: Text: {}", text);
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, std::move(str)); // str is moved
    t.join();
}
```

* Reťazec sa presunie do funkcie vlákna


### Ale funguje aj toto...

```cpp
void thread_func(std::string&& text) {
    std::println("Thread: Text: {}", text);
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, str); // str is copied
    t.join();
}
```


### Parametre sa vždy preposielajú ako dočasné objekty

* `std::thread` si vždy uchováva vlastné kópie parametrov
* Vstupná funkcia vlákna sa volá, akoby parametre boli dočasné objekty
* Hodnoty teda môžeme do objektu vlákna presunúť alebo skopírovať (deje sa to v konštruktore `std::thread`)
* Funkcia vlákna sa vždy volá s dočasnými objektmi, takže
     * rvalue referencie,
     * konštantné lvalue referencie a
     * parametre odovzdávané hodnotou
    sa naviažu správne


* Nekonštantné lvalue referencie sa však nenaviažu!

```cpp
void thread_func(std::string& text) {
    std::println("Thread: Text: {}", text);
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, str); // ERROR: cannot bind temporary to lval reference
    t.join();
}
```


### Na odovzdanie lvalue referencií použijeme `std::ref`

```cpp
#include <functional> // for std::ref

void thread_func(std::string& text) {
    std::println("Thread: Text: {}", text);
}

int main() {
    std::string str = "Hello, World!";
    std::thread t(thread_func, std::ref(str)); // OK: pass lval reference
    t.join();
}
```

* `std::ref` tu vytvorí obal, ktorý povie `std::thread`, aby odovzdal referenciu namiesto vytvorenia kópie
* Odkazovaný objekt musí prežiť vlákno, inak vznikne neplatná referencia a nedefinované správanie
* Samozrejme, môžeme použiť aj smerníky a životnosť spravovať ručne

---

## Komunikácia medzi vláknami

* Vlákna zdieľajú pamäť!
* Môžeme medzi nimi odovzdávať smerníky, takže zdieľanie údajov nie je problém
* Vlákna môžu komunikovať cez zdieľané objekty
* C++ nemá špeciálnu podporu komunikácie medzi vláknami (na rozdiel napríklad od kanálov v Go)
* Existujú knižnice tretích strán (napr. `boost::asio`)

---

## Menný priestor `this_thread`

* Menný priestor s funkciami na prácu s aktuálnym vláknom (sleep, yield)

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

# Pamäťový model

---

## Prístup k pamäti

* Súbeh (race condition)
* Prístup k jednému zdroju z viacerých vlákien bez správneho zamykania
* Vedie k nedefinovanému správaniu
* Zvláštne problémy, ktoré sa ťažko ladia

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

    std::println("Final counter value: {}", counter);
}
```


## Z pohľadu hardvéru

* Výsledok predchádzajúceho kódu je nedefinovaný, v praxi môže byť kdekoľvek medzi 100000 a 200000
* Inštrukcia zápisu nemusí byť atomická
    * Inštrukcie sa rozložia na mikrokód a až potom sa vykonajú
    * Prelínajúce sa zápisy môžu meniť pamäť nekonzistentne
* Každé jadro má vlastnú cache
    * Výrazne zrýchľuje prácu
    * Zmeny v jednom vlákne sa neprenášajú do ostatných jadier

---

## Príklad súbehu

```cpp
int a, b; // global, so zero initialized

void func() {
    std::println("{}", a); // ok
    b += 1; // not OK, must be synchronized
}

int main() {
    std::thread t1(func), t2(func);

    t1.join();
    t2.join();
}
```

---

## Ako sa tomu vyhnúť?

* Môžeme používať iba jedno (hlavné) vlákno alebo vlákna využívať len cez paralelné algoritmy (napr. `std::execution_policy`)
* V bezpečí sme aj vtedy, keď vlákna pracujú iba s navzájom oddelenými údajmi
* Ak vlákna zdieľajú údaje, musíme synchronizovať prístup k nim

---

## Synchronizácia

* C++ poskytuje synchronizačné primitíva na predchádzanie súbehom
* `std::mutex`
* `std::lock_guard`
* `std::barrier`
* `std::condition_variable`
* ...

---

## Dohoda so štandardom C++

* Štandard C++11 definuje pamäťový model viacvláknových programov
* Určuje, čo je a čo nie je dovolené
* Pravidlo je jednoduché:

**K jednému adresovateľnému miestu v pamäti (nie k jednotlivým bitom bitového poľa) môžu súčasne pristupovať dve alebo viac vlákien iba vtedy, ak všetky prístupy iba čítajú.**

* K danému miestu v pamäti teda môže pristupovať buď jeden zapisovateľ, alebo viacero čitateľov
* Musíme to zabezpečiť, inak má program nedefinované správanie

---

## `std::mutex`

* Primitívum vzájomného vylúčenia
* Mutex môže v danom okamihu vlastniť iba jedno vlákno
* Ostatné vlákna, ktoré sa ho pokúsia zamknúť, budú zablokované, kým sa neodomkne
* Vnútorne sa používajú synchronizačné primitíva OS (napr. kritické sekcie na Windows, futexy na Linuxe)

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

* Pokúsi sa zamknúť mutex bez blokovania
* Vráti true, ak sa zámok podarilo získať, inak false

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


## RAII so `std::lock_guard`

* Pri ručnom zamykaní a odomykaní mutexu ľahko vznikajú chyby
* Ak sa medzi zamknutím a odomknutím vyhodí výnimka, mutex zostane zamknutý a spôsobí uviaznutie

```cpp
std::mutex mtx;
int a;

void func() {
    std::lock_guard<std::mutex> lock(mtx); // locks mutex
    a += 1; // safe to run from multiple threads
} // mutex is automatically unlocked when lock goes out of scope
```


## Alebo so `std::unique_lock`

* Flexibilnejší ako `std::lock_guard`
* Dá sa opakovane zamykať a odomykať

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

## Uviaznutie (deadlock)

* Nastane, keď dve alebo viac vlákien navzájom čakajú na uvoľnenie zdrojov
* Všetky zúčastnené vlákna zostanú zablokované na neurčito

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


## Ako predísť uviaznutiu

* Viacero mutexov vždy zamykáme v rovnakom poradí
* Na zamknutie viacerých mutexov naraz použijeme `std::lock`

```cpp
std::mutex mtx1, mtx2;

void func1() {
    std::lock(mtx1, mtx2);
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // do work
}
```

* `std::lock` predchádza uviaznutiu pri zamykaní oboch mutexov: pokúsi sa zamknúť oba, a ak sa to nepodarí, už zamknuté mutexy odomkne a skúsi to znova (s odstupom, tzv. backoff)
* `std::adopt_lock` povie `std::lock_guard`, že mutex je už zamknutý, takže ho nemá znova zamykať, iba odomknúť pri opustení rozsahu platnosti

---

## Ďalšie riešenia synchronizácie typu `int`

### Atomické operácie

* Typy `std::atomic` poskytujú atomické operácie nad premennými
* Pri jednoduchých operáciách nepotrebujeme mutexy
* Vnútorne sa používajú atomické inštrukcie CPU, ktoré sú zvyčajne rýchlejšie ako mutexy a zabezpečujú správnu viditeľnosť pamäte medzi vláknami

```cpp
#include <atomic>

std::atomic<int> a(0);

void func() {
    a += 1; // atomic increment
}
```


### Kľúčové slovo `volatile`

* `volatile` oznamuje kompilátoru, že premennú môžu meniť vonkajšie vplyvy (napr. hardvér či iné vlákna)
* Bráni optimalizáciám kompilátora, ktoré predpokladajú, že premenná sa neočakávane nemení
* NEPOSKYTUJE synchronizáciu ani atomicitu
* Pri viacvláknovom programovaní je užitočné len zriedka, uprednostňujeme `std::atomic`

---

## Úložisko `thread_local`

* Premenné deklarované so špecifikátorom `thread_local` majú pre každé vlákno samostatnú inštanciu

```cpp
thread_local std::string thread_id = "0";

void func() {
    thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::println("Thread ID: {} hash {}", pthread_self(), thread_id);
}
```

---

# Ďalšie primitíva

---

## `std::call_once`

* Zabezpečí, že sa časť kódu vykoná iba raz, aj pri použití viacerých vlákien

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

* Ak viacero vlákien zavolá `func`, iba jedno vykoná `init`, ostatné počkajú na dokončenie inicializácie


## Výnimky a `std::call_once`

* Ak sa počas inicializácie vyhodí výnimka, nasledujúce volania `std::call_once` inicializáciu zopakujú (ak čaká iné vlákno, odblokuje sa a pokúsi sa funkciu zavolať znova)
* S jedným príznakom možno volať viacero rôznych funkcií, ale úspešne sa dokončí iba jedna, ostatné sa ignorujú

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

* Vráti počet hardvérových vlákien dostupných v systéme
* Je to iba odporúčanie, skutočný počet vlákien, ktoré môžu bežať súbežne, môže byť iný
* Na mojom systéme vracia 14 (2 výkonné jadrá s hyperthreadingom + 8 efektívnych jadier + 2 nízkopríkonové jadrá)

```cpp
unsigned int n = std::thread::hardware_concurrency();
std::println("Number of hardware threads: {}", n);

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

Note: Ak vytváranie ďalšieho vlákna alebo alokácia vo vektore vyhodí výnimku, treba pred zánikom vektora joinúť už vytvorené vlákna, inak ich deštruktory zavolajú `std::terminate`. Rovnako to platí pre neskorší príklad s bariérou. Výnimkovú bezpečnosť tu pre jednoduchosť neriešime; alternatívou je `std::jthread`, pri synchronizácii však treba ošetriť aj chýbajúcich účastníkov.

---

## `std::shared_mutex`

* Umožňuje viacerým vláknam súčasne čítať zdieľané údaje, ale zapisovať môže v danom okamihu iba jedno vlákno
* Dve nové metódy: `lock_shared()` a `unlock_shared()` pre zdieľaný prístup (čítanie)
* Základné `lock()` a `unlock()` slúžia na výlučný prístup (zápis)
* Užitočné tam, kde čítanie výrazne prevažuje nad zápisom
* Nové pomocné triedy: `std::shared_lock` na zdieľané zamykanie (`std::unique_lock` slúži na výlučné zamykanie)


```cpp
#include <shared_mutex>

std::shared_mutex rw_mutex;
int shared_data;

void reader() {
    std::shared_lock<std::shared_mutex> lock(rw_mutex); // shared lock for reading
    std::println("Read data: {}", shared_data);
}

void writer() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex); // exclusive lock for writing
    shared_data += 1;
}
```

---

## Ďalšie druhy mutexov

* `std::timed_mutex`
    * Mutex s podporou časového limitu
    * Metódy `try_lock_for()`, `try_lock_until()`
* `std::recursive_mutex`
    * Mutex, ktorý môže to isté vlákno zamknúť viackrát
    * Musí sa odomknúť toľkokrát, koľkokrát bol zamknutý
* `std::recursive_timed_mutex`
    * Kombinácia rekurzívneho mutexu a mutexu s časovým limitom
* `std::shared_timed_mutex`
    * Zdieľaný mutex s podporou časového limitu

---

## `std::semaphore` (C++20)

* Počítací semafor alebo binárny semafor
* Riadi prístup k zdieľanému zdroju pomocou počítadla
* `acquire()` zníži počítadlo, pri nulovej hodnote blokuje
* `try_acquire()` sa pokúsi znížiť počítadlo bez blokovania
* `release()` zvýši počítadlo a môže odblokovať čakajúce vlákna

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
* Podobný mutexu, ale so sémantikou semafora
* Mutex môže v danom okamihu vlastniť iba jedno vlákno, zatiaľ čo binárny semafor môže uvoľniť aj iné vlákno než to, ktoré ho získalo
* Binárny semafor je preto užitočný na signalizáciu medzi vláknami

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

## `std::latch` a `std::barrier` (C++20)

* Synchronizačné primitíva na koordináciu viacerých vlákien
* Dajú sa implementovať pomocou semaforov alebo podmienkových premenných, ide len o abstrakcie vyššej úrovne
* `std::latch`
    * Synchronizačné primitívum, ktoré umožňuje jednému alebo viacerým vláknam čakať na dokončenie súboru operácií v iných vláknach
    * Keď počítadlo dosiahne nulu, nedá sa obnoviť
* `std::barrier`
    * Synchronizačné primitívum, ktoré umožňuje skupine vlákien navzájom na seba čakať v spoločnom bode bariéry
    * Bariéru možno použiť opakovane


## `std::latch`

* Vytvára sa s počítadlom
* Vlákna volaním `count_down()` znižujú počítadlo
* Volaním `wait()` sa vlákna zablokujú, kým počítadlo nedosiahne nulu
* Keď počítadlo dosiahne nulu, všetky čakajúce vlákna sa odblokujú
* Metóda `try_wait()` umožňuje bez blokovania overiť, či je počítadlo nulové
* Metóda `arrive_and_wait()` kombinuje `count_down()` a `wait()`


## Príklad s `std::latch`

```cpp
constexpr int NUM_WORKERS = 5; // Define the number of worker tasks
std::latch worker_latch(NUM_WORKERS);

void worker_task(int id) {
    // Simulate initialization with varying durations
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (NUM_WORKERS - id + 1))); 

    // The worker signals that one task is complete and continues
    worker_latch.count_down();
    
    // The worker is now free to continue its independent work
    std::println("Worker {}: Resuming background job.", id);
}

int main() {
    std::vector<std::thread> workers;
    for (int i = 1; i <= NUM_WORKERS; ++i) {
        workers.emplace_back(worker_task, i); 
    }

    worker_latch.wait(); // Blocks until the latch counter reaches zero.
    
    std::println("Every worker finished setup and called count_down().");
    
    for (auto& t : workers) {
            t.join();
    }
}
```

---

## `std::barrier`

* Vytvára sa s počtom účastníkov a voliteľnou dokončovacou funkciou
* Vlákna volaním `arrive_and_wait()` oznámia príchod k bariére a čakajú na ostatné
* Keď určený počet vlákien zavolá `arrive_and_wait()`, vykoná sa voliteľná dokončovacia funkcia a všetky vlákna sa odblokujú
* Príchod a čakanie možno rozdeliť na volania `arrive()` a `wait()`: prvé oznámi príchod k bariére, druhé blokuje, kým neprídu všetci (výsledok `arrive` treba odovzdať do `wait`), používa sa to však zriedka
* Volanie `arrive_and_drop()` zníži počítadlo a vyradí volajúce vlákno z bariéry (už sa naň nebude čakať), čo je užitočné pri dynamickom počte vlákien


## Príklad s `std::barrier`

```cpp
#include <barrier>

constexpr int WORKERS = 4; // 4 workers

auto on_completion = [phase_count = 0]() mutable noexcept {
    phase_count++;
    std::println("Starting Phase {}", phase_count);
};

std::barrier sync_point(WORKERS, on_completion);

void worker(int id) {
    std::println("Data Worker {}: Performing initial setup.", id);
    if (id % 2 == 0) {
        sync_point.arrive_and_wait();
    } else {
        sync_point.arrive_and_drop(); // odd workers drop out after first phase
        return; // if we keep thread participate, strange things happen
    }

    std::println("Data Worker {}: Starting work.", id);
    sync_point.arrive_and_wait(); 

    std::println("Data Worker {}: All work done.", id);
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

* Vlákno s automatickým joinutím (joining thread, prípadne Josuttis thread)
* Podobná sémantika ako `std::thread`, ale s niekoľkými rozdielmi
    * V deštruktore automaticky volá join (netreba ručne volať `join()`)
    * Podporuje kooperatívne zrušenie cez `std::stop_token`

```cpp
#include <thread>

int main() {
    std::jthread jt([]() {
        // thread work here
    }); // automatically joined when jt goes out of scope
}
```


## `request_stop`

* `std::jthread` podporuje kooperatívne zrušenie
* Jeden stop token sa dá použiť aj pre viacero vlákien
* Ak funkcia odovzdaná do `std::jthread` prijíma ako prvý parameter `std::stop_token`, dostane ho automaticky

```cpp
#include <thread>

int main() {
    std::jthread jt([i = 0](std::stop_token st) mutable {
        while (!st.stop_requested()) {
           std::println("{}", i++);

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

* Synchronizačné primitívum, ktoré umožňuje vláknam čakať na splnenie určitých podmienok
* Používa sa spolu so `std::mutex` na ochranu zdieľaných údajov
* Metóda `wait()` zablokuje volajúce vlákno, kým nedostane upozornenie
* `notify_one()` prebudí jedno čakajúce vlákno
* `notify_all()` prebudí všetky čakajúce vlákna
* Typicky sa používa pri modeli producent-konzument


## Príklad producent-konzument

```cpp
// Shared resources
std::queue<int> buffer; // where produced items are stored
std::mutex mtx; // mutex to protect shared buffer
std::condition_variable cv; // condition variable for synchronization

// we will produce and consume fixed number of items
constexpr int MAX_BUFFER_SIZE = 10;
constexpr int ITEMS_TO_PRODUCE = 20;

void producer();
void consumer();

int main() {
    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    std::println("Main: All threads finished.");
    return 0;
}
```


### Vlákno producenta

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
            std::println("P: Produced item {}. Buffer size: {}", i, buffer.size());
        } // Lock is automatically released here

        // simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100) * i % 3);

        // Notify one waiting consumer that new data is available
        cv.notify_one(); 
    }
    std::println("P: Finished producing all items.");
}
```


### Vlákno konzumenta

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

            std::println("C: Consumed item {}. Buffer size: {}", data, buffer.size());
        } // Lock is automatically released here

        // Notify the producer that space is now available
        cv.notify_one(); 

        // simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100) * data % 2);
    }
    std::println("C: Finished consuming all items.");
}
```


### `wait` bez predikátu

* Ak sa `wait` zavolá bez predikátu, vlákno sa môže prebudiť aj bez splnenia podmienky (falošné prebudenie, spurious wakeup)
* Vždy používame verziu `wait` s predikátom alebo podmienku kontrolujeme v cykle

```cpp
std::unique_lock<std::mutex> lock(mtx);

cv.wait(lock); // may wake up spuriously

// Always recheck the condition in a loop
while (buffer.size() >= MAX_BUFFER_SIZE) {
    cv.wait(lock); // recheck condition
}
```

---

# Návrhové vzory

---

## Cache bezpečná pre viacero vlákien

* Predstavme si výpočtovo náročnú funkciu, ktorú potrebujeme opakovane volať s rovnakými parametrami z rôznych vlákien
* Bolo by vhodné ukladať výsledky volaní do cache, aby sme ich nemuseli počítať znova

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


## Zabezpečenie prístupu z viacerých vlákien

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


## Ak čítanie výrazne prevažuje nad zápisom

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


## Ak je kopírovanie hodnôt nákladné

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

        auto result = std::make_shared<std::string>(expensive_function(key));

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


## Môže byť metóda `const`?

* Metódu `compute` môžeme označiť ako `const`, ak mutex a cache označíme ako `mutable`
* To nám umožní meniť ich aj v metódach `const`
* Pri cachovaní je to užitočné, pretože logický stav objektu sa z pohľadu volajúceho nemení
* Je to aj jeden zo spôsobov, ako uvažovať o správnom použití `const` pri práci s viacerými vláknami
    * Objekt je buď logicky `const` (iba na čítanie) a môže k nemu pristupovať viacero vlákien súčasne
    * Alebo nie je a prístup k nemu sa musí synchronizovať externe
* Štandardná knižnica používa tento prístup, takže každá metóda `const` štandardných kontajnerov je bezpečná na čítanie z viacerých vlákien
* Metódy, ktoré nie sú `const`, nie sú bezpečné pri súbežnom prístupe a musia sa synchronizovať externe

---

## Kanál

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

note: Používame dve podmienkové premenné na samostatné upozorňovanie producentov a konzumentov, čo zvyšuje efektivitu. Súčasne môže čakať viacero producentov alebo konzumentov, preto po odobratí položky upozorníme iba producentov a naopak.


```cpp
void Channel::send(std::string value) {
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
std::optional<std::string> Channel::receive() {
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
void Channel::close() {
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
            std::println("Received: {}", *msg);
        }
    });

    // Threads will be joined automatically when going out of scope
}
```

---

# `std::async` a futures

---

## `std::async`

* Spustí funkciu asynchrónne (prípadne v novom vlákne) a vráti `std::future`, ktorý predstavuje výsledok funkcie

```cpp
#include <future>

int compute_factorial(int n) {
    if (n <= 1) return 1;
    return n * compute_factorial(n - 1);
}

std::future<int> fut = std::async(compute_factorial, 15);

int result = fut.get(); // blocks until the result is available
```

Note: Faktoriál je tu iba ilustračná funkcia. Pri bežnom 32-bitovom `int` sa už `13!` nezmestí do rozsahu, takže volania s argumentom 15 v tejto časti spôsobia pretečenie so znamienkom a nedefinované správanie. Pri spúšťaní príkladov použime menší argument (napr. 10) alebo vhodný číselný typ s kontrolou rozsahu; `try ... catch` toto pretečenie nezachytí.


## Výnimky v `std::async`

* Ak funkcia odovzdaná do `std::async` vyhodí výnimku, uloží sa do `std::future`
* Volanie `get()` na future znova vyhodí výnimku vo volajúcom vlákne

```cpp

std::future<int> fut = std::async([]() {
    throw std::runtime_error("Error in async task");
    return 42;
});

try {
    int result = fut.get(); // will rethrow the exception
} catch (const std::exception& e) {
    std::println("Caught exception: {}", e.what());
}
```


## Politiky spustenia

* `std::async` podporuje rôzne politiky spustenia
    * `std::launch::async` - vždy spustí funkciu v novom vlákne
    * `std::launch::deferred` - spustí funkciu až pri volaní `get()` alebo `wait()`, a to vo volajúcom vlákne
    * Predvolené je `std::launch::async | std::launch::deferred`, takže si implementácia môže vybrať

```cpp
std::future<int> fut = std::async(std::launch::deferred, compute_factorial, 15);

int result = fut.get(); // compute_factorial runs here (in the calling thread) and now
```


## Ručné použitie `std::promise` a `std::future`

* `std::promise` umožňuje nastaviť hodnotu alebo výnimku, ktorú možno získať cez `std::future`

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


## Deštruktor future

* Ak `std::future` zanikne pred volaním `get()` alebo `wait()` a súvisiaca asynchrónna úloha ešte neskončila, deštruktor bude blokovať **iba** vtedy, ak bola úloha spustená cez `std::async`
* Inak nebude blokovať ani neukončí program, čo môže viesť k nenápadným chybám
* Pred zánikom future sa vždy snažíme zavolať `get()` alebo `wait()`

Note: Čakanie v deštruktore sa týka posledného vlastníka zdieľaného stavu výpočtu spusteného s politikou `std::launch::async`. Predvolené volanie môže zvoliť `std::launch::deferred`; vtedy deštruktor výpočet nespustí ani naň nečaká a bez `get()` alebo `wait()` sa funkcia vôbec nevykoná.

```cpp
{
    std::future<int> fut = std::async(compute_factorial, 15);
    // do some other work
} // waits if launched asynchronously; a deferred call is never executed
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