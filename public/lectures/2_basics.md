# C++

## Nový začiatok

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

# Hello world

```cpp
#include <iostream>
 
int main() {
    std::cout << "Hello world!\n";
}
```

---

* C++ programy sa začínajú vykonávať volaním globálne prístupnej funkcie s názvom main
* Odporúčané signatúry sú

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
int main();

int main(int argc, char* argv[]);
```
</div>
<div style="flex: 1;">

Zvyčajne `argc` obsahuje počet parametrov + 1, argv potom obsahuje ako prvý prvok meno (cestu) programu a potom parametre
</div>
</div>

---

## `#include <iostream>`

* `#include` iba vloží súbor (zvyčajne hlavičkový súbor [header file]) do iného súboru 
* Súbory v `<>` obsahujú štandardné a/alebo systémové knižnice. Štandardné by mali byť dostupné na všetkých implementáciách vyhovujúcich danej verzií štandardu (my používame C++17/20)
* Súbory v `""` obsahujú naše [user defined] hlavičkové súbory

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
#include <memory>
#include <future>
#include <stdio.h>
#include <string.h>
```
</div>
<div style="flex: 1;">

Súbory bez prípony `.h` sú C++ štandardné hlavičkové súbory. Súbory z C knižnice majú príponu `.h`. 
</div>
</div>

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
#include <Windows.h>
#include <unistd.h>
```
</div>
<div style="flex: 1;">

Systémový hlavičkový súbor.
</div>
</div>

---

## Namespace `std::`

* Skoro všetky užitočné triedy a funkcie sú "skryté" v namespace `std`
* Vždy keď chceme niečo použiť zo štandardnej knižnice musíme najprv `include`-nuť správny súbor a potom s prefixom `std::` nájdeme triedy/funkcie, ktoré chceme 

![Kniha C++ štandardná knižnica](./lectures/2_basics/c++-standard-lib.png)

---

## `std::cout`

* `cout` je globálna premenná, reprezentujúca štandardný výstup
* Používa stream interface `std::ostream`
* Na vloženie naformátovaného výstupu slúži `operator<<`
    * Binárny operátor left shift (`a << b`) je preťažený v `std::ostream`
    * Vracia `a&`, takže umožňuje reťazenie 
    * Najprv naformátuje podporované dáta a potom ich vypíše

```cpp
std::cout << 3 << ' ' << "things.";
```

---

## `'\n'`

* Nový riadok (new line)
* V literatúre sa vyskytuje aj `std::endl`
* `std::endl` tiež flush-ne buffer
    * Pomalší zápis
    * Dáta sa ale nemôžu "stratiť"

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
std::cout << std::endl;

std::cout.put(std::cout.widen('\n'));
std::cout.flush();
```
</div>
<div style="flex: 1;">

Dva ekvivalentné programy. Funkcia `widen` konvertuje znak pomocou aktuálne používaného `locale` do typu aký používa stream.
</div>

---

## Undefined behavior

![Unicorns and rainbows](./lectures/2_basics/unicorn.png)

---

# Typy, základné operácie a funkcie

---

## Primitívne typy

* Všetky typy z jazyka C sú podporované

```cpp
int i; // signed integer uninitialized (0 or undefined)
unsigned int u = 1337ul; // unsigned integer
bool ok = false; // true/false
double pi = 3.14159; // floating point double precision
float e = 2.71828; // floating point single precision
char c = 'a'; // variant of ISO646 - ASCII
size_t n = 1'000'000'000; // possible to separate with "'"
```

* `short`, `long`, `long long` (rozdielne znamienkové signed typy)
* `unsigned short`, `unsigned`, `unsigned long` (pre neznamienkové typy)

---

## Presná bitová veľkosť

* `long` je na niektorej platforme 32bit a na inej 64bit, podobne aj `size_t`
* Riešia to typy definované v súbor `#include <cstdint>`
* Nemusia byť definované na všetkých platformách (`CHAR_BITS == 8`)

```cpp
int8_t i8 = 127;
int16_t i16 = 32'000;
uint32_t u32 = 4'000'000'000;
int64_t i64 = 10'000'000'000'000;
```

note: CHAR_BITS je nastavené na číslo, ktoré reprezentuje počet bitov c type `char`. Všetky platformy (napr. POSIX), ale požadujú `8`.

