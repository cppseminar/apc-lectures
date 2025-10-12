# C++

## Úvod

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Ľahký úvod do C++
* C vs. C++
* História C++
* Moderné C++
* Budúcnosť C++

---

# Krátke predstavenie C++

---

## Čo je C++?

* C++ je jeden z najnepochopenejších relevantných programovacích jazykov
* Zčasti za to môže meno C++
    * postfix increment vracia pôvodnú hodnotu 😉
* Zčasti krkolomné skratky, ktoré komunita používa (RAII, SFINAE, ADL, ODR, ...)
* Pravdepodobne aj neexistencia jednotného toolingu
   * package manager
   * build system

---

## Veľa explicitného spravovania pamäte

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

* V skutočnosti ak potrebujeme explicitne volať `free()`, alebo `delete` tak pravdepodobne robime niečo zle
* C++ síce nemá garbage collecter, ale objekty by sa mali o svoji pamäť automaticky postarať
* Lepšie by bolo povedať, že C++ nám umožňuje manuálne spravovať pamäť ak to potrebujeme
</div>
<div style="flex: 3;">

![Do I cast the result of malloc](./lectures/1_intro/EYm0ylHX0AANjFj.jpg)
</div>
</div>

---

## C with classes

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

* C++ sa v počiatkoch volalo C with classes
* Sú to ale dva dosť rozdielne jazyky, ktoré majú vlastné
    * štandardy
    * komunitu
    * idiomy
* Lepšie by bolo povedať, že C++ si udržiava (pokiaľ je to možné) kompatibilitu s C
* Preto integrovať C kód do C++ (a naopak) je veľmi jednoduché
</div>
<div style="flex: 3;">

![C with C++ diagram](./lectures/1_intro/c-with-classes.png)
</div>
</div>

---

## C++ je plné metaprogramovania 

* Je pravda, že C++ používalo metaprogramovanie pomocou templatov
* Kedysi bolo dosť dôležité poznať tieto triky, neboli iné štandardizované spôsoby
* Dnes sa bežné programy obíjdu bez týchto konštrukcií
* Samozrejme metaprogramovanie má svoje miesto
    * micro optimalizácie
    * tvorba knižníc
    * *zjednodušenie kódu*

---

## C++ roadmap

![C++ roadmap](./lectures/1_intro/timeline-2022-07.png)
<!-- .element: class="r-stretch" style="background: white;" -->

---

## C++ ...

* je moderný programovací jazyk
* obsahuje abstrakcie pre najčastejšie používané algoritmy, kontajnery a operácie
* poskytuje úplne kontrolu nad hardwarom, ak je to potrebné
* sa stále vyvíja 
* má obrovskú komunitu (konferencie CppCon, C++ Now, Meeting C++)

---

## História C++

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

* V 1979 *Bjarne Stroustrup* začal pracovať na C s triedami
* V 1983 jazyk premenoval na C++ a pridal virtuálne funkcie, preťažovanie operátorov a veľa ďalšieho
* Prvý veľký ISO štandard bol C++98/03
* <https://isocpp.org/>
</div>
<div style="flex: 3;">

<img src="./lectures/1_intro/bjarne.png" alt="Bjarne Stroustrup" width="100%" />
</div>
</div>


### C++11

* **Lambda funkcie** – Umožňujú definovať anonymné funkcie priamo v kóde.
* **Rvalue referencie** a Move semantika – Optimalizuje kopírovanie a presuny objektov.
* `auto` – Automatická dedukcia typu premenných.
* **range-based for loop** – Jednoduchší zápis cyklov pre kontajnery.
* `std::unique_ptr` a `std::shared_ptr` – Inteligentné ukazovatele na správu pamäte.
* `constexpr` – Umožňuje výpočty počas kompillácie, nie za behu programu.
* `std::thread` – Natívna podpora pre viacvláknové programovanie.


### C++14

* Bugfix C++11
* C++14 zjednodušil, zjednotil a optimalizoval použitie C++11 koncepcií.
* **Generické lambda funkcie** 


### C++17

