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

* Zneužívanie objektov vedie k čudný triedam
    * Factory, Manager, Helper, …
* Obyčajné funkcie sú často lepšie 
* Objekty nie sú namespace!

![Rube goldberd](./lectures/5_oop/rube-goldberg.png)

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
* Z hľadiska C++ sú to len funkcie, ktoré implicitne dostanú `this` smerník

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

* `operator delete[]` je potrebný na správne zdeštruovanie všetkých objektov vytvorených pomocou `new[]`
* MSVC ukladá počet alokácií pred vráteným blokom pamäte

![new of arrays](./lectures/5_oop/new[].png)

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
    * `str` je default skonštruovaný a potom zmenený

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
```
```cpp
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

`inline` je celkom dôležité, inak musíme inštanciáciu presunúť mimo hlavičkového súboru, inak budeme mať chybu linkera.

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

![Piliere oop - abstraction, encapsilation, inheritance, polymorphism](./lectures/5_oop/pillars-of-oop.png)

---

## Zapúzdrenosť (*encapsulation*)

* Triedy majú definovať interface a všetko ostatné je implementačný detail neviditeľný pre užívateľa
* Takto sa implementácia môže voľne meniť, ak interface ostane nedotknutý (alebo aspoň spätne kompatibilný)

Verejné členské premenné. <!-- .element: class="fragment strike" -->


## Výhody zapúzdrenia

* Refactor je oveľa jednoduchší a dá sa robiť postupne 
* Automatické testy testujú iba interface a nemusia používať implementačné detaily
* Lepšie definované invarianty
* Prenositeľný kód
* Vo všeobecnosti uľahčuje život

---

## Invarianty

* Triedy sú nástroje na udržiavanie invariantov
    * Konštruktor by mal nastaviť invarianty
    * Metódy by mali zachovávať invarianty
* V priebehu metód ale môže byť invariant rozbitý
* Musia ich potom nastaviť späť
* Nedosiahnuteľné ak dovolíme priamy zápis do členských premenných


## Konštruktory a invarianty

* Ideálne by konštruktory mali harmonizovať dáta a zaručiť aby invarianty sedeli

<table style="width: 100%;">
<tr>
<td style="width: 40%">

```cpp
class Number {
public: 
     Number(const char* str);
private:
    std::string data;
    bool negative = false;
};
```
</td>
<td rowspan=2>

```cpp
Number::Number(const char* str) {
    if (size_t len = strlen(str)) {
        if (str[0] == '-') {
            negative = true;
            ++str;
            --len;
        }
        for (size_t i = 0; i < len; ++i) {
            if (!isdigit(str[i]))
                throw std::runtime_error("Not a number");
        }
        data = str;
        return;
    }
    throw std::runtime_error("Empty string");
}
```
<!-- .element: class="showall" -->
</td>
</tr>
<tr>
<td>

Konštruktor by mal hneď ako zístí, že je niečo zle vyhodiť výjnimku. **Fail faster.**
</td>
</tr>
</table>

---

## Dedičnosť

* Triedy môžu tvoriť hierarchie, kde nová trieda je založená na starej a rozširuje ju pridaním funkcionality
* C++ dokonca podporuje viacnásobnú dedičnosť (nateraz ignorujeme)

![Sequences inheritance diagram](./lectures/5_oop/sequence.png)

---

## Vytváranie odvodených tried

```cpp
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class RGBA : public RGB {
public:
    RGBA(uint32_t color)
    {
        r = ((color & 0xff000000) >> 24);
        g = ((color & 0x00ff0000) >> 16);
        b = ((color & 0x0000ff00) >> 8);
        alpha = (color & 0x000000ff) / static_cast<double>(0xff);
    }
    double alpha;
};
```
<!-- .element: class="showall" -->

* Znak `":"` sa používa na odvodenie triedy od rodičovskej triedy (*base class*)
* Kľučové slovo `public` znamená, že rodič je viditeľný pre používateľov potomka (v postate sa vždy používa `public`)
* V potomkovi máme prístup ku všetkým `public` a `protected` častiam rodiča a preto ich môžeme v konštruktore môžeme použiť

---

## Volanie konštruktora predka

```cpp [|2-3|13-15|12]
struct RGB {
    RGB(uint8_t rr, uint8_t gg, uint8_t bb)
        : r(rr), g(gg), b(bb) { }
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class RGBA : public RGB {
public:
    RGBA(uint32_t color)
        //: r((color & 0xff000000) >> 24) // will not compile
        : RGB( (color & 0xff000000) >> 24,
               (color & 0x00ff0000) >> 16,
               (color & 0x0000ff00) >> 8 )
        , alpha((color & 0x000000ff) / static_cast<double>(0xff)) { }
    double alpha;
};
```

