# C++

## Nový začiatok

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Hello world!
* Typy, základné operácie a funkcie
* Základné zložené typy
* Smerníky a referencie
* Príkazy na riadenie programu
* Kompilácia, CMake

---

# Hello world!

```cpp
#include <iostream>
 
int main() {
    std::cout << "Hello world!\n";
}
```


* C++20 prinieslo moduly a C++23 štandardné knižničné moduly `std` a `std.compat`. Podpora v kompilátoroch a build systémoch je stále neúplná.
* My budeme takí hybridní, budeme používať moduly, ale spomenieme aj hlavičkové súbory.

```cpp
import std;

int main() {
    std::println("Hello World");
}
```

---

* C++ programy sa začínajú vykonávať volaním globálne prístupnej funkcie s názvom `main`
* Odporúčané signatúry sú

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
int main();

int main(int argc, char* argv[]);
```
</div>
<div style="flex: 1;">

Zvyčajne `argc` obsahuje počet parametrov + 1, `argv` potom obsahuje ako prvý prvok meno (cestu) programu a potom parametre
</div>
</div>

---

## `import`

* V C++20 pribudli moduly, ktoré môžu výrazne zrýchliť kompiláciu knižníc a zjednodušiť ich použitie
* Používajú kľúčové slová `import` a `export`
* Napríklad keď chceme použiť štandardnú knižnicu, môžeme ju importovať ako modul:

```cpp
import std; // use this
import std.compat; // more symbols, for compatibility with older codebases
```

---

## `#include <iostream>`

* `#include` iba vloží súbor (zvyčajne hlavičkový súbor [header file]) do iného súboru 
* Súbory v `<>` obsahujú štandardné a/alebo systémové knižnice. Štandardné by mali byť dostupné na všetkých implementáciách vyhovujúcich danej verzii štandardu (my používame C++20/23)
* Súbory v `""` obsahujú naše [user defined] hlavičkové súbory

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
#include <memory>
#include <future>
#include <stdio.h>
#include <string.h>
```
</div>
<div style="flex: 1;">

Súbory bez prípony `.h` sú C++ štandardné hlavičkové súbory. Funkcie C knižnice sú v C++ dostupné aj cez hlavičky ako `<cstdio>` a `<cstring>`, ktoré preferujeme pred `<stdio.h>` a `<string.h>`. 
</div>
</div>

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
#include <Windows.h>
#include <unistd.h>
```
</div>
<div style="flex: 1;">

Systémový hlavičkový súbor.
</div>
</div>


### Výhody `import std;` oproti `#include`

* Rýchlejšie kompilovanie, pretože modul sa načíta len raz
* Celá štandardná knižnica je dostupná, nie je potrebné `#include` jednotlivé hlavičkové súbory
* Jednoduchšie používanie, pretože nie je potrebné pamätať si všetky hlavičkové súbory a ich závislosti
* Budúcnosťou C++ je modulárny systém, ktorý nahradí tradičné `#include` direktívy

---

## Namespace `std::`

* Skoro všetky užitočné triedy a funkcie sú "skryté" v namespace `std`
* Vždy, keď chceme niečo použiť zo štandardnej knižnice, musíme najprv `import`-nuť štandardnú knižnicu a potom s prefixom `std::` nájdeme triedy/funkcie, ktoré chceme 

![Kniha C++ štandardná knižnica](./lectures/2_basics/c++-standard-lib.png)

---

## `std::println`

* `std::println` je nová funkcia v C++23
* Umožňuje jednoduché vypisovanie na štandardný výstup s automatickým novým riadkom
* Podobná ako `std::cout` s `'\n'`, ale pohodlnejšia

```cpp
std::println("Hello, {}!", "world");
```

* `{}` je zástupný symbol pre hodnotu, ktorá sa má vložiť do reťazca.

---

## `std::cout`

* `cout` je globálna premenná, reprezentujúca štandardný výstup
* Používa stream interface `std::ostream`
* Na vloženie naformátovaného výstupu slúži `operator<<`
    * Binárny operátor left shift (`a << b`) je preťažený v `std::ostream`
    * Vracia `a&`, takže umožňuje reťazenie 
    * Najprv naformátuje podporované dáta a potom ich vypíše

```cpp
std::cout << 3 << ' ' << "things.";
```

---

## `'\n'`

* Nový riadok (new line)
* V literatúre sa vyskytuje aj `std::endl`
* `std::endl` tiež flush-ne buffer
    * Pomalší zápis
  * Dáta sa odošlú do operačného systému, ale ich fyzické uloženie to negarantuje

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
std::cout << std::endl;

std::cout.put(std::cout.widen('\n'));
std::cout.flush();
```
</div>
<div style="flex: 1;">

Dva ekvivalentné programy. Funkcia `widen` konvertuje znak pomocou aktuálne používaného `locale` do typu, aký používa stream.
</div>

---

## Undefined behavior

![Unicorns and rainbows](./lectures/2_basics/unicorn.png)


* Undefined behavior nastáva, keď program vykonáva operácie, ktoré nie sú definované štandardom C++. Môže viesť k nepredvídateľným výsledkom, pádom programu alebo bezpečnostným chybám.
* Kompilátor nemusí varovať pred undefined behavior, preto je dôležité byť opatrný pri písaní kódu.
* Kompilátor môže optimalizovať kód spôsobom, ktorý predpokladá, že undefined behavior nenastane, čo môže viesť k nečakaným výsledkom, ak nastane.


```cpp
int test(int a) {
  if (a < a + 1) {
    return true;
  } else {
    return false;
  }
}
```

* Funkcia sa skompiluje na `return true`
<!-- .element: class="fragment" -->
* Tento jav je spôsobený tým, že pri type `int` je výraz `a < a + 1` vždy pravdivý, pokiaľ nenastane signed overflow, ktorý je undefined behavior. Unsigned overflow je naopak definovaný modulo $2^N$.
<!-- .element: class="fragment" -->

---

# Typy, základné operácie a funkcie

---

## Typy

* C++ je staticky typovaný jazyk (podobne ako C)
* Preto pred prvým použitím premennej musíme definovať jej typ
* Deklarácie typu sú podobné ako v C
* Syntax je `typ` `názov` `= hodnota` `;`, kde `= hodnota` je voliteľná

```cpp
int i; // signed integer uninitialized (0 or undefined)
```

---

## Primitívne typy

* Všetky typy z jazyka C sú podporované

```cpp
int i; // signed integer uninitialized (0 or undefined)
unsigned int u = 1337ul; // unsigned integer
bool ok = false; // true/false
double pi = 3.14159; // floating point double precision
float e = 2.71828f; // floating point single precision
char c = 'a'; // variant of ISO646 - ASCII
size_t n = 1'000'000'000; // possible to separate with "'"
```

* `short`, `long`, `long long` (rozdielne znamienkové signed typy)
* `unsigned short`, `unsigned`, `unsigned long` (pre neznamienkové typy)

---

## Presná bitová veľkosť

* `long` je na niektorej platforme 32bit a na inej 64bit, podobne aj `size_t`
* Riešia to typy definované v súbore `#include <cstdint>`
* Nemusia byť definované na všetkých platformách (`CHAR_BIT == 8`)
* Odporúčame zvyknúť si skoro vždy používať tieto typy

```cpp
int8_t i8 = 127;
int16_t i16 = 32'000;
uint32_t u32 = 4'000'000'000;
int64_t i64 = 10'000'000'000'000;
```

note: CHAR_BIT je nastavené na číslo, ktoré reprezentuje počet bitov v type `char`. Platformy kompatibilné s POSIX požadujú `8`.

---

## Usporiadanie v pamäti