* `std::optional` – Pre bezpečnú prácu s hodnotami, ktoré môžu byť neplatné.
* `std::variant` – Typovo bezpečná alternatíva k unionom pre rôzne typy hodnôt.
* `std::any` – Umožňuje uložiť ľubovoľný typ do jedného kontajnera.
* **Structured bindings** – Umožňuje jednoduché rozbalenie štruktúrnych dát do viacerých premenných.
* **Filesystem knižnica** – Podpora pre prácu so súborovým systémom.
* **Polymorfné alokátory** – Flexibilný a efektívny mechanizmus na správu pamäte, ktorý je nezávislý od konkrétneho alokátora.
* **Paralelné algoritmy** – Priamo v algoritmoch zo štandardnej knižnice.


### C++20

* **Koncepty (Concepts)** – Umožňujú špecificky obmedziť typy v šablónach.
* **Ranges** – Nová knižnica na prácu s rozsahmi dát (array, vector, ...).
* **Korutiny (Coroutines)** – Podpora pre asynchrónne operácie a sekvencie.
* **Moduly** – Zlepšujú kompiláciu a organizáciu kódu.
* `std::span` – Nevlastniace zobrazenie na sekvencie dát.
* **three-way comparison (<=>)** – Automatizuje definovanie porovnávanie.
* `std::format` – Nová formátovacie knižnica.


### C++23

* Štandardná knižnica ako modul (`import std;`)
* Dodefinovanie veľa nedefinovaného správania
* Vylepšená podpora pre ranges
* stacktrace knižnica
* `std::println` - Nový spôsob výstupu na konzolu

---

## Rozdelenie C++

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

* Core language
    * Type system
    * Control flow
    * “C layer”
    * Memory model
</div>
<div style="flex: 1;">

* Štandardná knižnica
    * Containers
    * Strings
    * Concurrency and atomics
    * Files and streams
    * Regular expressions
</div>
</div>

---

## Hlavné kompilátory


### gcc (g++)

<img src="./lectures/1_intro/GNU_Compiler_Collection_logo.svg.png" alt="gcc logo" style="width: 30%;" />


### clang (llvm)

![llvm logo](./lectures/1_intro/LLVMWyvernSmall.png)


### Visual Studio (msvc)

<img src="./lectures/1_intro/Visual_Studio_Icon_2022.svg.png" alt="msvc logo" style="width: 30%;" />


* V súčasnosti sú všetky tieto kompilátory dosť dobré a dosť štandardné
* Niektoré nové vlastnosti skôr podporuje jeden a inokedy druhý
* Portabilita kódu je oproti minulosti oveľa lepšia

---

## ISO Standard

* Málo jazykov má oficiálny ISO štandard, C++ ho má
* The Committee: WG21
* Zapojený ľudia priamo z priemyslu (Intel, Microsoft, Google, Red Hat, IBM a iné)

![C++ comitee C+20](./lectures/1_intro/comittee.png)

---

# Vlastnosti C++

---

## Abstrakcia

* Programátor by nemal potrebovať všetky implementačné detaily nato aby použil nejakú vlastnosť systému (na otvorenie a zapísanie súboru netreba vedieť všetky platformovo špecifické detaily súborového systému)
* Pomocou kompozície a dedičnosti sa potom dajú budovať komplexné systémy, ktoré abstrahujú od vrstiev nižšie 
* (Stále musí byť možné obísť abstrakciu a komunikovať priamo – takmer isto zlý postup)

---

## Efektivita

* C++ bol nadizajnovaný aby bol efektívny ako sa len dá
* There is no room for other language between C++ and metal
* Na tomto leveli je veľa bezpečnostných mechanizmov vypnutých
* Kompilátor verí programátorovi, že píše "dobrý" kód
* **LBYL** – look before you leap (opposed to **EAFP**)

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
// create final dessription
if (!UserNames.empty()) {
	// remove "; "
	UserNames.pop_back();
	UserNames.pop_back();
}

