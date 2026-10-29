# C++

## Move sémantika

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Storage class specifiers
* Čo sú lvalue a rvalue?
* Rvalue referencie
* Xvalues
* `std::move`
* RVO a NRVO

---

# Storage class specifiers

---

## `auto`

* Kľúčové slovo `auto` sa už v C++ nepoužíva na označenie *automatic storage duration* (podobne staré kľúčové slovo `register` bolo v C++17 úplne odstránené)
* Bez označenia premennej pomocou storage specifier-u sa použije automatická doba uloženia
* Miesto pre premennú je alokované na začiatku scope a dealokované na konci
* Konštruktory a deštruktory sa volajú *auto*maticky
* Miesto sa spravidla alokuje na stacku

---

## `static`

* `static` premenné vo funkciách sú inicializované, ak cez ne prejde beh programu (*control flow*)
* Deštruované sú po ukončení programu (vrátenie z `main` funkcie alebo volanie `exit`) v opačnom poradí, ako boli skonštruované
* Ak nejaká statická premenná nebola inicializovaná, tak nebude ani zdeštruovaná (toto je v povinnostiach kompilátora)


```cpp
struct A {
    A() { std::print("A"); }
    ~A() { std::print("~A"); }
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
    std::print("M");
} // ~A
```

* Konštruktor sa zavolá, iba ak sa funkcia `f` zavolá s parametrom `i == 0`. Deštruktor sa "zaregistruje", aby sa zavolal, keď program skončí. 

---

## `thread_local`

* Premenná sa inicializuje iba raz pre konkrétny thread, o tomto uvidíme viac na inej prednáške

---

## dynamic memory

* Okrem automatickej, statickej a thread-local doby uloženia existuje ešte dynamická pamäť na halde cez `new`/`delete`, ktorú sme už videli a spravujeme ju cez `std::unique_ptr`, `std::vector` či `std::string`

```cpp
int* i = new int(0);
// no exception can occur here
delete i;
```

---

# Globálne premenné a linkage

---

## Globálne premenné

* Sú inicializované ešte pred volaním funkcie `main` a zdeštruované po vrátení z funkcie `main`
* V jednom translation unit je poradie inicializácie rovnaké, ako sú napísané v súbore, a deštrukcia potom v opačnom poradí


## Nešpecifikované poradie

```cpp
// main.cpp
#include "version.h"
 
std::string version = GetAppVersion();
 
int main(int argc, char* argv[]) {
    std::println("{}", version);
}

```

```cpp
// version.cpp
#include "version.h"
 
uint32_t major = GetMajorNumber();
uint32_t minor = GetMinorNumber();
 
std::string GetAppVersion() {
    return std::to_string(major) + "." + std::to_string(minor);
}

```

* Všetky globálne premenné sú inicializované pred volaním `main`. Tu však jedna závisí od druhej v inom súbore (*translation unit*) – poradie ich inicializácie nie je určené (*static initialization order fiasco*) a pri čítaní ešte neinicializovaného objektu vedie k chybám alebo nedefinovanému správaniu.


## Globálne statické premenné

* Štandardné globálne premenné sú viditeľné v celom projekte (vo všetkých translation units, majú *external linkage*)
* Statické globálne premenné sú viditeľné iba v jednom translation unit (majú *internal linkage*)


### Pre bežné globálne premenné

```cpp
// file1.cpp
int global_variable = 67;

// file2.cpp
extern int global_variable; // will be the same variable
```

* `extern` hovorí kompilátoru, že táto premenná je definovaná niekde inde a až linker ju spojí dohromady
* `extern` sa dá použiť aj s funkciami, ak potrebujeme použiť funkciu z iného translation unit, ale nemáme k nemu header súbor
* Funkcie sú ale `extern` implicitne, teda `int func();` je to isté ako `extern int func();`


### Pre statické globálne premenné

```cpp
// file1.cpp
static int static_global_variable = 67;

// file2.cpp
//extern int static_global_variable; // linker error: not visible outside file1.cpp
static int static_global_variable = 43; // different variable
```

```cpp
// file2.cpp (modern C++ alternative: anonymous namespace)
namespace {
    int local_to_translation_unit = 43; // also internal linkage
}
```

* V modernom C++ namiesto `static` na globálnej úrovni často používame anonymný `namespace { ... }`, ktorý zaručí *internal linkage* aj pre typy a triedy, nielen pre premenné a funkcie.


### Bez všetkých týchto špecifikátorov

```cpp
// file1.cpp
int global_variable = 67;

// file2.cpp
int global_variable = 43; // error: redefinition of 'global_variable', linker error
```


### Riešenie pomocou `inline`

* V C++17 existuje kľúčové slovo `inline` pre globálne premenné, ktoré umožňuje definovať premennú v hlavičkovom súbore bez linker chyby

```cpp
// file1.cpp
inline int global_variable = 67;
inline int global_variable_2 = 102;

// file2.cpp
inline int global_variable = 43; // undefined, ODR violation
inline int global_variable_2 = 102; // OK
```

* ODR je *One Definition Rule*, hovorí, že v celom programe musí byť iba jedna definícia premennej/funkcie, deklarácie môžu byť viaceré
* Ak sa ODR poruší, tak je to nedefinované správanie programu, ale väčšinou to skončí linker chybou (nie je vyžadované štandardom)

---

## Jednoduché globálne a statické premenné