---

## Usporiadanie v pamäti

* Každá premenná ma v pamäti miesto, ktoré sa dá zistiť pomocou operátora `&`
* Veľkosť typov je do istého bodu závislá od implementácie a dá sa zistiť pomocou operátora `sizeof`
* Veľkosti sú v "char units" a nie bajtoch (zvyčajne je ale char unit 8bitov)

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
std::cout << sizeof(bool) << '\n'; // 1
std::cout << sizeof(short) << '\n'; // 2
std::cout << sizeof(float) << '\n'; // 4
std::cout << sizeof(long) << '\n'; // 4
std::cout << sizeof(long long) << '\n'; // 8
std::cout << sizeof(double) << '\n'; // 8
```
</div>
<div style="flex: 1;">

Na MS Windows Visual Studio.  `long` je 8 na gcc.
</div>

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
int i = 4.2; // warning narrowing
int j(4.2); // warning narrowing
int k{ 4.2 }; // error
```


## Automatická dedukcia

* V C++11 štandarde predefinovali význam kľúčového slova `auto`

```cpp
auto i = 5; // int  
auto j = 7.5; // double 
auto sq = sqrt(i); // whatever returns sqrt (double in this case)
auto first_name = "Bjarne"; // const char*, not std::string
auto surname = std::string("Stroustrup"); // std::string
auto ui = 56UL; // unsigned long
auto *ptr = &i; // int 
auto ptr = &i; // int*
auto* ptr = i; // compilation error  
```

* `auto*` sa nedá použiť ak vydedukovaný typ nie je smerník
* V C++ (aj C) je jedno či napíšete `int* a;`, alebo `int *a;`


## Pridlhé názvy typov 

* Týka sa hlavne šablón (template)
* Slovo auto umožní odstrániť veľa nezaujímavého písania a šumu
* Netreba to ale preháňať

```cpp
#include <map>
#include <utility>
#include <string>
 
int main() {
    std::map<std::string, std::pair<std::string, int>> employees;
 
    auto it = employees.find("Herb Sutter"); 
    std::map<std::string, std::pair<std::string, int>>::iterator it2 = it;
}

```

---

## Základné operácie