* Každá premenná má v pamäti miesto, ktoré sa dá zistiť pomocou operátora `&`
* Veľkosť typov je do istej miery závislá od implementácie a dá sa zistiť pomocou operátora `sizeof`
* Veľkosti sú v "char units" a nie bajtoch (zvyčajne je ale char unit 8bitov)

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
std::cout << sizeof(bool) << '\n'; // 1
std::cout << sizeof(short) << '\n'; // 2
std::cout << sizeof(float) << '\n'; // 4
std::cout << sizeof(long) << '\n'; // 4
std::cout << sizeof(long long) << '\n'; // 8
std::cout << sizeof(double) << '\n'; // 8
```
</div>
<div style="flex: 1;">

Na Windows je `long` typicky 4 bajty (LLP64 ABI), na 64-bitových Unix systémoch typicky 8 bajtov (LP64 ABI). Nezávisí to priamo od kompilátora.
</div>


## `std::adressof`

* Operátor `&` má jeden problém, môže byť preťažený a potom nevracia adresu premennej
* Je to síce zlý dizajn, ale keď budeme používať kód napísaný inými ľuďmi, môže sa to stať
* Riešením je použiť funkciu `std::addressof` z hlavičkového súboru `<memory>`

```cpp
#include <memory>

int* p = std::addressof(x);
```

---

## Zarovnanie typov (alignment)

* Každý typ má požiadavku na zarovnanie v pamäti (na aký násobok musí začať adresa typu)
   * Typ so zarovnaním 4 musí začínať na adrese deliteľnej 4, teda `0x0badc0d8` je OK, `0x0badc0d5` už nie
* V C++11 pribudol operátor `alignof`, ktorý vráti zarovnanie typu
* Zarovnanie je dôležité pre efektivitu a správne fungovanie na niektorých architektúrach

```cpp
std::cout << alignof(char) << '\n';    // Typicky 1
std::cout << alignof(int) << '\n';     // Typicky 4
std::cout << alignof(double) << '\n';  // Typicky 8

struct alignas(16) aligned_struct {
    int x;
};
std::cout << alignof(aligned_struct) << '\n'; // 16
```

---

## Neinicializované premenné

* Vždy inicializujte všetky premenné
    * Niektoré majú zmysluplný defaultný konštruktor 
    * Niektoré treba inicializovať explicitne

```cpp
int i = 4; // OK explicit
std::string s = "string"; // OK explicit
int j; // wrong 
std::string t; // OK, string has constructor 

int *p = nullptr; //OK
int *r; // wrong
```

---

## Deklarácia premenných

* Vždy deklarujte premenné najneskôr ako sa dá
    * Premenné patria do najvnútornejšieho scope-u
    * Toto pravidlo podporuje predchádzajúce

```cpp
int i = 0; // wrong
for (i = 0; i < 10; ++i) { } 

for (int j = 0; j < 10; ++j) { } // OK
```

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

```cpp
int k = 0;
for (int i = 0; i < 10; ++i) {
    k = i * i;
    // use k
}
```
</div>
<div style="flex: 1;">
❌
</div>
</div>

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

```cpp
for (int i = 0; i < 10; ++i) {
    int k = i * i;
    // use k
}
```
</div>
<div style="flex: 1;">
✅
</div>
</div>


### Existuje jedna výnimka 

* V cykloch, ktoré sú krátke a vykonávajú sa často, môžeme ušetriť veľa alokácií, ak presunieme niektoré premenné pred cyklus (stále ich treba inicializovať)

```cpp
for (int i = 0; i < n; ++i) {
    std::string bad = "Very long... string";
    // use string
}
```

```cpp
std::string good;
for (int i = 0; i < n; ++i) {
    good.assign("Very long... string");
    // use string
}
```

* Neplatí pre typy ako `int`, `double` a smerníky. Ich inicializácia nič nestojí. 

---

## C++ špeciality

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
int i = 7;
```
</div>
<div style="flex: 1;">

```cpp
int j(7);
```
</div>
<div style="flex: 1;">

```cpp
int k{ 7 };
```
</div>
</div>

* Okrúhle zátvorky pridané pre kompatibilitu s konštruktormi 
* Kučeravé zátvorky (C++11) pre lepšiu inicializáciu a chytenie bežných chýb počas kompilácie
* Primitívne typy by mali byť hneď inicializované

```cpp
int i = 4.2; // warning narrowing
int j(4.2); // warning narrowing
int k{ 4.2 }; // error
```


## Automatická dedukcia

* V C++11 štandarde predefinovali význam kľúčového slova `auto`

```cpp
auto i = 5; // int  
auto j = 7.5; // double 
auto sq = sqrt(i); // whatever returns sqrt (double in this case)
auto first_name = "Bjarne"; // const char*, not std::string
auto surname = std::string("Stroustrup"); // std::string

auto *ptr = &i; // int 
auto ptr = &i; // int*
auto* ptr = i; // compilation error  
```

* `auto*` sa nedá použiť, ak vydedukovaný typ nie je smerník


* Ak potrebujeme presné číselné typy, musíme použiť suffixy
   * `u` alebo `U` pre `unsigned int`
   * `l` alebo `L` pre `long`
   * `ll` alebo `LL` pre `long long`
   * `zu` pre `size_t` (C++23)

```cpp
auto a = 10l; // long
auto b = 20ul; // unsigned long
auto c = 30ull; // unsigned long long
```


## Pridlhé názvy typov 

* Týka sa hlavne šablón (`template`s)
* Slovo `auto` umožní odstrániť veľa nezaujímavého písania a šumu
* Netreba to ale preháňať

```cpp
#include <map>
#include <utility>
#include <string>
 
int main() {
    std::map<std::string, std::pair<std::string, int>> employees;
 
    auto it = employees.find("Herb Sutter"); 
    std::map<std::string, std::pair<std::string, int>>::iterator it2 = it;
}

```

---

## Almost always use `auto`

* Herb Sutter prišiel s myšlienkou, že vždy by sme mali používať `auto`

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
short i = 7;
std::string s = "default";
short* = &i;
```
</div>
<div style="flex: 1;">

```cpp
auto i = short(7);
auto s = std::string("default");
auto* = &i;
```
</div>
</div>

* Vyzerá to, že by to mohlo byť menej efektívne (kópia?) ale v skutočnosti si s tým kompilátor poradí
* Výhodou je, že typ nemôže ostať neinicializovaný, keďže `auto x;` je chyba kompilácie
* Rovnako ak zmeníme jeden typ, pomocou `decltype` vieme automaticky upraviť ďalšie

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
float f = 1.23;
float g = 0;
```
</div>
<div style="flex: 1;">

```cpp
auto f = 1.23f;
decltype(f) g = 0;
```
</div>
</div>

---

## `enum`

* `enum` je v podstate iba pomenované celé číslo
* Problém je, že takéto `enum`y nám zapĺňajú globálny namespace, keďže hodnoty sa dajú použiť bez názvu `enum`u

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
enum color {
  red = 0,
  blue, // 1
  green = 10,
  yellow, // 11
};
```
</div>
<div style="flex: 1;">

```cpp
color a = red;
color b = color::blue;
auto c = green;
int d = yellow;
```
</div>
</div>

<div class="fragment">

* Všetky premenné vpravo sa podarí skompilovať
* Často sa hodnoty `enum`u prefixovali názvom (napr. `color_red`, `color_blue`, `color_green` ...)
</div>


### scoped enums

* V C++11 pribudla možnosť vytvoriť scoped enums
* Používa sa kľúčové slovo `class`

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
enum class color {
  red = 0,
  blue, // 1
  green = 10,
  yellow, // 11
};
```
</div>
<div style="flex: 1;">

```cpp
color a = red;
color b = color::blue;
auto c = green;
int d = yellow;
int e = color::yellow;
```
</div>
</div>

<div class="fragment">

* Iba premenná `b` vpravo sa podarí skompilovať
* Strácame automatickú konverziu na `int`
</div>


### fixed underlying type

