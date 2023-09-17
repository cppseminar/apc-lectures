# C++

## Scope, triedy a základy std::

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Konštanty
* Scope
* Triedy a objekty
* Životnosť premenných
* Základné objekty zo štandardnej knižnice
   * `std::vector`
   * `std::string`

---

# Konštanty

---

## Konštanty

* Z istého pohľadu programovanie je o udržiavaný invariantov a konštantnosť hodnôt a premenných môže pri tom veľmi pomôcť
* Konštantné dáta môžu byť pristupované z viacerých vlákien bez obavy o nedefinované správanie (data race)
* V C++ existuje viacero spôsobov ako definovať koncept konštanty 
   * `const`
   * `constexpr`
   * `#define`
   * `enum`

---

## `#define`

* Preprocesorové makrá môžu byť použité ako konštanty
* Makrá sú expandované ešte pred samotnou kompiláciou, fungujú preto mimo typového systému v podstate iba textovo 
* Číselné konštanty sa lepšie vyjadrujú pomocou `constexpr`, alebo `enum`-u
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

## `constexpr`

* Konštanty počas kompilácie
* Užitočné ak potrebujeme *compile time constant* (napríklad veľkosť pola)
* V niektorých kontextoch `constexpr` implikuje `const`

```cpp
const int compute_size_1(int a) { return 2 * a*a; }
constexpr int compute_size_2(int a) { return 2 * a*a; }
 
int main() {
  int x[compute_size_1(7)]; // error

  int y[compute_size_2(7)]; // OK
}
```

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
* Výnimkou sú `const` objekty, ktoré musia byť inicializované hodnotou
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

# Scope

---

## Naujužitočnejší kus kódu

<h1 class="fragment"><code>}</code></h1>

---

## Životnosť lokálnych premenných

* Lokálne premenné vo funkciách začínajú existenciu pri deklarácií a sú zničené na konci scope-u, v ktorom boli deklarované

```cpp
void f() {
    std::string s = "Normal variable";
} // s is destroyed here
```

```cpp
void f() {
    auto *s = new std::string("Leak?");
} // s is destroyed here
// !!! but because s is just a pointer
// !!! there will be a resource leak

```

```cpp
void f() {
    auto *s = new std::string("OK");
    // resource associated with *s 
    // is released by calling delete 
    delete s; 
} // s is destroyed here

```

---

## Životnosť objektov

* <small>(Skoro)</small> každý objekt začína svoju existenciu volaním konštruktora
* <small>(Skoro)</small> každý objekt končí svoju existenciu volaním deštruktora

---

## Scope

* Scope je abstraktný koncept definovaný v štandarde, ale v princípe je to všetko ohraničené `{}`

<div style="display: flex; align-items: center;">
<div style="flex: 6;">

```cpp
void f() {
  // scope A
  int i = 0;
  if (i == 4) {
    // scope B
  } else {
    // scope C
    {
      // scope D
    }
  }
  for (auto i : {"a"}) {
    // scope E
  }
}
```
</div>
<div style="flex: 4;">

![Scopes diagram](./lectures/3_scope_class/scopes.png)
</div>

---

## Viditeľnosť

* Premenné sú viazané na scope, v ktorom boli deklarované, každý vnorený scope deklarovaný neskôr môže tiež túto premennú používať 
* Kompilátor to vynucuje 
* Pravidlo sme už spomínali **Vždy deklarujte premenné v najvnorenejšom scope ako sa dá.**

---

## Prečo?

* Kompilátor vie lepšie optimalizovať 
   * Konštruktory a deštruktory sa volajú iba keď je to potrebné
* Program sa jednoduchšie číta
   * Premenné sa nepoužívajú viackrát na rôzne úlohy (toto odrádza od používania veľmi všeobecných mien)
   * Súvisiace časti programu sú viac lokalizované, tak nie je potrebne scrolovať hore dole

---

## for cyklus

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
for (int i = 0; i < 100; ++i) {

}
```
</div>
<div style="flex: 1;">

```cpp
{
    int i = 0;
    for (; i < 100; ++i) {

    }
}
```
</div>
</div>

* Kusy kódu vyššie sú ekvivalentné
* Prvý je silno preferovaný a nie je dôvod používať druhý

<div class="fragment" style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
for (;;) {

}
```
</div>
<div style="flex: 1;">