* Všetky `+`, `-`, `*`, `/`, `%`, `++`, `--`, `|`, `&`, `^`, `&&`, `||`, `==`, `!=`, `<=`, `>=`, `<`, `>`, `=`, `?:`, `!`, `~` pracujú ako sa od nich očakáva, dokonca sú preťažené pre zložitejšie typy (tam kde to dáva zmysel)
* Precedencia je definovaná v štandarde, pri pochybách je lepšie použiť zátvorky
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
    <td><code>a</code> bude mať rovnakú hodnotu ako <code>b</code>, výsledkom je <code>&a</code></td>
  </tr>
  <tr>
    <td>Aritmeticke operátory</td>
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
    <td>Zvyšok po celočíselnom delení, funguje ina celočíselných hodnotách</td>
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
    <td>Výsledok je <code>true</code> ak sú <code>a</code> aj <code>b</code> pravda, inak <code>false</code>.</td>
  </tr>
  <tr>
    <td>Operátor negácie</td>
    <td><code>!</code></td>
    <td><code>!a</code></td>
    <td>Výsledok je <code>true</code> ak je <code>a</code> nepravda, a naopak.</td>
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
int a{ 7 * 5 }; // 35
int b{ 12 / 5 }; // 2
int c{ a++ }; // 35, a: 36
double f{ 12 / 5 }; // 2
double g{ 12 / 5. }; // 2.4 (actually 2.399999999999)
```

```cpp
bool even_c = (c % 2) == 0;
bool lower_f = f < g;
bool ok = even_c && !lower_f;
```

```cpp
unsigned x = 0x000000ff; // 255
unsigned char b = 0b11010001; // 209
unsigned y = ~x | 1; // 0xffffff01
unsigned z = x & y; // 0x00000001
```

```cpp
int a = 10;
a += 7; // 17, a = a + 7
a %= 6; // 5, a = a % 6
a ^= a; // 0, a = a XOR a
```

---

## Používajte zátvorky

```cpp
int i = 0x10001001;
if (i & 2 == 2) {
  std::cout << "Will this print?\n";
}
```

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

---

## Ternárny operátor

* `?:` je jediný operátor, ktorý používa tri operandy
* Užitočný pri jednoduchých podmienkach
* `a = pred ? op1 : op2;`
* Ak je predikát (`pred`) splnený, potom je výsledok `op1`, inak `op2`

```cpp
bool b = true /* some predicate */;
auto t = b ? 1 : 4; // 1
auto u = !b ? 1.0 : 4; // 4
auto v = b ? 1 : "4"; // will not compile
```

Druhý a tretí operand musia byť kompatibilné, inak zlyhá kompilácia.


### C vs C++

Ternárny operátor sa vyhodnocuje trochu inak v C

```cpp
int a, b;
// fill up a
a>=0? b=1 : b=2;
```

V C sa nedá skompilovať

```cpp
(a>=0 ? b=1 : b) = 2; // illegal in C
```

C++ má iné pravidlá

```cpp
a >= 0 ? (b = 1) : (b = 2); // OK C++
```

---

## Funkcie

* Funkcie musia mať, rovnako ako v C, návratový typ a zoznam parametrov
* Hodnoty sa vracajú z funkcií pomocou kľúčového slova return
* Ak je návratový "typ" `void`, potom funkcia nevracia nič a return iba skončí vykonávanie

```cpp
int rectangle(int a, int b) {
  return 2*a*b + 2*a*a + 2*b*b;
}
void print_rectangle(int a, int b) {
  std::cout << rectangle(a, b) << '\n';
  return; // no need to add return as last statement
}
```

Ak má funkcia návratový typ, potom musí obsahovať aspoň jeden return.

### Aký je výsledok nasledujúceho kódu

```cpp
int a = 1; 
int k = std::max(++a, a++);
```

Jedna z často vyskytujúcich odpovedí je 2, pretože výsledkom prefixového inkrementu je už zväčšená hodnota 

```cpp
int k = std::max(2, 1);
```

Poradie vyhodnocovania parametrov funkcie je nedefinované (nešpecifikované od C++17)


### Sekvencovanie (order of evaluation)

* Sequence points v predchádzajúcich štandardoch
* Napríklad: Parametre funkcie sú sekvencované pred volaním funkcie. 
* Veľmi zjednodušene: Medzi dvoma bodmi sa môže jedna premenná zmeniť nanajvýš raz
* V podstate: Ak nepoužívate veľmi exotické konštrukcie a vyhýbate sa ++/-- v rámci komplikovaných výrazov, tak je všetko v poriadku. 

---

# Základné "zložené" typy

---

## Polia

* Jednoduché C polia sú tiež funkčné v C++
* Najrýchlejšie, pretože polia sú vložené do objektov (funkcií)
* Alokované na stack-u

```cpp
int a[100]; // array of 100 ints
char b[sizeof(a)]; // array of 400(?) chars
bool c[10][10]; // array of 10 arrays of 10 bools
```

* Nevýhody
   * Veľkosť musí byť známa počas kompilácie, int a[n]; je platné C11, ale nie C++17
   * Zaberá pamäť, aj keď program nevyužíva celú kapacitu 
* V poriadku do pár stoviek bajtov


### Prístup k prvkom poľa

* Máme na výber `operator[]`, alebo priamo aritmetiku so smerníkmi (preferujeme operátor)
* Polia vždy začínajú na indexe `0` [zero based]
* Veľkosť poľa vieme získať pomocou `std::size`

```cpp
int arr[] = { 1, 2, 3, 4, 5 }; // we can omit array size
// then it will have size to accomodate all elements
bool ok = arr[0] == 1; // true
int undef = arr[5]; // undefined behavior (bad)
arr[arr[0] + 2] = 3; // { 1, 2, 3, 3, 5 }
for (size_t i = 0; i < std::size(arr); ++i) { // why ++i
  std::cout << arr[i] << '\n';
}

```

* Kedysi sa namiesto `std::size` používal `countof` trik. 

```c
#define countof(arr) (sizeof(arr)/sizeof(arr[0]))
```

---

## C-reťazce

* Reťazce v C sú iba polia plné znakov ukončené špeciálnym null znakom `'\0'`
* Preto sa im tiež hovorí aj *null terminated strings*

```cpp
char s[] = "ABCDE";
char terminator = s[5]; // defined, always 0
s[3] = 'X';
std::cout << s << '\n'; // ABCXE
char s1[10] = "12345";
s1[7] = 7;
std::cout << s1 << '\n'; // 12345
// stop at first \0
```

* Vždy sa uistite, že máte reťazce naozaj ukončené nulou, inak sa môžu stať zlé veci (undefined behavior)
* C-reťazce sú najrozšírenejším typom reťazcov na interface-och (najviac portable)


### Porovnávanie reťazcov

```cpp
const char* str;