```
</div>
<div style="flex: 1;">

Musíme sa uistiť, že UserNames obsahuje aspoň dva znaky, inak sa stanú zlé veci (nedefinované správanie).
</div>
</div>

note: EAFP = easier to ask forgiveness than permission

---

## Garbage collector

* Veľa súčasných programovacích jazykov má garbage collector
* C++ ho nemá 
* Ak máte pocit, že vám v C++ chýba garbage collector, tak ste niečo urobili veľmi nedobre

> C++ is my favorite garbage collected language because it generates so little garbage.  
>           — Bjarne Stroustrup

---

## Portabilita

* Štandardný C++ kód vám pravdepodobne pôjde skompilovať na každom z troch hlavných kompilátorov
* Existuje veľa menších, špecifických kompilátorov a tam máte tiež veľkú šancu
* Existujú spôsoby akými môžete podmieniť kompiláciu kusu kódu určitým kompilátorom/platformou
    * `#ifdef`
    * `__has_include`
    * `if constexpr`

---

## Kompatibilita s C

* C funkcie sa dajú priamo volať z C++
* Rovnako sa dajú z C++ programu "exportovať" funkcie, ktoré majú C linkage
* Preto je možné volať z ľubovoľného programovacieho jazyka C++ (asi každý má podporu pre C)

```cpp
extern "C" void print_primes(size_t limit) {
    // this function will have C linkage
}
```

---

## Statické typy

<ul>
  <li>
    Typy sú kontrolované počas prekladu programu (kompilácie)

```cpp
std::vector<int> nums{ 2, 3, 5, 7 };
std::string s = "Hello world";
s = nums;
```
  </li>
  <li>
    Info o nich štandardne nie je dostupné počas runtime (neplatíme za to čo nepoužívame)
  </li>
</ul>


* Všetky typy (premenné, návratové hodnoty funkcií, ...) musia byť deklarované a dostupné počas kompilácie. 
* Funguje "type inference" (`auto`, `decltype`), kedy kompilátor dokáže zistiť typ z použitia
* Každá premenná musí byť deklarovaná pred tým než sa použije
* *Pokiaľ sa dá premenné inicializujeme hneď ako to ide*

<div style="display: flex;">
<div style="flex: 1;">

```cpp
int bad;
// ...
bad = 1;
```
</div>
<div style="flex: 1;">

```cpp
int good = 1;
```
</div>
</div>

---

## Typový systém

* Veľa sivých miest v C typovom systéme, ktoré prevzalo aj C++
* Smerníky môžu byť ľubovoľne pretypované (ale môže nastať nedefinované správanie)
* Používajú sa implicitné konverzie a ich pravidlá sú zdĺhavé a komplikované
* Integer promotion tiež nie je zrovna intuitívny

* V posledných revíziách C++ je snaha odstrániť veľa nedefinovaných správaní 


### Fast inverse square

* John Carmack v engine Quake 3

```cpp
float Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;   // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);  // what the f***? 
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
    // y  = y * ( threehalfs - ( x2 * y * y ) ); 
    // 2nd iteration, this can be removed

    return y;
}
```


### Aký je výsledok nasledujúceho programu?

* Skompiluje sa? Je to definované? Aká je hodnota v `s`?

```cpp
std::string s = "Ingsoc"; 
s = 19.84;
```

* Skompiluje sa a s bude obsahovať jeden znak s číslom 19.
<!-- .element: class="fragment" -->

---

## Objektovo orientované

* Dedičnosť (inheritance), zapúzdrenosť (encapsulation) a polymorfizmus (polymorphism) sú plne podporované
* Viacnásobná dedičnosť sa dá tiež použiť


### Nasledujuci kód sa neskompiluje, prečo?

```cpp
class A {
private:
    virtual void f() = 0;
};
 
class B : public A {
protected:
    virtual void f() override { }
};
 
class C : public B, private A {
public:
    virtual void g() { B::f(); }
};
```

note: `A::f` je `virtual` a preto sa nedá dediť od `A` bez toho aby sme ju definovali.

---

## Knižnice na všetko

* Pre C++ existuje obrovské množstvo knižníc
* Bez problémov sa dajú použiť všetky C knižnice, tie existujú asi na všetko


### boost - http://www.boost.org/ 