* Kompilátor môže (a aj by mal) staticky (počas kompilácie) inicializovať globálne a statické premenné bez komplexnej inicializácie
* Pamäť pre globálne a statické premenné sa alokuje v dátovej sekcii binárky (*image*)

```cpp
int global_variable = 43;
 
int main(int argc, char* argv[]) {
    static const char* static_string = "Static initialization";
 
    std::println("{}", global_variable);
}
```

---

# lvalues & rvalues

---

## Disclaimer lvalue a rvalue

* Koncept lvalue a rvalue prešiel počas života C++ mnohými zmenami
* To, čo budeme ďalej používať, nie je úplne presné, je to taký historický zjednodušený model
* Preto ak vám niečo nebude dávať dokonalý zmysel, pýtajte sa alebo konzultujte štandard 😊

---

## lvalue

* V podstate všetko, čo môže byť na ľavej strane priradenia (l v názve je *left*)
* Väčšinou pomenované premenné

<table style="width: 80%; border: 0">
<tr>
<td>

```cpp
int i;
i = 3; 

std::string s;
s = "Lvalue"; 

std::vector<int> v;
v[0] = 0; 
```
</td>
<td>

```cpp
void f(std::string s) {
    s = "Still lvalue";
}

void f(std::string& s) {
    s = "Still...";
}

void f(const std::string& s) {
    // s = "We cannot change, but lvalue";
}
```
</td>
</tr>
</table>

* `T&` sa nazýva *lvalue reference*
* `const T&` sa nazýva *const lvalue reference*

---

## rvalue

* V podstate všetko, čo môže byť iba na pravej strane priradenia (*r* v názve je *right*)
* Väčšinou hodnoty bez mena alebo *temporaries*

<table style="width: 80%">
<tr>
<td>

```cpp
int i;
i = 3; // lvalue OK

int i;
3 = i; // 3 is rvalue
// error C2106: '=': left operand must be l-value
```
</td>
<td>

```cpp
int a = 1;
int b = a + 2; // (a + 2) is rvalue

a + 2 = b; // error: (a + 2) is rvalue
```
</td>
</tr>
</table>

---

## Dočasné premenné

* *Temporaries*
* Všetky premenné na predchádzajúcich slidoch mali mená
* Pomenované premenné zostávajú žiť až po koniec scope-u, v ktorom boli skonštruované
* Nepomenované premenné (dočasné) sú zdeštruované na konci celého výrazu, v ktorom vznikli (zvyčajne pri `;`)

```cpp
int main() {
    std::string("My temporary string");
 
    std::println("{}", std::string("File ") + __FILE__);
}
```


## rvalue a temporaries

* Temporaries sú vo všeobecnosti `rvalue`
* Do dočasnej premennej nemá zmysel priraďovať, lebo jej hodnotu aj tak nebudeme môcť spracovať

```cpp
int() = 4; // error
```

```cpp
std::string s = "lvalue"; 
std::string() = s; // ???
```

<div class="fragment">

String je zaujímavý. Ako objekt má preťažený operátor priradenia. Preto sa nepoužije zabudovaný operátor priradenia, ale funkcia `std::string& operator=(const std::string&)`. Preto sa kód skompiluje, kompilátor nemôže vedieť, že operátor priradenia nemá žiadne vedľajšie účinky. V tomto prípade môže teoreticky vyhodiť výnimku. 
</div>

---

## Adresa v pamäti

* Iný spôsob, ako sa pozrieť na lvalue/rvalue, je pomocou operátora `&`
* Objekt je lvalue, ak sa dá zobrať jeho adresa 

```cpp
std::string s = "lvalue"; 
&s; // ok
 
&std::string(); // error
```

---

## Zložené výrazy

* Vlastnosť *lvalue* a *rvalue* je vlastnosťou výrazov (nie iba jednoduchých premenných)

<table>
<tr>
<td style="width: 50%">

```cpp
int i, j;
i + 4;
j = i + 1;
```
</td>
<td>

Výsledok výrazu `i + 4` je rvalue, keďže operátor `+` vracia novú hodnotu. Operátor `=` na druhej strane vracia samého seba, takže výsledkom je lvalue.
</td>
</tr>
<tr>
<td>

```cpp
std::ofstream output;
output << "New value";
```
</td>
<td>

Výsledok operátora `<<` je lvalue referencia (taký typ vracia operátor). 
</td>
</tr>
<tr>
<td>

```cpp
std::string f() { return std::string(); }

f();
```
</td>
<td>

Výsledok volania funkcie `f` je rvalue. 
</td>
</tr>
</table>

---

## lvalue vs rvalue

| lvalue | rvalue |
| ------ | ------ |
| Môže byť na ľavej strane operátora priradenia. | Musí byť na pravej strane operátora priradenia. |
| Často má meno. | Väčšinou nemá meno. |
| Môžeme získať adresu. | Adresa sa nedá získať. |
| Väčšinou má dlhšiu životnosť (scope). | Väčšinou býva iba temporary. |

---

## Volanie funkcie a lvalue/rvalue

Je výsledkom nasledujúceho kódu lvalue alebo rvalue?

```cpp
function();
```


* Závisí to od návratovej hodnoty
    * Napríklad ak vraciame hodnotu `int function();`, tak je to rvalue
    * Ak vraciame referenciu `int& function();`, tak je to lvalue

<table>
<tr>
<td style="width: 30%">

```cpp
int function();

int main() {
    function() = 4;
}
```
</td>
<td>

