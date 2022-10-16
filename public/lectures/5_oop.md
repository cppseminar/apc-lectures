# C++

## Objektovo orientované programovanie

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* OOP vo všeobecnosti
* Čo už vieme o triedach?
* `new` a `delete`
* Hlavné piliere OOP
    * Abstrakcia
    * Zapuzdrenie
    * Dedičnosť
    * Polymorfizmus
* `std::unique_ptr<T>`
* `dynamic_cast<T>`

---

# OOP

---

## Objektová paradigma

* Jedna z mnohých
    * Imperatívne
    * Funkcionálne
    * Štruktúrované
    * Logické
    * Deklarativne
* Väčšina moderných programovacích jazykov je viacpradigmová

---

## História

* Simula (*1967*)

![Simula logo](/lectures/5_oop/simula-logo.png)
<!-- .element: class="center" -->

* C++ je ním priamo ovplyvnené
* OOP sa stalo veľmi populárne
    * Java, C#, Object Pascal, Smalltalk, ...
* <small>V posledných rokoch popularita klesá</small>

---

## Objekty

* Objekty v programovacích jazykoch by ideálne mali reprezentovať jeden dobre definovaný koncept (napr. obdĺžnik)
* Objekty sú definované asociovanými dátami (napr. dva body v rovine)
* Metódy objektu slúžia na 
    * pozorovanie stavu (napr. obvod, obsah, ...)
    * menenie stavu (presun, otočenie, preklopenie, ...)


## C++ trieda

<table style="width: 80%">
<tr>
<td>

```cpp
class Rectangle {
public:
    Rectangle(Point a, Point b);
    void Rotate(double deg);
    void Move(double x, double y);
    double Perimeter() const;
private:
    Point a;
    Point b;
    double deg;
};
```
</td>
<td>

```cpp
struct Point {
    double x;
    double y;
};
```
</td>
</tr>
</table>

Struct a class sú v C++ v podstate to isté, iba jedno je `private` a druhé `public` by default. 

---

## Programovanie s objektami

<!-- .slide: data-background-image="/lectures/5_oop/puzzle.png" data-background-opacity="0.2" -->

* Všetko je objekt
* Objekty medzi sebou interagujú a tým tvoria program
* V podstate OOP je ako puzzle
    * Nadizajnujeme všetky časti (objekty)
    * Definujeme medzi nimi vzťahy a interakcie 
    * Program je hotový


## Realita

<!-- .slide: data-background-image="/lectures/5_oop/puzzle.png" data-background-opacity="0.2" -->

![Morpheus trashing OOP](/lectures/5_oop/morpheus.png)
<!-- .element: class="center" -->

* OOP nie je vždy odpoveď na všetky problémy
* Často sú lepšie riešenia

---

## Objekty v C++ nie sú nutné

* Zneužívanie objektov vedie k čudný triedam
    * Factory, Manager, Helper, …
* Obyčajné funkcie sú často lepšie 
* Objekty nie sú namespace!

![Rube goldberd](/lectures/5_oop/rube-goldberg.png)
<!-- .element: class="center" -->

---

# Opakovanie

---

## Konštruktory a deštruktory

* Objekty začínajú svoj život volaním konštruktora
* Skonštruované objekty sú automaticky deštruované keď idú von zo scope-u

```cpp
class Object {
public:
    Object() { }
    Object(int i) { /* ... */ }
    ~Object() { }
};
```

Konštruktorov môže byť viacero, deštruktor iba jeden. 

---

## Inicializácia členov

```cpp [|4-6|8-10]
class Strings {
public:
    Strings()
        : second(first + "2")
        , first("1")
        , third(second + "3")
    {  }
    std::string first;
    std::string second;
    std::string third;
};
```

<div class="fragment">

Inicializačný list iba definuje aké konštruktory sa majú zavolať a nie ich poradie.

Poradie v akom sa konštruuje je poradie v súbore.

First = `"1"` Second = `"12"` Third = `"123"`
</div>

---

## Členské premenné

