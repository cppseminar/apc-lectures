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
    * Imperatívna
    * Funkcionálna
    * Štruktúrovaná
    * Logická
    * Deklaratívna
* Väčšina moderných programovacích jazykov je viacparadigmová

---

## História

* Simula (*1967*)

![Simula logo](./lectures/5_oop/simula-logo.png)

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

---

## Programovanie s objektmi

<!-- .slide: data-background-image="./lectures/5_oop/puzzle.png" data-background-opacity="0.2" -->

* Všetko je objekt
* Objekty medzi sebou interagujú a tým tvoria program
* V podstate OOP je ako puzzle
    * Nadizajnujeme všetky časti (objekty)
    * Definujeme medzi nimi vzťahy a interakcie 
    * Program je hotový


## Realita

<!-- .slide: data-background-image="./lectures/5_oop/puzzle.png" data-background-opacity="0.2" -->

![Morpheus trashing OOP](./lectures/5_oop/morpheus.png)

* OOP nie je vždy odpoveď na všetky problémy
* Často sú lepšie riešenia

---

## Objekty v C++ nie sú nutné

* Zneužívanie objektov vedie k čudným triedam
    * Factory, Manager, Helper, …
* Obyčajné funkcie sú často lepšie 
* Objekty nie sú namespace!

![Rube Goldberg](./lectures/5_oop/rube-goldberg.png)

---

# Opakovanie

---

## Konštruktory, deštruktory a inicializácia

* Objekty začínajú svoj život volaním konštruktora a sú automaticky deštruované pri opustení scope-u
* Trieda môže mať viacero konštruktorov, ale iba jeden deštruktor
* Inicializačný list určuje, aké konštruktory členov sa zavolajú, ale **poradie inicializácie je vždy dané poradím deklarácie v triede** (deštrukcia prebieha v opačnom poradí)

```cpp [|3-6|8-10]
class Strings {
public:
  Strings()
    : second(first + "2")
    , first("1")
    , third(second + "3")
  { }
  std::string first;  // "1"
  std::string second; // "12"
  std::string third;  // "123"
};
```


## Členské premenné a metódy

* Každý objekt má vlastné inštancie členských premenných
* Metódy definujú, čo vieme s objektmi robiť – z hľadiska C++ sú to funkcie s implicitným parametrom `this` (`const` metódy nemôžu meniť stav objektu)

```cpp
class File {
public:
  size_t GetSize() const { return size; }
  void SetSize(size_t n) {
    std::filesystem::resize_file(path, n); /* ... */ 
  }
  void Move(std::string dest) {
    std::filesystem::rename(path, dest); /* ... */
  }
private:
  std::string path;
  size_t size = 0;
  bool temporary = false;
};
```


## Modifikátory prístupu

* Používaním kľúčových slov môžeme zamedziť nechcenému prístupu k vnútornostiam triedy
* `struct` a `class` sa líšia iba predvolenou viditeľnosťou (`public` vs. `private`)

| `public` | `protected` | `private` |
| -------- | ----------- | --------- |
| všetci | trieda sama, potomkovia + `friend` | iba trieda sama + `friend` |

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
  std::println("{} {}", static_cast<void*>(i), *i); // 0x0032d340 43

  // return; // bad idea
  delete i; // we must clean the memory manually
}
```

Smerník sám osebe je v automatickej pamäti, teda sa zruší automaticky, ale hodnota, ktorá je v ňom, už nie. 

---

## Alokácia polí

Špeciálne operátory `new[]` a `delete[]`

```cpp
int main() {
  std::string *s = new std::string[4];
  std::fill_n(s, 4, "Hello World");

  std::println("{} {}", static_cast<void*>(s), s[3]); // 0x0049d344 Hello World

  // delete s; // never
  delete[] s; // we must clean the memory manually
}
```


## Prečo `delete[]`?

* `operator delete[]` je potrebný na správne zdeštruovanie všetkých objektov vytvorených pomocou `new[]`
* MSVC ukladá počet alokácií pred vráteným blokom pamäte

![new of arrays](./lectures/5_oop/new[].png)

* `new[]` vráti `0x0052D344` v príklade vyššie
* `04 00 00 00` v predchádzajúcom čísle je vlastne veľkosť poľa

notes: Veľkosť poľa je potrebná, aby sme vedeli zavolať všetky deštruktory. Inak by sme nevedeli, koľko ich tam je. 

---

## Hlásenie chýb

* Ak program nevie alokovať viac pamäte, potom alokácia zlyhá
* `malloc` vráti `NULL`
* `new` a `new[]` vyhodia výnimku (`std::bad_alloc`)

```cpp
auto* person = new Person();
if (person == nullptr /*or NULL*/) {
  // error handling
  exit(EXIT_FAILURE);
}
```

Test na `nullptr` je zbytočný, `new` nikdy nevracia `NULL`, ale vyhadzuje výnimku.
<!-- .element: class="fragment" -->


## `std::nothrow`

* Je možné prinútiť `new`, aby nevyhadzoval výnimku, ale vracal `nullptr`
* Potrebné, ak sa používa `new` v kóde, ktorý nevie pracovať s výnimkami

```cpp
int* i = new (std::nothrow) int[0x7fffffff / 4];

std::println("{}", static_cast<void*>(i)); // probably 0x0

delete[] i;
```

`nothrow` verzia nie je rýchlejšia, v skutočnosti na MSVC je pomalšia.
<!-- .element: class="fragment" -->

---

# Nové veci

---

## Delegované konštruktory

```cpp [|6]
class Object {
public:
  Object()
    : str("Object") { 
  }
  Object(int i) : Object() {
    // do more stuff
    str += std::to_string(i);
  }
private:
  std::string str;
};

