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
* `std::shared_ptr`

---

# Storage class specifiers

---

## `auto`

* Kľúčové slovo `auto` sa už v C++ nepoužíva na označenie *automatic storage duration*
* Bez označenia premennej pomocou storage specifier-u sa použije *auto* (teda automatická pamäť)
* Miesto pre premennú je alokované na začiatku scope a dealokované na konci
* Konštruktory a deštruktory sa volajú *auto*maticky
* Miesto sa spravidla alokuje na stacku

---

## `register`

* Kľúčové slovo `register` sa používa na označenie premennej, ktorá sa bude veľa používať, a teda kompilátor by ju mal dať prednostne do CPU registrov
* V C sa nedá takejto premennej zobrať adresa, v C++ to ide
* Kompilátory ignorujú
* Od C++17 je to reserved keyword, bez významu

```cpp
register int n = 0;
for (register int i = 0; i < 1'000'000; ++i) {
  n += i;
}
```

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

* Dynamická pamäť znamená, že miesto si musíme alokovať ručne
* V C++ sa robí pomocou `new` a `delete`
* My ideálne nepoužívame priamo alokácie pamäte, ale
    * `std::unique_ptr`
    * `std::vector`
    * `std::string`

```cpp
int* i = new int(0);
// no exception can occur here
delete i;
```

---

# Value categories

---

## Globálne premenné

* Sú inicializované ešte pred volaním funkcie `main` a zdeštruované po vrátení z funkcie `main`
* V jednom translation unit je poradie inicializácie rovnaké, ako sú napísané v súbore, a deštrukcia potom v opačnom poradí


## Nedefinované poradie

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

* Všetky globálne premenné sú inicializované pred volaním `main`. Tu jedna závisí od druhej v inom súbore (*translation unit*) a to je nedefinované správanie.


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
//extern int static_global_variable; // error
static int static_global_variable = 43; // different variable
```

```cpp
// file1.cpp
int global_variable = 67;

// file2.cpp
static int global_variable = 43; // different variable
```


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

* V podstate všetko, čo môže byť na pravej strane priradenia (*r* v názve je *right*)
* Väčšinou premenné bez mena alebo *temporaries*

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
char *c;
c = "String literal";

char *c;
"String literal" = c;

```
</td>
</tr>
</table>

---

## Dočasné premenné

* *Temporaries*
* Všetky premenné na predchádzajúcich slidoch mali mená
* Pomenované premenné zostávajú žiť až po koniec scope-u, v ktorom boli skonštruované
* Nepomenované premenné (dočasné) sú zdeštruované hneď po tom, čo boli použité (na tom istom riadku)

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

<small>V starších MSVC (kompilátor vo Visual Studio) toto išlo skompilovať. Od VS2019 to už nejde.</small>

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
    func_value(std::string{}); // OK, copy
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
* Niektorí programátori preferujú smerník ako výstupnú hodnotu aby to bolo explicitné

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
* Aktuálne na to je najlepšie použiť smerník

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
* Po pridaní `const` pred `std::optional` to už skompilovať ide, ale robí sa kópia, keďže typ `std::string` a `std::optional<std::string>` nie sú rovnaké


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

* Nový string sa vytvorí pred volaním funkcie `func` a hneď potom sa zavolá deštruktor


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

---

# Implementácia rvalue referencií

---

## Príklad buffer

```cpp
class buffer {
public:
  buffer(size_t n) : size(n) { ptr = malloc(size); }
 
  ~buffer() { free(ptr); }
 
  void *get() const { return ptr; }
 
private:
  void *ptr = nullptr;
  size_t size;
};
```

* Jednoduchá trieda na reprezentáciu bufferu
* Nevie meniť veľkosť, iba alokovať a dealokovať


## copy

* Kopírovací konštruktor a operátor priradenia by mali skopírovať objekt

### Kopírovací konštruktor