Neskompiluje sa, výsledkom funkcie je rvalue, a preto nemôže byť na ľavej strane priradenia. Nikdy by sme neboli schopní pozorovať výsledok toho priradenia, a preto je to pravdepodobne chyba.
</td>
</tr>
<tr>
<td>

```cpp
int& function();

int main() {
    function() = 4;
}
```
</td>
<td>

V poriadku, výsledkom funkcie je lvalue, a preto môže byť na ľavej strane priradenia. Výsledok priradenia s veľkou pravdepodobnosťou môže mať vplyv na náš program.
</td>
</tr>
</table>

---

# Referencie

---

## Modifikovateľné referencie

* Referencie, ktoré nemajú pred sebou napísané `const`
* Vieme ich získať z lvalue výrazov
* Z rvalue ich nevieme získať (kompilačná chyba), dôvod je ten, že keďže rvalue sú spravidla temporaries, tak takáto referencia by nedávala zmysel


```cpp
std::string s;

std::string& ref1 = s;
std::string& ref2 = "const char*";
std::string& ref3 = std::string{};
```

* Skompiluje sa iba `ref1`. 
* `ref2` a `ref3` sa neskompilujú, lebo do lvalue modifikovateľnej referencie nevieme priradiť rvalue
* V podstate je to podobné ako získavanie adresy pomocou operátora `&` (ešte aj znak je ten istý)

---

## Konštantné referencie

* Referencie, ktoré majú pred sebou napísané `const`
* Vieme ich použiť na čítanie hodnôt, ale nie na zmenu
* Dáva zmysel, že ich vieme vytvoriť aj z lvalue, aj z rvalue hodnôt

```cpp
std::string s;

const std::string& ref1 = s;
const std::string& ref2 = "const char*";
const std::string& ref3 = std::string{};
```

* Všetky sa skompilujú

---

## Čo urobí tento kód?

```cpp
int main(int argc, char* argv[]) {
    const std::string& s = std::string("Temp");
 
    std::println("{}", s);
    return 0;
}
```
<div class="fragment">

* Používame temporary premennú po tom, čo skončil riadok, na ktorom bola vytvorená
* Konštantné referencie predlžujú scope temporary premenných
</div>

---

# Parametre funkcií

---

## Aké typy zvoliť?

* Môžeme použiť volanie hodnotou, referenciou (modifikovateľnou) alebo konštantnou referenciou
* Každé má svoje výhody a nevýhody

![Homer Simpson in power plant](./lectures/move/simpson_power_plant.png)

---

## Volanie hodnotou

```cpp
void func(std::string s) {
 
}
```

* Vždy urobí kópiu (pomalé, ak sa musí kopírovať veľa pamäte)
* Užitočné pre malé typy (niekedy môžu byť referencie pomalšie ako kópie, keďže kompilátor môže lepšie optimalizovať)
* Mal by to byť default pre primitívne typy (`int`, `char`, `double`, ...)

---

## Konštantná referencia

```cpp
void func(const std::string& s) {
 
}
```

* Hodnota sa nedá modifikovať, preto si môžeme byť istí, že funkcia nebude meniť tento parameter
* Mal by byť default pre skoro všetko
* Nič sa nemusí kopírovať, lebo na pozadí sa len posunie jeden pointer


## `const` by mal vždy ostať `const`

![Do not use const_cast](./lectures/move/const_cast.png)

---

## Modifikovateľná referencia

```cpp
void func(std::string& s) {
 
}
```

* Tu predpokladáme, že funkcia `func` bude parameter `s` modifikovať (ak by to nerobila, tak tam treba dať `const`)
* Použiteľné pre vstupno-výstupné parametre, pre iba výstupné treba použiť návratovú hodnotu
* Takýto parameter sa dá volať iba pomocou lvalue; keďže má byť výstupný, tak potrebujeme, aby sme ho vedeli použiť aj neskôr
* angl. mutable references

---

## Volanie rvalue (temporary) hodnotami

```cpp
void func_value(std::string s);
void func_const(const std::string& s);
void func_ref(std::string& s);

int main() {
    func_value(std::string{}); // OK
    func_const(std::string{}); // OK
    //func_ref(std::string{}); // cannot compile
}
```


## Volanie lvalue hodnotami

```cpp
void func_value(std::string s);
void func_const(const std::string& s);
void func_ref(std::string& s);

int main() {
    std::string s;

    func_value(s); // OK, copy
    func_const(s); // OK
    func_ref(s); // OK
}
```


## Volanie `const` lvalue hodnotami

```cpp
void func_value(std::string s);
void func_const(const std::string& s);
void func_ref(std::string& s);

int main() {
    const std::string s;

    func_value(s); // OK, copy
    func_const(s); // OK
    // func_ref(s); // error, cannot bind const lvalue to non-const reference
}
```

---

## Smerníky?

* Smerníky sú vlastne iba hodnoty, takže sa kopírujú
* Niektorí programátori preferujú smerník ako výstupnú hodnotu, aby to bolo explicitné

<table style="width: 100%">
<tr>
<td>

```cpp
void func(std::string& s) { }
 
int main() {
    std::string s;
    // will this modify or not?
    func(s); 
}
```
</td>
<td>

```cpp
void func(std::string* s) { }
 
int main() {
    std::string s;
    // explicit
    func(&s); 
}

```
</td>
</tr>
</table>


## Nepovinné parametre

* Niekedy môže funkcia chcieť parameter, ktorý je nepovinný
* Aktuálne je na to jedna z najlepších možností použiť smerník