if (str == "Name:") {
  // ... 
}
```

```cpp
const char* str;

if (strcmp(str, "Name:")) {
  // ... 
}
```

Prvá možnosť iba porovná smerníky (adresy), pretože C-reťazec je pole a polia sa automaticky konvertujú na smerníky. Literál je tiež pole. Ak chceme porovnať obsah reťazcov, pomôže nám volanie funkcie zo štandardnej knižnice. 


### Operácie s C-stringami

```cpp
const char* str = "My string";
std::cout << strlen(str) << '\n'; // 9
// str[strlen(str)] == 0
char copy[128];
strcpy(copy, str); // copy, make sure buffer is long enough
strcat(copy, str); // concatenation
std::cout << copy << '\n'; // My stringMy string
strstr(copy, "string"); // returns 3s
```

* Všetky operácie z C sú podporované
* `strpbrk`, `strspn`, `strtok`...

note: strpbrk - Scans the null-terminated byte string pointed to by dest for any character from the null-terminated byte string pointed to by breakset, and returns a pointer to that character. strspn - Returns the length of the maximum initial segment (span) of the null-terminated byte string pointed to by dest, that consists of only the characters found in the null-terminated byte string pointed to by src. 

---

## Štruktúry

* Nemusíme typedef-ovať ako v C
* V C++ máme aj class, ale o tom neskôr

```cpp
struct point {
    float X;
    float Y;
}; // semicolon is ultracritical!!!

struct circle {
    point P;
    int Radius;
    char color[16];
};
```


### Prístup k prvok

* Pomocou operátora `.`

```cpp
circle c;
c.Radius = 10;
c.P.X = 7.5;
c.P.Y = 12;

point p = { 7, 9 };
point q;
p.X = q.X; // undefined, initialize we must
```

---

# Pointers and references

---

## Smerníky (pointers) a referencie

* Každá (pomenovaná) premenná má v pamäti svoje miesto
* Adresu tohto miesta vieme získať pomocou unárneho operátora &
* Opačný proces, získanie hodnoty na adrese, sa robí pomocou operátora dereferencie `*`
* Referencia je iba alias inej premennej (vnútorne je implementovaná ako smerník)

![xkcd komix o smernikoch](./lectures/2_basics/pointers.png)


##  Null smerník

* Adresa (`0`) je rezervovaná ako neplatná 
* Užitočné na identifikáciu neinicializovaného smerníka 
* Dereferencia invalidného smerníka je nedefinovaná
* Konštanty pre null
   * `NULL`, makro z jazyka C
   * `0`, starý C++ typ
   * `nullptr`, preferované v moderných C++

![wrestler dereferencing a null pointer](./lectures/2_basics/null_pointer.png)
<https://www.youtube.com/watch?v=HSmKiws-4NU>


## Základné operácie so smerníkami

```cpp
int a, b; // a and b are uninitialized
int *ptr = &a; // ptr is some valid pointer e.g. 0x0029f97c
*ptr = 1; // a == 1
ptr = &b; // pointer to b
*ptr = 2; // b == 2, a == 1
```

```cpp
int &ref = a; // ref == 1, ref is reference to a
ref = 3; // a == 3, ref == 3
// the is no way to reinitialize ref to another variable
// &ref = b; error
```

```cpp
ptr = &ref; // ref is alias to a
*ptr = 4; // a == 4. ref == 4
ptr[0] = 5; // the same as *ptr = 5;
```

```cpp
ptr = nullptr;
ptr = 0;
ptr = NULL;
*ptr = 1;
```


### Operator `->`

```cpp
struct MyStruct {
    int a;
    int b;
};
 
int main() {
  MyStruct s;
  s.a = 0;
 
  MyStruct* ptr;
  ptr = &s;
  ptr->b = 0; // (*ptr).b = 0;
 
  const MyStruct* cptr = &s;
  // cptr->a = 1; error
  std::cout << cptr->a;
}
```

```cpp
struct MyStruct
{
    int a;
    int b;
};
 
