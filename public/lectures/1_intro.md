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

* Ak v bežnom kóde musíme explicitne volať `free()` alebo `delete`, pravdepodobne nepoužívame najvhodnejší spôsob správy pamäte
* C++ nemá garbage collector, no objekty, kontajnery a smart pointery sa môžu o uvoľnenie pamäte postarať automaticky
* Presnejšie: C++ umožňuje manuálnu správu pamäte tam, kde je potrebná, ale v modernom kóde sa jej snažíme vyhnúť
</div>
<div style="flex: 3;">

![Do I cast the result of malloc](./lectures/1_intro/EYm0ylHX0AANjFj.jpg)
</div>
</div>


## C with classes

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

* C++ sa na začiatku vývoja aj volalo **C with Classes**
* C a C++ sú dnes dva samostatné jazyky, ktoré majú vlastné
    * štandardy
    * komunitu
    * idiomy
* C++ si však zachováva vysokú mieru kompatibility s C, hoci nie každý C program je zároveň platným C++ programom
* C kód sa preto dá do C++ integrovať pomerne jednoducho; opačným smerom treba vytvoriť C kompatibilné rozhranie
</div>
<div style="flex: 3;">

![C with C++ diagram](./lectures/1_intro/c-with-classes.png)
</div>
</div>


## C++ je plné metaprogramovania 

* C++ podporuje metaprogramovanie, najmä pomocou šablón (templates)
* V minulosti bolo dôležité poznať zložité šablónové techniky, pretože často neexistovali jednoduchšie štandardné riešenia
* Dnes sa bežný aplikačný kód bez pokročilého metaprogramovania väčšinou zaobíde
* Metaprogramovanie má stále svoje miesto, napríklad pri
  * tvorbe generických knižníc
  * výpočtoch počas prekladu
  * kontrole typov a zjednodušení kódu


## C++ je vždy rýchlejšie

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

* C++ umožňuje písať veľmi efektívny kód, ale samotný jazyk vysoký výkon nezaručuje
* Výsledný výkon závisí najmä od
    * algoritmov a dátových štruktúr
    * práce s pamäťou
    * kvality implementácie a nastavenia kompilátora
* Dobre napísaný program v inom jazyku môže byť rýchlejší ako zle napísaný program v C++
* C++ ponúka efektívne abstrakcie, ale stále treba rozumieť tomu, čo program robí
</div>
<div style="flex: 3;">