```

* Nahrádzajú pomocné súkromné `Init()` metódy volané z viacerých konštruktorov
* V rámci konštruktora môžeme buď použiť inicializačný list členov, alebo zavolať iný konštruktor tej istej triedy (nikdy nie oba súčasne)
* Pozor na nekonečnú rekurziu 

---

## Defaultný konštruktor

```cpp
struct A {
  A() { } // default constructor

  A(int i) : i(i) { }
  int i;
};

struct B {
  B(int i) : i(i) { }
  int i;
};

int main() {
  A a;
  // B b; // will not compile, no default constructor available
}
```

* Ak v triede zadefinujeme akýkoľvek vlastný konštruktor, kompilátor už predvolený (bezparametrický) konštruktor automaticky nevygeneruje


## Radšej `= default`

```cpp
struct A {
  A() { }
  int i = 0;
};

struct B {
  B() = default;
  B(int i) : i(i) { }
  int i = 0;
};

int main() {
  A a;
  B b;
}
```

* Pomocou `= default` explicitne požiadame kompilátor, aby vygeneroval predvolený konštruktor
* Je to čitateľnejšie ako prázdne `{}` a pre jednoduché štruktúry to navyše zachováva *triviálnosť* typu (kompilátor môže niektoré operácie lepšie optimalizovať)


## Zakázanie volania cez `= delete`

```cpp
class DatabaseConnection {
public:
  explicit DatabaseConnection(int port) { }
  DatabaseConnection(double) = delete;
};

int main() {
  DatabaseConnection db1(5432);   // OK
  // DatabaseConnection db2(3.14); // compiler error: deleted function
}
```

* Kľúčové slovo `= delete` (od C++11) zakáže volanie daného konštruktora alebo funkcie
* Na rozdiel od starého triku s `private` metódou bez tela dostaneme jasnú chybu kompilácie na mieste volania
* Často sa používa aj na zakázanie kopírovania objektov (uvidíme neskôr)

---

## Statické dátové členy

* Zdieľané všetkými inštanciami
* Môžu mať modifikátory prístupu 
* Iný názov pre **globálne premenné**
* Užitočné pre cache, zdieľaný mutex

```cpp
class A {
public:
  // one copy for each instance
  std::string str;
  // one copy per all instances
  inline static std::string s_str;
};
```

`inline` je celkom dôležité, inak musíme inštanciáciu presunúť mimo hlavičkového súboru, inak budeme mať chybu linkera.

---

## Statické metódy

* Statické metódy nie sú viazané na konkrétnu inštanciu a priamo vidia iba statické členy triedy
* V podstate sú to len bežné funkcie, ktoré majú prístup k privátnym častiam triedy
* Užitočné, ak nejaká funkcia naozaj patrí k triede, aj keď nepotrebuje `this` smerník

```cpp
class StringUtils {
public:
  static std::string Join(const std::vector<std::string>& v, char d);
  static std::vector<std::string> Split(const std::string& s, char d);
};

```

Namiesto tohto treba použiť `namespace`, ak vôbec. Statické funkcie nie sú v C++ veľmi rozšírené.


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
namespace GoodProject {
  void Fun() { };
}

namespace BetterProject {
  void Fun() { };
}
```
</td>
<td>

```cpp
int main() {
  //Fun(); // error
  GoodProject::Fun();
  BetterProject::Fun();
}

```
</td>
</tr>


## Globálny namespace

* Existuje jeden globálny `namespace` `::`

```cpp
::std::string s; // absolute path
```

* Pomocou `using namespace` môžeme vytiahnuť symboly do aktuálneho namespace

```cpp
using namespace BetterProject;

int main() {
  Fun(); // BetterProject::Fun
  GoodProject::Fun();
  BetterProject::Fun();
  ::GoodProject::Fun();
}
```

---

## Konštanty v triedach

```cpp
class Cache {
  static constexpr size_t DEFAULT_CACHE_SIZE = 100;
  
  explicit Cache(size_t size = DEFAULT_CACHE_SIZE) { }
};
```

* Pekné je, ak sú nejako oddelené od ostatných premenných (`ALL_CAPS`, alebo `kPascalCase`, ...)
* Používame iba vtedy, ak konštanty naozaj patria do triedy
* Pre konštanty známe počas prekladu používame `static constexpr`

---

# Piliere OOP

---

![Piliere OOP - abstraction, encapsulation, inheritance, polymorphism](./lectures/5_oop/pillars-of-oop.png)

---

## Zapuzdrenosť (*encapsulation*)

* Triedy majú definovať interface a všetko ostatné je implementačný detail neviditeľný pre používateľa
* Takto sa implementácia môže voľne meniť, ak interface ostane nedotknutý (alebo aspoň spätne kompatibilný)
* Hlavnou úlohou zapuzdrenia je udržiavanie **invariantov** (konštruktor ich nastaví alebo pri neplatných dátach vyhodí výnimku, metódy ich zachovávajú)

Verejné členské premenné. <!-- .element: class="fragment strike" -->


## Výhody zapuzdrenia

* Refactor je oveľa jednoduchší a dá sa robiť postupne 
* Automatické testy testujú iba interface a nemusia používať implementačné detaily
* Lepšie definované invarianty
* Prenositeľný kód
* Vo všeobecnosti uľahčuje život

---

## Dedičnosť