* Každý `enum` má číselný typ, ktorý tvorí jeho základ, tento určuje veľkosť aj zarovnanie
* Pred C++11 mal každý enum typ `int` (alebo iný číselný typ, ktorý vie reprezentovať všetky hodnoty `enum`u)
* Od C++11 to vieme priamo definovať

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
enum color : uint32_t {
  red = 0,
  blue, // 1
  green = 10,
  yellow, // 11
};
```
</div>
<div style="flex: 1;">

```cpp
enum class color : uint32_t {
  red = 0,
  blue, // 1
  green = 10,
  yellow, // 11
};
```
</div>
</div>

---

## Základné operácie

* Všetky `+`, `-`, `*`, `/`, `%`, `++`, `--`, `|`, `&`, `^`, `&&`, `||`, `==`, `!=`, `<=`, `>=`, `<`, `>`, `=`, `?:`, `!`, `~` pracujú ako sa od nich očakáva, dokonca sú preťažené pre zložitejšie typy (tam kde to dáva zmysel)
* Precedencia je definovaná v štandarde, pri pochybnostiach je lepšie použiť zátvorky
* **PROTIP**: Nikdy neignorujte upozornenia kompilátora (ako napríklad priradenie v `if` príkaze)


<table style="font-size: 70%;">
  <tr>
    <th>Meno</th>
    <th>Operátor</th>
    <th>Príklad</th>
    <th>Výsledok</th>
  </tr>
  <tr>
    <td>Operátor priradenia</td>
    <td><code>=</code></td>
    <td><code>a = b</code></td>
    <td><code>a</code> bude mať rovnakú hodnotu ako <code>b</code>, výsledkom je lvalue odkazujúca na <code>a</code></td>
  </tr>
  <tr>
    <td>Aritmetické operátory</td>
    <td><code>+</code>, <code>-</code>, <code>*</code></td>
    <td><code>a + b</code></td>
    <td>Ako štandardné matematické operácie, výsledkom je nová hodnota</td>
  </tr>
  <tr>
    <td>Operátor delenia</td>
    <td><code>/</code></td>
    <td><code>a / b</code></td>
    <td>Ak sú oba typy celočíselné, tak celočíselné delenie, inak bežné floating point delenie</td>
  </tr>
  <tr>
    <td>Operátor zvyšku</td>
    <td><code>%</code></td>
    <td><code>a % b</code></td>
    <td>Zvyšok po celočíselnom delení, funguje iba na celočíselných hodnotách</td>
  </tr>
  <tr>
    <td>Aritmetické priradenia</td>
    <td><code>+=</code>, <code>-=</code>, <code>*=</code>, <code>/=</code>, <code>%=</code></td>
    <td><code>a += b</code></td>
    <td>Rovnaké ako <code>a = a + b;</code>.</td>
  </tr>
  <tr>
    <td>Prefix/postfix inkrement</td>
    <td><code>++</code>, <code>--</code></td>
    <td><code>++a</code></td>
    <td>Rovnaké ako <code>a += 1;</code>.</td>
  </tr>
  <tr>
    <td>Relačné operátory</td>
    <td><code><=</code>, <code>>=</code>, <code>==</code>, <code>!=</code>, <code><</code>, <code>></code></code></td>
    <td><code>a <= b</code></td>
    <td>Výsledok je <code>true</code> alebo <code>false</code>, <code>!=</code> je nerovnosť.</td>
  </tr>
  <tr>
    <td>Logické operátory</td>
    <td><code>&&</code>, <code>||</code></td>
    <td><code>a && b</code></td>
    <td>Výsledok je <code>true</code>, ak sú <code>a</code> aj <code>b</code> pravdivé, inak <code>false</code>.</td>
  </tr>
  <tr>
    <td>Operátor negácie</td>
    <td><code>!</code></td>
    <td><code>!a</code></td>
    <td>Výsledok je <code>true</code>, ak je <code>a</code> nepravdivé, a naopak.</td>
  </tr>
  <tr>
    <td>Bitové operácie</td>
    <td><code>&</code>, <code>|</code>, <code>^</code></td>
    <td><code>a & b</code></td>
    <td>Výsledok je bitový AND.</td>
  </tr>
  <tr>
    <td>Address of</td>
    <td><code>&</code></td>
    <td><code>&a</code></td>
    <td>Adresa premennej <code>a</code>.</td>
  </tr>
  <tr>
    <td>Dereferencia</td>
    <td><code>*</code></td>
    <td><code>*a</code></td>
    <td>Hodnota na adrese v premennej <code>a</code>.</td>
  </tr>
</table>


### Príklad 

```cpp
int a{ 7 * 5 }; // 35
int b{ 12 / 5 }; // 2
int c{ a++ }; // 35, a: 36
double f{ 12 / 5 }; // 2
double g{ 12 / 5. }; // 2.4 (actually 2.399999999999)
```

```cpp
bool even_c = (c % 2) == 0;
bool lower_f = f < g;
bool ok = even_c && !lower_f;
```

```cpp
unsigned x = 0x000000ff; // 255
unsigned char b = 0b11010001; // 209
unsigned y = ~x | 1; // 0xffffff01
unsigned z = x & y; // 0x00000001
```

```cpp
int a = 10;
a += 7; // 17, a = a + 7
a %= 6; // 5, a = a % 6
a ^= a; // 0, a = a XOR a
```

---

## Používajte zátvorky

```cpp
int i = 0x10001001;
if (i & 2 == 2) {
  std::cout << "Will this print?\n";
}
```

<div class="fragment">

Áno. `==` má vyššiu precedencia ako `&`.

```cpp
int i = 0x10001001;
if ((i & 2) == 2) {
  std::cout << "Will this print?\n";
}
```

Občas je to v poriadku aj bez, záleží aj od osobných preferencií

```cpp
int i = 1;
if (i == 2 || i == 3) {
  std::cout << "i is 2 or 3.\n";
}
```
</div>

---

## Ternárny operátor

* `?:` je jediný operátor, ktorý používa tri operandy
* Užitočný pri jednoduchých podmienkach
* `a = pred ? op1 : op2;`
* Ak je predikát (`pred`) splnený, potom je výsledok `op1`, inak `op2`

```cpp
bool b = true /* some predicate */;
auto t = b ? 1 : 4; // 1
auto u = !b ? 1.0 : 4; // 4.
auto v = b ? 1 : "4"; // will not compile
```

Druhý a tretí operand musia byť kompatibilné, inak zlyhá kompilácia.


### C vs C++

Ternárny operátor sa vyhodnocuje trochu inak v C

```cpp
int a, b;
// fill up a