```cpp
buffer(const buffer &other)
    : size(other.size)
    , ptr(malloc(size)) {
    if (!ptr) {
        // malloc failed
        throw std::bad_alloc();
    }
    // copy buffer
    memcpy(ptr, other.ptr, size);
}
``` 


### Kopírovací operátor priradenia

```cpp
buffer& operator=(const buffer& rhs) {
    // self-assignment check
    if (this == &rhs)
        return *this;

    void* new_ptr = nullptr;

    if (rhs.size > 0) {
        new_ptr = malloc(rhs.size);
        if (!new_ptr) {
            // malloc failed
            throw std::bad_alloc();
        }
        memcpy(new_ptr, rhs.ptr, rhs.size);
    }

    // Free old data only after new allocation succeeds
    free(ptr);

    ptr = new_ptr;
    size = rhs.size;

    return *this;
}
```

---

## `move`

* Move konštruktor a operátor priradenia by mali použiť zo "starého" objektu najviac, ako sa dá

### Move konštruktor

```cpp
buffer(buffer&& other)
    : size(other.size) 
    , ptr(other.ptr) {
}
```

### Move operátor priradenia

```cpp
buffer& operator=(buffer&& rhs) {
    if (&rhs == this)
        return *this;

    size = rhs.size;
    ptr = rhs.ptr;
    return *this;
}
```


## Move musí nechať objekt vo validnom stave

* Deštruktor sa stále zavolá, takže nemôžeme nechať objekt v nevalidnom stave
* Teoreticky stačí, aby sme nechali objekt v stave, aby sa dal zdeštruovať, niektorí ale preferujú objekt naozaj vymazať

```cpp
buffer(buffer&& other)
    : size(other.size) 
    , ptr(other.ptr) {
    other.ptr = nullptr;
}
```

```cpp
buffer(buffer&& other)
    : size(other.size) 
    , ptr(other.ptr) {
    other.size = 0;
    other.ptr = nullptr;
}
```


## move operator=

```cpp
buffer &operator=(buffer &&rhs) {
    if (&rhs == this)
        return *this;

    free(ptr);
    size = rhs.size;
    rhs.size = 0;
    ptr = rhs.ptr;
    rhs.ptr = nullptr;
    return *this;
}

```


## `std::exchange`

* Môžeme použiť `std::exchange` na zjednodušenie implementácie
* `c = std::exchange(a, b)` urobí `c = a` a `a = b`
* Nie atomic exchange

```cpp
buffer &operator=(buffer &&rhs) {
    if (&rhs == this)
        return *this;

    free(ptr);

    size = std::exchange(rhs.size, 0);
    ptr = std::exchange(rhs.ptr, nullptr);
    return *this;
}

```

---

## Ako použiť move?

* Temporaries si vyberú rvalue referenciu, keďže sú rvalue
* Ak náhodou rvalue referencia neexistuje, tak sa uspokoja aj s konštantnou lvalue referenciou
* Temporaries sú hneď zdeštruované, tak ich môžeme rozobrať a použiť ďalej

```cpp
int main() {
    buffer a(500);
    buffer b(40);

    b = a;          // copy
    b = buffer(60); // move
}
```

---

## Návratová hodnota z funkcie

```cpp
buffer function() {
    buffer a(500);

    memset(a.get(), '0', 500);

    return a;
}
 
int main() { 
    buffer b = function(); 
}
```

* Ak sa z funkcie vracia lokálna premenná rovnakého typu, ako je návratový typ, tak sa urobí automatický *move*
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


## `std::tie`

* Používa sa na automatické naviazanie premenných z n-tice
* Urobí vlastne n-ticu lvalue referencií
* Všetko sa move-ne, ak sa dá

```cpp
std::tuple<int, int, std::string> more() { 
    return {1, 2, "string"}; 
}

int main() {
    int i, j;
    std::string s;

    std::tie(i, j, s) = more();
}
```


## `std::ignore`

* Ak nás niektorá hodnota z n-tice nezaujíma, môžeme použiť `std::ignore`
* Niekedy sa používa aj na potlačenie upozornení, ktoré vyskočia z atribútu `[[nodiscard]]`, ale to nie je úplne špecifikované