```cpp
void func(const std::string* s) { }
 
int main() {
    func(nullptr); 
}
```


## `std::optional` sa nedá použiť

```cpp
void func(std::optional<std::string>& s) { }
 
int main() {
    std::string s;

    func(s); 
}
```

* Kód vyššie nejde skompilovať
* Po pridaní `const` pred `std::optional` to už skompilovať ide, ale vytvorí sa dočasný `std::optional<std::string>`, do ktorého sa reťazec `s` skopíruje (keďže typy nie sú rovnaké)


## Preťaženie funkcie

* V C++ môžeme funkcie preťažiť

```cpp
void func() { }
void func(const std::string& s) { }

int main() {
    func(); 
    func("New string");
}
```

* Toto je ale problém, ak máme veľa nepovinných parametrov
* Stačí sa pozrieť na štandardnú knižnicu, tam každá funkcia má veľa preťažení

---

## Automatická tvorba temporary premenných

* V C++ sa veľmi často tvoria temporary premenné
* Totiž ak dva typy nie sú rovnaké, ale jeden sa dá konvertovať na druhý, tak sa táto konverzia použije a urobí sa temporary premenná

```cpp
void func(const std::string& s) { }
 
int main() {
    func("New string"); // == func(std::string("New string"));
 
    return 0;
}
```

* Nový string sa vytvorí pred volaním funkcie `func` a hneď potom sa zavolá deštruktor (preto pre čisto čítané reťazce preferujeme `std::string_view`, ktorý takúto dočasnú alokáciu nerobí)


## Explicitné konštruktory

```cpp
void func(const std::vector<int>& v) { }
 
int main() {
    func(15); 
 
    return 0;
}
```

* `vector` má konštruktor, ktorý berie jedno číslo, je to vlastne veľkosť vectora
* Toto sa ale nepodarí skompilovať, lebo tento konštruktor je explicitný (`explicit`), a teda sa nepodarí ho použiť
* Explicitné konštruktory boli zavedené už pred C++11 (v C++11 pribudli explicitné konverzné operátory)

---

## Problém

* S referenciami (či už konštantnými, alebo modifikovateľnými) nevieme rozlíšiť medzi volaním s lvalue a s rvalue

```cpp
void func(const std::string &s) { 
  std::string copy = s; 
}
 
int main() {
  std::string perm_string;
  func(perm_string);
 
  func("temp string");
 
  return 0;
}
```

* Ak by sme to vedeli, mohli by sme sa správať inak pre lvalue a pre rvalue
* Keďže rvalue už nikto neuvidí, tak by sme ho mohli použiť vo funkcii (priamo pamäť tejto premennej)

---

# Move sémantika

---

## `std::move`

```cpp
int main() {
  std::string s = "My lo ... ong string";
  std::string new_s = std::move(s);
 
  // s is now empty
}
```

* `std::move` použije move sémantiku namiesto kópie
* Vnútorné časti stringu sa "vytrhajú" a použijú sa v novom stringu 
* Pointer na vnútorný buffer sa iba presunie
<!-- .element: class="fragment" -->

---

## rvalue referencie

* Mechanizmus, ktorý nám toto umožňuje, sú rvalue referencie
* Umožňujú nám rozlíšiť medzi lvalue a rvalue referenciami, a teda vieme, ktoré hodnoty môžeme `move`-nuť a ktoré musíme kopírovať
* Funkcie môžu byť preťažené na tieto referencie

---

## `&&`

* Symbol pre rvalue referencie je `&&`
* Nemá veľké využitie mimo typov parametrov funkcií
* Väčšinou nemusíte tvoriť nové funkcie s rvalue referenciami
* Je to hlavne dôležité pre tvorcov kontajnerov, aby sa zbytočne nekopírovali
* <small>Existujú aj konštantné rvalue referencie, ale na tie môžeme rovno zabudnúť</small>


## `const&` a `&` dokopy

```cpp
void func(const std::string& s) { }

void func(std::string& s) { }
 
int main() {
    std::string s1;
    func(s1);

    const std::string s2;
    func(s2);

    func(std::string());
}
```

* Všetko pôjde skompilovať
    * Prvé volanie zavolá `func(std::string& s)`
    * Druhé volanie zavolá `func(const std::string& s)`
    * Tretie volanie zavolá `func(const std::string& s)`, keďže viazanie rvalue na modifikovateľnú lvalue referenciu je zakázané


## Ako s volaním hodnotou?

```cpp
void func(const std::string& s) { }

void func(std::string& s) { }

void func(std::string s) { }
```

Nepodarilo by sa skompilovať, každé volanie by bolo nejednoznačné. Buď máme volanie hodnotou, alebo všetky ostatné, dokopy sa nemajú veľmi radi. 


## Pridáme rvalue referencie do mixu

```cpp
void func(std::string& s) { }

void func(const std::string& s) { }

void func(std::string&& s) { }

int main() {
    std::string s1;
    func(s1);

    const std::string s2;
    func(s2);

    func(std::string());
}
```

* Každé volanie zavolá svoju príslušnú funkciu
* Vieme teda rozlíšiť medzi lvalue a rvalue
* Môžeme to využiť na písanie rýchlejšieho kódu


## `std::string&` nám netreba

* Ako sme spomínali, modifikovateľné lvalue referencie veľmi nepotrebujeme, majú zmysel iba vo veľmi špecifických prípadoch
* Realisticky nám stačí iba konštantná lvalue referencia a modifikovateľná rvalue referencia
* Ostatné je skôr ezoterika 😉


