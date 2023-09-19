# C++

## Úvod do predmetu

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Ciele predmetu
* Organizácia štúdia
* Prerekvizity
* Ľahký úvod do C++
* C vs. C++
* Moderné C++

---

# Ciele predmetu

---

## Prečo práve tento predmet?

* C++ s nami ešte chvíľu ostane
* Pokusy o nahradenie inými jazykmi tu boli, ale viac menej všetky zlyhali
* Zameriame sa na implementácie riešení problémov v C++, nie na návrh riešení
* Dôraz bude aj na "výzor" kódu a na jeho bezpečnosť ("škaredý kód" môže mať ťažšie získať plný počet bodov)

---

## Disclaimer

* Autori prednášok (a cvičení) nepoužívajú C++ na všetku svoju prácu
* Tiež si nemyslia, že C++ je najlepší programovací jazyk na svete 
* C++ je málokedy správna odpoveď na problém

---

## Programovanie v C++

* Hlavný cieľ je naučiť sa programovať v C++ s dôrazom na moderné a bezpečné konštrukcie
* Pokiaľ sa bude dať, budeme používať štandardnú knižnicu

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

A Tour of C++  
Bjarne Stroustrup  
Addison-Wesley Professional; (September 24, 2022)  
ISBN-13: 978-0136816485  
<https://www.amazon.com/Tour-C-Bjarne-Stroustrup-dp-0136816487/dp/0136816487/>  
Časti prístupné online - <https://isocpp.org/tour>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/Tour3English-large.jpg" alt="A Tour of C++ (3rd edition)" style="width: 70%;" />
</div>
</div>

---

## Literatúra

* TODO update this
* Stránka predmetu
* <https://cppseminar.eset.sk>


<div style="display: flex; align-items: center;">
<div style="flex: 7;">

C++17 In Detail  
Bartłomiej Filipek  
Independently published (July 18, 2019)  
ISBN-13: 978-1798834060  
<https://www.amazon.com/17-Detail-Exciting-Features-Standard/dp/1798834065>  
<https://www.cppindetail.com/>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/cpp17indetail.png" alt="C++17 In detail" style="width: 70%;" />
</div>
</div>


<div style="display: flex; align-items: center;">
<div style="flex: 7;">

C++20 - The Complete Guide  
Nicolai M. Josuttis  
Independently published (November 7, 2022)  
ISBN-13:  978-3967309201   
<https://www.amazon.de/-/en/Nicolai-M-Josuttis/dp/3967309207/>  
<https://cppstd20.com/>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/covercpp20opt255x317.png" alt="C++20 - The Complete Guide" style="width: 70%;" />
</div>
</div>

---

## Výsledok nášho snaženia

* C++ v roku 2023
* Beautiful code
* Fun! (sort of 😀)

![Compile and works first time, what did I do wrong?](./lectures/1_intro/joke-sort-of.png)

---

# Organizácia štúdia

---

## Kontakt

<ul>
  <li>
    Kontaktná e-mail adresa je
    <p style="font-size: larger; text-align: center;">
      <a href="mailto:cpp@eset.sk">cpp@eset.sk</a>
    </p>
  </li>
  <li>
    <a href="cppseminar.eset.sk">Stránka predmetu</a>
    <ul>
      <li>Oznamy</li>
      <li>Užitočné linky</li>
      <li>Úlohy</li>
    </ul>
  </li>
</ul>

---

## Rozvrh

* Prednášky budú každý týždeň 
    * Pondelok 11:00, trvanie 1,5 hod
    * Miestnosť -1.65 (Aula Minor) na FIIT
* Cvičenia budú každý týždeň
    * 1. skupina utorok 14:00
    * 2. skupina utorok 16:00
    * 3. skupina streda 15:00
    * ESET Lab (miestnosť 4.46 na FIIT), maximálna kapacita +-16 ľudí 
    * Prvý krát môžete prísť na ľubovoľné cvičenie, ak bude problém s kapacitou nejak to vyriešime