* Zbierka knižníc, veľa z nich nakoniec skončí v upravenej forme v štandardnej knižnice
* Kontajnery, práca s geometriou, formátovanie stringov, tvorenie parserov, sieťová komunikácia

<img src="./lectures/1_intro/Boost.png" alt="boost logo" style="width: 30%; background: white;" />

---

## Package manager

* Snáď všetky úspešná jazyky majú package manager, JS má npm, Python má pip, ...
* V C++ žiaden defacto štandardný nie je
    * **Conan** distribuuje skompilované binárky, čo sa viacerým používateľom zdá jemne povedané nešťastné
    * **Vcpkg** je open source nástroj od Microsoftu, ktorý sa snaží byť managerom pre všetky knižnice, bohužiaľ občas sa vyskytne nekompatibilita až úplná neskompilovateľnosť

---

## Je C++ ťažký jazyk?

* Áno aj nie
* Ak ho iba používate nemusia vás trápiť variadické template, argument dependent lookup, ... 
* Ak ale idete do hĺbky, skrýva sa tam veľa drakov...

![boost logo](./lectures/1_intro/Psalter_World_Map,_c.1265_dragons.jpg)

---

# C vs. C++

---

![yoda](./lectures/1_intro/yoda.png)

> You must unlearn what you have learned
>           — Master Yoda

---

![bjarne quote](./lectures/1_intro/bjarne-quote.png)

Je naozaj veľmi dôležité aby ste vedeli čo robíte, inak C++ nie je pre vás. 

* Vyrábame security problémy
* Kód sa nebude dať maintainovať
* Pravdepodobne ani s tou rýchlosťou to nebude terno

---

## Je C rýchlejšie ako C++?

* Neexistuje žiaden dôvod prečo by C malo byť rýchlejšie
* Skoro všetky C programy sú platné C++ programy
* Naopak C++ má potenciál byť rýchlejšie

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

```cpp
void SortCxx(size_t n) {
    std::vector<int> v(n);
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = rand();
    }

    std::sort(v.begin(), v.end());
}
```
</div>
<div class="fragment" style="flex: 3; font-size: calc(2 * var(--r-main-font-size));">

10,2s
</div>
</div>


<div style="display: flex; align-items: center;">
<div style="flex: 7;">

```c
void SortC(size_t n) {
    int* a = (int*)malloc(n * sizeof(int));
    if (a) {
        for (size_t i = 0; i < n; ++i) {
            a[i] = rand();
        }
        qsort(a, n, sizeof(int), cmp_int);

        free(a);
    }
}

```
</div>
<div class="fragment" style="flex: 3; font-size: calc(2 * var(--r-main-font-size));">

16,4s
</div>
</div>

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

Treba dokonca vlastnú funkciu na porovnanie prvkov.
</div>
<div style="flex: 1;">

```c
int cmp_int(const void* a, const void* b) {
    return *(const int*)a - *(const int*)b;
}
```
</div>
</div>

---

## Makrá

* V C sa používali makrá na generovanie funkcií, ktoré boli akoby type generic

```c
#define MAX(a, b)
```

* Ďalej sa používali na vytvorenie funkcií, ktoré kompilátor musí inlinovať (teda neurobí naozaj *call*)


### Aký je problém s nasledujúcim makrom?

```c
#define SQUARE(x) x * x;
```

<div class="fragment" style="display: flex;">
<div style="flex: 1;">

```c
if (SQUARE(x) > 100) {
    // do stuff
}
```
</div>
<div style="flex: 1;">

```c
if (1*1; > 100) {
    // do stuff
}
```
</div>
</div>

<div class="fragment">

Ak aj odstránime `;` stále sú tam problémy

<div style="display: flex;">
<div style="flex: 1;">

```c
int x = SQUARE(1 + 1);
```
</div>
<div style="flex: 1;">

```c
int x = 1 + 1*1 + 1;
```
</div>
</div>
</div>

<div class="fragment" style="display: flex;">
<div style="flex: 1;">

```c
x = SQUARE(++x);
```
</div>
<div style="flex: 1;">

Sequencing problems 😢
</div>
</div>