## Sink parametre (pass-by-value + `std::move`)

* Ak funkcia alebo konštruktor potrebuje **vlastnú kópiu** parametra (napr. uloží si ho do členskej premennej), nemusíme písať dve preťaženia (`const std::string&` a `std::string&&`)
* Stačí prijať parameter **hodnotou** a následne ho presunúť cez `std::move`:

```cpp
class Person {
public:
    explicit Person(std::string name)
        : name_(std::move(name)) { } // 1 copy + 1 move for lvalue, 1–2 moves for rvalue
private:
    std::string name_;
};
```

* Ak zavoláme `Person(s)` s lvalue, urobí sa jedna kópia do parametra `name` a lacný move do `name_`
* Ak zavoláme `Person("Bjarne")` s rvalue, neurobí sa žiadna kópia – iba move!

---

# Implementácia rvalue referencií

---

## Príklad `Socket`

```cpp
class Socket {
public:
    explicit Socket(int port) : fd_(open_socket(port)) { }

    ~Socket() {
        if (fd_ >= 0) {
            close_socket(fd_);
        }
    }

    int fd() const { return fd_; }

private:
    int fd_ = -1; // OS file descriptor / handle (-1 means invalid/closed)
};
```

* Trieda obaľujúca systémový zdroj (napr. sieťový socket alebo otvorený súbor)
* Konštruktor otvorí spojenie a deštruktor ho automaticky zavrie, ak je `fd_ >= 0` (RAII)


## Kopírovanie pre `Socket` nedáva zmysel

* Čo by malo znamenať skopírovanie otvoreného sieťového spojenia?
* Ak by dva objekty `Socket` mali rovnaké `fd_`, oba deštruktory by zavolali `close_socket(fd_)` nad tým istým deskriptorom!
* Preto kopírovací konštruktor a kopírovací `operator=` explicitne **zakážeme**:

```cpp
class Socket {
public:
    explicit Socket(int port) : fd_(open_socket(port)) { }
    ~Socket() { if (fd_ >= 0) close_socket(fd_); }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    // ...
};
```

---

## `move` (prvý, naivný pokus)

* Presunúť otvorený `Socket` (napr. vrátiť ho z funkcie alebo uložiť do `std::vector<Socket>`) však dáva zmysel!
* Nestačí však len skopírovať `fd_` (pozri ďalší slide!):

### Move konštruktor

```cpp
Socket(Socket&& other) noexcept
    : fd_(other.fd_) { // BUG: other still has the same fd_!
}
```

### Move operátor priradenia

```cpp
Socket& operator=(Socket&& rhs) noexcept {
    if (&rhs == this)
        return *this;

    fd_ = rhs.fd_; // BUG: old fd_ was not closed and rhs still has fd_!
    return *this;
}
```


## Move musí nechať objekt vo validnom stave

* Deštruktor presunutého objektu (`other`) sa na konci jeho životnosti stále zavolá!
* Ak by sme v `other.fd_` nechali pôvodný deskriptor, deštruktor `~Socket()` by nám živé spojenie okamžite zavrel!
* Preto **musíme** nastaviť `other.fd_ = -1` (stav „neplatný/zavretý socket“, pri ktorom `~Socket()` neurobí nič)

```cpp
Socket(Socket&& other) noexcept
    : fd_(other.fd_) {
    other.fd_ = -1; // required so ~Socket() does not close our moved socket!
}
```


## move `operator=`

* Pri priradení do existujúceho objektu musíme navyše najprv zavrieť náš starý socket (ak bol otvorený):

```cpp
Socket& operator=(Socket&& rhs) noexcept {
    if (&rhs == this)
        return *this;

    if (fd_ >= 0) {
        close_socket(fd_); // close our previous socket first
    }
    fd_ = rhs.fd_;
    rhs.fd_ = -1;
    return *this;
}
```


## `std::exchange`

* Môžeme použiť `std::exchange` (z `<utility>`) na zjednodušenie implementácie
* `c = std::exchange(a, b)` vráti starú hodnotu `a` a zároveň priradí `a = b`
* Nie je to atomická operácia (`std::atomic::exchange`)

```cpp
Socket(Socket&& other) noexcept
    : fd_(std::exchange(other.fd_, -1)) {
}

Socket& operator=(Socket&& rhs) noexcept {
    if (&rhs == this)
        return *this;

    if (fd_ >= 0) {
        close_socket(fd_);
    }
    fd_ = std::exchange(rhs.fd_, -1);
    return *this;
}
```

---

## Ako použiť move?

* Dočasné objekty (*temporaries*) si vyberú rvalue referenciu, keďže sú rvalue
* Keďže temporaries hneď zaniknú, môžeme z nich bezpečne prevziať otvorený socket

```cpp
int main() {
    Socket a(8080);
    Socket b(9000);

    // b = a;             // compiler error: copy is deleted!
    b = Socket(443);      // OK: closes port 9000 and moves port 443 into b
    b = std::move(a);     // OK: moves port 8080 from a into b (a.fd() is now -1)
}
```

---

## Návratová hodnota z funkcie

```cpp
Socket connect_to_server() {
    Socket s(8080);

    send_handshake(s.fd());

    return s;
}
 
int main() { 
    Socket client = connect_to_server(); 
}
```