int main() {
  MyStruct s;
  s.a = 0;
 
  MyStruct &ref = s;
  ref.b = 0;
 
  const MyStruct &cptr = s;
  // cptr.a = 1; error
  std::cout << cptr.a;
}
```

Referencie nemôžu byť neinicializované. Neexistuje taká vec ako neplatná referencia.

---

## Pamäť 

TOTO nejak ukazat ako to funguje...

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


## Aritmetika so smerníkmi

* C++ predpokladá lineárnu (neprerušovanú) pamäť, preto k smerníkom môžeme pripočítavať a odpočítavať hodnoty a získame ďalšie smerníky (nie nutné platné)
* Programátori sú zodpovedný za dereferencovanie iba platných smerníkov
* Inkrement a dekrement posúva o veľkosť typu (nie `1`)
   * `int*` sa zvýši o štyri bajty (`sizeof(int)`)
   * `char*` sa zvýši o jeden bajt (`sizeof(char)`)

```cpp
int arr[5] = { 1 }; // 1, 0, 0, 0, 0
int *ptr = &arr[3]; // ptr points to fourth element
*ptr = 1; // 1, 0, 0, 1, 0
++ptr; // ptr points to fifth element
*ptr = 2; // 1, 0, 0, 1, 2
ptr = ptr - 4; // first element
*ptr = 3; // 3, 0, 0, 1, 2
```


## Segmentation fault

* Zďaleka najčastejšou chybou v C++ programoch je Segmentation fault (Access violation)
* `0xC0000005`
* Príčiny
   * Čítanie neexistujúcej pamäte
   * Zapisovanie pamäte iba na čítanie *read only*
* Chyby
   * Pretečenie *Buffer overflow*
   * Dereferencovanie nulového smerníka

  
### Je nasledujúci výraz platný C++? Ak áno aký je výsledok?

```cpp
std::cout << 2["ABCDE"] << std::endl;
```

* `operator[]` *subscript operator* je definovaný ako `a[b] = *(a + b)`

```cpp
std::cout << *(2 + "ABCDE") << std::endl;
std::cout << *("ABCDE" + 2) << std::endl;
```

---

# Kompilácia

---

## Zdrojový súbor

* C a C++ pracujú s dvoma typmi súborov
   * Zdrojové súbory (`*.cpp`, `*.cc` alebo `*.c` pre C súbory)
   * Hlavičkové súbory (`*.h`, `*.hpp`, bez prípony)
* Každý `.cpp` súbor je zvyčajne spárovaný s `.h` súborom, ktorý deklaruje verejný interface a ten je potom v `.cpp` súbore implementovaný 
* Aj `.cpp` aj `.h` súbory môže include-ovať iné `.h` súbory, kvôli použitiu tried a funkcií, ktoré daný `.h` súbor deklaruje
* Hlavičkové súbory môžu obsahovať aj implementáciu, a niekedy aj musia, obyčajne sa tomu snažíme vyhýbať 

---

## `#include`

* include directíva iba nakopíruje referencovaný súbor na svoje miesto
   * `#include <filename> / #include "filename"`
   * Rozdiel iba mieste hľadania súborov 
* `<>` systémové a `""` užívateľsky definované
* Hlavný dôvod prečo používame include je zviditeľnenie symbolov zo súboru (triedy, funkcie, premenné, konštanty, makrá, šablóny, ...)

---

## Viacnásobný `#include`

![boromir lamenting multiple include](./lectures/2_basics/multiple-include.png)

* `#include` súbory iba nakopíruje, takže sa symboly ľahko redefinujú (chyba pri kompilácií)


### Define guards

* Definovanie unikátneho makra a potom test predíde viacnásobnému vloženiu súboru. 

```cpp
#ifndef MY_FILE_H
#define MY_FILE_H

// content of header file

#endif
```

* Nevyzerá ale pekne a unikátnosť vedie k pridlhým názvom.
* Namiesto toho sa dá použiť #pragma once na začiatku súboru
* Funguje na každom používanom kompilátore, ale nie je C++ štandard 
* Moduly v C++20 toto celé zmenia a mali by pomôcť s organizáciou kódu

---

## Kompilátor a linker 