---

## Dochádzka

* Prednášky aj cvičenia sú nepovinné, nebudeme kontrolovať dochádzku
* Na niektorých cvičeniach ale budú úlohy a testy, tak tam odporúčame príjsť

![Travolta looking very confused](./lectures/1_intro/travolta.gif)

---

## Hodnotenie

* Rozdelenie hodnotenia
   * Počas semestra sa bude dať získať 60 bodov
   * Na skúške potom zvyšných 40 bodov
* Známkovanie
   * A (100-92)
   * B (91-83)
   * C (82-74)
   * D (73-65)
   * E (64-56)
* Žiadna časť predmetu nie je povinná, takže ak budete mať zo semestra viacej ako 55 bodov, tak na skúšku ani nemusíte chodiť a máte E

---

## Skuška

* Pozostáva z dvoch častí
   * Test na preskúšanie teórie (ako rozumiete C++)
   * Programovanie
* Na konci semestra bude "testovacia" skúška, aby ste si to mohli vyskúšať

* Teoreticky sa toto rozdelenie ešte môže zmeniť

---

## Projekt

* Počas semestra budeme mať aj jeden väčší projekt
* Predstavený bude niekedy v prvých týždňoch semestra
* Bude sa dať získať 20 bodov, plus nejaký bonus

---

## Cvičenia

* Občas budú úlohy a testy (vždy dopredu oznámime)
* Teoretické testy 
   * 2 x 10 bodov
   * Desať otázok so 4 možnosťami
   * Na papier
* Praktické úlohy
   * 2 x 10 bodov
   * Odovzdávanie bude cez náš portál
   * Na vlastných PC, alebo na tých čo sú v učebni
* Inak sa budeme venovať tomu čo sa prebralo na prednáške
* Zadanie a konzultácie projektu budú tiež na cvičeniach

---
 
# Prerekvizity

---

## Trochu skúseností s programovaním

<div style="display: flex; align-items: center;">
<div style="flex: 1;">
  
* Predpokladáme aspoň základnú znalosť programovania v C, alebo rovno C++
* Pravdepodobne sa dá predmet zvládnuť aj keď poznáte skôr iné jazyky
* Musíte ale poznať základné koncepty
</div>
  <div style="flex: 1;">
    <img src="./lectures/1_intro/code-works.png" alt="My code doesn't work I have no idea why" />
  </div>
</div>

---

## Koncepty, ktoré by ste mali ovládať

* Čo sú to *typy* – `int`, `char`, `string`
* Ako funguje *control flow* – `if`, `for`, `while`
* *Funkcie* a ich volanie, *rekurzia*
* Letmo sa týchto tém dotkneme na nasledujúcej prednáške, ale určite sa im nebude venovať do hĺbky
* V podstate by ste mali byť schopný "čítať" program v C

---

## Platforma