```cpp
std::string s;

std::tie(std::ignore, std::ignore, s) = more();
```

```cpp
[[nodiscard]] bool important();

std::ignore = important();
```

---

## Structured bindings

* Pridané v C++17

```cpp
#include <tuple>

std::tuple<int, std::string, float> f() {
    return { 10, std::string("String"), 0.1f };
}

int main() {
    auto [a, b, c] = f();

    std::println("{}", a); // print 10, a is int and copied
    std::println("{}", b); // print String, b is std::string and moved
    std::println("{}", c); // print 0.1, c is float and copied
}
```

note: Technicky sú tie typy s &&, teda int&&, std::string&&, float&&.


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
    buffer a(100);
    buffer b(20);

    b = static_cast<buffer&&>(a);
    b = std::move(a);
}
```

* Ak zavoláme explicitne move a žiadna funkcia s rvalue referenciou neexistuje, tak sa zavolá preťaženie s konštantnou referenciou
* <http://en.cppreference.com/w/cpp/language/overload_resolution>


## `std::unique_ptr`

* `std::move` sa hodí na použitie s `std::unique_ptr`
* Pomocou *move* sa odstránia dangling pointre, ktoré by mohli vzniknúť, ak použijeme copy

```cpp
std::unique_ptr<std::string> ptr;

ptr = std::make_unique<std::string>("HI."); // new string is moved

// auto p = ptr; // will not compile, unique_ptr has no copy
auto p = std::move(ptr); // move, so ptr is empty
```

---

## Implementácia move konštruktora

```cpp
struct S {
    std::string s;
    int i;
    void* p;

    S(S&& other) {
        s = std::move(other.s);
        i = other.i;
        p = std::exchange(other.p, nullptr);
    }
};
```

* So stringom musíme použiť `std::move`, s ostatnými je to jedno (move a copy `int`u sú rovnaké)
* V samotnej funkcii `S(S&& other)` je `other` premenná s menom, a preto sa k nej správame ako k lvalue
* Aby sme zachovali `move`, musíme zavolať `std::move`

---

## Automatické generovanie

* Kompilátor vygeneruje *move konštruktor* a *move operátor priradenia*, iba ak je to bezpečné
    * Ak neexistuje `user defined` kopírovací konštruktor alebo operátor priradenia
    * Ak neexistuje `user defined` deštruktor
* Ak chceme vynútiť generovanie, použijeme `= default`

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


## Ako je to s kopírovacími operáciami?

* Ako sme hovorili, kompilátor vygeneruje kopírovací konštruktor a operátor priradenia vždy automaticky
* Existuje ale výnimka
   * Ak je definovaný `user defined` move konštruktor alebo operátor priradenia, tak sa kopírovacie operácie nevygenerujú
   * Taký fix štandardu, kde sa snažili zabrániť automatickému kopírovaniu, ak je definovaný *move*


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
* Problém je, že funkcie môžu mať side effecty, a preto nemôžu volanie kopírovacieho konštruktora len tak vypustiť
* Alebo môžu? Čo ak zakážeme copy konštruktoru side effects<!-- .element: class="fragment" -->
* Potom by sme mohli objekt priamo skonštruovať tam, kde má byť, a kópiu vôbec nevolať<!-- .element: class="fragment" -->


## Return value optimization (RVO)

* Kopírovací a move konštruktor podľa štandardu nemôžu mať side effects
* Objekt sa vytvorí priamo v stacku volajúcej funkcie
* Aplikuje sa, ak vraciame z funkcie vždy nový objekt
* Povinné v C++17 (pod menom URVO - unnamed RVO)

```cpp
std::string f(int i) {
    if (i < 10) {
        return "Less than 10";
    } else {
        return "Greater or eq 10";
    }
}
```


## Príklad na zlé použitie

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

* V každom bode, kde sa vracia z funkcie, vracajme tú istú premennú
* Ideálne definujeme jednu návratovú premennú a vraciame len tú 

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
* Copy alebo move nesmú mať side effecty
* Nepoužíva sa v debug builde; to, že to nevidíte počas debugovania, neznamená, že to nebude v release kóde (závisí od kompilátora a verzie štandardu)
* Preferujme kratšie funkcie (väčšia šanca, že nám to vyjde)
* Ak sa dá, vracajme buď vždy nový objekt, alebo vždy tú istú lokálnu premennú
* Nepreháňajme to, move je už aj tak dostatočne rýchly


## Side effects v copy/move

* Side effecty v copy/move sú zakázané
* Resp. nie zakázané, ale kompilátor môže predpokladať, že tam nie sú, a optimalizovať podľa toho (RVO/NRVO)
* Toto je jedno z dvoch miest, kde je povolené vynechať side effects (druhé sú niektoré optimalizácie pri volaní `new`)
* Najlepšie je robiť triedy tak, aby sme dodržali takzvanú "Rule of zero" stratégiu (nepísať vlastné copy/move/deštruktory)
* Nezabúdať, že niekedy potrebujeme `virtual` deštruktor (toto ale nebýva problém)


## `std::move` môže pokaziť NRVO

```cpp
std::string f(int i) {
    std::string result;
    std::getline(std::cin, result);
    result += "\n";

    return std::move(result);
}
```

* `std::move` v tomto prípade pokazí NRVO
* Štandard pri *copy elision* vyžaduje, aby boli typy úplne rovnaké, čo tu nie sú
    * Návratová hodnota je `std::string`
    * Výsledok `std::move` je `std::string&&`

---

## Čo je zle s nasledujúcim kódom?

```cpp
class buffer {
public:
    buffer(buffer &&other);
    /*...*/
};
 