a>=0? b=1 : b=2;
```

<div class="fragment">

V C sa nedá skompilovať

```cpp
(a>=0 ? b=1 : b) = 2; // illegal in C
```

C++ má iné pravidlá

```cpp
a >= 0 ? (b = 1) : (b = 2); // OK C++
```
</div>

---

## Funkcie

* Funkcie musia mať, rovnako ako v C, návratový typ a zoznam parametrov
* Hodnoty sa vracajú z funkcií pomocou kľúčového slova `return`
* Ak je návratový "typ" `void`, potom funkcia nevracia nič a `return` iba skončí vykonávanie

```cpp
int rectangle(int a, int b) {
  return 2*a*b + 2*a*a + 2*b*b;
}
```

```cpp
void print_rectangle(int a, int b) {
  std::cout << rectangle(a, b) << '\n';
  return; // no need to add return as last statement
}
```

Ak vykonávanie funkcie s návratovým typom iným ako `void` dosiahne jej koniec bez vrátenia hodnoty, nastáva undefined behavior. Výnimkou je funkcia `main`, ktorá implicitne vráti `0`.


### Aký je výsledok v C++14?

```cpp
int a = 1; 
int k = std::max(++a, a++);
```
<div class="fragment">

Undefined behavior. Vyhodnotenia argumentov sú navzájom neosekvenované a obe modifikujú `a`.
</div>

---

### Od C++17

```cpp
int a = 1;
int k = std::max(++a, a++);
```

<div class="fragment">

* Argumenty sú navzájom *indeterminately sequenced*
* Jeden argument sa vyhodnotí celý pred druhým, ale poradie nie je určené
* Výsledkom je `k == 3` a `a == 3`
</div>

---

### Prečo je výsledkom vždy `3`?

Poradie argumentov nie je určené. `std::max` ich však prijíma ako `const T&`.

<div style="display: flex;">
<div style="flex: 1;">

**Najprv zľava**

1. `++a`: `a == 2`, referencia na `a`
2. `a++`: hodnota `2`, potom `a == 3`
3. `max` porovná `3` a `2`

</div>
<div style="flex: 1;">

**Najprv sprava**

1. `a++`: hodnota `1`, potom `a == 2`
2. `++a`: `a == 3`, referencia na `a`
3. `max` porovná `3` a `1`

</div>
</div>

V oboch prípadoch platí `k == 3` a `a == 3`.

---

### Čo zostáva undefined behavior?

```cpp
int a = 1;
int k = ++a + a++;
```

<div class="fragment">

* Operátor `+` neurčuje poradie vyhodnotenia svojich operandov
* Obe modifikácie `a` sú navzájom neosekvenované
* Undefined behavior aj v C++17 a novších štandardoch
</div>

---

### Poradie vyhodnocovania

* Od C++11 štandard opisuje poradie pomocou vzťahov *sequenced before*, *indeterminately sequenced* a *unsequenced*.
* Dve neosekvenované modifikácie tej istej premennej, alebo modifikácia neosekvenovaná voči čítaniu jej hodnoty, spôsobujú undefined behavior.
* Od C++17 sú argumenty funkcie navzájom *indeterminately sequenced*: ich vyhodnotenia sa neprekrývajú, ale poradie nie je určené.
* Vyhnite sa používaniu operátorov `++`, `--` a `=` na tej istej premennej v komplexnom výraze.

---

# Základné "zložené" typy

---

## Polia

* Jednoduché C polia sú tiež funkčné v C++
* Majú súvislé uloženie prvkov bez dodatočnej dynamickej alokácie
* Lokálne polia s automatickou dobou života sú v praxi zvyčajne uložené na zásobníku

```cpp
int a[100]; // array of 100 ints
char b[sizeof(a)]; // array of 400(?) chars
bool c[10][10]; // array of 10 arrays of 10 bools
```

* Nevýhody
   * Veľkosť musí byť známa počas kompilácie, `int a[n];` je platné C99, ale nie C++20
   * Zaberá pamäť, aj keď program nevyužíva celú kapacitu 
* V poriadku do pár stoviek bajtov


### Inicializácia poľa

<ul>
  <!-- we need this to compensate for default margin and i do not want to create new one off class -->
  <style scoped>
    p {
        margin: 0.3em !important;
    }
  </style>
  <li style="display: flex;">
    <div style="flex: 1;">

```cpp
int x[10];
```
</div>
    <div style="flex: 2;">
  
Pole desiatich `int`ov, ktoré *nie je* inicializované.
</div>
  </li>

  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
std::string x[10];
```
</div>
    <div style="flex: 2;">
  
Pole desiatich `std::string`ov, ktoré sú inicializované na prázdne reťazce.
</div>
  </li>

  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int x[10] = {};
```
</div>
    <div style="flex: 2;">
  
Pole desiatich `int`ov, ktoré *je* inicializované na hodnoty `0`.
</div>
  </li>

  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int x[10] = { 1, 2, 3};
```
</div>
    <div class="fragment" style="flex: 2;">
  
Pole desiatich `int`ov, ktoré *je* inicializované na hodnoty `1, 2, 3, 0, 0, ...`.
</div>
  </li>

  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int x[] = { 1, 2, 3 };
```
</div>
    <div style="flex: 2;">
  
Pole troch `int`ov, ktoré *je* inicializované na hodnoty `1, 2, 3`.
</div>
  </li>

  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int x[3] = { 1, 2, 3, 4 };
```
</div>
    <div style="flex: 2;">
  
Kompilačná chyba (*too many initializers*).
</div>
  </li>

</ul>


### Prístup k prvkom poľa

* Máme na výber `operator[]`, alebo priamo aritmetiku so smerníkmi (preferujeme operátor)
* Polia vždy začínajú na indexe `0` [zero based]
* Veľkosť poľa vieme získať pomocou `std::size`

```cpp
int arr[] = { 1, 2, 3, 4, 5 }; // we can omit array size
// then it will have size to accommodate all elements
bool ok = arr[0] == 1; // true
int undef = arr[5]; // undefined behavior (bad)
arr[arr[0] + 2] = 3; // { 1, 2, 3, 3, 5 }

for (size_t i = 0; i < std::size(arr); ++i) { // why ++i
  std::cout << arr[i] << '\n';
}

```

---

## C-reťazce

* Reťazce v C sú iba polia plné znakov ukončené špeciálnym null znakom `'\0'`
* Preto sa im tiež hovorí aj *null terminated strings*

<div style="display: flex;">
<div style="flex: 1;">

```cpp
char s[] = "ABCDE";
```
</div>
<div style="flex: 2;">
  
Pole šiestich znakov, ekvivalentné `{'A', 'B', 'C', 'D', 'E', '\0'}`.
</div>
</div>

<div style="display: flex;">
<div style="flex: 1;">

```cpp
char first = s[0];
char last = s[4];
char terminator = s[5];
```
</div>
<div style="flex: 2;">
  
`first` má hodnotu `'A'`, `last` je `'E'` a `terminator` je `'\0'`
</div>
</div>

<div style="display: flex;">
<div style="flex: 1;">

```cpp
s[2] = 'X';
std::cout << s << '\n';
```
</div>
<div style="flex: 2;">
  
Vypíše `"ABXDE"`, stringy môžeme aj modifikovať. 
</div>
</div>

<div style="display: flex;">
<div style="flex: 1;">

```cpp
char s1[10] = "12345";
s1[7] = 7;
std::cout << s1 << '\n'; // 12345
// stop at first \0
```
</div>
<div style="flex: 2;">
  
Vyrobí pole desiatich znakov a na mieste `0` až `4` bude string `"12345"`, ostatné znaky budú inicializované na `\0`
</div>
</div>


### Porovnávanie reťazcov

<div style="display: flex;">
<div style="flex: 1;">

```cpp
const char* str = "Name:";

if (str == "Name:") {
  // ... 
}
```
</div>
<div style="flex: 1;">
  
```cpp
const char* str = "Name:";

if (strcmp(str, "Name:") == 0) {
  // ... 
}
```
</div>
</div>

* Prvá možnosť iba porovná smerníky (adresy), pretože C-reťazec je pole a polia sa automaticky konvertujú na smerníky. Literál je tiež pole
* Ak chceme porovnať obsah reťazcov, pomôže nám volanie funkcie zo štandardnej knižnice `strcmp`
* C-reťazce sú najrozšírenejším typom reťazcov na interface-och (sú najviac portable)


### Raw literals

* Konštrukcii `"string"` sa hovorí aj literál
* Ak chceme v rámci neho použiť niektoré znaky (nový riadok, `"`, `'\'`, ...) musíme ich escapovať pomocou `\`
* Niektoré reťazce vyzerajú veľmi zle so všetkými escape sekvenciami (regex, cesty k súborom...)
* Môžeme použiť raw literály. **R**"**delimiter(**string**)delimiter**", delimiter je nepovinný a užitočný hlavne ak samotný reťazec obsahuje znak `)`

<div style="display: flex;">
<div style="flex: 1;">

```cpp
const char path[] = "\"C:\\Users\\cppseminar\"";
```
</div>
<div style="flex: 1;">
  
```cpp
const char path[] = R"("C:\Users\cppseminar")";
```
</div>
</div>

<div style="display: flex;">
<div style="flex: 1;">

```cpp
const char json[] = R"({
  "author": "Bjarne",
  "version": "C++20 (2022)"
})";
```
</div>
<div style="flex: 1;">
  
```cpp
const char json[] = R"###({
  "author": "Bjarne",
  "version": "C++20 (2022)"
})###";
```
</div>
</div>


### Operácie s C-stringami

* Vždy sa uistite, že máte reťazce naozaj ukončené nulou, inak sa môžu stať zlé veci (undefined behavior)

```cpp
const char* str = "Hello";
std::cout << strlen(str) << '\n'; // 5
// str[strlen(str)] == 0