TODO

---

# Code style

---

## Načo pravidlá?

* Pomáha začiatočníkom so štartom. Je jednoduchšie začať na kóde, ktorý vyzerá rovnako a riadi sa nejakými pravidlami
* Niektoré konštrukcie sú nebezpečné a nemali by sa používať. (Väčšinou sú pozostatky z histórie.)
* Projekt vyzerá viacej profesionálne ak je všetky pekne uhladené

> “Managing senior programmers is like herding cats”  
> — Dave Platt 

---

## Priručky

* <https://google.github.io/styleguide/>
* <https://gcc.gnu.org/codingconventions.html>
* Existuje aj veľa iných, je potrebné sledovať, či si nevyberáme nejakú staršiu, môže obsahovať neaktuálne pravidlá
* Neexistuje príručka pravidiel, ktoré by sa dali použiť na všetky projekty, kernel módový ovládač má iné požiadavky ako GUI aplikácia

---

## Core guidelines

* Skôr ako príručka je to text pojednávajúci o vhodnosti istých konštrukcií
* Dáva veľa voľnosti 
* <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>

---

## Názvy symbolov by mali byť jasné a bez chýb

![kim jong un requesting lunch](./lectures/2_basics/lunch.png)

* Pluginy na kontrolu gramatiky 
* Mená funkcií a premenných by mali byť popisné bez používania skratiek
* Výnimka sú skratky všeobecne známe v IT komunite (*HTTTP*, *DNS*, ... sú OK)

---

## Konzistentnosť

* Vo všeobecnosti je dobrý nápad držať sa jednej schémy pre pomenovanie a formátovanie funkcií 
   * *PascalCase*, *camelCase*, *snake_case*
   * Medzery medzi operátormi, (), taby vs. medzery
   * Maďarská notácia (Hungarion notation) už skôr nie 😎

```cpp
GetProductInfo(&dwProductCode, szProductVer);
 
OS_INFO os_info;
FillOsInfo(os_info);
 
FEATURES_INFO featuresInfo;
FillFeaturesInfo(featuresInfo);
CPlugin *pUpdPlugin = pMain->FindPlugin(PLUGIN_ID_EUPD);
struct 
{
	CCI_SPEC_GET_ACTIVE_USERNAME_REPLY Hdr;
	char Buffer[2048];
} usernameBuffer;
```

---

A foolish consistency is the hobgoblin of little minds, adored by little statesmen and philosophers and divines.

-- Ralph Waldo Emerson

![Ralph Waldo Emerson](./lectures/2_basics/Ralph-Waldo-Emerson-1860.webp)

---

## Pravidlá pre funkcie

* Väčšina funkcií by nemala mať postranné efekty (*side effects*), pre rovnaké vstupy by mali vrátiť rovnaké výstupy
* Z názvu (a parametrov) funkcie by malo byť zrejmé čo je úlohou funkcie
* Kratšie funkcie sú lepšie funkcie
* Micro optimalizácia všetkého je veľmi zlý nápad (ale kód nesmie byť zbytočne neefektívny)
* Každá funkcia by mala mať jednu činnosť čo vykonáva

---

## Efektívny kód by default

* Z dvoch ekvivalentných prístupov si vyberieme ten efektívnejší

```cpp
std::vector<int> v;
v.push_back(1);
v.push_back(2);
// ...
v.push_back(10);
```

```cpp
std::vector<int> v = {
    1, 2, /* ... */ 10,
};
```

* Kus kódu vľavo môže v skutočnosti urobiť niekoľko alokácií (a teda aj kopírovania), kód vpravo je aj prehľadnejší aj urobí iba jednu alokáciu

---

## V jednoduchosti je krása

* Kratší kód zvyčajne obsahuje menej chýb 
* (no code = no bugs)
* Vyhýbajte sa "write only" algoritmom
* Ak je niečo pomalé, treba to najprv odmerať (profiler) a potom meniť