* Ak sa z funkcie vracia lokálna premenná rovnakého typu, ako je návratový typ, tak sa urobí automatický *move* (prípadne NRVO)
* Táto premenná prestáva existovať (je lokálna), takže dáva zmysel ju *move*-nuť
* Toto je definované v štandarde a môžeme sa na to spoľahnúť

---

## Výstupné parametre

* Ako sme už spomínali, sú viac-menej prekonané
* Používali sa, aby sme sa vyhli kopírovaniu
    * S move už nikdy nekopírujeme (move majú všetky štandardné kontajnery)
    * Aj pred move sémantikou to bolo neoptimálne správanie
* Môže to dávať zmysel jedine vtedy, ak typ nie je ani copy, ani move konštruovateľný. 

---

## Vracanie viacerých hodnôt

* Použite pár (`pair`)
    * `first` je prvý člen
    * `second` je druhý
* Alebo n-ticu (`tuple`)
    * Pristúpiť sa dá pomocou `std::get<N>`
    * Alebo `std::get<T>`

```cpp
std::tuple<int, int, std::string> more() { 
    return {1, 2, "string"}; 
}
```

```cpp
auto a = more();
int i = std::get<0>(a);
int j = std::get<1>(a);
std::string s = std::get<2>(a);
```

```cpp
auto a = more();
int i = std::get<int>(a); // fails to compile, more than one int
std::string s = std::get<std::string>(a); // OK
```


## `std::tie` a `std::ignore`

* Na priradenie n-tice do **už existujúcich** premenných môžeme použiť `std::tie` (vytvorí n-ticu lvalue referencií; pre nové premenné dnes preferujeme *structured bindings* na ďalšom slide)
* Ak je na pravej strane rvalue, prvky sa do premenných presunú (*move*)
* Ak nás niektorá hodnota nezaujíma, použijeme `std::ignore`

```cpp
int i;
std::string s;

std::tie(i, std::ignore, s) = more();
```

---

## Structured bindings

* Pridané v C++17
* Umožňujú rozbaliť `tuple`, `pair`, pole alebo `struct` do samostatných mien

```cpp
#include <tuple>

std::tuple<int, std::string, float> f() {
    return { 10, std::string("String"), 0.1f };
}

int main() {
    auto [a, b, c] = f();

    std::println("{}", a); // prints 10 (int)
    std::println("{}", b); // prints String (std::string)
    std::println("{}", c); // prints 0.1 (float)
}
```

note: Pod kapotou sa vytvorí skrytá premenná pre celú n-ticu a mená `a`, `b`, `c` odkazujú na jej prvky.


## Funguje aj so `struct`

```cpp
struct S {
    int i;
    std::string s;
};

S f() { return S{ 10, "string" }; }

int main() {
    auto [a, b] = f();

    std::println("{}", a);
    std::println("{}", b);
}
```


## Vlastné typy

```cpp [2-4|14-21|15-16|18-20|6-11|24-28|]
struct S {
    int i; // I want this to be second
    float f; // I do not want to destructure this
    std::string s; // I want this to be first

    template<std::size_t Index>
    std::tuple_element_t<Index, S>& get() // we should define const overload...
    {
        if constexpr (Index == 0) return s;
        if constexpr (Index == 1) return i;
    }
};

namespace std {
    template<>
    struct tuple_size<S> : std::integral_constant<size_t, 2> {};

    template<size_t Index>
    struct tuple_element<Index, S>
        : tuple_element<Index, tuple<std::string, int>> {};
}

int main() {
    S s{ 13, 0.1f, "string" };
    auto [a, b] = s;

    std::println("{}", a); // print string
    std::println("{}", b); // print 13
}
```

---

# Xvalues

---

## rvalue = xvalue + prvalue

* prvalues sú pure rvalues
    * Temporaries
    * Literals
* xvalues sú eXpiring values (hodnoty, ktoré nám ďalej už netreba)
* xvalues sú vytvorené pomocou pretypovania na `&&` alebo pomocou `std::move`, čo je vlastne vnútorne cast na `T&&`
* Môžeme teda vyvolať *move* explicitne


## Použitie xvalues

```cpp
int main() {
    Socket a(8080);
    Socket b(9000);

    b = static_cast<Socket&&>(a);
    b = std::move(a); // same thing: casts lvalue 'a' to Socket&&
}
```

* Ak zavoláme explicitne `std::move` na typ, ktorý nemá žiadnu funkciu s rvalue referenciou, tak sa bezpečne zavolá preťaženie s konštantnou referenciou (urobí sa kópia, ak nie je zakázaná)
* <http://en.cppreference.com/w/cpp/language/overload_resolution>


## `std::unique_ptr`

* `std::move` sa hodí na použitie s `std::unique_ptr`
* Keďže `unique_ptr` má zakázané kopírovanie, vlastníctvo vieme presunúť iba pomocou *move*

```cpp
std::unique_ptr<std::string> ptr;

ptr = std::make_unique<std::string>("HI."); // temporary unique_ptr is moved into ptr

// auto p = ptr; // will not compile, unique_ptr has no copy
auto p = std::move(ptr); // move ownership, so ptr is now nullptr
```

---

## Implementácia move konštruktora

```cpp
struct S {
    std::string s;
    int i;
    void* p;

    S(S&& other) noexcept
        : s(std::move(other.s))
        , i(other.i)
        , p(std::exchange(other.p, nullptr)) {
    }
};
```