int main() {
    std::vector<buffer> v;

    for (int i = 0; i < 100; ++i) {
        v.push_back(buffer(i));
    }
}
```

<div class="fragment">

* `push_back` môže realokovať pamäť
* Rovnako ak nastane výnimka počas `push_back`, mal by sa vector vrátiť do stavu ako pred volaním
* S move, ktorý môže vyhodiť výnimku, je toto nemožné
* Preto vector použije v tomto prípade radšej kópiu
* Preto move operácie musia byť `noexcept`, inak nemajú až taký zmysel
</div>

Notes: Ak vector nemá kópiu, ale iba noexcept(false) move, tak bohužiaľ push_back nevie toto úplne splniť a môže sa stať, že po zlyhanom push_back ostane vector v nešpecifikovanom stave.

---

# `std::shared_ptr`

---

## Čo je `std::shared_ptr`?

* Smart pointer s automatickým počítaním referencií (*reference counting*)
* Viacero `shared_ptr` môže vlastniť ten istý objekt súčasne
* Objekt sa zmaže automaticky, keď posledný `shared_ptr` zanikne
* Thread-safe počítanie referencií (ale nie thread-safe prístup k objektu)
* Definovaný v `<memory>`

```cpp
#include <memory>

std::shared_ptr<int> ptr(new int(10));

std::shared_ptr<int> ptr1 = std::make_shared<int>(42);
std::shared_ptr<int> ptr2 = ptr1; // copy, reference count = 2

std::println("{}", ptr1.use_count()); // prints 2
std::println("{}", *ptr1);            // prints 42

// ptr1 and ptr2 are destroyed -> object is automatically deleted
```

---

## Ako funguje reference counting?

* Každý `shared_ptr` si drží pointer na objekt + pointer na *control block*
* Control block obsahuje najmä:
  * Počet `shared_ptr` (strong references)
  * Počet `weak_ptr` (weak references) 

```cpp
std::shared_ptr<std::string> ptr1 = std::make_shared<std::string>("Hello");
// use_count() == 1

{
    std::shared_ptr<std::string> ptr2 = ptr1; // copy constructor
    // use_count() == 2
    
    std::shared_ptr<std::string> ptr3;
    ptr3 = ptr1; // copy assignment operator  
    // use_count() == 3
} // ptr2 and ptr3 are destroyed, use_count() == 1