```cpp
// Nebol eset, musime getnut
WCHAR		BufferW[256];
UINT_PTR	nSize = sizeof(BufferW);
if (NODPROT_OK == CCIoctl(0, 0, CCI_GET_STRING, &dwStringId, sizeof(dwStringId), BufferW, sizeof(BufferW), &nSize))
{
	size_t	nStrLen;
	TCHAR * pStringT;
 
	nStrLen = wcslen(BufferW);
	pStringT = (TCHAR*)malloc((nStrLen + 1) * sizeof(pStringT[0]));
 
	if (NULL != pStringT)
	{
		MakeNativeStringFromUnicode(BufferW, pStringT, nStrLen+1);
		EnterCriticalSection(&m_CriticalSection);
		// Este jeden check, co ak to uz neikto nastavil
		if (NULL == m_Strings[dwStringId])
			m_Strings[dwStringId] = pStringT;
		else
			free(pStringT);
		LeaveCriticalSection(&m_CriticalSection);
	}
}
```

---

# Konštanty

---

## Konštanty

* Z istého pohľadu programovanie je o udržiavaný invariantov a konštantnosť istých hodnôt a premenných môže pri tom veľmi pomôcť
* Konštantné dáta môžu byť pristupované z viacerých vlákien bez obavy o nedefinované správanie (data race)
* V C++ existuje viacero spôsobov ako definovať koncept konštanty 
   * `const`
   * `constexpr`
   * `#define`
   * `enum`

---

## `const`

* `const` znamená, že "objekt" sa nesmie meniť
* Dá sa obísť pomocou `const_cast`-u
* PROTIP: Nikdy nezahadzujte z objektov const
* Užitočné pri referenciách a smerníkoch (hlavne pri parametroch do funkcií)

```cpp
const int f(int a, const int& b, int& c) {
  int i = 1;
  const int j = 2;
  i = j;
  // j = i; // will not compile
  return i;
}
```

```cpp
void g() {
  int a = 3;
  // it is OK, that f return const int
  // it will be copied, so no problem
  int i = f(1, 2, a); 
}
```


### `const` a globálne objekty

* Globálne premenné sú vždy inicializované na `0` (*zero initialized*)
* Výnimkou sú const objekty, ktoré musia byť inicializované hodnotou
* Väčšinou sú umiestnené do pamäti iba na čítanie a preto pokus o zápis spôsobí access violation

```cpp
int v;
const int c = 1;
 
void main() {
  std::cout << v << " " << c << std::endl; // 0 1
  
  //c = 2; // will not compile
  *const_cast<int*>(&c) = 4; // will compile
}
```

---

## `constexpr`

* Konštanty počas kompilácie
* Užitočné ak potrebujeme compile time constatnt (napríklad veľkosť pola)
* V niektorých kontextoch constexpr implikuje const

```cpp
const int compute_size_1(int a) { return 2 * a*a; }
constexpr int compute_size_2(int a) { return 2 * a*a; }
 
int main() {
  int x[compute_size_1(7)]; // error

  int y[compute_size_2(7)]; // OK
}
```

---

## `#define`

* Preprocesorové makrá môžu byť použité ako konštanty
* Makrá sú expandované ešte pred samotnou kompiláciou, fungujú preto mimo typového systému v podstate iba textovo 
* Číselné konštanty sa lepšie vyjadrujú pomocou constexpr, alebo enum-u
* Občas užitočné pri reťazcoch 
   * Zreťazovanie (concatenation) počas kompilácie

```cpp
#define DIRECTORY "C:"
#define FILENAME "log.txt"
#define SEPARATOR "\\"
#define PATH DIRECTORY SEPARATOR FILENAME
 
void main() {
  std::cout << PATH << std::endl; // "C:\log.txt"
}

```

---

## String konštanty

```cpp
#define STR1 "My string"    
 
// const char[] will disable string pooling 
const char STR2[] = "My string";
 
// just pointer, not array of chars
const char *STR3 = "My string";
 
static_assert(sizeof(STR2) == sizeof(STR1), "Not the same size.");
static_assert(sizeof(STR3) == sizeof(STR1), "Not the same size."); // fail

const char *STR4 = STR1;
 
bool b = STR4 == STR3; // true if string pooling is enabled
 
bool str2_same = (void*)STR2 == (void*)&STR2; // ???
bool str3_same = (void*)STR3 == (void*)&STR3; // ???
```

Polia sú automaticky konvertované [array decay] na smerníky

---

## Raw literal

* Niektoré reťazce vyzerajú veľmi zle so všetkých escape sekvenciami (regex, cesty k súborom...)

```cpp
const char *path = "\"C:\\Program Files\"";
```