* Triedy môžu tvoriť hierarchie, kde nová trieda je založená na starej a rozširuje ju pridaním funkcionality
* C++ dokonca podporuje viacnásobnú dedičnosť (nateraz ignorujeme)

![Sequences inheritance diagram](./lectures/5_oop/sequence.png)

---

## Vytváranie odvodených tried

```cpp
struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

class RGBA : public RGB {
public:
  RGBA(uint32_t color)
  {
    r = ((color & 0xff000000) >> 24);
    g = ((color & 0x00ff0000) >> 16);
    b = ((color & 0x0000ff00) >> 8);
    alpha = (color & 0x000000ff) / static_cast<double>(0xff);
  }
  double alpha;
};
```
<!-- .element: class="showall" -->

* Znak `":"` sa používa na odvodenie triedy od rodičovskej triedy (*base class*)
* Kľúčové slovo `public` znamená, že rodič je viditeľný pre používateľov potomka (v podstate sa vždy používa `public`)
* V potomkovi máme prístup ku všetkým `public` a `protected` častiam rodiča, a preto ich môžeme v konštruktore použiť

---

## Volanie konštruktora predka

```cpp [|2-3|13-15|12]
struct RGB {
  RGB(uint8_t rr, uint8_t gg, uint8_t bb)
    : r(rr), g(gg), b(bb) { }
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

class RGBA : public RGB {
public:
  RGBA(uint32_t color)
    //: r((color & 0xff000000) >> 24) // will not compile
    : RGB(static_cast<uint8_t>((color & 0xff000000) >> 24),
          static_cast<uint8_t>((color & 0x00ff0000) >> 16),
          static_cast<uint8_t>((color & 0x0000ff00) >> 8))
    , alpha((color & 0x000000ff) / static_cast<double>(0xff)) { }
  double alpha;
};
```

* Z konštruktora môžeme volať konštruktor rodiča (do inicializačného listu nevieme zahrnúť členy predka)


## Dedenie konštruktorov (`using`)

```cpp
class NamedRGB : public RGB {
public:
  using RGB::RGB; // inherit all constructors from RGB

  void Print() const {
    std::println("{}, {}, {}", r, g, b);
  }
};

int main() {
  NamedRGB color(255, 128, 0); // calls inherited RGB(uint8_t, uint8_t, uint8_t)
}
```

* Konštruktory sa automaticky nededia
* Ak potomok nepridáva členské premenné vyžadujúce špeciálnu inicializáciu, môžeme konštruktory predka zdediť pomocou `using Base::Base;` (od C++11)

---

## Skrývanie mien pri dedení (*name hiding*)

```cpp
class Base {
public:
  void Write(int x) { std::println("int: {}", x); }
  void Write(double x) { std::println("double: {}", x); }
};

class Derived : public Base {
public:
  void Write(int x) { std::println("Derived int: {}", x); }
};

int main() {
  Derived d;
  d.Write(3.14); // prints "Derived int: 3"!
}
```

* Ak potomok zadefinuje metódu s rovnakým menom ako predok, **skryje všetky preťaženia** tohto mena z predka (bez ohľadu na parametre)


## Sprístupnenie preťažení cez `using`

```cpp
class Derived : public Base {
public:
  using Base::Write; // bring Base::Write overloads into Derived scope

  void Write(int x) { std::println("Derived int: {}", x); }
};

int main() {
  Derived d;
  d.Write(42);   // prints "Derived int: 42"
  d.Write(3.14); // prints "double: 3.14"
}
```

* Pomocou `using Base::Write;` sprístupníme preťažené metódy predka aj v potomkovi

---

## Modifikátor dedenia

```cpp
class PublicDerived : public Base { };       // is-a relationship
class ProtectedDerived : protected Base { }; // rarely used
class PrivateDerived : private Base { };     // implemented-in-terms-of
```

| Člen v `Base` | `public` dedenie | `protected` dedenie | `private` dedenie |
| --- | --- | --- | --- |
| `public` | `public` | `protected` | `private` |
| `protected` | `protected` | `protected` | `private` |
| `private` | neprístupný | neprístupný | neprístupný |

* Pri `class` je predvolené dedenie `private`, pri `struct` je predvolené `public`
* V praxi takmer vždy používame `public` dedenie; namiesto `private` dedenia uprednostňujeme kompozíciu

---

## Usporiadanie v pamäti

<table style="width: 80%;">
<tr>
<td>

```cpp
class A { };
class B { };
class C { };
 
class X : public A {
  B b_;
};
 
class Y : public X {
  A a_;
  C c_;
};
```
</td>
<td>

![Memory layout](./lectures/5_oop/memory-layout.png)
</td>
</tr>
</table>


## MSVC vie zobraziť usporiadanie

* `/d1 reportAllClassLayout`
* `/d1 reportSingleClassLayout<name>`

![reportAllClassLayout](./lectures/5_oop/reportSingleClassLayout.png)

* `/d1 reportSingleClassLayoutRGBA`
* Ako môžeme vidieť, celá naša trieda `RGBA` má `16` bajtov a `RGB` má `3` bajty, teda kompilátor zarovnal `double` alpha na 8 bajtov.

---

## Konštrukcia a deštrukcia

* Členy a rodičia triedy sa konštruujú v poradí 
    * Všetci rodičia v poradí zľava doprava, ako sú uvedení v zdrojovom kóde
    * Všetky členské premenné zhora nadol v poradí, ako sú uvedené v zdrojovom kóde