* Budeme používať MS Windows a na ňom najnovšie Visual Studio 2022 (úplne stačí [Community edition](https://visualstudio.microsoft.com/vs/community/))
* Všetko by malo fungovať aj na iných platformách (Linux, Mac)
* C++20 kompatibilný kompilátor
* Pre VS2022 máme pripravený aj template, ktorý všetko nastaví ako má
* Niečo existuje aj pre VS Code

---

## Algoritmy

* Na cvičeniach ani skúške nebudeme priamo od vás chcieť vymýšľať/študovať algoritmy (maximálne ako bonus), no veľmi odporúčam algoritmy dátové štruktúry poznať
* Ak ste také predmety nemali, tak odporúčam si ich dať
* Môžeme od vas ale chcieť algoritmy naimplementovať

---

<!-- .slide: data-background-image="./lectures/1_intro/DALL·E 2023-07-24 00.32.08 - elephant in the barely lit room.png" data-background-opacity="0.2" -->

<img class="fragment" src="./lectures/1_intro/ChatGPT_logo.svg" alt="Chat-GPT logo" width="400" />

---

# Náš prvý program

---

## Čítanie čísel

<ul>
  <li>Musíme niekde začať, tak skúsme niečo jednoduché</li>
  <li class="fragment">Úloha, naprogramujte aplikáciu, ktorá najprv načíta čísla zo vstupu (prázdny riadok bude koniec zadávania) a potom ich vypíše ich druhú mocninu usporiadanú podľa veľkosti</li>
  <li class="fragment">Level prvý ročník stredná škola</li>
</ul>


### Pseudokód (Python)

```py
numbers = []
while True:
    num = input("")
    if num == "":
        break

    number = int(num)
    numbers.append(number)

sorted_squares = sorted([num**2 for num in numbers])

for square in sorted_squares:
    print(square)
```

---

## Najprv C

```c
#include <stdio.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main() {
    int arr[1024]; // will be enough for everyone
    int n = 0;
    while (scanf("%d", &arr[n]) > 0) {
        ++n;
    }
    for (int i = 0; i < n; ++i) {
        arr[i] *= arr[i];
    }
    qsort(arr, n, sizeof(int), cmpfunc);
    for (int i = 0; i < n; ++i) {
        printf("%d", arr[i]);
    }
}
```


## Problémy

<ul>
  <li>Nefunguje to 😉</li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```c
int n = 0;
while (scanf("%d", &arr[n]) > 0) {
    ++n;
```
</div>
    <div style="flex: 3;">

Prázdny string to neukončí, `scanf` totiž čaká na aspoň jeden nonwhitespace znak aby z neho naformátoval číslo.
    </div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```c
int main() {
    int arr[1024];
    int n = 0;
```
</div>
    <div style="flex: 3;">
      Takýto veľký buffer určite nebude stačiť, vystavuje sa tým bezpečnostným problémom (buffer overflow).
    </div>
  </li>
</ul>

---

## Použijeme `malloc`

```c
#include <stdio.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main() {
    int cap = 1024;
    int* arr = (int*)malloc(cap * sizeof(int));
    int n = 0;
    char buf[128];
    while (scanf("%s", buf) > 0) {
        if (buf[0] == 0)
            break;
        arr[n] = atoi(buf);
        ++n;
        if (n == cap) {
            cap += 1024;
            arr = (int*)realloc(arr, cap * sizeof(int));
        }
    }
    for (int i = 0; i < n; ++i) {
        arr[i] *= arr[i];
    }
    qsort(arr, n, sizeof(int), cmpfunc);
    for (int i = 0; i < n; ++i) {
        printf("%d", arr[i]);
    }
}
```


## Problémy

<ul>
  <li>Stále to nefunguje 😎</li>
  <li class="fragment" style="display: flex;">
<div style="flex: 2;">

```cpp [2]
char buf[128];
while (scanf("%s", buf) > 0) {
    if (buf[0] == 0)
```
</div>
    <div style="flex: 3;">

Prázdny string to neukončí, `scanf` totiž čaká na aspoň jeden nonwhitespace znak ak aj chceme `%s`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
<div style="flex: 2;">

```cpp [2]
int n = 0;
char buf[128];
while (scanf("%s", buf) > 0) {
```
</div>
    <div style="flex: 3;">
      Ak by to aj šlo, máme to ďalší fixný buffer.
    </div>
  </li>
  <li class="fragment" style="display: flex;">
<div style="flex: 2;">

```cpp [2]
   break;
arr[n] = atoi(buf);
++n;
```
</div>
    <div style="flex: 3;">
      <code>atoi</code> môže zlyhať.
    </div>
  </li>
  <li class="fragment" style="display: flex;">
<div style="flex: 2;">

```cpp [2]
    cap += 1024;
    arr = (int*)realloc(arr, cap);
}
```
</div>
    <div style="flex: 3;">

`realloc` nepoužívame dobre, tiež môže zlyhať.
    </div>
  </li>
  <li class="fragment">
    Zaćína to byť dosť komplikované a labilné.
  </li>
</ul>

---

## Chat-GPT

```c [|6|10|15|2,4]
int main() {
    int numbers[100];
    int count = 0;
    char input[100];
        
    while (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] == '\n')
            break;
        
        numbers[count] = atoi(input);
        count++;
    }
    
    for (int i = 0; i < count; i++) {
        numbers[i] = pow(numbers[i], 2);
    }
    
    qsort(numbers, count, sizeof(int), compare);
    
    for (int i = 0; i < count; i++) {
        printf("%d\n", numbers[i]);
    }
    
    return 0;
}
```

* `include`, funkcia `compare` a komentáre vynechané kvôli prehľadnosti

note: `fgets` číta po koniec riadku, nie je to ale OK, lebo ak tam niekto napráska viacej cifier ako 100, tak už nie sme podľa zadania.

---

## C++ cca rok 1999

```cpp
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> arr;
    std::string buf;

    while (std::getline(std::cin, buf)) {
        if (buf == "")
            break;
        arr.push_back(std::stoi(buf));
    }

    for (std::vector<int>::iterator it = arr.begin(); it != arr.end(); ++it) {
        *it *= *it;
    }

    std::sort(arr.begin(), arr.end());
    for (std::vector<int>::iterator it = arr.begin(); it != arr.end(); ++it) {
        std::cout << *it << ' ';
    }
}
```


* Dáta ukladáme do dynamického poľa (`std::vector`)
* Ako buffer na čítanie vstupu použijeme `std::string`
* Keďže používamé `std::sort` nemusime programovať porovnávaciu funkciu
* Na výpis používame `std::cout`

---

## C++ cca rok 2012

```cpp
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> arr;

    std::string buf;
    while (std::getline(std::cin, buf)) {
        if (buf == "")
            break;

        arr.push_back(std::stoi(buf));
    }

    for (auto& i : arr) {
        i *= i;
    }

    std::sort(std::begin(arr), std::end(arr));
    for (auto& i : arr) {
        std::cout << i << ' ';
    }
}
```

---

## C++ cca rok 2022

```cpp
import <iostream>;
import <vector>;
import <string>;
import <algorithm>;

int main() {
    std::vector<int> numbers;
    std::string input;
    
    while (std::getline(std::cin, input) && !input.empty()) {
        numbers.push_back(std::stoi(input));
    }

    std::ranges::transform(numbers, numbers.begin(), [](int number) { return number * number; });

    std::ranges::sort(numbers);

    for (int number : numbers) {
        std::cout << number << std::endl;
    }
}
```


## Problémy

* Pre úspešné skompilovanie treba pridať rôzne experimental prepínače
* Na MSVC je potrebné zapnúť podporu modulov a generovanie header unitov
* Aj 3 roky po oficiálnom zapracovaní modulov do štandardu je to bieda 😢

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

* C++ sa na začiatku volalo C with classes
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
* Kedysi bolo dosť dôležité poznať tieto triky, neboli iné štandardizované konštrukcie
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
* Major update v 2011 C++11, pridané type inference, lambdy, rval references
* Ďalšie revízie C++14, C++17, C++20, C++23
* <https://isocpp.org/>
</div>
<div style="flex: 3;">

<img src="./lectures/1_intro/bjarne.png" alt="Bjarne Stroustrup" width="100%" />
</div>
</div>

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
std::vector<int> nums{ 2, 3, 5, 7 };
std::string s = "Hello world";
s = nums;
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
<!-- .element: class="showall" -->


### Aký je výsledok nasledujúceho programu?

* Skompiluje sa? Je to definované? Aká je hodnota v `s`?

```cpp
std::string s = "Ingsoc"; 
s = 19.84;
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


### C++ používa primitíva, ktorá samy spravujú pamäť

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
