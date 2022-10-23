
# Globálne a statické premenné

---

## Dočasné premenné

* *Temporaries*
* Všetky premenné na predchádzajúcich slidoch mali mená
* Pomenované premenné zostávajú žiť až po koniec scope-u, v ktorom boli zkonštruované
* Nepomenované premenné (dočasné) sú zdeštruované hneď po tom čo boli použité (na tom istom riadku)

```cpp
int main() {
    string("My temporary string");
 
    cout << string("File ") + __FILE__ << endl;
}
```

---

## Globálne premenné

* Sú inicializované ešte pred volaním funkcie `main` and zdeštruované po vrátení z funkcie `main`
* V jednom translation unit je poradie inicializácie rovnaké ako sú napísané v súbore a deštrukcia potom v opačnom poradí


## Nedefinované poradie

```cpp
// main.cpp
#include "Version.h"
 
std::string g_Version = GetAppVersion();
 
int main(int argc, char* argv[]) {
    cout << g_Version;
}

```

```cpp
// Version.cpp
#include "Version.h"
 
uint32_t g_Major = GetMajorNumber();
uint32_t g_Minor = GetMinorNumber();
 
string GetAppVersion() {
    return std::to_string(g_Major) + "." + std::to_string(g_Minor);
}

```

Všetky globálne premenné sú inicializované pred volaním `main`. Tu jedna závisí od druhej v inom súbore (*translation unit*) a to je nedefinované správanie.

---

## Statické premenné

* static premenné vo funkciách sú inicializované ak cez nich prejde beh programu (*control flow*)
* Sú potom deštruované po vrátení z `main` funkcie a opačnom poradí ako boli skonštruované
* Ak nejaká statická premenná nebola inicializovaná, tak nebude ani zdeštruovaná (toto je v povinnostiach kompilátora)


## Statické premenné

```cpp
struct A {
    A() { std::cout << "A"; }
    ~A() { std::cout << "~A"; }
};

void f(int i) {
    if (i == 0) {
        static A a;
    }
}

int main() {
    f(4); // nothing
    f(0); // A
    f(0); // nothing
    std::cout << "M";
} // ~A
```

Konštruktor sa zavolá iba ak sa funkcia `f` zavolá s parametrom `i == 0`. Deštruktor sa "zaregistruje" aby sa zavolal, keď program skončí. 

---

## Jednoduché globálne a statické premenné

* Kompilátor môže (a aj by mal) staticky (počas kompilácie) inicializovať globálne a statické premenné bez komplexnej inicializácie

```cpp
int global_variable = 43;
 
int main(int argc, char* argv[]) {
    static char* static_string = "Static initialization";
 
    std::cout << global_variable;
}
```

---

## Automatická pamäť

* Všetky druhy definícií premenných sú v podstate automatické, teda deštruktor sa vola implicitne 
* Pamäť lokálnych premenných je alokovaná zo zásobníka (*stack*)
* Pamäť pre globálne a statické premenné sa alokuje v dátovej sekcii binárky (*image*)
* Veľkosť musí byť jasná počas kompilácie