### Inline funkcie

<ul>
  <li>Kompilátor je pravdepodobne lepší ako my v rozhodovaní ktoré funkcie inlinovať</li>
  <li class="fragment">V C++ existovalo klúčové slovo <code>inline</code>, v minuloti služilo na inline funkcie, dnes má už skôr iné významy

```c
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
```

```cpp
template<typename T> inline T max(T a, T b) {
    return a > b ? a : b;
}
```
</li>
  <li class="fragment">Najlepšie je použiť štandardnú knižnicu

```cpp
int k = std::max({ 1, 3, 4, 8, 10, -1 });
```
</li>
</ul>

---

## Explicitná správa pamäte

* C neposkytuje veľa nástrojov na uľahčenie správy pamäte
* Programátori majú preto tendenciu používať pamäť na zásobníku (stack), keďže tá sa sama uprace po opustení aktuálneho frame-u

<div class="fragment" style="display: flex; align-items: center;">
<div style="flex: 1;">

```c
char title[128] = "";
char text[512] = "";
char tooltip[512] = "";
char menuTitle[128] = "";
char menuLink[512] = "";
char macro[512];
```
</div>
<div style="flex: 1;">
Bug in code  
➡
</div>
<div style="flex: 1;">

```c
char title[256] = "";
char text[1024] = "";
char tooltip[1024] = "";
char menuTitle[256] = "";
char menuLink[1024] = "";
char macro[1024];
```
</div>


### C++ používa primitíva, ktoré samy spravujú pamäť

* `std::string` je objekt zodpovedný za reprezentáciu jedného reťazca, stará sa o inteligentnú správu pamäte a schováva implementačné detaily
* `std::vector`, `std::map`, `std::list` – kontajnery 

```cpp
std::vector<std::string> path_fragments;
/* ... */  

// remove empty strings from path fragments
path_fragments.erase(std::remove_if(std::begin(path_fragments), 
std::end(path_fragments), [](const std::string& val) {
    return val.empty();
}), std::end(path_fragments));
```

---

## Manipulácia stringov

* Ťažké a veľmi rozvláčne v C

```cpp
const char* name = "example";
size_t file_name_len = strlen(name) + strlen(".txt") + 1;
char* file_name = (char*)malloc(file_name_len);
strcpy_s(file_name, file_name_len, name);
strcat_s(file_name, file_name_len, ".txt");
```


* Jednoduché v C++ 
* Na 99% rovnako rýchle ako riešenie vyššie (niekedy ešte rýchlejšie – SSO)

```cpp
std::string name = "example";
std::string file_name = name + ".txt";
```

---

# Budúcnosť

---

## Vývoj C++

* Aktuálne je nastavený model, každé tri roky nová verzia
* Do C++ sa hlavne pridáva, odoberanie je s ohľadom na obrovské codebase problematické
* Kompilátory sú ale občas pozadu

---

## Iné jazyky

* C++ je veľmi starý programovací jazyk
* Pokusom o nahradenie bolo a je viacero
* **D** bol myslený ako priamy nástupca, aktuálne je popularita veľmi nízka
* **Go** je viacej high level (má napríklad garbage collector) jazyk postavený okolo goroutines, s príchodom cloudu sa začal použivať vo väčšej miere
* **Rust** NIST odporúča C a C++ nepoužívať v kritických systémoch, veľmi ľahko sa dá urobiť nepovolená pamäťová operácia, ako jednu z náhrad odporúčil Rust. Výhodou je v celku unikátny model vlastníctva pamäte, ktorý umožnuje mať bezpečnú aplikáciu aj bez garbagge collectoru.

---

## C++2 (Syntax 2)