* Vykoná sa telo konštruktora<small>, prípadne viacerých</small>
* Deštrukcia sa robí v opačnom poradí


## Príklad

<table style="width: 80%;">
<tr>
<td>

```cpp
class A {
public:
  A() { std::println("A"); }
  ~A() { std::println("~A"); }
};

// same for other classes
class B { };
class C { };
class D { };
 
class X : public A, public B {
  C b_;
  D d_;
};
```
</td>
<td>

* Konštrukcia X
    * A
    * B
    * C
    * D
* Deštrukcia
    * ~D
    * ~C
    * ~B
    * ~A
</td>
</tr>
</table>

---

## Nevýhody dedičnosti

* Dedičnosť je veľmi silné prepojenie tried
* Väčšinou je lepšia kompozícia
* Ak niekto niečo pridá do rodiča, automaticky to preberú aj potomkovia
* Hlavne obrovské hierarchie musia byť dobre nadizajnované, inak sa vedia rýchlo vymknúť spod kontroly

---

## Kompozícia

<table style="width: 80%;">
<tr>
<td style="width: 40%;">

```cpp
// instead of this
class B : public A
{
public:
    // ...
};

// use this
class B
{
public:
private:
    A a_;
};
```
</td>
<td>

* Namiesto dedenia jednoducho zapuzdríme triedu
* Rovnako sa to dá dosiahnuť private dedičnosťou, ale takto mi to príde bežnejšie a lepšie *(upozornenie: osobný názor)*

</td>
</tr>
</table>

---

## Viacnásobná dedičnosť a „Diamond problem“

<div style="display: flex; align-items: center;">
<div style="flex: 6;">

```cpp
struct Device { int id; };
struct Scanner : public Device { };
struct Printer : public Device { };

struct Copier : public Scanner, public Printer { };

int main() {
  Copier c;
  // c.id = 1; // error: 'id' is ambiguous
  c.Scanner::id = 1;
  c.Printer::id = 2;
}
```
</div>
<div style="flex: 4;">

* `Copier` obsahuje **dve** nezávislé inštancie `Device`
* Riešením v C++ je virtuálne dedenie (`virtual public Device`), ktoré má však výkonnostnú aj mentálnu réžiu
* Viacnásobnej dedičnosti tried s dátami sa preto radšej vyhýbame
</div>
</div>

---

## Polymorfizmus

* Teoreticky typy nie sú polymorfné, môžu mať len polymorfnú sémantiku 
* Na úrovni programovacích jazykov sa polymorfizmom zvyčajne myslí virtual dispatch
* Pre C++ je polymorfný typ taký, ktorý má aspoň jednu virtuálnu funkciu
* V jazykoch ako Java sú predvolene všetky metódy virtuálne, v C++ nie

---

## Virtuálne funkcie

```cpp
class Sequence {
public:
    virtual int64_t Next() { return 0; }
};

void PrintSeq(Sequence& seq, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::println("{}", seq.Next());
  }
}
```

* Nájdenie smerníka na funkciu Next je odložené z kompilácie až na beh programu
* Ak by bol `seq` parameter potomok Sequence, tak sa zavolá funkcia potomka
* Optimalizátor môže a aj chce devirtualizovať toto volanie, pretože volanie virtuálnej funkcie je trochu drahšie ako priame volanie

---

## Konkrétne implementácie

```cpp
class ConstSeq : public Sequence {
public:
  ConstSeq(int64_t c)
    : c_(c) {}

  virtual int64_t Next() override {
    return c_;
  }
private:
  int64_t c_;
};
```

* Kľúčové slovo `override` označuje, že funkcia existuje aj v rodičovi a táto funkcia ju prekonáva
* Obe kľúčové slová `virtual` aj `override` sú v potomkovi nepovinné – funkcia je virtuálna podľa toho, že ju tak definoval predok, a `override` iba pomáha pri kompilácii chytiť chyby



## Konkrétne implementácie

```cpp
class ArithmeticSeq : public Sequence {
public:
  ArithmeticSeq(int64_t start, int64_t inc)
    : state_(start)
    , increment_(inc) {}

  virtual int64_t Next() override {
    int64_t ret = state_;
    state_ += increment_;
    return ret;
  }

private:
  int64_t state_;
  int64_t increment_;
};
```
<!-- .element: class="showall" -->

---

## Volanie virtuálnych funkcií 

```cpp [|3]
void PrintSeq(Sequence &seq, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::println("{}", seq.Next());
  }
}

int main(int argc, char *argv[]) {
  ConstSeq c_seq(10);
  PrintSeq(c_seq, 10);

  ArithmeticSeq a_seq(0, 10);
  PrintSeq(a_seq, 10);
  return 0;
}
```

<small>C++ podporuje virtuálne volanie iba podľa jedného parametra</small>

---

## `override`

<table style="width: 100%">
<tr>
<td style="width: 50%">

```cpp
class Sequence {
public:
    virtual int64_t Next() { return 0; }
};

void PrintSeq(Sequence& seq, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::println("{}", seq.Next());
  }
}

class One : public Sequence {
public:
  virtual int64_t Next() const {
    return 1;
  }
};

int main(int argc, char* argv[]) {
  One seq;
  PrintSeq(seq, 10);
}

```
<!-- .element: class="showall" -->
</td>
<td>

Funkcia Next v triede One je v skutočnosti nová funkcia, lebo je označená ako const, a teda neprekonáva funkciu z predka. 

Použitím kľúčového slova override sa nepodarí tento program skompilovať. 