char msg[128];
strcpy(msg, str); // copy, make sure buffer is long enough
strcat(msg, " world!"); // concatenation

std::cout << msg << '\n'; // Hello world!
strstr(msg, "rld"); // returns pointer to string or NULL
```

* Všetky operácie z C sú podporované
* `strpbrk`, `strspn`, `strtok`...

note: strpbrk - Scans the null-terminated byte string pointed to by dest for any character from the null-terminated byte string pointed to by breakset, and returns a pointer to that character. strspn - Returns the length of the maximum initial segment (span) of the null-terminated byte string pointed to by dest, that consists of only the characters found in the null-terminated byte string pointed to by src. 

---

## Štruktúry

* Používame kľúčové slovo `struct` a spravidla ich nebudeme `typedef`-ovať ako v C
* V C++ máme aj `class`, ale o tom neskôr

```cpp
struct point {
  float x;
  float y;
}; // semicolon is ultracritical!!!

struct circle {
  point p; // structure inside structure
  float radius;
  int color;
  char name[32]; // array in structure
};
```


### Inicializácia štruktúr

* Štandardne je štruktúra neinicializovaná, takže čítanie jej hodnôt je nedefinované správanie (tieto pravidlá sú iné, ak máme definovaný konštruktor, ale o tom neskôr)

<ul>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
point p = { 7, 9 };
```
</div>
    <div style="flex: 3;">
  
Členské premenné štruktúry vieme priamo inicializovať pomocou `{}`. *aggregate initialization*
</div>
  </li>
  <li style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
point p = { 7 };
```
</div>
    <div class="fragment" style="flex: 3;">
  
Členská premenná `x` sa nastaví na `7`, `y` sa inicializuje pomocou `int{}`, teda `0`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
circle c = { { 7, 9 }, 5.0, 1 };
```
</div>
    <div style="flex: 3;">
  
Inicializéry môžu byť aj vnorené.
</div>
  </li>
</ul>


<div style="display: flex;">
<div style="flex: 2;">

```cpp
circle c = { 
  .radius = 5.0, 
  .color = 0xffc0cb 
};
```
</div>
<div style="flex: 3;">
  
Môžeme aj vymenovať členov, ktoré sa majú inicializovať

* Ostatné sa inicializujú na `0` (alebo pomocou defaultného konštruktora)
* Musia byť v poradí, v akom sú v štruktúre, inak nastanú chyby kompilácie
* C++20 designated initializers
</div>
</div>


### Prístup k prvkom

* Pomocou operátora `.`

```cpp
point p = { 7, 9 };
point q;
q = p; // copy
q.x = 0; // p.x is still 7

circle c;
c.radius= 10;
c.p.x = 7.5;
c.p.y = 12;
```

---

# Pointers and references

---

## Smerníky (pointers) a referencie

* Každá (pomenovaná) premenná má v pamäti svoje miesto
* Adresu tohto miesta vieme získať pomocou unárneho operátora `&`
* Opačný proces, získanie hodnoty na adrese, sa robí pomocou operátora dereferencie `*`
* Referencia je alias iného objektu; vnútorne je zvyčajne implementovaná ako smerník.

![xkcd komiks o smerníkoch](./lectures/2_basics/pointers.png)


##  Null smerník

<div style="display: flex; align-items: center;">
<div style="flex: 6;">

* Adresa (`0`) je rezervovaná ako neplatná 
* Užitočné na identifikáciu neinicializovaného smerníka 
* Dereferencia invalidného smerníka je nedefinovaná
* Konštanty pre null
   * `NULL`, makro z jazyka C
   * `0`, starý C++ typ
   * `nullptr`, preferované v moderných C++
</div>
<div style="flex: 4;">
<a href="https://www.youtube.com/watch?v=HSmKiws-4NU">
  <img src="./lectures/2_basics/null_pointer.jpg" alt="wrestler dereferencing a null pointer" />
</a>
</div>
</div>


## `int *a` alebo `int* a`?

* Oba zápisy sú ekvivalentné
* Pozor ale pri inicializácii viacerých premenných na jednom riadku <small>(čo inak skôr neodporúčame)</small>

<ul>
  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int *a, b;
```
</div>
    <div style="flex: 5;">
  
Premenná `a` je smerník `int*`, premenná `b` je typu `int`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int* a, b;
```
</div>
    <div style="flex: 5;">
  
Premenná `a` je smerník `int*`, premenná `b` je typu `int`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 1;">

```cpp
int *a, *b;
```
</div>
    <div style="flex: 5;">
  
Premenná `a` je smerník `int*`, premenná `b` je smerník `int*`.
</div>
  </li>
</ul>

---

## Základné operácie so smerníkami

<ul>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int a; 
int b = 7;
```
</div>
    <div style="flex: 3;">
  
Premenná `a` je neinicializovaná, premenná `b` má hodnotu `7`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
int *a_ptr = &a;
int *b_ptr = &b;
```
</div>
    <div style="flex: 3;">
  
Premenná `a_ptr` je nainicializovaná na smerník na `a`, premenná `b_ptr` na `b`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
int c = *a_ptr;
```
</div>
    <div style="flex: 3;">
  
Nedefinované správanie, keďže `a` nie je inicializovaná a čítať neinicializovanú pamäť je undefined.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
*a_ptr = 1;
```
</div>
    <div style="flex: 3;">
  
OK, `a` inicializujeme na `1`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
b_ptr = a_ptr;
```
</div>
    <div style="flex: 3;">
  
OK, smerník na `b` zmeníme, aby ukazoval na `a`.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
*b_ptr = b;
```
</div>
    <div style="flex: 3;">
  
OK, `a` nainicializujeme na hodnotu `b`, teda `7`.
</div>
  </li>
</ul>


### Null smerník

<ul>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int a = 0;

int *ptr = std::addressof(a);
```
</div>
    <div style="flex: 3;">

Nemusíme použiť operátor `&`, ale funkciu `std::addressof`.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
ptr = 0;
```
</div>
    <div style="flex: 3;">

`ptr` je null smerník.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
ptr = NULL;
```
</div>
    <div style="flex: 3;">

`ptr` je null smerník.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
ptr = nullptr;
```
</div>
    <div style="flex: 3;">

`ptr` je null smerník.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
*ptr = 0;
```
</div>
    <div style="flex: 3;">

Nedefinované správanie.
</div>
  </li>
  <li class="fragment" style="display: flex;">
    <div style="flex: 2;">

```cpp
std::cout << ptr;
```
</div>
    <div style="flex: 3;">

Samotný smerník čítať môžeme.
</div>
  </li>
</ul>


### Segmentation fault

* Zďaleka najčastejšou chybou v C++ programoch je Segmentation fault (Access violation)
* `0xC0000005`
* Príčiny
   * Čítanie neexistujúcej pamäte
   * Zapisovanie pamäte iba na čítanie *read only*
* Chyby
   * Pretečenie *Buffer overflow*
   * Dereferencovanie nulového smerníka


### Array to pointer decay

* Polia sú automaticky konvertované (*array decay*) na smerníky

```cpp
void f(int* ptr) {
  std::cout << ptr[0];
}