```cpp
while (true) {

}
```
</div>
</div>

<div class="fragment">

* Pri tomto nekonečnom cykle si môžeme vybrať, oba spôsoby sú v poriadku
</div>

---

# Objektovo orientované programovanie

---

## Trieda (`class`)

* Šablóna pre vytváranie nových objektov (inštancií)
* Statické počas kompilácie, nové triedy sa nedajú vytvárať počas behu programu
* Obsahujú dáta (fields) a metódy (methods)
* Dáta sú zapuzdrené, teda ich životnosť je naviazaná na životnosť objektu, v ktorom sú definované
* `operator.` sa používa na prístup k členom 
* V podstate to isté ako `struct`
* Hlavný cieľ tried je stráženie invariantov

---

## Hlavičkový súbor / interface

```cpp
class widget { // meno triedy
public: // modifikator pristupu
  widget(const char* s); // konstruktor
  ~widget(); // destruktor
 
  int observe() const; // metoda
  void mutate(int x); // metoda
 
private: // modifikator pristupu
  int data = 0; // data, instancne premenne
  std::string str;
}; // ; je velmi dolezita
```

* Triedy majú konštruktory a deštruktory, ktoré sa volajú pri vytvorení alebo deštrukcií
* Metódy sú funkcie, ktoré implicitne dostanú smerník na objekt, nad ktorým boli zavolané
* Inštančné premenné sú dáta, ktoré sú spojené s daným objektom
* Ak nedáte na koniec `;`, neskôr v súbore sa bude kompilátor sťažovať

---

## Zdrojový súbor / implementácia

```cpp
widget::widget(const char* s)
    : data(10) // inicializacny list v konstruktor
    , str(s) {
}
 
widget::~widget() {
    std::cout << "~widget" << '\n';
}
 
// konstantne metody nemozu menit stav objektu
int widget::observe() const {
    // data++; // error
    std::cout << data << '\n';
    return data;
}
 
void widget::mutate(int x) {
    data += x; // OK
    std::cout << data << '\n';
}
```

---

## Inicializačný list v konštruktore

* Miesto na volanie konštruktorov členských premenných
* Môže závisieť od parametrov konštruktora

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class foo {
public:
  foo(int x) {
    // s is default constructed 
    i = x;
    s = std::to_string(x);
    f = x;
  }
private:
  std::string s;
  int i;
  float f;
};
```
</div>
<div style="flex: 1;">

```cpp
class foo {
public:
  foo(int x)
    : i(x)
    , s(std::to_string(x))
    , f(x) {
    // everything inside class is constructed
    // add other steps required for class to
    // function properly
  }
private:
  std::string s;
  int i;
  float f;
};

```
</div>
</div>

---

## Metódy objektov

* Obyčajné funkcie s jedným implicitným parametrom `this` (smerník na inštanciu objektu)
* Vnútri metódy sa dajú referencovať všetky členské premenné

```cpp
class string_index {
public:
  void set_index(int i) {
    this->i = i;
    s = std::to_string(i);
  }

  void print() const {
    // i = 0; or whatever is error
    std::cout << s << '\n';
  }
private:
  int i = 0;
  std::string s;
};
```

* Nejednoznačnosť je vyriešená explicitným použitím this smerníka.
* Konštantné funkcie majú konštantný this smerník, preto nevedia meniť členské premenné.

---

## Get a Set metódy

* Tiež sa volajú mutator a accessor
* Zapuzdrujú členské premenné
* V iných jazykoch sa volajú aj properties

```cpp
class person {
public:
  person(const std::string& name) 
    : name(name) { }
 
  int get_age() const { return age; }
  void set_age(int age) { 
    assert(age >= 0); 
    this->age = age; 
  }
 