Neexistuje dôvod, prečo by ste override nepoužili.
</td>
</tr>
</table>

---

## Abstraktné funkcie

* Niekedy nedáva zmysel, aby predok mal virtuálnu metódu implementovanú
* Navyše abstraktné triedy (také, čo majú aspoň jednu abstraktnú metódu) sa nedajú inštanciovať

```cpp
class Sequence {
public:
    virtual int64_t Next() = 0;
};

int main() {
  Sequence seq; // compiler error
  PrintSeq(seq, 10);
}
```

* `= 0` definuje abstraktnú metódu, tá potom musí byť niekde prekonaná, inak triedu nevieme inštanciovať.

---

# `std::unique_ptr`

---

## `std::unique_ptr`

* Ak naozaj potrebujeme dynamickú alokáciu pamäte (napríklad pre polymorfné objekty), použijeme chytré smerníky (*smart pointers*) z hlavičky `<memory>`
* `std::unique_ptr<T>` je RAII obal nad surovým smerníkom s výlučným vlastníctvom

```cpp
std::unique_ptr<Sequence> seq(new ArithmeticSeq(1, 10));
std::println("{}", seq->Next());
```

* Operátory `*` a `->` sú preťažené, takže sa používa rovnako ako surový smerník
* `unique_ptr` automaticky zavolá `delete` nad spravovaným objektom, keď opustí scope (aj pri výnimke)
* **Moderné C++ programy by nikdy nemali volať `delete` priamo**


## Lepšie použiť `std::make_unique`

* Menej písania a žiadne priame použitie `new` v kóde

```cpp
auto seq = std::make_unique<ArithmeticSeq>(1, 10);
std::println("{}", seq->Next());
```

---

## Operácie nad `unique_ptr`

| Funkcia | Efekt |
| ------- | ----- |
| `get()` | Vráti manažovaný smerník. |
| `reset(X)` | Zdeštruuje smerník a nastaví ho na X. |
| `release()` | Vráti manažovaný smerník a už sa oňho viac nestará. |
| `swap()` | Vymení dva unique_ptr. |

* unique_ptr je taký rýchly ako surový smerník 
* Často je aj v pamäti reprezentovaný práve ako jeden smerník 
* Teda žiadna penalizácia pri behu programu


```cpp
int main(int argc, const char* argv[]) {
  std::unique_ptr<Sequence> p(new ConstSeq(4)); // we can assign derived class to base class
  Sequence* a = p.get(); // returns raw pointer 
  // delete a; // error p will also call delete in ~ 
  a = p.release(); // release raw pointer (compatibility)
  delete a; // OK, p is now nullptr, but not wise 
  p.reset(new ConstSeq(0)); // delete managed object and assign new one
} // delete is called
```

---

## `std::unique_ptr` virtuálne metódy

Smart pointer sa správa ako obyčajný pointer, teda volania metód používajú virtuálny dispatch.

```cpp
void PrintSeq(Sequence& seq, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    std::println("{}", seq.Next()); // virtual dispatch
  }
}

int main() {
  auto seq = std::make_unique<ArithmeticSeq>(1, 10);
  PrintSeq(*seq, 5);
}
```

---

## Kopírovanie `unique_ptr`

* `unique_ptr` je jedinečným vlastníkom pamäte (objektu)
* Bitová kópia by toto pokazila, a preto nie je podporovaná
* Ale môže byť presunutý (pomocou operácie `move`, tomu sa budeme venovať neskôr)

```cpp
auto ptr = std::make_unique<std::string>("Unique");

//auto p1 = ptr; // will not compile
auto p2 = std::move(ptr);
assert(ptr == nullptr); // true
assert(*p2 == "Unique"); // true
```

---

## Funkcia objekt iba používa

* Ak funkcia nerieši životnosť objektu, **neodovzdávame `std::unique_ptr`**
* Použijeme referenciu (`*ptr`), ak objekt musí existovať, alebo surový smerník (`ptr.get()`), ak môže byť `nullptr`

```cpp
void UseRequired(Sequence& seq) {
  std::println("{}", seq.Next());
}

void UseOptional(Sequence* seq) {
  if (seq != nullptr)
    std::println("{}", seq->Next());
}

int main() {
  auto ptr = std::make_unique<ArithmeticSeq>(1, 10);
  UseRequired(*ptr);
  UseOptional(ptr.get());

  ConstSeq stack_seq(42);
  UseRequired(stack_seq); // works with stack objects too!
}
```


## Funkcia mení samotný `unique_ptr`

* Referenciu `std::unique_ptr<T>&` použijeme iba vtedy, keď má funkcia zmeniť, na čo `unique_ptr` ukazuje (napr. zavolať `reset` alebo priradiť nový objekt)

```cpp
void ReplaceWithZero(std::unique_ptr<Sequence>& seq) {
  seq = std::make_unique<ConstSeq>(0);
}

int main() {
  auto ptr = std::make_unique<ArithmeticSeq>(1, 10);
  ReplaceWithZero(ptr); // old object is deleted, ptr now owns ConstSeq(0)
}
```


## Vytváranie objektov cez factory funkciu

```cpp
std::unique_ptr<Sequence> CreateSequence(std::string_view type) {
  if (type == "arith")
    return std::make_unique<ArithmeticSeq>(0, 10);
  return std::make_unique<ConstSeq>(1);
}

int main() {
  auto seq = CreateSequence("arith");
  PrintSeq(*seq, 5);
}
```

* Volajúci dostane jasné výlučné vlastníctvo vytvoreného objektu
* Konkrétne podtriedy môžu zostať skryté v implementačnom súbore