int a[100] = { 1, 2 };
f(a); // will work
```

<div class="fragment">

```cpp
// const char[] will disable string pooling 
const char array[] = "My string";
 
// just pointer, not array of chars
const char *pointer = "My string";
```
</div>

<ul>
  <li style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
sizeof(array) == sizeof(pointer)
```
</div>
    <div class="fragment" style="flex: 3;">
  
`false`, veľkosť smerníka je vždy rovnaká.
</div>
  </li>
  <li style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
(void*)array == (void*)&array;
```
</div>
    <div class="fragment" style="flex: 3;">
  
`true`, pole sa vie implicitne konvertovať na smerník.
</div>
  </li>
  <li style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
(void*)pointer == (void*)&pointer;
```
</div>
    <div class="fragment" style="flex: 3;">
  
`false`, adresa smerníka je vlastne nový smerník.
</div>
  </li>

</ul>


### Operator `->`

* `->` je iba skratka za `*` a `.` spolu
* Dá sa preťažiť, čo sa celkom aj využíva, pri chytrých smerníkoch

```cpp
struct pair {
    int a;
    int b;
};
 
int main() {
  pair p;
  p.a = 1;
 
  pair* ptr;
  ptr = &p;
  ptr->b = 0; // (*ptr).b = 0;
 
  std::cout << ptr->a; // 1
}
```

---

## Aritmetika so smerníkmi

* C++ predpokladá lineárnu (neprerušovanú) pamäť, preto k smerníkom môžeme pripočítavať a odpočítavať hodnoty a získame ďalšie smerníky (nie nutné platné)
* Programátori sú zodpovední za dereferencovanie iba platných smerníkov
* Inkrement a dekrement posúva o veľkosť typu (nie `1`)
   * `int*` sa zvýši o štyri bajty (`sizeof(int)`)
   * `char*` sa zvýši o jeden bajt (`sizeof(char)`)


<div style="display: flex;">
  <div style="flex: 2;">

```cpp
int arr[5] = { 1 }; // 1, 0, 0, 0, 0
int *ptr = &arr[3];
```
</div>
    <div style="flex: 3;">
  
`ptr` je smerník na tretí (zero based) prvok poľa
</div>
</div>
<div style="display: flex;">
  <div class="fragment" style="flex: 2;">

```cpp
*ptr = 4;
```
</div>
    <div class="fragment" style="flex: 3;">
  
Modifikujeme pole
</div>
  </div>
  <div style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
++ptr;
*ptr = 5;
```
</div>
    <div class="fragment" style="flex: 3;">
  
Posunieme o jeden, takže ukazujeme na posledný prvok poľa.
</div>
  </div>
  <div style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
++ptr;
// *ptr = 6;
```
</div>
    <div class="fragment" style="flex: 3;">
  
Posunieme o jeden, takže ukazujeme mimo poľa. Takýto smerník nesmieme dereferencovať.
</div>
  </div>
  <div style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
ptr = ptr - 4;
*ptr = 2; 
// 1, 2, 0, 4, 5
```
</div>
    <div class="fragment" style="flex: 3;">
  
Znovu sa vrátime do poľa na druhý prvok, ten už môžeme modifikovať.
</div>
  </div>


### array subscript operator

<ul style="display: block;">
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int a = 0;

int *ptr = &a;
```
</div>
    <div style="flex: 3;">

`ptr` je adresa premennej `a`.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
ptr[0]
```
</div>
    <div style="flex: 3;">

Hodnota na pozícii `ptr`.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
ptr[10]
```
</div>
    <div style="flex: 3;">

```cpp
*(ptr + 10)
```
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int a[10] = { 1 };
int *ptr = a;
int **ptrptr = &ptr;
ptrptr[0][0] = 2;
```
</div>
    <div style="flex: 3;">

```cpp
**ptrptr == 2
```

Polia sa dajú implicitne konvertovať na smerníky. Vieme vyrobiť aj smerník na smerník.
</div>
  </li>
</ul>


### Je nasledujúci výraz platný C++? Ak áno, aký je výsledok?

```cpp
std::cout << 2["ABCDE"] << std::endl;
```

<div class="fragment">

`operator[]` *subscript operator* je definovaný ako `a[b] = *(a + b)`

```cpp
std::cout << *(2 + "ABCDE") << std::endl;
std::cout << *("ABCDE" + 2) << std::endl;
```
</div>

---

## Referencie

* C++ okrem smerníkov obsahuje aj referencie
* Majú zmysel hlavne pri volaní funkcií
* V podstate sú to aliasy na premenné implementované pod kapotou ako smerníky 

<ul style="display: block;">
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int a = 0;

int &b = a;
```
</div>
    <div style="flex: 3;">

`b` je referencia na `a`.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
b = 10;
```
</div>
    <div style="flex: 3;">

Zmení sa hodnota `a`.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int &c;
```
</div>
    <div style="flex: 3;">

Chyba kompilácie, referencie nemôžu byť neinicializované.
</div>
  </li>
  <li style="display: flex;">
    <div style="flex: 2;">

```cpp
int &d = &nullptr;
```
</div>
    <div style="flex: 3;">

Toto sa tiež nedá urobiť.
</div>
  </li>
  <li style="display: flex;">
    <div class="fragment" style="flex: 2;">

```cpp
int *x = nullptr;
int &u = *x;
```
</div>
    <div class="fragment" style="flex: 3;">

Ide skompilovať, ale je to nedefinované správanie, `x` sa nedá dereferencovať.
</div>
  </li>
</ul>

---

## `const` smerníky a referencie

* Premenné v C++ môžu byť `const`, tieto sa potom nedajú meniť
* Kompilátor to vynucuje

<div style="display: flex;">
<div style="flex: 2;">

```cpp
int a = 0;

std::cout << a;
a = 10;
```
</div>
<div style="flex: 3;">

`a` je typu `int`, aj čítanie aj modifikovanie je povolené.
</div>
</div>

<div style="display: flex;">
<div style="flex: 2;">

```cpp
const int b = 0;

std::cout << b;
// b = 10;
```
</div>
<div style="flex: 3;">

`b` je typu `const int`, čítanie je povolené, ale modifikovanie by bola kompilačná chyba.
</div>
</div>


<div style="display: flex;">
<div style="flex: 2;">

```cpp
int x = 1;
int *r = &x;
const int *c = &x;
```
</div>
<div style="flex: 3;">

`r` je smerník na `int` a `c` je smerník na konštantný `int`. Konštantný smerník by mal typ `int* const`.
</div>
</div>

<div style="display: flex;">
<div style="flex: 2;">

```cpp
std::cout << *r << *c;
```
</div>
<div style="flex: 3;">

Čítať môžeme cez smerník na `int` aj cez smerník na konštantný `int`.
</div>
</div>

<div style="display: flex;">
<div style="flex: 2;">

```cpp
*r = 12;
// *c = 13
```
</div>
<div style="flex: 3;">

Výsledkom `*r` je `int`, takže sa modifikovať dá, výsledok `*c` je `const int`, takže nám to kompilátor nedovolí.
</div>
</div>

---

## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <style scoped>
      td {
        font-family: monospace;
      }
      .address {
        font-size-adjust: 0.2;
      }
      .current {
        background-color: red;
      }
      </style>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x00010000</td>
        <td>0xdeadbeef</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td>0x0badc0de</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000003</td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td>0x0badc0de</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000003</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fff8</td>
        <td>0x0badc0de</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
int* p = nullptr;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000003</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td>0x00000000</td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
int* p = nullptr;
p = &a;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000003</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td><span style="color: red;">0x00010000</span></td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
int* p = nullptr;
p = &a;
*p = 8;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td><span style="color: red;">0x00000008</span></td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td>0x00010000</td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
int* p = nullptr;
p = &a;
*p = 8;
p = &dummy;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000008</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td><span style="color: red;">0x0000fffc</span></td>
      </tr>
      <tr>
        <td><span style="color: red;">➡</span></td>
        <td class="address">0x0000fff4</td>
        <td>0xc0ffeeee</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>