// ptr1 is destroyed, use_count() == 0 -> object is deleted
```

---

## Vytváranie `shared_ptr`

### `std::make_shared` (preferované)

```cpp
auto ptr = std::make_shared<std::string>("Hello World");
auto ptr2 = std::make_shared<std::vector<int>>(10); // 10 elements with value 0
```

### Konštruktor

```cpp
std::shared_ptr<std::string> ptr(new std::string("Hello"));
```

### Prečo preferovať `make_shared`?

* **Výkon**: Jedna alokácia namiesto dvoch (objekt + control block)
* **Exception safety**: Atomické vytvorenie objektu a control blocku
* **Pamäťová efektivita**: Lepšia lokalita pamäte
* Nevýhoda: Tým, že objekt a control block sú v jednej alokácii, môže pamäť ostať dlhšie alokovaná, ak existujú `weak_ptr` na ten istý objekt

---

## Bezpečnosť výnimiek

### Problém s konštruktorom

```cpp
void dangerous_function(std::shared_ptr<A> a, std::shared_ptr<B> b);

// possible memory leak in C++14, fixed in C++17
dangerous_function(
    std::shared_ptr<A>(new A()),  // may throw exception
    std::shared_ptr<B>(new B())   // if this throws, A might not be deleted
);
```

### Bezpečné riešenie

```cpp
dangerous_function(
    std::make_shared<A>(),
    std::make_shared<B>()
);
```

---

## Základné operácie

```cpp
std::shared_ptr<std::string> ptr = std::make_shared<std::string>("Test");

// Access to the object
std::println("{}", *ptr);           // dereferencing
std::println("{}", ptr->length());  // member access

// State information
std::println("{}", ptr.use_count());              // reference count
std::println("{}", static_cast<void*>(ptr.get())); // raw pointer (be careful!)
if (ptr) { /* not nullptr */ }

// Reset and swap
ptr.reset();                       // releases reference, may delete object
ptr.reset(new std::string("New")); // releases old, sets new

std::shared_ptr<std::string> other;
ptr.swap(other);                   // swaps contents
```

---

## Move sémantika s `shared_ptr`

```cpp
std::shared_ptr<std::string> create_string() {
    return std::make_shared<std::string>("Hello");
}

int main() {
    // RVO/move - no control block copying
    auto ptr1 = create_string();
    
    // Move - control block is moved, no increment/decrement
    auto ptr2 = std::move(ptr1); // ptr1 is now nullptr
    
    // Copy - reference counter is incremented  
    auto ptr3 = ptr2;  // use_count() == 2
}
```

* Move je rýchlejší ako copy (žiadne atomické operácie s počítadlom)
* Používajte move, keď už `shared_ptr` nepotrebujete

---

## `std::enable_shared_from_this`

### Ako získať `shared_ptr` zvnútra objektu

```cpp
class Widget {
public:
    void do_something() {
        // how to get shared_ptr<Widget> pointing to *this?
    
        std::shared_ptr<Widget> bad_ptr(this); // wrong, double delete!
        
        process(bad_ptr);
    }
    
private:
    void process(std::shared_ptr<Widget> widget);
};

int main() {
    auto widget = std::make_shared<Widget>();
    widget->do_something();
}
```


### Použitím `std::enable_shared_from_this`

```cpp
class Widget : public std::enable_shared_from_this<Widget> {
public:
    void do_something() {
        // Correct - uses existing control block
        auto self = shared_from_this();
        process(self);
    }
    
    // Factory pattern
    static std::shared_ptr<Widget> create() {
        return std::make_shared<Widget>();
    }
    
private:
    void process(std::shared_ptr<Widget> widget);
    
    // If constructor is private, force usage of factory
    Widget() = default;  
};