## Zhrnutie: `unique_ptr` a funkcie

| Zámer | Typ parametra / návratu |
| --- | --- |
| Iba použiť objekt (musí existovať) | `Sequence&` / `const Sequence&` |
| Iba použiť objekt (môže chýbať) | `Sequence*` / `const Sequence*` |
| Prebrať vlastníctvo objektu | `std::unique_ptr<Sequence>` |
| Presmerovať alebo uvoľniť `unique_ptr` | `std::unique_ptr<Sequence>&` |
| Vytvoriť a vrátiť nový objekt | návrat `std::unique_ptr<Sequence>` |

---

## `make_unique` a `move` (presun vlastníctva)

```cpp
void f(std::unique_ptr<Sequence> p) 
{ 
} // p is deleted

int main(int argc, const char* argv[])
{
  auto p = std::make_unique<ConstSeq>(5);
  // f(p); error
  f(std::move(p)); // ownership is transferred 
  assert(p == nullptr); // true
} // nothing is deleted

```

**Je tu jeden obrovský problém. Aký?**
<!-- .element: class="fragment fade-down" -->


## Kolekcia objektov

```cpp
int main() {
  // std::vector<Sequence> v; // will not work
  // because Sequence is abstract class and it is not
  // possible to instantiate it, even without abstract
  // class it will not work, this vector can only store
  // Sequence and not its child, the same problem was
  // with function parameters, & can fix it, here we
  // need to use pointers
  std::vector<std::unique_ptr<Sequence>> seqs;
  seqs.push_back(std::make_unique<ArithmeticSeq>(1, 10));
  seqs.push_back(std::make_unique<ConstSeq>(0));
  return 0;
}

```

* Používanie objektov z vectora urobí virtuálne volanie počas behu programu
* Pretože konkrétne objekty majú rozdielnu veľkosť, nemôžu byť priamo vo vectore, ale musia byť uložené ako smerníky


## Problém

```cpp
  return 0;
  // here destructor is called 
  // so unique ptr will delete all Sequences 
  // bad things will happen, because destructor 
  // is not virtual in Sequence 
}
```

* Predok (ak sa cez neho mažú potomkovia) by mal mať vždy **virtuálny deštruktor**
* Deštruktor môže byť aj abstraktný, ale musí mať implementáciu (takže radšej nie, je to taká technikalita)

---

## Virtuálny deštruktor

```cpp
class Base { };
 
class Derived : public Base {
public:
  ~Derived() 
  { 
  }
private:
  std::string s_{ "Hello world!!!!!!!" };
};
 
int main() {
  Base* p = new Derived();
  // ...
  delete p;
}

```

* `delete p` bez virtuálneho deštruktora v `Base` má **nedefinované správanie** (zvyčajne sa zavolá iba deštruktor `Base`, deštruktor `Derived` sa nikdy nezavolá a stratí sa pamäť stringu)
* Musíme do `Base` dať verejný virtuálny deštruktor, potom sa problém vyrieši


```cpp
class Base {
public:
    virtual ~Base() = default;
};
```

---

## Čisto abstraktné rozhrania (*Interfaces*)

* V C++ neexistuje kľúčové slovo `interface` (ako v Jave alebo C#)
* Rozhranie vytvoríme ako triedu, ktorá:
    * nemá žiadne nestatické členské premenné,
    * má všetky metódy čisto virtuálne (`= 0`),
    * má verejný virtuálny deštruktor (`virtual ~Interface() = default;`)

```cpp
class Serializable {
public:
  virtual ~Serializable() = default;
  virtual std::string Serialize() const = 0;
};

class Printable {
public:
  virtual ~Printable() = default;
  virtual void Print() const = 0;
};
```


## Implementácia viacerých rozhraní

```cpp
class Document : public Serializable, public Printable {
public:
  std::string Serialize() const override { return content_; }
  void Print() const override { std::println("{}", content_); }

private:
  std::string content_ = "Hello C++";
};
```

* Viacnásobná dedičnosť čisto abstraktných rozhraní je v C++ bezpečná a bežne používaná
* Keďže rozhrania neobsahujú dáta, nevzniká problém duplicitných členských premenných (*diamond problem*)

---

## __vfptr

* Tabuľka virtuálnych funkcií (zvyčajná implementácia, štandard ju priamo nepredpisuje)
* Každý objekt, ktorý je polymorfný (aspoň jedna virtuálna funkcia), má v sebe jeden smerník na tabuľku
* Volanie virtuálnych metód potom funguje tak, že sa pozrie do tabuľky, vyberie sa z nej smerník na funkciu a zavolá sa

![__vfptr](./lectures/5_oop/vfptr.png)

---

## Konštruktor a virtuálne metódy

Skompiluje sa nasledujúci program? Aký je výsledok?

<table style="width: 100%">
<tr>
<td style="width: 40%;">

```cpp
class Base {
public:
  Base() { print(); }

  virtual void print() {
    std::println("Base");
  }
};
 
class Derived : public Base {
  virtual void print() override {
    std::println("Derived");
  }
};

int main() {
  Derived d;
}
```
<!-- .element: class="showall" -->
</td>
<td class="fragment">

Vypíše sa `"Base"`. 

Keď sa konštruuje `Derived`, najprv sa skonštruujú všetci predkovia (`Base`). Konštruktor `Base` volá virtuálnu funkciu, ale `Derived` ešte nie je skonštruované, takže volanie do `Derived` by bolo veľmi nebezpečné. 

Smerník na tabuľku virtuálnych funkcií sa nastavuje priebežne na začiatku každého konštruktora podľa práve konštruovanej triedy.
</td>
</tr>
</table>


## Virtuálne metódy v konštruktoroch a deštruktoroch

![Grumpy cat no](./lectures/5_oop/grumpy-no.png)

**Jednoducho nie**
<!-- .element: class="fragment" -->

---

## Spoločný interface

* Potomkovia môžu vystupovať ako rodič
* Môžeme vyrobiť funkcie, ktoré akceptujú rodiča (interface) a potom do nich pošleme konkrétnu implementáciu

```cpp [|5]
class File { /* ... */ };
 
class TemporaryFile : public File { /* ... */ };
 
void ReadInput(File& file);
 
int main() {
  TemporaryFile tempFile;
  ReadInput(tempFile);
}
```

Referencia vo funkcii je veľmi dôležitá. 
<!-- .element: class="fragment" -->


## Slicing

* Nie list slicing z Pythonu ;)
* Hodnoty v C++ (nie referencie a smerníky) nosia celú konkrétnu hodnotu (musia vedieť svoju veľkosť), ak treba, tak sa kopírujú (pomocou kopírovacieho konštruktora / operátora priradenia)