## Pamäť 

<!-- .slide: data-auto-animate -->

<div style="display: flex;">
  <div data-id="code" style="flex: 1; text-align: left;">

```cpp
int a = 3;
int dummy;
int* p = nullptr;
p = &a;
*p = 8;
p = &dummy;
float f = 2.7;
```
  </div>
  <div data-id="memory" style="flex: 1; text-align: left;">
    <table>
      <tr>
        <td colspan="3">...</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x00010000</td>
        <td>0x00000008</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fffc</td>
        <td>0xcdcdcdcd</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff8</td>
        <td>0x0000fffc</td>
      </tr>
      <tr>
        <td></td>
        <td class="address">0x0000fff4</td>
        <td>0x402ccccd</td>
      </tr>
      <tr>
        <td colspan="3">...</td>
      </tr>
    </table>
  </div>
</div>

---

## Volanie funkcií 

```cpp
void f(int a, const int& b, int& c) {
    // a is copied , changes in function will
    // not affect caller
    a = 2;
    // b is not copied, but cannot be changed
    // b = 2; // will not compile
    // c is not copied and changes will affect
    // the caller
    c = 2;
}

void g() {
    int x = 1; int y = 1; int z = 1;
    f(x, y, z);
    std::cout << (x == 1 && y == 1 && z == 2); // 1
}
```

* Výhodou volania `&` a `const &` je, že na pozadí sa len presunie smerník a nie celý typ
* Nekonštantná referencia sa v súčasnosti už veľmi nepoužíva, má zmysel len pri vstupno-výstupných parametroch

---

# Príkazy na riadenie programu

---

## `if`

```cpp
int i = 0;
std::cin >> i;
if (i % 2 == 0) {
  std::cout << "Even\n";
}
```

Ak je podmienka splnená, tak sa pokračuje vo vykonávaní tela `if`-u, inak sa preskočí.


```cpp
int i = 0;
std::cin >> i;
if (i % 2 == 0) {
  std::cout << "Even\n";
} else {
  std::cout << "Odd\n";
}
```

Ak je podmienka splnená, tak sa pokračuje vo vykonávaní tela `if`-u, inak sa vykoná `else` vetva.


```cpp
int i = 0;
if (std::cin >> i; i % 2 == 0) {
  std::cout << "Even\n";
} else {
  std::cout << "Odd\n";
}
```

V `if`-e je inicializácia, podobne ako vo `for` cykle, podmienka je potom za `;`. **C++17**


```cpp
if (int i = 0; std::cin >> i && i % 2 == 0) {
  std::cout << "Even\n";
} else {
  std::cout << "Odd\n";
}
```

Aj samotnú deklaráciu môžeme dať do inicializácie `if`.

<div class="fragment">

Čo je zlé s kódom vyššie?
</div>
<div class="fragment">

Ak zadám `fff`, tak to vypíše `"Odd"`, musíme kontrolovať stream pomocou `std::cin.fail()`.
</div>


### Pôjde nasledujúci kód skompilovať a ak áno, aký je výsledok?

```cpp
if (char* p = (char*)malloc(2)) {
  std::cout << "A";
} else {
  std::cout << "B";
}
```

<div class="fragment">

Deklarácia v rámci `if`-u funguje, odkedy je C++ na svete. Stačí, aby sa inicializovaná premenná dala skonvertovať na `bool`. Pozor, premenná je potom dostupná aj v `else` vetve. 
</div>

---

## `switch`

```cpp
enum class Type { Number, String, Null };
Type t = Type::Number;

switch (t) {
case Type::Number:
    [[fallthrough]]; // state intent, no compiler warning
case Type::String:
    std::cout << "Has value\n";
    break;
case Type::Null:
    std::cout << "No value\n";
    break;
default:
    std::cout << "??\n";
}
```

---

## `for`

```cpp
std::vector<int> numbers = { /* ... */ };

for (size_t i = 0; i < numbers.size(); ++i) {
  if (numbers[i] % 2 == 0)
    continue; // will skip the rest of the for body and continue
  if (numbers[i] > 10)
    break; // prematurely stop the loop
  if (numbers[i] == 4)
    return; // if hit this will break out of enclosing function

  std::cout << numbers[i];
}
```

V C++11 a vyššie existuje lepší spôsob, ako iterovať prvky

---

## Range based for loop

* Syntaktický cukor okolo iterátorov nad kontajnermi
* Starý spôsob iterovania cez kontajnery bol 

```cpp
std::vector<int> numbers = { /* ... */ };

for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); ++it) {
  if (*it % 2 == 0)
    continue; // will skip the rest of the for body and continue
  if (*it > 10)
    break; // prematurely stop the loop
  if (*it == 4)
    return; // if hit this will break out of enclosing function

  std::cout << *it;
}
```


* Plus mínus ekvivalentný range based cyklus
* Premenná v cykle sa kopíruje, pokiaľ je to primitívny typ (ako `int`), tak je to ešte OK

```cpp
std::vector<int> numbers = { /* ... */ };

for (int i : numbers) {
  if (i % 2 == 0)
    continue; // will skip the rest of the for body and continue
  if (i > 10)
    break; // prematurely stop the loop
  if (i == 4)
    return; // if hit this will break out of enclosing function

  std::cout << i;
}
```


* Pre zložitejšie typy a prípady, keď treba modifikovať prvky kontajnera, musíme použiť referencie

```cpp
std::vector<std::string> names = { /* ... */ };

for (const auto& name : names) {
  std::cout << name;
}
```

```cpp
std::vector<int> numbers = { /* ... */ };

for (int& i : numbers) {
  i = i * i * i;

  std::cout << i;
}
```

---

## Inicializácia v range based

* C++20 pridal inicializáciu do range based for loopu

```cpp
int numbers[] = { 1, 4, 5, 7, 8 };
for (int v = 0; auto i : numbers) {
  // v is here defined
}
```

---

## `while`, `do`

```cpp
std::vector<int> get_numbers(size_t n) {
  std::vector<int> result;

  while (result.size() < n) {
    int x = 0;
    std::cin >> x;

    if (!std::cin) // not a number, failbit set
      break;

    result.push_back(x);
  }

  return result;
}

```


### Pôjde nasledujúci kód skompilovať a ak áno, aký je výsledok?

```cpp
int i = 1;
do {
  std::cout << i << std::endl;
  --i;
  if (i == 0)
    continue;
} while (i > 0);
```

Vypíše iba `1`, `continue` vždy skáče na koniec cyklu.

---

# Kompilácia

---

## Zdrojový súbor

* C a C++ pracujú s dvoma typmi súborov
   * Zdrojové súbory (`*.cpp`, `*.cc` alebo `*.c` pre C súbory)
   * Hlavičkové súbory (`*.h`, `*.hpp`, bez prípony)
* Každý `.cpp` súbor je zvyčajne spárovaný s `.h` súborom, ktorý deklaruje verejný interface a ten je potom v `.cpp` súbore implementovaný 
* Aj `.cpp` aj `.h` súbory môžu include-ovať iné `.h` súbory kvôli použitiu tried a funkcií, ktoré daný `.h` súbor deklaruje
* Hlavičkové súbory môžu obsahovať aj implementáciu, a niekedy aj musia, obyčajne sa tomu snažíme vyhýbať 

---

## `#include`

* include directíva iba nakopíruje referencovaný súbor na svoje miesto
   * `#include <filename> / #include "filename"`
  * Rozdiel je iba v mieste hľadania súborov 
* `<>` systémové a `""` užívateľsky definované
* Hlavný dôvod prečo používame include je zviditeľnenie symbolov zo súboru (triedy, funkcie, premenné, konštanty, makrá, šablóny, ...)

---