int main() {
    auto widget = Widget::create();
    widget->do_something(); // OK
}
```

---

## Obmedzenia

### `shared_from_this()` vyžaduje existenciu `shared_ptr`

* Inak skončí výnimkou `std::bad_weak_ptr` a nepodarí sa získať `shared_ptr`

```cpp
class Widget : public std::enable_shared_from_this<Widget> {
public:
    void problematic() {
        auto self = shared_from_this(); // EXCEPTION if no shared_ptr exists
    }
};

Widget w;           // object on stack
w.problematic();    // std::bad_weak_ptr exception

Widget* w2 = new Widget(); // object on heap
w2->problematic();  // std::bad_weak_ptr exception
delete w2;

auto w3 = std::make_shared<Widget>();
w3->problematic();  // OK
```


### Nemožno volať v konštruktore

```cpp
class Widget : public std::enable_shared_from_this<Widget> {
public:
    Widget() {
        // ERROR - shared_ptr does not exist yet!
        auto self = shared_from_this(); // std::bad_weak_ptr
    }
};
```


### Implementácia `std::enable_shared_from_this`

* Keď sa na toto pozriete, vyzerá to trochu čudne, lebo parent musí už byť skonštruovaný (teda `std::enable_shared_from_this` časť), ale ako môže byť skonštruovaný, keď ešte neexistuje `std::shared_ptr`, ktorý vlastní tento objekt?
* V skutočnosti je to riešené tak, že funkcionalitu `enable_shared_from_this` inicializuje konštruktor `std::shared_ptr`, keď je objekt prvýkrát zabalený do `std::shared_ptr`
* Preto je dôležité, aby `std::shared_ptr` bol vytvorený cez `std::make_shared` alebo priamo cez konštruktor `std::shared_ptr`, a nie aby sa objekt vytvoril samostatne a potom sa naň vytvoril `std::shared_ptr`.

---

## Asymetrický ownership

* Ak potrebujeme smerník child -> parent a zároveň parent -> child
* Pri využití `shared_ptr` vzniká cyklus vlastníctva (loop)
* Objekty sa nikdy nezničia, pretože každý vlastní druhého

```cpp
class Child;

struct Parent {
    std::vector<std::shared_ptr<Child>> children;
    ~Parent() { std::println("Parent destroyed"); }
};

struct Child {
    std::shared_ptr<Parent> parent; // loop in ownership
};

auto parent = std::make_shared<Parent>();
auto child = std::make_shared<Child>();
parent->children.push_back(child);
child->parent = parent; // now this will never be deleted
```


### Riešenie s `std::weak_ptr`

```cpp
class Child;

struct Parent {
    std::vector<std::shared_ptr<Child>> children;
    ~Parent() { std::println("Parent destroyed"); }
};

struct Child {
    std::weak_ptr<Parent> parent; // weak reference is fine
};

auto parent = std::make_shared<Parent>();
auto child = std::make_shared<Child>();
parent->children.push_back(child);
child->parent = parent; // everything is fine

std::shared_ptr<Parent> p = child->parent.lock(); // get shared from weak
```

---

## `weak_ptr` operácie

```cpp
std::shared_ptr<int> shared = std::make_shared<int>(42);
std::weak_ptr<int> weak = shared;

// test if object still exists
if (!weak.expired()) {
    std::println("Object still exists");
}

// Safe access
if (auto locked = weak.lock()) {
    std::println("{}", *locked);  
} // locked is destroyed here

shared.reset(); // object is destroyed
std::println("{}", weak.expired()); // true
auto locked2 = weak.lock();         // returns nullptr
```

---

## Performance

* `std::unique_ptr` je v podstate taký rýchly ako raw pointer
* `std::shared_ptr` je pomalší kvôli atomickým operáciám na počítadle referencií pri kopírovaní a ničení, dá sa zlepšiť použitím `std::move`
* `std::shared_ptr` používa viac pamäte (objekt plus control block) a niekedy môže spôsobiť horšiu lokalitu pamäte (ak control block a objekt nie sú v jednej alokácii)

---

# ĎAKUJEM

## Otázky?