  // readonly property
  const std::string& get_name() const { return name; }
private:
  int age = 0;
  std::string name;
};
```

* Ak nedovolíme priamy prístup k premenným, tak môžeme vynútiť kontroly, alebo závislosti


## `const std::string&`?

* Na predchádzajúcom slide je uvedená konštantná referencia na štandardný `string`
* Zatiaľ nás nemusí trápiť, čo to presne je, iba si môžete zapamäť, že ak do funkcie dávam std::string (resp. akýkoľvek zložitý objekt), tak ho predám ako `const&`, kód bude výrazne rýchlejší
* Nemusí sa kopírovať celý objekt

<div class="fragment" style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
void f(std::string s) {
  // string s sa vzdy skopiruje
}
```
</div>
<div style="flex: 1;">

```cpp
void f(const std::string& s) {
  // string s sa nekopiruje, iba sa presunie smernik
}
```
</div>
</div>

---

## Vytváranie nových inštancií 

* Ak aspoň jeden konštruktor úspešne skončí (nevyvolá výnimku), potom je objekt považovaný za skonštruovaný

```cpp
class object {
public:
    object();
    object(int i);
    object(const std::string& s);
};
 
int main() {
    object a; // call to object::object()
    object b(0); // call to object::object(int)
    object c("string"); // call to object::object(const std::string&)
}
```

---

## Deštruovanie inštancií

* Akonáhle má byť inštancia zdeštruovaná (skončila sa jej životnosť), tak sa zavolá deštruktor
* Trieda môže mať ľubovoľne veľa konštruktorov, ale iba jeden deštruktor

```cpp
class object {
public:
    object() { std::cout << "object" << '\n'; }
    object(int) { std::cout << "object(int)" << '\n'; }
    ~object() { std::cout << "~object" << '\n'; }
};
 
int main() {
    object a, b{ 1 }, c(1);
    // destructors are called in reverse c, b, a
}
```

---

## C++ most vexing parse

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class A {
public:
  A() { std::cout << "A"; }
  ~A() { std::cout << "~A"; }
};
int main() { A a(); }
```
</div>
<div style="flex: 1;">

Nová premenná typu `A` s menom `a` je skonštruovaná volaním `A::A()`.
</div>
</div>

<div class="fragment">

*Scott Meyers, 2001*

Vlastne je to deklarácia funkcie s menom `a`, ktorá nemá žiaden parameter `a` vracia objekt typu `A`.

Väčšina programátorov očakáva nový objekt, štandard ale vyžaduje deklaráciu funkcie. 

`A a{};` funguje správne.
</div>

---

## Explicitné konštruktory

* Ak konštruktor triedy `C` obsahuje iba jeden parameter typu `T`, potom tento konštruktor sa môže použiť  a implicitnú konverziu z `T` na `C`

```cpp
class Convert {
public:
    Convert(int i) { std::cout << i << '\n'; }
    explicit Convert(std::string f) { std::cout << f << '\n'; }
};
 
int f(const Convert&);
 
int main() {
    f(15); // OK, Convert::Convert(int) is used as conversion
    f(5.4); // OK (warning), :(
    // f(std::string("abc")); will not compile
}
```

* Pravidlo: Všetky jednoparametrové konštruktory označíme automaticky slovom `explicit`.

---

## Modifikátory prístupu

* `public` Každý môže pristupovať k danej premennej, alebo metóde.
* `protected` Iba zdedené triedy a priatelia môžu pristupovať.
* `private` Pristupovať môže iba samotná trieda, *alebo priatelia*.

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class A {
public:
    A(int n) : data(n) { }
    int get_data() const { return data; }
    int tag;
private:
    int data = 0;
    friend void modify(A& a, int n);
};

void modify(A& a, int n) {
    a.data = n;
}
```
</div>
<div style="flex: 1;">

```cpp
int main() {
    A a(10);
    a.tag = 10;
    std::cout << a.GetData(); // 10
    // a.data = 11; // error
    Modify(a, 11);
    std::cout << a.GetData(); // 11
}
```
</div>
</div>

`friend` trieda, alebo funkcia môže pristupovať ak k privátnym dátam. 

---