* Dáta v objektoch
* Rovnako uložené v pamäti ako štruktúry v C
* Každý objekt má vlastné inštancie (nezdieľajú sa s triedou)

```cpp
class File {
private:
    std::string path;
    size_t size; /* i want it always*/
    bool temporary;
};
```

---

## Metódy objektov

* Definujú čo vieme s objektami robiť
* Z hľadiska C++ sú to len funkcie, ktoré implicitne dostanú this smerník

```cpp
class File {
public:
    size_t GetSize() const { return size; }
    void SetSize(size_t n) {
        std::filesystem::resize_file(path, n); /* ... */ 
    }
    void Move(std::string dest) {
        std::filesystem::rename(path, dest); /* ... */
    }
};
```

---

## Modifikátory prístupu

* Používaním kľúčových slov môžeme zamedziť nechcenému prístupu k objektom

| `public` | `protected` | `private` |
| -------- | ----------- | --------- |
| všetci | potomkovia triedy | iba trieda sama + `friend` |

```cpp
class File {
public:
    size_t GetSize();
    void SetSize(size_t n);
    void Move(std::string dest);
private:
    std::string path;
    size_t size;
    bool temporary;
};
```

---

# OOP != `new`

---

## Dynamická pamäť

* Je možné alokovať pamäť počas behu programu z haldy (heap)
* `malloc` a `free` zo `stdlib.h`
* Operátory `new` a `delete`

```cpp
int main() {
    int *i = new int(43);
    std::cout << i << " " << *i; // 0032D340 43
 
    // return; // bad idea
    delete i; // we must clean the memory manually
}
```

Smerník sám o sebe je v automatickej pamäti, teda sa zruší automaticky, ale hodnota, ktorá je v ňom už nie. 

---

## Alokácia polí

Špeciálne operátory `new[]` a `delete[]`

```cpp
int main() {
    std::string *s = new std::string[4];
    std::fill_n(s, 4, "Hello World");
 
    std::cout << s << " " << s[3].c_str(); // 0049D344 Hello World
 
    // delete s; // never
    delete[] s; // we must clean the memory manually
}
```

---

## Prečo `delete[]`?

* `operator[]` je potrebný na správne zdeštruovanie všetkých objektov vytvorených pomocou `new[]`
* MSVC ukladá počet alokácií pred vráteným blokom pamäte

![new of arrays](/lectures/5_oop/new[].png)
<!-- .element: class="center" -->

* `new[]` vráti `0x0052D344` v príklade vyššie
* `04 00 00 00` v predchádzajúcom čísle je vlastne veľkosť poľa

notes: Veľkosť pola je potrebná aby sme vedeli zavoľať všetky deštruktory. Inak by sme nevedeli koľko ich tam je. 

---

## Hlásenie chýb

* Ak program nevie alokovať viacej pamäte, potom alokácia zlyhá
* `malloc` vráti `NULL`
* `new` a `new[]` vyhodia výnimku (`std::bad_alloc`)

```cpp
auto* person = new Person();
if (person == nullptr /*or NULL*/) {
    // error handling
    exit(EXIT_FAILURE);
}
```

Test na `nullptr` je zbytočný, new nikdy nevracia `NULL`, ale vyhadzuje výnimku.
<!-- .element: class="fragment" -->

---

## `std::nothrow`

* Je možné prinútiť `new` aby nevyhadzoval výnimku ale vracal `nullptr`
* Potrebné ak sa používa `new` v kóde, ktorý nevie pracovať s výnimkami

```cpp
int* i = new (std::nothrow) int[0x7fffffff / 4];

std::cout << i << '\n'; // probably 0

delete i;
```

`nothrow` verzia nie je rýchlejšia, v skutočnosti na MSVC je pomalšia.
<!-- .element: class="fragment" -->

---

# Nové veci

---

## Init metódy

```cpp
class Object {
public:
    Object() { Init(); }
    Object(int i) {
        Init();
        // do more stuff
        str += std::to_string(i);
    }
private:
    void Init() {
        // initialize object
        str = "Object";
    }
    std::string str;
};
```