* Expiriment od [Herba Suttera](https://github.com/hsutter/cppfront)
* Nová C++ syntax, ktorá zjednodušuje a odstraňuje nebezpečné konštrukcie (resp. ich robí explicitnými)
* "Kompilátor" preloží súbor, kde sa nachádza aj pôvodná syntax aj nová do pôvodnej

```cpp
#include <iostream>                             // Cpp1
#include <string_view>                          // Cpp1

N: namespace = {                                        // Cpp2
    hello: (msg: std::string_view) =                    // Cpp2
        std::cout << "Hello, (msg)$!\n";                // Cpp2
}                                                       // Cpp2

int main() {                                    // Cpp1
    auto words = std::vector{ "Alice", "Bob" }; // Cpp1
    N::hello( words[0] );                       // Cpp1
    N::hello( words[1] );                       // Cpp1
    std::cout << "... and goodnight\n";         // Cpp1
}
```

---

## Carbon

* Nový programovací jazyk od [Chadlera Carrutha](https://github.com/chandlerc) z Google
* Ako C++ je v podstate kompatibilné s C, tak Carbon chce byť kompatibilný s C++

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```c++
// C++ code used in both Carbon and C++:
#include <stdfloat>

struct Circle {
  std::float32_t r;
};

// Carbon exposing a function for C++:
package Geometry;
import Cpp library "circle.h";
import Math;

fn PrintTotalArea(circles: [Cpp.Circle]) {
  var area: f32 = 0;
  for (c: Cpp.Circle in circles) {
    area += Math.Pi * c.r * c.r;
  }
  Print("Total area: {0}", area);
}
```
</div>
<div style="flex: 1;">

```c++
// C++ calling Carbon:
#include <vector>
#include "circle.h"
#include "geometry.carbon.h"

auto main() -> int {
  std::vector<Circle> circles = {{1.0}, {2.0}};
  // A Carbon slice supports implicit construction
  // from `std::vector`, similar to `std::span`.
  Geometry::PrintTotalArea(circles);
  return 0;
}
```
</div>
</div>

---

# LLMs

---

## Náhodné číslo

> C99 , máme len funkciu rand, ako urobiť aby sme  dostali číslo od 0 do 99 vrátane. 


### Výsledok

```c [|5]
// Inicializácia generátora náhodných čísel
srand(time(NULL)); // Inicializácia so súčasným časom

// Generovanie náhodného čísla v rozsahu 0 až 99
int random_number = rand() % 100; // Získa číslo od 0 do 99

// Výpis náhodného čísla
printf("Náhodné číslo: %d\n", random_number);
```


### ...pokračovanie konverzácie

> Je to bezpečné? 


### Výsledok

```c [|7]
srand(time(NULL));

// Generate random number until it falls within the desired range
int random_number;
do {
    random_number = rand();
} while (random_number >= RAND_MAX - (RAND_MAX % 100));

random_number %= 100;

printf("Random number: %d\n", random_number);
```

note: RAND_MAX teoreticky môže byť 32, alebo podobné malé číslo. 


### To isté v C++

> C++20, máme len standardnú knižnicu, ako urobiť aby sme dostali náhodné číslo od 0 do 99 vrátane. 

```cpp
// Inicializácia generátora náhodných čísel
std::random_device rd;  // Získanie náhodného zariadenia
std::mt19937 gen(rd());  // Inicializácia generátora Mersenne Twister
std::uniform_int_distribution<> dis(0, 99);  // Rozdelenie na celé čísla od 0 do 99

// Generovanie náhodného čísla
int random_number = dis(gen);
```

---

## Čítanie `int` zo stdin

> C99, napíš funkciu, ktorá nemá žiadne parametre a vráti uint32_t, ktorý prečíta zo stdin. 


### Výsledok

```c
uint32_t number;
// Prečítaj číslo zo stdin
if (scanf("%u", &number) != 1) {
    // Ak sa nepodarí prečítať číslo, vráť 0 alebo môžeš spracovať chybu iným spôsobom
    fprintf(stderr, "Chyba pri čítaní čísla.\n");
    return 0; // Alebo iná hodnota indikujúca chybu
}
return number;
```

note: scanf pri overflow je undefined


### ...pokračovanie konverzácie

> Je to bezpečné? 


### Výsledok

```c [|1|11]
char buffer[32]; // Buffer na načítanie vstupu
uint32_t number;

// Prečítaj riadok zo stdin
if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Pokús sa previesť reťazec na číslo
    char *endptr;
    number = strtoul(buffer, &endptr, 10); // Prevedenie na unsigned long

    // Over, či sme prečítali celé číslo a či nie je pretečené
    if (endptr == buffer || *endptr != '\n' || number > UINT32_MAX) {
        fprintf(stderr, "Chyba: Neplatný vstup.\n");
        return 0; // Alebo iná hodnota indikujúca chybu
    }
    return number;
} else {
    fprintf(stderr, "Chyba pri čítaní vstupu.\n");
    return 0; // Alebo iná hodnota indikujúca chybu
}
```

note: buffer malý, nekontroluje sa, či sme načítali celý riadok, number > UINT32_MAX nikdy nenastane.


### To isté v C++

> C++20, napíš funkciu, ktorá nemá žiadne parametre a vráti uint32_t, ktorý prečíta zo stdin. 

```cpp [|11-13]
uint32_t value;
std::cout << "Zadajte nezáporné celé číslo (uint32_t): ";
while (true) {
    std::cin >> value;

    // Skontrolujeme, či bol vstup úspešný a či je hodnota v platnom rozsahu
    if (std::cin.good()) {
        break; // Vstup bol úspešný
    }
    else {
        std::cin.clear(); // Obnovíme stav cin
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorujeme zlý vstup
        std::cout << "Neplatný vstup. Skúste to znova: ";
    }
}
return value;
```

note: cin si okontroluje overflow

---

# Quick quiz 

---

## Shortest C++ program?

<div style="display: flex;">
  <div style="flex: 1;">

```cpp
int main() { }
```
  </div>
  <div style="flex: 1;">

```cpp
int main() {
    return 0;
}
```
  </div>
</div>

<ul>
  <li class="fragment">

Aká je hodnota vrátená z funkcie `main`?
  </li>
  <li class="fragment">

Ak návratová hodnota nie je špecifikovaná, použije sa `0` ako implicitná návratová hodnota z funkcie `main`.
  </li>
  <li class="fragment">

Môžeme namiesto int použiť `void`?

```cpp
void main() { }
```
  </li>
  <li class="fragment">

Nie, štandard umožňuje iba `int` ako návratovú hodnotu.
  </li>
</ul>

---

## Parametre funkcie main

<ul>
  <li>

Ktoré z nasledujúcich prototypov funkcie main sú platné v C++ programe?

```cpp
int main() { } // 1
int main(int argc) // 2
int main(int argc, char** argv) // 3
int main(int argc, char* argv[]) // 4
int main(int argc, char** argv, char** x) // 5
```
  </li>
  <li class="fragment">

V podstate sú všetky dobré, ale iba bez parametrov a (`int`, `char**`) musia dovoľovať všetky implementácie, ostatné sú *implementation defined*.
  </li>
</ul>

---

## `argc` a `argv`

<ul>
  <!-- we need this to compensate for default margin and i do not want to create new one off class -->
  <style scoped>
    p {
        margin: 0.3em !important;
    }
  </style>
  <li style="display: flex; align-items: center;">
    <div style="flex: 2;">

Čo je `argc`?
    </div>
    <div class="fragment" style="flex: 2;">

Počet parametrov predaných z prostredia do programu (počet konzolových parametrov +1).
    </div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

Čo je `argv`?
    </div>
    <div class="fragment" style="flex: 2;">

Pole parametrov predaných z prostredia do programu (konzolové parametre + ...).
    </div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

Akú hodnotu má `argv[0]`?
    </div>
    <div class="fragment" style="flex: 2;">

Názov programu alebo `""`.
    </div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

Akú hodnotu má `argv[1]`?
    </div>
    <div class="fragment" style="flex: 2;">

Prvý parameter.
    </div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

Akú hodnotu má `argv[argc-1]`?
    </div>
    <div class="fragment" style="flex: 2;">

Posledný parameter.
    </div>
  </li> 
  <li style="display: flex;">
    <div style="flex: 2;">

Akú hodnotu má `argv[argc]`?
    </div>
    <div class="fragment" style="flex: 2;">

`0` (`NULL`)
    </div>
  </li> 
</ul>

---

# ĎAKUJEM

## Otázky?