```cpp
void ReadInput(File* file);
 
int main() {
  TemporaryFile temp_file;
  ReadInput(&temp_file);
}
```

```cpp
void ReadInput(File file);
 
int main() {
  TemporaryFile temp_file;
  ReadInput(temp_file);
}
```

Druhé pravdepodobne nie je dobre. Pri volaní sa vykopíruje iba base trieda. 

---

## Predávanie potomkov do funkcií

* Referencie a smart smerníky sú najlepšie
* Posielanie hodnotou nemusí dobre fungovať (slicing)
* Triedy, ktoré sa nemajú dať inštanciovať, by mali byť abstraktné
* Triedy, z ktorých sa nemá dať dediť, potom označíme ako `final`

```cpp
class A final {};
class B : public A {}; 
```

Kód vyššie sa neskompiluje.


## `final` na metódach a devirtualizácia

```cpp
class Sequence {
public:
  virtual ~Sequence() = default;
  virtual int64_t Next() = 0;
};

class ConstSeq : public Sequence {
public:
  explicit ConstSeq(int64_t c) : c_(c) {}
  int64_t Next() override final { return c_; }
private:
  int64_t c_;
};
```

* `final` môžeme použiť aj na konkrétnu virtuálnu metódu – žiadny ďalší potomok ju už nemôže prekonať
* Ak pracujeme s typom `ConstSeq&` (alebo je celá trieda `final`), kompilátor s istotou pozná cieľovú metódu a môže volanie **devirtualizovať** (zavolať priamo alebo inlinovať bez čítania `__vfptr`)

---

## Kedy sa virtuálna funkcia nevolá virtuálne?

```cpp
class Derived : public Base {
public:
  void Process() override {
    Base::Process(); // explicit call to base implementation, not virtual!
    // do additional work in Derived
  }
};

void Run(Derived& d) {
  d.Base::Process(); // also calls Base::Process() directly
}
```

* Ak pri volaní explicitne uvedieme názov triedy (`Base::Process()`), virtuálny mechanizmus sa nepoužije
* Bežne sa to využíva v prekonanej metóde potomka, keď chceme najprv vykonať pôvodnú logiku predka a potom pridať vlastnú

---

## `private` vs `virtual`

Skompiluje sa nasledujúci program? 

```cpp
class A {
private:
  virtual int f() const = 0;
public:
  void print() const { std::println("{}", f()); }
};
 
class B : public A {
public:
  virtual int f() const override { return 0; }
};
 
int main() {
  B().print();
}
```
<!-- .element: class="showall" -->

Áno, virtuálne volania nemajú nič spoločné s modifikátormi prístupu. Toto sa aj využíva, keď verejný interface je statický a privátne funkcie sú virtuálne – tie sa potom dajú prekonať v potomkovi. 
<!-- .element: class="fragment" -->

---

## Ako používať triedy?

* Ak vyrobíte niekde dedenie, radšej sa uistite, či nemá mať predok virtuálny deštruktor
* Vždy používajte `override` na označenie metód
* Nerobte všetky funkcie virtuálne, C++ nie je Java, program je pomalší a málokedy to dáva zmysel

---

# `dynamic_cast<T>`

---

## `dynamic_cast<T>`

* Používa sa pri polymorfných typoch na downcast
* Alebo pri viacnásobnej dedičnosti na sidecast
* Kontroluje sa počas behu programu
* Ak nemôže byť vykonaný
    * Vráti `nullptr` pre smerníkové typy
    * Vyhodí výnimku `std::bad_cast` pre referencie
* Na to, aby pracoval správne, musí kompilátor podporovať RTTI (*run-time type information*)


## Použitie `dynamic_cast<T>`

```cpp
class A { };

class B : public A { };

void f(A* a) {
  B* b = dynamic_cast<B*>(a);
}
```

Neskompiluje sa, lebo `dynamic_cast` môžeme použiť iba na typy, ktoré sú polymorfné (majú tabuľku virtuálnych funkcií).


```cpp
class A { public: virtual ~A() = default; };

class B : public A { };

void f(A* a) {
  B* b = dynamic_cast<B*>(a);
}
```

Vráti buď smerník na `B`, alebo `nullptr`, ak premenná `a` nenesie `B`, ale iba `A`.

---