## Aký je rozdiel medzi class a struct v C++?

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class MyClass
{
    // ...
};
```
</div>
<div style="flex: 1;">

```cpp
struct MyClass
{
    // ...
};
```
</div>
</div>

<div class="fragment">

Existuje iba jeden malý rozdiel. Všetky členy v štruktúre sú predvolene `public`, v triede `private`.

Niektorí programátori používajú stále triedy, iný preferujú štruktúry pre *POD typy (plain old data)* a urobia všetko `public`.
</div>

---

## Skrývanie informácie

* Information hiding
* Triedy poskytujú *interface* a skrývajú všetky *implementačné detaily* (všetky členské premenné by mali byť privátne)
* Používateľov tried by mali zaujímať iba verejné časti a nikdy by sa nemali chytať vnútornosti tried
* Pokiaľ niečo nie je explicitne napísané, treba predpokladať, že je to nedefinované

---

# Triedy a scope

---

## Život objetku

* Vždy keď sa má objekt vytvoriť zavolá sa definovaný konštruktor
* Ak je konštrukcia úspešná, garantovanie sa nám zavolá deštruktor na konci života objektu

---

## Kde je bug v nasledujúcom kuse kódu?

```cpp
int main() {
  std::string *ptr = nullptr;
    
  int i;
  if (std::cin >> i) {
    std::string s = std::to_string(55);
    s += std::to_string(i);
 
    ptr = &s;
  }
 
  if (ptr != nullptr)
    cout << *ptr;
}
```

<div class="fragment">

Na konci `if`u (tesne pred `}`) mieste je reťazec `s` zdeštruovaný a teda všetky referencie a smerníky sú neplatné a nesmú sa použiť. 

Test na `nullptr` stále prejde, je na programátorovi aby toto urobil dobre.
</div>

---

## Členy v triedach

* Ich život je zviazaný (bound to) so životom objektu, ktorý ich obsahuje
* Aké je poradie deštrukcie členov triedy?
* (Je to poradie v súbore, alebo v inicializačnom liste, alebo nešpecifikované...)

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class B {
public:
  B() : y("Y"), x("X") { 
    std::cout << "B";
  }
  ~B() {
    std::cout << "~B";
  }
private:
  A x;
  A y;
};
```
</div>
<div style="flex: 1;">

```cpp
class A {
public:
  A(const char* v) : s(v) {
    std::cout << s;
  }
  ~A() { std::cout << "~" << s; }
private:
  const char* s;
};
```
</div>
</div>

<div class="fragment">

* X Y B ~B ~Y ~X
* Poradie je rovnaké ako v súbore
* Najprv sa skonštruujú členy, potom sa zavolá samotný konštruktor, deštrukcia je potom v opačnom poradí
</div>

---

# Štandardná knižnica

---

# Prehľad

* Štandard má okolo 1500 strán a väčšina je venovaná opisu štandardnej knižnice
* Obsahuje všetko, čo C knižnica
* Vstup a výstup spolu s lokalizáciou
* Podpora vlákien a atomických premenných
* Matematické operácie
* Regulárne výrazy
* ...
* Kontainery a algoritmy

---

## STL

<div style="display: flex; align-items: center;">
<div style="flex: 2;">

* Standard Template Library
* **Alexander Stephanov** (1979)
* Formálny návrh na pridanie STL do C++ knižnice v roku 1994
* Generické programovanie bez najmenšej straty efektívnosti
</div>
<div style="flex: 1;">
<div style="width: 25vw; margin: auto;">
    <img src="./lectures/3_scope_class/Alexander_Stepanov.jpg" style="width: 100%; margin-bottom: 0;" />
    <div style="font-size: 3vh; text-align: right;">
        Source <a href="https://en.wikipedia.org/wiki/Alexander_Stepanov#/media/File:Alexander_Stepanov.jpg">Wiki</a>
    </div>
</div>
</div>
</div>

> STL ≠ C++ Standard Library

---

## Kontainery a algoritmy

![Iterators between containers and algorithms](./lectures/3_scope_class/iterators.png)

* Interátory poskytujú jednotný interface na prácu s kontainermi, ten potom algoritmy využívajú 
* Ak máme n kontainerov a m algoritmov

<p style="font-size: larger; text-align: center;">
  <b>O(n+m) vs O(nm)</b>
</p>

---

## Kontainery

* vector<T>
* array<T, N>
* map<T>, set<T>
* multimap<T>
* list<T>, forward_list<T>
* unordered_map<T>
* string

---

# vector

---