* **R**"**delimiter(**string**)delimiter**", delimiter je nepovinný a užitočný hlavne ak samotný reťazec obsahuje znak `)`

```cpp
const char *path = R"("C:\Program Files")";
```

---

## `enum`

* `enum` je v podstate iba pomenované celé číslo (aké presne vieme definovať)

```cpp
enum class FormatType : uint8_t {
  First = 0,
  Second, // 1
  Tenth = 10,
  Eleventh, // 11
};
```

* Bez kľúčového slova class hodnoty enum-u sa dostanú do globálneho namespace-u (rovnako ako v C), to nie je ideálne

---

## Čím viac const tým lepšie

* Uľahčí to programovanie
* Treba ale vybrať správny postup

```cpp
typedef WORD ECP_RET_ERROR;
 
static const ECP_RET_ERROR ECP_OK = 0x0000;
static const ECP_RET_ERROR ECP_ERROR = 0x0001;
```

* Kód vyššie kričí, že máme použiť enum
* Ak použijeme enum class ani nebudeme musieť prefixovať hodnoty (ostane nám OK, Error, ...)

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

Ak je podmienka splnená, tak sa pokračuje vo vykonávaní tela if-u, inak sa preskočí.

```cpp
int i = 0;
std::cin >> i;
if (i % 2 == 0) {
  std::cout << "Even\n";
} else {
  std::cout << "Odd\n";
}
```

Ak je podmienka splnená, tak sa pokračuje vo vykonávaní tela if-u, inak sa vykoná else vetva.

```cpp
int i = 0;
if (std::cin >> i; i % 2 == 0) {
  std::cout << "Even\n";
} else {
  std::cout << "Odd\n";
}
```

V if-e je inicializácia, podobne ako vo for cykle, podmienka je potom za `;`. **C++17**

### Pôjde nasledujúci kód skompilovať a ak áno aký je výsledok?

```cpp
if (char* p = (char*)malloc(2)) {
  std::cout << "A";
} else {
  std::cout << "B";
}
```

Deklarácia v rámci if-u funguje odkedy je C++ na svete. Stačí aby sa inicializovaná premenná dala skonvertovať na bool. Pozor premenná je potom dostupná aj v else vetve. 

---

## `switch`

```cpp
enum class Type { Number, String, Null };
Type t = Type::Number;

switch (t) {
case Type::Number:
    [[fallthrough]]; // state intent, no compiler warning
case Type::String:
    std::cout << "Has value\n";
    break;
case Type::Null:
    std::cout << "No value\n";
    break;
default:
    std::cout << "??\n";
}
```

---

## `for`

```cpp
for (size_t i = 0; i < numbers.size(); ++i) {
  if (numbers[i] % 2 == 0)
    continue; // will skip the rest of the for body and continue
  if (numbers[i] > 10)
    break; // prematurely stop the loop
  if (numbers[i] == 4)
    return; // if hit this will break out of enclosing function
  std::cout << numbers[i];
}
```

V C++11 a vyššie existuje lepší spôsob ako iterovať prvky

---

## Range based for loop

* Syntaktický cukor okolo iterátorov nad kontajnermi

```cpp
for (int i : numbers) {
  if (i % 2 == 0)
    continue; // will skip the rest of the for body and continue
  if (i > 10)
    break; // prematurely stop the loop
  if (i == 4)
    return; // if hit this will break out of enclosing function
  std::cout << numbers[i];
}
```

---

## Inicializácia v range based

* C++20 pridal inicializáciu do range based for loopu

```cpp
int numbers[] = { 1, 4, 5, 7, 8 };
for (int v = 0; auto i : numbers) {
  // v is here defined
}
```

---

## `while`, `do`

```cpp
std::vector<int> GetNumbers(size_t n) {
  std::vector<int> result;

  while (result.size() < n) {
    int x = 0;
    std::cin >> x;

    if (!std::cin) // not a number, failbit set
      break;
  }

  return result;
}

```


### Pôjde nasledujúci kód skompilovať a ak áno aký je výsledok?

```cpp
int i = 1;
do {
  std::cout << i << std::endl;
  --i;
  if (i == 0)
    continue;
} while (i > 0);
```

Vypíše iba 1, continue vždy skáče na koniec cyklu.

---

# ĎAKUJEM

## Otázky?