## Zlé pretypovanie

```cpp
class A { 
public: 
  virtual ~A() { } 
};

class B : public A { };

void f(A& a) {
  B* b1 = static_cast<B*>(&a);
  B& b2 = static_cast<B&>(a);
}
```

**`static_cast`** sa vždy podarí, ale nemusí vyrobiť požadovaný výsledok. Je na programátorovi, aby sa uistil, že typ je naozaj taký, aké pretypovanie robíme. 


```cpp
class A { 
public: 
  virtual ~A() { } 
};

class B : public A { };

void f(A& a) {
  B* b3 = dynamic_cast<B*>(&a);
  B& b4 = dynamic_cast<B&>(a);
}
```

**`dynamic_cast`** kontroluje typ počas behu (využíva `__vfptr`) a vráti `nullptr`, ak pretypujeme na smerník (`B*`), alebo vyhodí výnimku `std::bad_cast`, ak chceme referenciu (`B&`). Časté používanie `dynamic_cast` však zvyčajne svedčí o zlom dizajne.

---

## Alternatíva k OOP polymorfizmu: `std::variant`

* Ak dopredu poznáme konečnú množinu typov, nemusíme vždy vytvárať hierarchiu tried, virtuálne metódy a alokovať objekty cez `std::unique_ptr`
* Od C++17 môžeme použiť `std::variant<Ts...>` (typovo bezpečnú alternatívu k `union`), ktorá drží hodnotu práve jedného z uvedených typov priamo na mieste (bez alokácie na halde)

```cpp
struct ConstSeq {
  int64_t c;
  int64_t Next() { return c; }
};

struct ArithmeticSeq {
  int64_t state;
  int64_t inc;
  int64_t Next() {
    int64_t ret = state;
    state += inc;
    return ret;
  }
};

using AnySequence = std::variant<ConstSeq, ArithmeticSeq>;
```


## Kolekcia s `std::variant`

```cpp
int main() {
  std::vector<AnySequence> seqs;
  seqs.push_back(ArithmeticSeq{1, 10});
  seqs.push_back(ConstSeq{0});

  for (auto& seq : seqs) {
    if (auto* arith = std::get_if<ArithmeticSeq>(&seq)) {
      std::println("{}", arith->Next());
    } else if (auto* c = std::get_if<ConstSeq>(&seq)) {
      std::println("{}", c->Next());
    }
  }
}
```

* Všetky prvky sú uložené súvislo priamo v pamäti `std::vector` (žiadne smerníky na haldu ani `__vfptr`)
* Ešte elegantnejšie spracovanie pomocou `std::visit` a lambda funkcií si ukážeme na nasledujúcej prednáške


## Dedičnosť vs. `std::variant`

| Vlastnosť | Dedičnosť + `unique_ptr` | `std::variant` |
| --- | --- | --- |
| Množina typov | Otvorená (ľubovoľný počet potomkov) | Uzavretá (pevne vymenované typy) |
| Pridanie nového typu | Jednoduché (nová podtrieda) | Zmena definície `variant` |
| Pridanie novej operácie | Zmena rozhrania a všetkých potomkov | Jednoduché (nová funkcia) |
| Uloženie v `std::vector` | Smerníky na haldu (`unique_ptr`) | Priamo hodnoty v súvislej pamäti |

---

# `std::any`

---

## `std::any`

* Od C++17 (v hlavičke `<any>`) umožňuje uložiť ľubovoľnú kopírovateľnú hodnotu akéhokoľvek typu
* Je to typovo bezpečná náhrada za `void*`, ktorá si navyše sama spravuje životnosť uloženého objektu (RAII)
* Malé objekty sa zvyčajne uložia priamo vnútri `std::any` (*Small Buffer Optimization*), väčšie sa alokujú na halde

```cpp
std::any value = 42;            // holds int
value = 3.14;                   // holds double
value = std::string("Hello!");  // holds std::string

assert(value.has_value());
value.reset();                  // destroys the contained object
assert(!value.has_value());
```


## Prístup k hodnote v `std::any`

* Na získanie hodnoty musíme poznať jej presný typ a použiť `std::any_cast<T>`
* Rovnako ako pri `dynamic_cast`:
    * `std::any_cast<T>(value)` pri nesprávnom type vyhodí výnimku `std::bad_any_cast`
    * `std::any_cast<T>(&value)` so smerníkom vráti pri nesprávnom type `nullptr`

```cpp
std::any item = std::string("C++23");

// 1. Pointer version: returns nullptr on type mismatch
if (auto* str = std::any_cast<std::string>(&item)) {
  std::println("String: {}", *str);
}

// 2. Value/reference version: throws std::bad_any_cast on mismatch
try {
  int number = std::any_cast<int>(item);
  std::println("{}", number);
} catch (const std::bad_any_cast& e) {
  std::println("Wrong type: {}", e.what());
}
```


## Kedy použiť `std::any`?

* `std::any` nepoužívame namiesto šablón ani namiesto `std::variant`
* Ak vopred poznáme možné typy, **vždy uprednostníme `std::variant`** (je rýchlejší, nealokuje na halde a kompilátor skontroluje všetky typy)
* `std::any` sa hodí iba tam, kde množina typov nie je vopred známa:
    * užívateľský kontext (*user data*) v callbackoch a udalostiach,
    * heterogénne mapy vlastností (napr. `std::map<std::string, std::any>`),
    * prenášanie dát cez vrstvy, ktoré nemusia poznať konkrétny typ.

---

# ĎAKUJEM

## Otázky?