![C++ speed](https://i.programmerhumor.io/2025/07/22c401d4afb13fb3bcada83aaf285a88b00fbcc7233d0f8c66db0a45b662b3ed.jpeg)
</div>
</div>


## C++ je objektovo orientovaný jazyk

* C++ podporuje objektovo orientované programovanie, ale nie je naň obmedzené
* Podporuje aj procedurálne, generické a funkcionálne programovanie
* Triedy a dedičnosť sú nástroje, nie povinný spôsob návrhu každého programu
* Vhodný štýl závisí od problému, ktorý riešime

---

## C++ ...

* je moderný **multiparadigmový** jazyk, ktorý podporuje procedurálne, objektové, generické aj funkcionálne programovanie
* poskytuje **abstrakcie** pre bežné algoritmy, kontajnery, prácu s pamäťou aj systémové operácie
* umožňuje písať **vysokoúrovňový kód** a zároveň zachovať **kontrolu** nad výkonom, pamäťou a hardvérom
* za efektívnosť však platíme väčšou zložitosťou a zodpovednosťou programátora
* zachováva kompatibilitu s C a používa sa od zabudovaných systémov až po veľké aplikácie
* aktívne sa vyvíja a má rozsiahlu **komunitu** (CppCon, C++ Now, Meeting C++)

---

## C++ roadmap

![C++ roadmap](./lectures/1_intro/timeline-2022-07.png)
<!-- .element: class="r-stretch" style="background: white;" -->

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

## Z čoho sa skladá C++

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

* Jazyk C++
  * Typový systém a syntax
  * Riadenie toku programu
  * Triedy, dedičnosť a polymorfizmus
  * Šablóny a koncepty
  * Model objektov a pamäte
</div>
<div style="flex: 1;">

* Štandardná knižnica
  * Kontajnery, iterátory a ranges
  * Algoritmy a numerické operácie
  * Reťazce a regulárne výrazy
  * Smart pointery, `optional`, `variant` a ďalšie utility
  * Súbory, streamy a filesystem
  * Vlákna, atomiky a synchronizácia
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


* GCC, Clang aj MSVC majú kvalitnú implementáciu C++ štandardu a produkujú efektívny kód
* Podpora najnovších vlastností štandardu môže byť medzi kompilátormi rozdielna a často nepríde naraz
* Kód používajúci štandardné vlastnosti C++ je väčšinou dobre prenositeľný medzi platformami

---

## ISO Standard

* Málo jazykov má oficiálny ISO štandard, C++ ho má
* The Committee: WG21
* Zapojený ľudia priamo z priemyslu (Intel, Microsoft, Google, Red Hat, IBM a iné)

![C++ comitee C+20](./lectures/1_intro/comittee.png)

---

# Vlastnosti C++

---

## Abstrakcie bez zbytočnej réžie

* C++ umožňuje zabaliť implementačné detaily do typov a funkcií s jednoduchým rozhraním
* Dobre navrhnutá abstrakcia nemusí byť pomalšia ako ručne napísaný nízkoúrovňový kód
* Tento princíp sa označuje ako **zero-overhead abstraction**

```cpp
std::vector<int> values{4, 1, 3, 2};
std::ranges::sort(values);
```

`std::vector` spravuje pamäť a `std::ranges::sort` pozná typ prvkov. Kompilátor pritom stále dokáže vytvoriť efektívny strojový kód.

---

## Kontrola nad výkonom

* C++ nepredpisuje garbage collector ani virtuálny stroj
* Programátor môže ovplyvniť rozloženie dát, alokácie aj životnosť objektov
* Za túto kontrolu platíme väčšou zodpovednosťou: jazyk nás nechráni pred každou chybou
* Výkon treba merať; použitie C++ ho samo osebe nezaručuje
* There is no room for other language between C++ and metal
* **LBYL** – look before you leap (opposed to **EAFP**)

```cpp
struct Point {
  float x;
  float y;
};

std::vector<Point> points;
points.push_back({1.0f, 2.0f});
points.pop_back();
//points.pop_back(); // ouch
```

note: EAFP = easier to ask forgiveness than permission

---

## Deterministická životnosť objektov

* Objekt sa zničí v presne určenom okamihu, napríklad pri opustení bloku
* Deštruktor môže automaticky uvoľniť pamäť, zavrieť súbor alebo odomknúť mutex
* Tento princíp sa nazýva **RAII**

```cpp
void write_report() {
  std::ofstream file{"report.txt"};
  file << "hotovo\n";
} // súbor sa tu automaticky zavrie
```

C++ nepotrebuje garbage collector na bežnú správu zdrojov. RAII navyše spravuje aj zdroje, ktoré garbage collector nerieši.

> C++ is my favorite garbage collected language because it generates so little garbage.  
>           — Bjarne Stroustrup

---

## Portabilita

* Štandardný C++ kód možno preložiť rôznymi kompilátormi a pre rôzne platformy
* Štandardná knižnica poskytuje prenositeľné rozhrania napríklad pre vlákna, čas a súborový systém
* Platformovo špecifický kód je vhodné oddeliť za vlastné rozhranie

```cpp
if constexpr (std::endian::native == std::endian::little) {
  // rozhodnutie podľa vlastnosti cieľovej platformy
}
```

**Portabilitu obmedzujú závislosti od operačného systému, rozšírenia kompilátora a nedefinované správanie.**

---

## Spolupráca s C

* C++ dokáže volať C knižnice a exportovať funkcie s C linkage
* C rozhranie je jednoduchý spoločný menovateľ aj pre mnohé ďalšie jazyky
* C ABI však neprenáša C++ triedy, preťažené funkcie ani výnimky

```cpp
extern "C" int count_primes(std::uint32_t limit) noexcept {
  // C kompatibilné rozhranie
}
```

---

## Statický typový systém

* Typy sa kontrolujú počas prekladu a mnohé chyby sa odhalia ešte pred spustením programu
* `auto` typ neodstraňuje; kompilátor ho odvodí z inicializačného výrazu
* Silné typy dokážu zabrániť zámene hodnôt s odlišným významom

```cpp
std::vector<int> numbers{2, 3, 5, 7};
auto count = numbers.size(); // typ je std::size_t

std::string text = numbers; // chyba počas prekladu
```

**Premenné inicializujeme hneď pri deklarácii, aby nikdy neexistovali bez platnej hodnoty.**

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

## Nedefinované správanie

* Niektoré chyby nemajú štandardom určený výsledok
* Kompilátor môže predpokladať, že v korektnom programe nikdy nenastanú
* Program preto môže zdanlivo fungovať a zlyhať po zapnutí optimalizácií alebo na inej platforme

```cpp
std::vector<int> values{10, 20, 30};
std::cout << values[3]; // prístup mimo rozsahu: undefined behavior
```

Pri vývoji pomáhajú warnings, sanitizery, statická analýza a testy. Ak chceme kontrolu rozsahu, môžeme použiť `values.at(3)`.

---

## Generické programovanie

* Šablóny umožňujú písať algoritmy nezávislé od konkrétneho typu
* Koncepty pomenúvajú požiadavky na typy a zlepšujú chybové hlásenia
* Rovnaká abstrakcia tak môže fungovať pre rôzne typy bez runtime polymorfizmu

```cpp
template<std::totally_ordered T>
T smaller(T left, T right) {
  return left < right ? left : right;
}
```

---

## Knižnice a nástroje

* Štandardná knižnica pokrýva kontajnery, algoritmy, vlákna, filesystem a mnoho ďalších oblastí
* Rozsiahly ekosystém dopĺňajú napríklad **Boost**
* C++ nemá jeden povinný build system ani package manager
  * často sa používa **CMake**
  * závislosti môžu spravovať **Conan** alebo **vcpkg**
* Flexibilita ekosystému je výhoda, ale komplikuje zostavenie a distribúciu projektov


### [Boost](https://www.boost.org/)

* Rozsiahla a modulárna zbierka kvalitných open-source C++ knižníc
* Pokrýva napríklad sieťovú komunikáciu, geometriu, parsovanie, grafy, matematiku a prácu so systémom
* Viaceré knižnice ovplyvnili štandardné C++ alebo sa stali základom jeho neskorších súčastí
* Boost však nie je súčasťou štandardnej knižnice a jednotlivé moduly sa líšia rozhraním, závislosťami aj náročnosťou použitia

<img src="./lectures/1_intro/Boost.png" alt="boost logo" style="width: 30%; background: white;" />

---

## Cena za kontrolu

* Bežný C++ kód nemusí používať všetky pokročilé vlastnosti jazyka
* Jazyk však zachováva desaťročia spätnej kompatibility a ponúka viac úrovní abstrakcie
* Výsledkom je vysoký potenciál, ale aj rozsiahly a zložitý jazyk
* Dôležité je používať moderné idiomy, štandardnú knižnicu a automatické nástroje kontroly

---

## Je C++ ťažký jazyk?

* Základy moderného C++ sa dajú naučiť bez poznania všetkých detailov jazyka
* Pri bežnom programovaní si vystačíme s rozumnou podmnožinou, štandardnou knižnicou a zaužívanými idiomami
* Náročnosť rastie pri návrhu knižníc, optimalizácii výkonu, práci s viacerými platformami a starším kódom
* Do hĺbky treba rozumieť **životnosti objektov**, **typovému systému**, **šablónam** aj **nedefinovanému správaniu**
* Cieľom nie je poznať celé C++, ale vedieť bezpečne vybrať správne nástroje pre daný problém

![Here be dragons](./lectures/1_intro/Psalter_World_Map,_c.1265_dragons.jpg)

---

# C vs. C++

---

![yoda](./lectures/1_intro/yoda.png)

> You must unlearn what you have learned
>           — Master Yoda

* V tejto časti si ešte ukážeme C konštrukcie v ďaľších prednáškach sa tiež môžu vyskytnúť ale iba okrajovo, keďže stále sa s nimi môžete stretnúť, ale do moderného C++ nepatria

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
void sort_cpp(size_t n) {
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
* V podstate mŕtvy projekt

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
* Stále vo vývoji

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

# Quick quiz `main`

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