* Jedna `Init` metóda, ktorá vie inicializovať objekt.
* Volá sa zo všetkých konštruktorov
* Problémy
    * Nedajú sa volať konštruktory členov
    * `m_str` je default skonštruovaný a potom zmenený

---

## Delegované konštruktory

```cpp [|7]
class Object {
public:
  Object() { 
    // initialize object
    str = "Object";
  }
  Object(int i) : Object() {
    // do more stuff
    str += std::to_string(i);
  }
private:
  std::string str;
};

```

* V rámci konštruktora môžeme buď použiť inicializačný list, alebo zavolať iný konštruktor (nikdy nie oba súčasne)
* Pozor na nekonečnú rekurziu 

---

## Defaultný konštruktor

```cpp
struct A {
    A() { }
    int i;
};

struct B {
    B() = default;
    int i;
};

int main() {
    std::vector<A> va(100);
    auto cva = va; // will copy construct
    std::vector<B> vb(100);
    auto cvb = vb; // will memmove
}

```

* Použitím slova default zachová triviálnosť typu, teda niektoré operácie sa dajú lepšie optimalizovať

---

## Statické dátove členy

* Zdieľané všetkými inštanciami
* Môžu byť modifikátory prístupu 
* Iný názov pre **globálne premenné**
* Užitočné pre cache, zdieľaný mutex

```cpp
class A {
public:
    // one copy for each instances 
    std::string str;
    // one copy per all instances
    inline static std::string s_str;
};
```

Inline je celkom dôležité, inak musíme inštanciáciu presunúť mimo hlavičkového súboru, inak budeme mať chybu linkera.

---

## Statické metódy

* Statické metódy pracujú nad statickými členskými premennými
* V podstate sú to len bežné funkcie, ktoré majú prístup k privátnym častiam
* Užitočné ak nejaká funkcia naozaj patrí k objektom aj keď nepotrebuje `this` smerník

```cpp
class StringUtils {
public:
    static std::string Join(const std::vector<std::string>& v, char d);
    static std::vector<std::string> Split(const std::string& s, char d);
};

```

Namiesto tohto treba `namespace` aj vôbec. Statické funkcie nie sú veľmi rozšírené v C++.


## Pristupovanie k statickým metódam

```cpp
int main(int argc, char* argv[]) {
    std::vector<std::string> strings;

    StringUtils().Join(strings, '.'); // bad

    StringUtils a; //even worse
    a.Join(strings, '.');

    StringUtils::Join(strings, '.'); // OK
}
```

Rovnako sa pristupuje aj k verejným statickým členským premenným.

---

## `namespace`

* Na zoskupenie funkcií nepoužívame triedy, ale `namespace`
* Hlavne na riešenie konfliktov, nie hierarchie

<table style="width: 80%">
<tr>
<td>

```cpp
namespace GoodProject {
    void Fun() { };
}

namespace BetterProject {
    void Fun() { };
}
```
</td>
<td>

```cpp
int main() {
    //Fun(); // error
    GoodProject::Fun();
    BetterProject::Fun();
}

```
</td>
</tr>


## Globálny namespace

* Existuje jeden globálny `namespace` `::`

```cpp
::std::string s; // absolute path
```

* Pomocou `using namespace` môžeme vytiahnuť symboly do aktuálneho namespace

```cpp
using namespace BetterProject;

int main() {
    Fun(); // BetterProject::Fun
    GoodProject::Fun();
    BetterProject::Fun();
    ::GoodProject::Fun();
}
```

---

## Konštanty v triedach

```cpp
class Cache {
    static const size_t DEFAULT_CACHE_SIZE = 100;
    
    Cache(size_t size = DEFAULT_CACHE_SIZE) { };
};
```

* Pekné je, ak sú nejak oddelené od ostatných premenných (`ALL_CAPS`, alebo `kPascalCase`, ...)
* Používame iba ak konštanty naozaj patria do triedy
* Môžu nebyť `static`, potom ich je nutné inicializovať v konštruktore

---

# Piliere OOP

---