* So stringom musíme použiť `std::move`, s ostatnými je to jedno (move a copy `int`u sú rovnaké)
* V samotnej funkcii `S(S&& other)` je `other` premenná s menom, a preto sa k nej správame ako k lvalue
* Aby sme pri inicializácii člena `s` zavolali move konštruktor (a nie copy), musíme zavolať `std::move(other.s)`

---

## Automatické generovanie špeciálnych operácií

* **Move konštruktor a move `operator=`** kompilátor vygeneruje automaticky, iba ak trieda nemá vlastný (*user-declared*) kopírovací konštruktor, kopírovací `operator=` ani deštruktor
* **Kopírovací konštruktor a kopírovací `operator=`** sa zase nevygenerujú (sú implicitne `= delete`), ak má trieda vlastný move konštruktor alebo move `operator=`
* Ak chceme generovanie explicitne povoliť alebo zakázať, použijeme `= default` alebo `= delete`:

```cpp
class my_class {
public:
    my_class(my_class&&) = default;
    my_class& operator=(my_class&&) = default;
private:
    int i;
    std::string s;
    std::vector<int> v;
};
```


## Rule of five

* Ak definujeme jednu z týchto piatich operácií, mali by sme definovať všetky
    * Kopírovací konštruktor
    * Kopírovací operátor priradenia
    * Move konštruktor
    * Move operátor priradenia
    * Deštruktor
* Definícia je, aj keď použijeme `= default` alebo `= delete`

---

## Kanonická implementácia

* Nasledujúce implementácie vygeneruje kompilátor
* Ak sú OK automaticky vygenerované, tak nepíšte vlastné
* Kde sa dá, pridá sa `noexcept` (ak majú všetky premenné `noexcept` move)

```cpp
MyClass(MyClass &&other)
    : i(std::move(other.i))
    , s(std::move(other.s))
    , v(std::move(other.v)) {}
```

```cpp
MyClass &operator=(MyClass &&rhs) {
    i = std::move(rhs.i);
    s = std::move(rhs.s);
    v = std::move(rhs.v);
    return *this;
}
```

---

## Je toto validná move implementácia?

```cpp [7-8|]
class A { /*implementation*/ };
class B { /*implementation*/ };
 
class C {
public:
  C(C &&other) 
    : a(std::move(other.a))
    , b(other.b) {}
 
  C &operator=(C &&rhs) {
    a = std::move(rhs.a);
    b = rhs.b;
    return *this;
  }
private:
  A a;
  B b;
};
```
<!-- .element: class="showall" -->

* Copy je validná implementácia move
<!-- .element: class="fragment" -->
* Pre niektoré typy sa ani nedá inak
<!-- .element: class="fragment" -->
* Move je iba optimalizácia
<!-- .element: class="fragment" -->

---

## Nekopírovateľné typy

* Pre niektoré typy môže byť veľmi ťažké, až nemožné naimplementovať copy
    * `std::ifstream`
    * `std::unique_ptr`
* Move sa ale takmer vždy dá naimplementovať

```cpp
std::unique_ptr<int> f(std::unique_ptr<int> i) {
    std::unique_ptr<int> ret(new int);
    *ret = *i + 1;
    return ret; // i will be destroyed here
}

int main() {
    std::unique_ptr<int> a(new int(0));
    // f(a); fails to compile
    a = f(std::move(a));
}
```

---

## Štandardná knižnica a move

* Move je všade v štandardnej knižnici
* Vector sa snaží urobiť move, keď sa realokuje
* `push_back` môže *move*-nuť prvky do vectora
* Štandardné kontajnery (`std::string`, `std::vector`, ...) sa dajú *move*-nuť
* Veľa funkcionality funguje automaticky a tam, kde sa robila kópia, sa od C++11 začal robiť *move*
* Stačilo prekompilovať novým kompilátorom a mali sme rýchlosť zadarmo

---

## Move vectora

```cpp
int main() {
    std::vector<std::string> v(1000000, std::string(1000, 'a'));
 
    // one million strings with one thousand characters will be 
    // copied, that is roughly one billion bytes, it can take 
    // a while
    auto copy = v;
 
    // three pointers are copied 
    auto move = std::move(v);
}
```

* Move vie veľmi zmenšiť potrebu kopírovania
* Môžu z toho ale vzniknúť aj bugy

---

# RVO a NRVO

---

## Nasledujúci kód urobí copy alebo move?

```cpp
std::string f() {
    return std::string("New string");
}
 
int main() {
    std::string s = f();
}
```

Je lepšie takto?<!-- .element: class="fragment" -->

```cpp
void f(std::string &s) { 
    s.assign("New string"); 
}
 
int main() {
    std::string s;
    f(s);
}
```
<!-- .element: class="fragment" -->


**Zle napísané kompilátory by mohli urobiť kópiu (pred C++11) alebo move (po C++11). Ale už v starom svete (pred C++11) sa prvý kus kódu transformoval na niečo ekvivalentné druhému kusu kódu.**


## copy a move

* Kópia vie byť veľmi drahá, pokojne aj desiatky megabajtov a desiatky sekúnd
* Move je výrazne rýchlejší, na záver sa len presunie pár pointrov
* Existuje ale RVO a NRVO, ktoré nám vyoptimalizuje aj tých pár bajtov
* Musíme pre to ale už niečo urobiť

---

## Return value optimization (RVO)