## Viacnásobný `#include`

![boromir lamenting multiple include](./lectures/2_basics/multiple-include.png)

* `#include` súbory iba nakopíruje, takže sa symboly ľahko redefinujú (chyba pri kompilácii)


<div style="display: flex;">
<div style="flex: 4;">

```cpp
int f(int a, int b);
```
</div>
<div style="flex: 1;">

function.h
</div>
</div>

<div style="display: flex;">
<div style="flex: 4;">

```cpp
#include "header.h"
#include "header.h"

int f(int a, int b) {
  return a + b;
}
```
</div>
<div style="flex: 1;">

function.cpp
</div>
</div>

<div class="fragment">
Toto je OK, signatúry funkcie môžeme redefinovať.
</div>


<div style="display: flex;">
<div style="flex: 4;">

```cpp
struct point {
  int x;
  int y;
}
```
</div>
<div style="flex: 1;">

point.h
</div>
</div>

<div style="display: flex;">
<div style="flex: 4;">

```cpp
#include "point.h"
#include "point.h"

int f(point p) {
  return p.x + p.y;
}
```
</div>
<div style="flex: 1;">

function.cpp
</div>
</div>

<div class="fragment">
Toto nie je OK, štruktúry sa nemôžu redefinovať.
</div>


### Define guards

<ul>
<li>

Definovanie unikátneho makra a potom test predíde viacnásobnému vloženiu súboru.

```cpp
#ifndef MY_FILE_H
#define MY_FILE_H

// content of header file

#endif
```
</li>
<li>

Nevyzerá ale pekne a unikátnosť vedie k pridlhým názvom.
</li>
</ul>


### `#pragma once`

* Namiesto define guards sa dá použiť `#pragma once` na začiatku súboru
* Funguje na každom používanom kompilátore, ale nie je C++ štandard 
* Moduly v C++20 toto celé zmenia a mali by pomôcť s organizáciou kódu, bohužiaľ stále úplne nefungujú

```cpp
#pragma once

// content of header file
```

---

<!-- .slide: data-auto-animate -->

## Kompilátor a linker

<div style="display: flex;">
  <div style="flex: 1; text-align: left;">
    <span data-id="begin">Začiatok kompilácie</span>
  </div>
  <div style="flex: 1; text-align: left;">
    Na začiatku kompilácie máme spravidla zdrojové súbory <code>*.cpp</code> a hlavičkové súbory <code>*.h</code>.
  </div>
</div>


<!-- .slide: data-auto-animate -->

## Kompilátor a linker

<div style="display: flex;">
  <div style="flex: 1; text-align: left;">
    <span data-id="begin"><small>Začiatok kompilácie</small></span><br/>
    <span data-id="preprocess">Preprocessing...</span>
  </div>
  <div style="flex: 1; text-align: left;">
    Potom sa spustí C preprocesor, ten</br>
    <ul style="margin-left: 2em;">
      <li>expanduje makrá</li>
      <li>includuje hlavičkové súbory.</li>
    </ul></br>
    Po tejto fáze ostanú iba zdrojové súbory, ktoré už neobsahujú žiadnu direktívu preprocesora.
  </div>
</div>


<!-- .slide: data-auto-animate -->

## Kompilátor a linker

<div style="display: flex;">
  <div style="flex: 1; text-align: left;">
    <span data-id="begin"><small>Začiatok kompilácie</small></span><br/>
    <span data-id="preprocess"><small>Preprocessing...</small></span><br/>
    <span data-id="compile">Compiling...</span>
  </div>
  <div style="flex: 1; text-align: left;">
    Fáza kompilácie vyrobí zo zdrojových súborov machine code. Počas kompilácie sa robia aj všetky kontroly syntaxe, typov, ... Ak to od kompilátora chceme, urobí sa aj optimalizácia kódu. Výstupom sú objektové súbory <code>*.obj</code>, ktoré obsahujú inštrukcie pre procesor, ale niektoré funkcie ešte nemusia byť známe.
  </div>
</div>


<!-- .slide: data-auto-animate -->

## Kompilátor a linker

<div style="display: flex;">
  <div style="flex: 1; text-align: left;">
    <span data-id="begin"><small>Začiatok kompilácie</small></span><br/>
    <span data-id="preprocess"><small>Preprocessing...</small></span><br/>
    <span data-id="compile"><small>Compiling...</small></span><br/>
    <span data-id="link">Linking...</span>
  </div>
  <div style="flex: 1; text-align: left;">
    Linker spojí objektové súbory do jednej výstupnej binárky. Jeho úloha je urobiť relokácie, teda pofixovať adresy funkcií v objektových súboroch (ak je v jednom objektovom súbore referencia na funkciu v inom objektovom súbore). V istých prípadoch môže robiť aj optimalizácie (vyhadzovať funkcie, ktoré sa nikdy nevolajú...).
  </div>
</div>


<!-- .slide: data-auto-animate -->

## Kompilátor a linker

<div style="display: flex;">
  <div style="flex: 1; text-align: left;">
    <span data-id="begin"><small>Začiatok kompilácie</small></span><br/>
    <span data-id="preprocess"><small>Preprocessing...</small></span><br/>
    <span data-id="compile"><small>Compiling...</small></span><br/>
    <span data-id="link"><small>Linking...</small></span><br/>
    <span>Koniec kompilácie</span>
  </div>
  <div style="flex: 1; text-align: left;">
    Výstupom je spravidla binárka</br>
    <ul style="margin-left: 2em;">
      <li>spustiteľný súbor (<code>*.exe</code>)</li>
      <li>dynamická knižnica (<code>*.dll</code>, <code>*.so</code>).</li>
    </ul></br>
  </div>
</div>

---

## CMake

* CMake je nástroj na automatizáciu procesu buildovania
* Umožňuje definovať projekt a jeho závislosti v súbore `CMakeLists.txt`
* Podporuje multiplatformový vývoj a generovanie projektov pre rôzne IDE

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(my_executable main.cpp)
```


### `cmake_minimum_required`

* Určuje minimálnu požadovanú verziu CMake


### `project`

* Definuje názov projektu a voliteľne jeho jazykové požiadavky
* V tomto prípade je to C++ `project(MyProject LANGUAGES CXX)`


### `set`

* Nastavuje premenné CMake
* `CMAKE_CXX_STANDARD 20` nastavuje štandard C++ na C++20
* `CMAKE_CXX_STANDARD_REQUIRED ON` znamená, že požadovaný štandard musí byť podporovaný kompilátorom
* `CMAKE_CXX_EXTENSIONS OFF` znamená, že sa nepovolia žiadne rozšírenia kompilátora (namiesto `std=gnu++20` sa použije `std=c++20`)


### `add_executable`

* Definuje spustiteľný súbor a jeho zdrojové súbory
* V tomto prípade vytvára spustiteľný súbor `my_executable` zo súboru `main.cpp`


### Generovanie build systému a build

* CMake generuje build systém (Makefile, Ninja, Visual Studio project, ...) na základe `CMakeLists.txt`
* Tento build systém sa potom použije na kompiláciu a linkovanie projektu

```bash
# Create a build directory
mkdir build
cmake -S . -B build
```

```bash
# Build the project using cmake command
cmake --build build

# Or using make if Makefile was generated (default on Unix systems)
make -C build
```


## Rôzne konfigurácie

* CMake umožňuje definovať rôzne build konfigurácie (Debug, Release, RelWithDebInfo, MinSizeRel)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

* Konfigurácie môžu mať rôzne optimalizačné úrovne a nastavenia pre ladenie
   * **Debug**: žiadna optimalizácia, plné ladenie
   * **Release**: plná optimalizácia, žiadne ladenie
   * **RelWithDebInfo**: optimalizácia s ladením
   * **MinSizeRel**: optimalizácia pre minimálnu veľkosť

---

# ĎAKUJEM

## Otázky?