* Z konštruktora môžeme volať konštruktor rodiča (do inicializačného listu nevieme zahrnúť členy predka)

---

## Usporiadanie v pamäti

<table style="width: 80%;">
<tr>
<td>

```cpp
class A { };
class B { };
class C { };
 
class X : public A {
    B b_;
};
 
class Y : public X {
    A a_;
    C c_;
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
* Ako môžeme vidieť celá naša trieda `RGBA` má `16` bajtov a `RGB` má `3` bajty, teda kompilátor zarovnal `double` alpha na 8 bajtov.

---

## Konštrukcia a deštrukcia

* Členy a rodičia triedy sa konštruujú v poradí 
    * Všetci rodičia v poradí zľava doprava ako sú uvedené v zdrojovom kóde
    * Všetky členské premenné zhora nadol v poradí ako sú uvedené v zdrojovom kóde
* Vykoná sa telo konštruktora<small>, prípadne viacerých</small>
* Deštrukcia sa robí v opačnom poradí


## Priklad

<table style="width: 80%;">
<tr>
<td>

```cpp
class A {
public:
    A() { std::cout << "A\n"; }
    ~A() { std::cout << "~A\n"; }
};

// same for other classes
class B { };
class C { };
class D { };
 
class X : public A, public B {
    C b_;
    D d_;
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
* Väčšinou je lepšie kompozícia
* Ak niekto niečo pridá do rodiča automaticky  pre preberú aj potomkovia
* Hlavne obrovské hierarchie musia byť dobre nadizajnované, inak sa vedia rýchlo vymknúť kontrole

---

## Kompozícia

<table style="width: 80%;">
<tr>
<td style="width: 40%;">

```cpp
// instead of this
class B : public A
{
public:
    // ...
};

// use this
class B
{
public:
private:
    A a_;
};
```
</td>
<td>

* Namiesto dedenia jednoducho zapúzdrime triedu
* Rovnako sa to dá dosiahnuť private dedičnosťou, ale takto mi to príde bežnejšie a lepšie *(upozornenie: osobný názor)*

</td>
</tr>
</table>

---

## Empty base optimization

* Aká je jednota nasledujúceho výrazu? 

```cpp
class A {};

sizeof(A);
```
<!-- .element: class="fragment" -->

1, v C++ žiadne dva objekty nemôžu zdieľať tú istú pamäť a preto to nemôže byť 0.
<!-- .element: class="fragment" -->

* Aká je jednota nasledujúceho výrazu?
<!-- .element: class="fragment" -->

```cpp
class A {};
class B {
  A a_;
  int i_;
};
sizeof(B);
```
<!-- .element: class="fragment" -->

Veľkosť B je veľkosť A plus veľkosť int, obe musia mať adresu, preto je výsledok väčší ako sizeof(int).
<!-- .element: class="fragment" -->


## Empty base optimization

* Ak použijeme dedičnosť, potom sa nedá priamo odkazovať na premennú typu `A`
* Kompilátor môže optimalizovať veľkosť objektu a prázdna trieda `A` nezaberie ani jeden bajt

```cpp
class A {};
class B : public A {
  int i_;
};

sizeof(B) == sizeof(int);
```

*Poznámka: prázdna trieda je trieda, ktorá nemá žiadne nestatické členské premenné.*

---

## Polymorfizmus

* Teoreticky typy nie sú polymorfné, môžu mať len polymorfnú sémantiku 
* Na úrovni programovacích jazykov sa polymorfizmom zvyčajne myslí virtual dispatch
* Pre C++ je polymorfný typ taký, ktorý má aspoň jednu virtuálnu funkciu
* V jazykoch ako Java sú predvolene všetky metódy virtuálne, v C++ nie

---

## Virtuálne funkcie

```cpp
class Sequence {
public:
    virtual int64_t Next() { return 0; }
};

void PrintSeq(Sequence& seq, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        std::cout << seq.Next() << '\n';
    }
}
```

* Nájdenie smerníka na funkciu Next je odložené z kompilácie až na beh programu
* Ak by bol `seq` parameter potomok Sequence, tak sa zavolá funkcia potomka
* Optimalizátor može a aj chce devirtualizovať toto volanie, pretože volanie virtuálnej funkcie je trochu drahšie ako priame volanie

---

## Konkrétne implementácie

```cpp
class ConstSeq : public Sequence {
public:
  ConstSeq(int64_t c)
    : c_(c) {}

  virtual int64_t Next() override {
    return c_;
  }
private:
  int64_t c_;
};
```

* Kľúčové slovo `override` označuje, že funkcia existuje aj v rodičovi a táto funkcia je prekonáva
* Obe kľúčové slová `virtual` aj `override` sú irelevantné, virtuálna funkcia je podľa toho, že jej predok ju tam definoval, `override` iba pomáha pri kompilácií chytiť chyby



## Konkrétne implementácie

```cpp
class ArithmeticSeq : public Sequence {
public:
  ArithmeticSeq(int64_t start, int64_t inc)
    : state_(start)
    , increment_(inc) {}

  virtual int64_t Next() override {
    int64_t ret = state_;
    state_ += increment_;
    return ret;
  }

private:
  int64_t state_;
  int64_t increment_;
};
```
<!-- .element: class="showall" -->

---

## Volanie virtuálnych funkcií 

```cpp [|3]
void PrintSeq(Sequence &seq, size_t n) {
    for (size_t i = 0; i < n; ++i) {
      std::cout << seq.Next() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    ConstSeq c_seq(10);
    PrintSeq(c_seq, 10);

    ArithmeticSeq a_seq(0, 10);
    PrintSeq(a_seq, 10);
    return 0;
}
```

<small>C++ podporuje virtuálne volanie iba podľa jedného parametru</small>

---

## `override`

<table style="width: 100%">
<tr>
<td style="width: 50%">

```cpp
class Sequence {
public:
    virtual int64_t Next() { return 0; }
};

void PrintSeq(Sequence& seq, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        std::cout << seq.Next() << '\n';
    }

}

class One : public Sequence {
public:
    virtual int64_t Next() const {
        return 1;
    }
};

int main(int argc, char* argv[]) {
    One seq;
    PrintSeq(seq, 10);
}

```
<!-- .element: class="showall" -->
</td>
<td>

Next funkcia v triede One v skutočnosti je nová funkcia, lebo je označená ako const a teda neprekonáva funkciu z predka. 

Použitím kľučového slova override sa nepodarí tento program skompilovať. 

Neexistuje dôvod, prečo by ste override nepoužili.
</td>
</tr>
</table>

---

## Abstraktné funkcie

* Niekedy nedáva zmysel, aby predok mal virtuálnu metódu implementovanú
* Navyše abstraktné triedy (také čo majú aspoň jednu abstraktnú metódu) sa nedajú inštanciovať

```cpp
class Sequence {
public:
    virtual int64_t Next() = 0;
};

int main() {
    Sequence seq; // compiler error
    PrintSeq(seq, 10);
}
```

* `= 0` definuje abstraktnú metódu, tá potom musí byť niekde prekonaná inak nevieme inštanciovať.

---

# RAII

---

## Najčastejšia chyba začiatočníkov

* Všetky alokované zdroje musia byť uvoľnené
* Pamäť je iba jedným zo zdrojov
    * Každý `malloc` musí mať svoj `free`
    * Každý `new` musí mať svoj `delete`
* Inak máme na krku resource leak (memory leak)

![Pacman memory](./lectures/5_oop/pacman-memory.png)

---

## C++ má riešenie

* RAII – **R**esource **a**cquisition **i**s **i**nitialization
* Konštruktory sú zodpovedné za alokáciu zdrojov
* Deštruktory sú zodpovedné za uvolnenie 
* Všetko je automaticky generované kompilátorom
* **Bezpečné a efektívne bez námahy!**
* Netreba žiaden garbage collector!


```cpp
class FileReader {
public:
    FileReader(const char* path)
        : m_file(fopen(path, "r")) {
    }
    ~FileReader() {
        if (m_file == NULL)
            fclose(m_file);
    }
    bool IsOK() const { return m_file != NULL; }
    void Read(std::vector<char>& v) {
        size_t n = fread(v.data(), 1, v.size(), m_file);
        v.resize(n);
    }
private:
    FILE* m_file;
};

int main() {
    FileReader f("text.txt");
    std::vector<char> v(1024);
    f.Read(v);
    v.push_back(0); // null
    std::cout << v.data() << '\n';
}
```

Explicitná kontrola, či sa všetko podarilo, nie veľmi dobre  ľahko sa na to zabudne, ako ostatne aj tu... 
<!-- .element: class="fragment" -->

Bolo by treba použiť výnimky.
<!-- .element: class="fragment" -->

---

## Objektovo orientované ≠ `new`

V jazykoch ako Java máme k dispozícií garbage collector, preto sú objekty automaticky zdeštruovanie v istom okamihu po tom ako sa prestanú používať

```cpp [|3-10]
int main() {
    FileReader *file = new FileReader(R"(C:\...\file.txt)");
    if (file->IsOK())     {
        std::vector<char> buffer(1024);
        if (file->Read(buffer))
        {
            buffer.push_back('\0');
            std::cout << buffer.data();
        }
    }
 
    delete file;
}
```

Ak niekde vo vyznačenom bloku nastane výnimka, alebo niekto v ňom zavolá return, tak máme memory leak a aj súbor ostane otvorený. 
<!-- .element: class="fragment" -->

---

## V C++ existuje `"}"`

```cpp
int main() {
    FileReader file(LR"(C:\Users\koscelansky\Desktop\ppp.txt)");
    if (file.IsOK()) {
        std::vector<char> buffer(1024);
        if (file.Read(buffer)) {
            buffer.push_back('\0');
            std::cout << buffer.data();
        }
    }
}
```

* Kratšie, krajšie, rýchlejšie a bez jednej alokácie
* Navyše je to exception safe 

---

## `std::unique_ptr`

* Ak naozaj potrebujeme alokáciu pamäte, tak máme k dispozícií chytré smerníky (smart pointers)
* V hlavičke `#include <memory>`

```cpp
std::unique_ptr<FileReader> file(new FileReader(LR"(C:\...\file.txt)"));
if (file->IsOK()) {
    std::vector<char> buffer(1024);
    if (file->Read(buffer))     {
        buffer.push_back('\0');
        std::cout << buffer.data();
    }
}
```

* Operátory sú preťažené, takže `->` sa práva ako `->` nad surovými smerníkmi
* `unique_ptr` automaticky zavolá `delete` nad hodnotou, ktorú drží, keď ide mimo scope
* **Moderné C++ programy by nikdy nemali volať `delete` priamo**


## Lepšie použiť `std::make_unique`

* Menej písania
* Nikde nie je spomenuté `new` v kóde

```cpp
auto file = std::make_unique<FileReader>(LR"(C:\...\file.txt)");
if (file->IsOK()) {
    std::vector<char> buffer(1024);
    if (file->Read(buffer)) {
        buffer.push_back('\0');
        std::cout << buffer.data();
    }
}
```

---

## Operácie nad `unique_ptr`

| Funkcia | Efekt |
| ------- | ----- |
| `get()` | Vráti manažovaný smerník. |
| `reset(X)` | Zdeštruuje smerník a nastaví ho na X. |
| `release()` | Vráti manažovaný smerník a už sa oňho viacej nestará. |
| `swap()` | Vymení dva unique_ptr. |

* unique_ptr je tak rýchly ako surový smerník 
* Často je aj v pamäti reprezentovaný práve ako jeden smerník 
* Teda žiadna penalizácia pri behu programu


```cpp
int main(int argc, const char* argv[]) {
    std::unique_ptr<Sequence> p(new ConstSeq(4)); // we can assign derived class to base class
    Sequence* a = p.get(); // returns raw pointer 
    // delete a; // error p will also call delete in ~ 
    Sequence* a = p.release(); // release raw pointer (compatibility)
    delete a; // OK, p is now nullptr, but not wise 
    p.reset(new ConstSeq); // delete managed object and assign new one
} // delete is called
```

---

## `std::unique_ptr` virtuálne metódy

Smart pointer sa správa ako obyčajný pointer, teda volania metód používajú virtuálny dispatch.

```cpp
void PrintSeq(std::unique_ptr<Sequence>& seq, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        std::cout << seq->Next() << '\n'; // virtual dispatch
    }
}
```

---

## Kopírovanie `unique_ptr`

* `unique_ptr` je jedinečným vlastníkom pamäte (objektu)
* Bitová kópia by toto pokazila a preto nie je podporovaná
* Ale môže byť presunutý (pomocou operácie `move`, tomu sa budeme venovať neskôr)

```cpp
auto ptr = std::make_unique<std::string>("Unique");

//auto p1 = ptr; // will not compile
auto p2 = std::move(ptr);
assert(ptr == nullptr); // true
assert(*p2 == "Unique"); // true
```

---

## `make_unique` a `move`

```cpp
void f(std::unique_ptr<Sequence> p) 
{ 
} // p is deleted

int main(int argc, const char* argv[])
{
    auto p = std::make_unique<ConstSeq>(5);
    // f(p); error
    f(std::move(p)); // ownership is transfered 
    p == nullptr; // true
} // nothing is deleted

```

**Je tu jeden obrovský problém. Aký?**
<!-- .element: class="fragment fade-down" -->


## Kolekcia objektov

```cpp
int main() {
    // std::vector<Sequence> v; // will not work
    // because Sequence is abstract class and it is not
    // possible to instantiate it, even without abstract
    // class it will not work, this vector can only store
    // Sequence and not its child, the same problem was
    // with function parameters, & can fix it, here we
    // need to use pointers
    std::vector<std::unique_ptr<Sequence>> seqs;
    seqs.push_back(std::make_unique<ArithmeticSeq>(1, 10));
    seqs.push_back(std::make_unique<ConstSeq>(0));
    return 0;
}

```

* Používanie objektov z vectora urobí virtuálne volanie počas behu programu
* Pretože konkrétne objekty majú rozdielnu veľkosť nemôžu byť priamo vo vectore, ale musia byť uložené ako smerníky


## Problém

```cpp
    return 0;
    // here destructor is called 
    // so unique ptr will delete all Sequences 
    // bad things will happen, because destructor 
    // is not virtual in Sequence 
}
```

* Predok (ak sa cez neho mažú potomkovia) by mal mať vždy **virtuálny deštruktor**
* Deštruktor môže byť aj abstraktný, ale musí mať implementáciu (takže radšej nie, je to taká technikalita)

---

## Virtuálny deštruktor

```cpp
class Base { };
 
class Derived : public Base {
    ~Derived() 
    { 
    }
    std::string s_{ "Hello world!!!!!!!" };
};
 
void main() {
    Base* p = new Derived();
    // ...
    delete p;
}

```

* `delete p` zavolá deštruktor `Base`, preto deštruktor `Derived` sa nikdy nezavolá a stratí sa nám pamäť stringu
* Musíme do `Base` dať virtuálny deštruktor, potom sa problém vyrieši


```cpp
class Base {
    virtual ~Base() = default;
};
```

---

## __vfptr

* Tabuľka virtuálnych funkcií 
* Každý objekt, ktorý je polymorfný (aspoň jedna virtuálna funkcia) má v sebe jeden smerník na tabuľku
* Volanie virtuálnych metód potom funguje tak, že sa pozrie do tabuľku vyberie sa s nej smerník na funkcia a zavolá sa

![__vfptr](./lectures/5_oop/vfptr.png)

---

## Konštruktor a virtuálne metódy

Skompiluje sa nasledujúci program? Aký je výsledok?

<table style="width: 100%">
<tr>
<td style="width: 40%;">

```cpp
class Base {
public:
    Base() { print(); }
 
    virtual void print()
    {
        std::cout << "Base";
    }
};
 
class Derived : public Base {
    virtual void print() override
    {
        std::cout << "Derived";
    }
};

int main() {
    Derived d;
}
```
<!-- .element: class="showall" -->
</td>
<td class="fragment">

Vypíše sa `"Base"`. 

Keď sa konštruuje `Derived`, najprv sa vykonštruujú všetci predkovia (`Base`). `Base` konštruktor volá virtuálnu funkciu, ale `Derived` ešte nie je skonštruované, takže volanie do `Derived` by bolo veľmi nebezpečné. 

Tabuľka virtuálnych funkcií sa inicializuje po skonštrovaní.
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
class File { /* ... */ };
 
class TemporaryFile : public File { /* ... */ };
 
void ReadInput(File& file);
 
void main() {
    TemporaryFile tempFile;
    ReadInput(tempFile);
}
```

Referencia vo funkcií je veľmi dôležitá. 
<!-- .element: class="fragment" -->


## Slicing

* Nie list slicing z Pythonu ;)
* Hodnoty v C++ (nie referencie a smerníky) nosia celú konkrétnu hodnotu (musia vedieť svoju veľkosť), ak treba tak sa kopírujú

```cpp
void ReadInput(File* file);
 
void main() {
    TemporaryFile temp_file;
    ReadInput(&temp_file);
}

```

```cpp
void ReadInput(File file);
 
void main() {
    TemporaryFile temp_file;
    ReadInput(temp_file);
}
```

Druhé pravdepodobne nie je dobre. Pri volaní sa vykopíruje base trieda. 

---

## Predávanie potomkov do funkcií

* Referencie a smart smerníky sú najlepšie
* Posielanie hodnotou nemusí dobre fungovať (slicing)
* Triedy, ktoré sa nemajú dať inštanciovať by mali byť abstraktné
* Triedy, z ktorých sa nemá dať dediť potom označíme ako `final`

```cpp
class A final {};
class B : public A {}; 
```

Kód vyššie sa neskompiluje.

---

## `private` vs `virtual`

Skompiluje sa nasledujúci program? 

```cpp
class A {
private:
    virtual int f() const = 0;
public:
    void print() const { std::cout << f() << std::endl; }
};
 
class B : public A {
public:
    virtual int f() const override { return 0; }
};
 
int main() {
    B().print();
}
```
<!-- .element: class="showall" -->

Áno, virtuálne volanie nemajú nič s modifikátormi prístupu. Toto sa aj využíva, keď verejný interface je statický a privátne funkcie sú virtuálne, tie sa potom dajú prekonať v potomkovi. 
<!-- .element: class="fragment" -->

---

## Ako používať triedy?

* Ak vyrobíte niekde dedenie, radšej sa uistite, či nemá mať predok virtuálny deštruktor
* Vždy používajte override na označenie metód
* Nerobte všetky funkcie virtuálne, C++ nie je Java, program je pomalší a málokedy to dáva zmysel

---

# `dynamic_cast<T>`

---

## `dynamic_cast<T>`

* Používa sa pri polymorfných typov na downcast
* Alebo pri viacnásovnej dedičnosti na sidecast
* Kontroluje sa počast behu programu
* Ak nemôže byť vykonaný
    * Vráti nullptr pre smerníkové typy
    * Vyhodí výnimku std::bad_cast pre referencie
* Na to aby pracoval správne, musí kompilátor podporovať RTTI (*run-time type information*)


## Použitie `dynamic_cast<T>`

```cpp
class A { };

class B : public A { };

void f(A* a) {
    B* b = dynamic_cast<B*>(a);
}
```

Neskompiluje sa, lebo `dynamic_cast` môžeme použiť iba na typy, ktoré sú polymorfné (majú tabuľku virtuálnych funkcií).


```cpp
class A { virtual ~A() { } };

class B : public A { };

void f(A* a) {
    B* b = dynamic_cast<B*>(a);
}
```

Vráti buď smerník na B, alebo nullptr, ak premenná A nenesie B, ale iba A. 

---

## Zlé pretypovanie

```cpp
class A { 
public: 
    virtual ~A() { } 
};

class B : public A { };

void f(A& a) {
    B* b1 = static_cast<B*>(&a);
    B& b2 = static_cast<B&>(a);
}
```

**`static_cast`** sa vždy podarí, ale nemusí vyrobiť požadovaný výsledok. Je na programátorovi aby sa uistil, že typ je naozaj taký, aké pretypovanie robíme. 


```cpp
class A { 
public: 
    virtual ~A() { } 
};

class B : public A { };

void f(A& a) {
    B* b3 = dynamic_cast<B*>(&a);
    B& b4 = dynamic_cast<B&>(a);
}
```

**`dynamic_cast`** kontroluje typ počas behu a vráti `nullptr` ak pretypujeme na smerník (`B*`), alebo vyhodí výnimku, ak chceme referenciu (`B&`).

---

## Dynamické `typeid`

```cpp [|1-3|5-7,15|9-11,16]
class A { };
class B : public A { public: virtual ~B() { } };
class C : public B { };

void f(const A& a) {
    std::cout << typeid(a).name() << "\n";
}

void g(const B& b) {
    std::cout << typeid(b).name() << "\n";
}

int main() {
    C c;
    f(c); // prints A
    g(c); // prints C
}
```
<!-- .element: class="showall" -->

Ak použije operátor typeid na polymorfný typ, tak sa zistenie typu odloží až na beh programu.
<!-- .element: class="fragment" -->

---

## `typeid` a `dynamic_cast`

* Obe používajú `__vfptr` na implementáciu zistenia typu
* Podľa toho aký je smerník na tabuľku virtuálnych funkcií vieme zistiť o aký typ ide
* Veľa používanie týchto funkcionalít svedčí o zlom dizajne, niekedy sa aj napriek tomu môžu hodiť

---

# ĎAKUJEM

## Otázky?