* Standard committee si uvedomovala, že C++ je o rýchlosti
* Kopírovanie je ale veľmi drahé
* Problém je, že konštruktory môžu mať vedľajšie účinky (*side effects*), a preto kompilátor bežne nemôže volanie funkcie len tak vynechať
* Alebo môže? Čo ak v štandarde povolíme vynechať volanie copy/move konštruktora aj vtedy, ak má side effects?<!-- .element: class="fragment" -->
* Potom môže kompilátor skonštruovať objekt priamo tam, kde má byť, a kópiu ani move vôbec nevolať (copy elision)<!-- .element: class="fragment" -->


## Return value optimization (RVO)

* Pri návrate z funkcie sa objekt vytvorí priamo v pamäti volajúcej funkcie (bez medzikópie a bez move)
* Aplikuje sa, keď z funkcie vraciame priamo nový dočasný objekt (prvalue)
* Od C++17 je táto optimalizácia **povinná** (*guaranteed copy elision*, niekedy označovaná aj ako URVO – *unnamed RVO*)

```cpp
std::string f(int i) {
    if (i < 10) {
        return "Less than 10";
    } else {
        return "Greater or eq 10";
    }
}
```


## Príklad na zmiešané použitie

```cpp [3|5-6|8|11]
std::string func(const std::string& s) {
    if (s.empty()) {
        return "Default value";
    } else {
        std::string result;
        std::cin >> result;
        if (!std::cin) {
            return "Default value";
        }

        return result;
    }
}
```

---

## Named return value optimization (NRVO)

* Ak v tele funkcie pracujeme s pomenovanou lokálnou premennou, snažíme sa v každom `return` vracať tú istú premennú
* Ideálne definujeme jednu návratovú premennú a vraciame len tú (na rozdiel od RVO nie je NRVO štandardom garantované, ale kompilátory ho bežne robia)

```cpp
std::string f(int i) {
    std::string result;
    if (i % 2) {
        result = "Odd";
    } else {
        result = "Even";
    }
    return result;
}
 
int main() { 
    std::string s = f(0); 
}
```


## ... transformuje kompilátor ako

```cpp
void f(int i, std::string &ret_val) {
    if (i % 2) {
        ret_val = "Odd";
    } else {
        ret_val = "Even";
    }
}
 
int main() {
    std::string s;
    f(0, s);
}
```

* Funkcia môže mať viacero `return`-ov, ale vždy sa musí vracať tá istá premenná

---

## RVO a NRVO manuál

* Vždy používajte návratovú hodnotu, nie výstupné parametre
* V copy ani move konštruktore sa nespoliehajte na vedľajšie účinky (kompilátor ich volanie môže vynechať)
* NRVO sa nemusí použiť v debug builde; to, že ho nevidíte počas debugovania, neznamená, že nebude v release kóde (závisí od kompilátora a nastavení)
* Preferujme kratšie funkcie (väčšia šanca, že NRVO vyjde)
* Ak sa dá, vracajme buď vždy priamo nový objekt (RVO), alebo vždy tú istú lokálnu premennú (NRVO)
* Nepreháňajme to, move je už aj tak dostatočne rýchly


## Side effects v copy/move

* Na vedľajšie účinky v copy/move konštruktoroch sa **nesmieme spoliehať**
* Štandard dáva kompilátoru výnimku: môže vynechať volanie copy/move konštruktora aj vtedy, ak má vedľajšie účinky (napr. výpis na konzolu alebo počítadlo kópií)
* Toto je jedno z mála miest v C++, kde môže optimalizácia zmeniť pozorovateľné správanie programu (ďalším je napr. spájanie alokácií pri volaní `new`)
* Najlepšie je navrhovať triedy tak, aby sme dodržali *Rule of zero* (nepísať vlastné copy/move/deštruktory)
* Nezabúdajme, že pri polymorfných bázových triedach potrebujeme `virtual` deštruktor


## `std::move` pri `return` pokazí NRVO

```cpp
std::string f(int i) {
    std::string result;
    std::getline(std::cin, result);
    result += "\n";

    return std::move(result); // DO NOT DO THIS!
}
```

* `return std::move(result);` na lokálnej premennej **zabráni NRVO**!
* NRVO vyžaduje, aby výraz v `return` bolo priamo meno lokálnej premennej (`return result;`), kým `std::move(result)` je volanie funkcie vracajúce `std::string&&`
* Pri obyčajnom `return result;` kompilátor najprv skúsi NRVO (nulová réžia) a ak sa nedá použiť, automaticky urobí *move* – písať `std::move` pri `return` lokálnej premennej je teda vždy len na škodu

---

## Čo je zle s nasledujúcim kódom?

```cpp
class Buffer {
public:
    Buffer(const Buffer& other);
    Buffer(Buffer&& other);
    /*...*/
};
 
int main() {
    std::vector<Buffer> v;

    for (int i = 0; i < 100; ++i) {
        v.push_back(Buffer(i));
    }
}
```

<div class="fragment">

* `push_back` môže realokovať pamäť
* Rovnako ak nastane výnimka počas `push_back`, mal by sa vector vrátiť do stavu ako pred volaním
* S move, ktorý môže vyhodiť výnimku, je toto nemožné
* Preto vector použije pri realokácii radšej kópiu (`std::move_if_noexcept`)
* Preto move operácie označujeme `noexcept`, inak ich kontajnery pri realokácii nemusia použiť
</div>

Notes: Ak vector nemá kópiu, ale iba noexcept(false) move, tak bohužiaľ push_back nevie toto úplne splniť a môže sa stať, že po zlyhanom push_back ostane vector v nešpecifikovanom stave.

---

# ĎAKUJEM

## Otázky?