# C++

## Scope, triedy a základy std::

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Scope
* Triedy a objekty
* Životnosť premenných
* Konštanty
* Základné objekty zo štandardnej knižnice
   * `std::vector`
   * `std::string`

---

# Scope

---

## Najužitočnejší kus kódu

<h1 class="fragment"><code>}</code></h1>

---

## Životnosť lokálnych premenných

* Lokálne premenné s automatickou dobou uloženia začínajú existovať pri deklarácii a sú zničené na konci scope-u, v ktorom boli deklarované

```cpp
void f() {
  std::string s = "Normal variable";
} // s is destroyed here
```

```cpp
void f() {
  auto *s = new std::string("Leak?");
} // s is destroyed here
// !!! but because s is just a pointer
// !!! there will be a resource leak

```

```cpp
void f() {
  auto *s = new std::string("OK");
  // resource associated with *s 
  // is released by calling delete 
  delete s; 
} // s is destroyed here

```

---

## Životnosť objektov

* <small>(Skoro)</small> každý objekt začína svoju existenciu volaním konštruktora
* <small>(Skoro)</small> každý objekt končí svoju existenciu volaním deštruktora

---

## Scope

* Scope je abstraktný koncept definovaný v štandarde
* Veľmi zhruba je to všetko ohraničené `{}`

<div style="display: flex; align-items: center;">
<div style="flex: 6;">

```cpp
void f() {
  // scope A
  int i = 0;
  if (i == 4) {
    // scope B
  } else {
    // scope C
    {
      // scope D
    }
  }
  for (auto i : {"a"}) {
    // scope E
  }
}
```
</div>
<div style="flex: 4;">

![Scopes diagram](./lectures/3_scope_class/scopes.png)
</div>


## Globálny scope

* Každá translation unit (v podstate jeden súbor zdrojového kódu) má svoj vlastný globálny scope
* Deklarácie mimo funkcií, tried a iných vnorených scope-ov patria do globálneho scope-u od bodu svojej deklarácie

```cpp
int i = 0; // in global scope

int main() { // in global scope
  int j = 0; // in scope of function main
}
```


## Vnorené scope-y a viditeľnosť

* Meno je viditeľné od svojej deklarácie po koniec scope-u
* Vnorený scope môže používať mená z vonkajšieho scope-u
* Po opustení scope-u už jeho lokálne mená nie sú dostupné

```cpp
void f() {
  // i is not visible here
  int i = 0;

  if (i == 0) {
    int result = i + 1; // i and result are visible
  }

  // result is not visible here
}
```

---

## Deklarujte premenné čo najlokálnejšie

```cpp
void print_result(bool enabled) {
  if (enabled) {
    const std::string result = compute_result();
    std::cout << result;
  }
}
```

* Premenná existuje iba tam, kde je potrebná
* Jej význam vidíme spolu s deklaráciou
* Nemôže sa omylom použiť v nesúvisiacej časti funkcie
* Konštruktor a deštruktor sa zavolajú iba vo vetve, ktorá premennú používa

**Pravidlo:** Premennú deklarujte v najvnorenejšom scope a čo najbližšie k prvému použitiu.

---

## for cyklus

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
for (int i = 0; i < 100; ++i) {

}
```
</div>
<div style="flex: 1;">

```cpp
{
    int i = 0;
    for (; i < 100; ++i) {

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
for (;;) {

}
```
</div>
<div style="flex: 1;">

```cpp
while (true) {

}
```
</div>
</div>

<div class="fragment">

* Pri tomto nekonečnom cykle si môžeme vybrať; oba spôsoby sú v poriadku
</div>

note: nekonečný cyklus je inak viacmenej undefined

---

# Objektovo orientované programovanie

---

## Trieda (`class`)

* Šablóna pre vytváranie nových objektov (inštancií)
* Triedy sú statické počas kompilácie; nové triedy sa nedajú vytvárať počas behu programu
* Obsahujú dáta (fields) a metódy (methods)
* Životnosť členských objektov je vďaka kompozícii naviazaná na životnosť objektu, v ktorom sú definované
* `operator.` sa používa na prístup k členom
* V podstate ide o to isté ako `struct`
* Hlavný cieľ tried je stráženie invariantov

---

## Hlavičkový súbor / rozhranie

```cpp
class widget { // class name
public: // access modifier
  widget(const char* s); // constructor
  ~widget(); // destructor
 
  int observe() const; // method
  void mutate(int x); // method
 
private: // access modifier
  int data = 0; // data, instance variables
  std::string str;
}; // ; is very important
```

* Triedy majú konštruktory a deštruktory, ktoré sa volajú pri vytvorení alebo deštrukcii objektu
* Metódy sú funkcie, ktoré implicitne dostanú smerník na objekt, nad ktorým boli zavolané
* Inštančné premenné sú dáta, ktoré sú spojené s daným objektom
* Ak nedáte na koniec `;`, neskôr v súbore sa bude kompilátor sťažovať

---

## Zdrojový súbor / implementácia

```cpp
widget::widget(const char* s)
  : data(10) // constructor initializer list
  , str(s) {
}
 
widget::~widget() {
  std::cout << "~widget" << '\n';
}
 
// const methods cannot modify the object state
int widget::observe() const {
  // data++; // error
  std::cout << data << '\n';
  return data;
}
 
void widget::mutate(int x) {
  data += x; // OK
  std::cout << data << '\n';
}
```

---

## Inicializačný list v konštruktore

* Miesto na volanie konštruktorov členských premenných
* Môže závisieť od parametrov konštruktora

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class foo {
public:
  foo(int x) {
    // s is default constructed 
    i = x;
    s = std::to_string(x);
    f = x;
  }
private:
  std::string s;
  int i;
  float f;
};
```
</div>
<div style="flex: 1;">

```cpp
class foo {
public:
  foo(int x)
    : i(x)
    , s(std::to_string(x))
    , f(x) {
    // everything inside class is constructed
    // add other steps required for class to
    // function properly
  }
private:
  std::string s;
  int i;
  float f;
};

```
</div>
</div>

---

## Metódy objektov

* Obyčajné funkcie s jedným implicitným parametrom `this` (smerník na inštanciu objektu)
* Vnútri metódy sa dajú referencovať všetky členské premenné

```cpp
class string_index {
public:
  void set_index(int i) {
    this->i = i;
    s = std::to_string(i);
  }

  void print() const {
    // i = 0; or whatever is error
    std::cout << s << '\n';
  }
private:
  int i = 0;
  std::string s;
};
```

* Nejednoznačnosť je vyriešená explicitným použitím smerníka `this`.
* Konštantné funkcie majú konštantný smerník `this`, preto nemôžu meniť členské premenné.

---

## Metódy get a set

* Nazývajú sa aj mutator a accessor
* Zapuzdrujú členské premenné
* V iných jazykoch sa volajú aj properties

```cpp
class person {
public:
  person(const std::string& name) 
    : name(name) { }
 
  int get_age() const { return age; }
  void set_age(int age) { 
    assert(age >= 0); 
    this->age = age; 
  }
 
  // readonly property
  const std::string& get_name() const { return name; }
private:
  int age = 0;
  std::string name;
};
```

* Ak nedovolíme priamy prístup k premenným, môžeme vynútiť kontroly alebo závislosti


## `const std::string&`?

* Na predchádzajúcej snímke je uvedená konštantná referencia na štandardný `string`
* Ak funkcia objekt iba číta, odovzdanie cez `const&` zabráni kopírovaniu. Odovzdanie hodnotou však môže byť vhodnejšie pre malé typy, pri presune alebo ak funkcia potrebuje vlastnú kópiu
* Výber medzi hodnotou a `const&` závisí od typu a zamýšľaného použitia

<div class="fragment" style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
void f(std::string s) {
  // string s is always copied
}
```
</div>
<div style="flex: 1;">

```cpp
void f(const std::string& s) {
  // string s is not copied, only a pointer is passed
}
```
</div>
</div>

---

## Vytváranie nových inštancií 

* Ak aspoň jeden konštruktor úspešne skončí (nevyvolá výnimku), potom je objekt považovaný za skonštruovaný

```cpp
class object {
public:
  object();
  object(int i);
  object(const std::string& s);
};
 
int main() {
  object a; // call to object::object()
  object b(0); // call to object::object(int)
  object c("string"); // call to object::object(const std::string&)
}
```

---

## Deštruovanie inštancií

* Hneď ako má byť inštancia zdeštruovaná (skončila sa jej životnosť), zavolá sa deštruktor
* Trieda môže mať ľubovoľne veľa konštruktorov, ale iba jeden deštruktor

```cpp
class object {
public:
  object() { std::cout << "object" << '\n'; }
  object(int) { std::cout << "object(int)" << '\n'; }
  ~object() { std::cout << "~object" << '\n'; }
};
 
int main() {
  object a, b{ 1 }, c(1);
  // destructors are called in reverse c, b, a
}
```

---

## C++ most vexing parse

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class A {
public:
  A() { std::cout << "A"; }
  ~A() { std::cout << "~A"; }
};
int main() { A a(); }
```
</div>
<div style="flex: 1;">

Nová premenná typu `A` s menom `a` je skonštruovaná volaním `A::A()`.
</div>
</div>

<div class="fragment">

*Scott Meyers, 2001*

V skutočnosti je to deklarácia funkcie s menom `a`, ktorá nemá žiadny parameter a vracia objekt typu `A`.

Väčšina programátorov očakáva nový objekt, štandard však vyžaduje deklaráciu funkcie. 

`A a{};` funguje správne.
</div>

---

## Explicitné konštruktory

* Ak konštruktor triedy `C` obsahuje iba jeden parameter typu `T`, môže sa použiť na implicitnú konverziu z `T` na `C`

```cpp
class Convert {
public:
  Convert(int i) { std::cout << i << '\n'; }
  explicit Convert(std::string f) { std::cout << f << '\n'; }
};
 
int f(const Convert&);
 
int main() {
  f(15); // OK, Convert::Convert(int) is used as conversion
  f(5.4); // OK (warning), :(
  // f(std::string("abc")); will not compile
}
```

* Pravidlo: Všetky jednoparametrové konštruktory označíme automaticky slovom `explicit`.

---

## Modifikátory prístupu

* `public` – Každý môže pristupovať k danej premennej alebo metóde.
* `protected` – Pristupovať môže samotná trieda, jej priatelia a za určitých podmienok aj odvodené triedy.
* `private` – Pristupovať môže iba samotná trieda *alebo priatelia*.

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
class A {
public:
  A(int n) : data(n) { }
  int get_data() const { return data; }
  int tag;
private:
  int data = 0;
  friend void modify(A& a, int n);
};

void modify(A& a, int n) {
  a.data = n;
}
```
</div>
<div style="flex: 1;">

```cpp
int main() {
  A a(10);
  a.tag = 10;
  std::cout << a.get_data(); // 10
  // a.data = 11; // error
  modify(a, 11);
  std::cout << a.get_data(); // 11
}
```
</div>
</div>

`friend` trieda alebo funkcia môže pristupovať aj k privátnym dátam. 

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

Niektorí programátori vždy používajú triedy, iní preferujú štruktúry pre *POD typy (plain old data)* a všetko označia ako `public`.
</div>

---

## Skrývanie informácie

* Information hiding
* Triedy poskytujú *rozhranie* a skrývajú všetky *implementačné detaily* (všetky členské premenné by mali byť privátne)
* Používateľov tried by mali zaujímať iba verejné časti a nikdy by nemali pristupovať k vnútornostiam tried
* Pokiaľ niečo nie je explicitne napísané, treba predpokladať, že je to nedefinované

---

# Triedy a scope

---

## Život objektu

* Vždy, keď sa má objekt vytvoriť, zavolá sa definovaný konštruktor
* Ak je konštrukcia úspešná, pri bežnom ukončení scope sa zavolá deštruktor; nútené ukončenie programu, napríklad cez `std::abort`, deštruktory automatických objektov nevolá

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
    std::cout << *ptr;
}
```

<div class="fragment">

Na konci `if`u (tesne pred `}`) je reťazec `s` zdeštruovaný, a teda všetky referencie a smerníky naň sú neplatné a nesmú sa použiť.

Test na `nullptr` stále prejde. Je na programátorovi, aby to urobil správne.
</div>

---

## Členy v triedach

* Ich život je zviazaný (bound to) so životom objektu, ktorý ich obsahuje
* Aké je poradie deštrukcie členov triedy?
* (Je to poradie v súbore, v inicializačnom liste alebo je nešpecifikované...)

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

# Konštanty

---

## Konštanty

* Z istého pohľadu je programovanie o udržiavaní invariantov a konštantnosť hodnôt a premenných pri tom môže veľmi pomôcť
* Ku konštantným dátam možno pristupovať z viacerých vlákien bez obavy z nedefinovaného správania (data race)
* V C++ existuje viacero spôsobov, ako definovať koncept konštanty 
   * `const`
   * `constexpr`
   * `#define`
   * `enum`

---

## `#define`

* Preprocesorové makrá možno použiť ako konštanty
* Makrá sú expandované ešte pred samotnou kompiláciou, preto fungujú mimo typového systému v podstate iba textovo 
* Číselné konštanty sa lepšie vyjadrujú pomocou `const` alebo `enum`-u
* Občas užitočné pri reťazcoch 
   * Zreťazovanie (concatenation) počas kompilácie

```cpp
#define DIRECTORY "C:"
#define FILENAME "log.txt"
#define SEPARATOR "\\"
#define PATH DIRECTORY SEPARATOR FILENAME
 
int main() {
  std::cout << PATH << std::endl; // "C:\log.txt"
}

```

---

## `const`

* `const` znamená, že "objekt" sa nesmie meniť
* `const_cast` môže odstrániť `const` z prístupovej cesty, ale zmena objektu, ktorý bol deklarovaný ako `const`, má nedefinované správanie
* PROTIP: Nikdy nezahadzujte `const` z objektov
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

* Objekty so statickou dobou uloženia (napr. globálne premenné) sú pred ďalšou inicializáciou nulovo inicializované (*zero initialized*)
* `const` skalárne objekty musia mať inicializátor; triedny 
* `const` objekty môžu byť umiestnené do pamäte iba na čítanie; pokus o ich zmenu má nedefinované správanie a môže spôsobiť access violation

```cpp
int v;
const int c = 1;
 
int main() {
  std::cout << v << " " << c << std::endl; // 0 1
  
  //c = 2; // will not compile
  // *const_cast<int*>(&c) = 4; // compiles, but writing to c is undefined behavior
}
```

---

## `constexpr`

* Idea `constexpr` bola, že umožníme kompilátoru vyhodnotiť niektoré výrazy počas kompilácie
   * Vieme získať *compile time constant*, ktorú vieme napríklad použiť ako veľkosť pola
  * Nemusíme hodnoty predpočítavať ručne, ale môžeme to nechať na kompilátor
   * Kedysi sa na to používali šablóny, ale to bolo veľmi nepraktické a neprehľadné
* V novších verziách C++ sa `constexpr` rozširovalo a odstraňovali sa obmedzenia

```cpp
constexpr size_t a = 10;
```


## `constexpr` a `const`

* Pri deklaráciách premenných `constexpr` implikuje `const`
* Nasledujúce dva zápisy sú ekvivalentné

```cpp
constexpr size_t a = 10;
```

```cpp
constexpr const size_t a = 10;
```

* Pozor pri smerníkoch a referenciách: `constexpr` smerník je sám konštantný, ale objekt, na ktorý ukazuje, nemusí byť `const`
* Konštantný výraz môže obsahovať adresu objektu so statickou dobou uloženia alebo adresu funkcie
* Od C++20 možno počas konštantného vyhodnocovania použiť aj dynamickú alokáciu, ale alokovaná pamäť sa musí v tom istom vyhodnotení uvoľniť a jej adresa nesmie uniknúť do runtime

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
int a;

constexpr int* p = &a; // OK
```
</div>
<div style="flex: 1;">

```cpp
const int a = 0;

constexpr int* p = &a; // error
const int* q = &a; // OK
int * const r = &a; // error, similar to constexpr
```
</div>
</div>


## `constexpr` funkcie

* `constexpr` funkcie sú funkcie, ktoré môžu byť vyhodnotené počas kompilácie
* V C++11 boli veľmi obmedzené: mohli obsahovať iba jeden return statement a iba niektoré operácie
* V C++14 sa to značne zlepšilo a stále sa to rozširuje
* V C++20 už existuje veľmi veľa `constexpr` funkcií (konštruktory stringov, vektorov, ...)

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

```cpp
constexpr bool is_prime(uint32_t num) {
  if (num < 2) 
    return false;
  for (uint32_t i = 2; i * i <= num; ++i) {
    if (num % i == 0) 
      return false;
  }
  return true;
}
```
</div>
<div style="flex: 1;">

```cpp
constexpr size_t count_primes(uint32_t n) {
  size_t count = 0;
  for (uint32_t i = 2; i <= n; ++i) {
    if (is_prime(i)) {
      ++count;
    }
  }
  return count;
}
```
</div>
</div>


```cpp
constexpr size_t n = count_primes(100); // 25
constexpr size_t large_count = count_primes(10000000); // may exceed implementation limits

int x[n]; // OK
int y[count_primes(5)]; // OK
```


## `constexpr` funkcie v nekonštantnom kontexte

* `constexpr` funkcie môžu byť volané aj s runtime hodnotami
* Pokiaľ nenútime kompilátor, aby vyhodnotil výraz počas kompilácie, môže sa rozhodnúť, či ho vyhodnotí počas kompilácie alebo počas behu programu
* Ako kompilátor donútiť?
  * Výsledok priradíme do `constexpr` premennej
   * Výsledok použijeme ako veľkosť poľa

```cpp
int main() {
  size_t x;
  std::cin >> x;

  std::cout << count_primes(x) << '\n'; // OK
  std::cout << count_primes(10000000) << '\n'; // OK
  // int x[count_primes(x)]; // error
}
```

note: Máme urobiť všetky funkcie `constexpr`? Asi nie, ale... podobne ako urobiť všetko `const`...


## `constexpr` konštruktory?

* Pred C++20 vedelo `constexpr` v podstate simulovať iba stack
* Mohli sme deklarovať premenné, ale nie priamo alokovať pamäť na heape
* V C++20 sa to zmenilo a môžeme alokovať pamäť na heape, ale nemôžeme posunúť adresu do runtime

```cpp
//constexpr std::vector<int> v{ 1, 2, 3}; // error, will leak address to runtime
constexpr std::vector<int> w; // OK, no allocation
```

* Funkcie môžu byť komplexné, napríklad môžu predpočítavať tabuľky alebo iné dáta

```cpp
constexpr std::vector<uint32_t> get_primes(uint32_t n) {
  std::vector<uint32_t> result;
  
  for (uint32_t i = 0; i < n; ++i) {
    if (is_prime(i)) {
      result.push_back(i);
    }
  }

  return result;
}

int a[get_primes(100).size()]; // OK, no pointer leak to runtime
```

note: <https://quuxplusone.github.io/blog/2023/09/08/constexpr-string-firewall/>

---

# Štandardná knižnica

---

# Prehľad

* Štandard má okolo 1500 strán a väčšina je venovaná opisu štandardnej knižnice
* Obsahuje všetko, čo obsahuje knižnica jazyka C
* Vstup a výstup spolu s lokalizáciou
* Podpora vlákien a atomických premenných
* Matematické operácie
* Regulárne výrazy
* ...
* Kontajnery a algoritmy

---

## STL

<div style="display: flex; align-items: center;">
<div style="flex: 2;">

* Standard Template Library
* **Alexander Stepanov** (1979)
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

## Kontajnery a algoritmy

![Iterators between containers and algorithms](./lectures/3_scope_class/iterators.png)

* Iterátory poskytujú jednotné rozhranie na prácu s kontajnermi, ktoré potom využívajú algoritmy 
* Ak máme n kontajnerov a m algoritmov

<p style="font-size: larger; text-align: center;">
  <b>O(n+m) vs O(nm)</b>
</p>

---

## Kontajnery

* `vector<T>`
* `array<T, N>`
* `map<Key, T>`, `set<Key>`
* `multimap<Key, T>`
* `list<T>`, `forward_list<T>`
* `unordered_map<Key, T>`
* `string`

---

# vector

---

## `std::vector<T>`

* Abstrakcia nad dynamickým poľom (`T` musí spĺňať požiadavky operácií, ktoré nad vectorom používame; nemôže byť napríklad referencia ani `void`)
* Garantovane súvislá pamäť (požiadavka platí spätne od C++98 cez LWG 69)
* Vector sa stará o alokáciu svojej pamäte, automaticky ju zväčšuje, keď treba, a dealokuje ju v deštruktore 

<table style="font-size: 70%;">
  <tr>
    <th>Operácia</th>
    <th>Zložitosť</th>
    <th>Poznámka</th>
  </tr>
  <tr>
    <td>insert</td>
    <td>O(n)</td>
    <td>O(1) amortizovane pri vkladaní na koniec</td>
  </tr>
  <tr>
    <td>erase</td>
    <td>O(n)</td>
    <td></td>
  </tr>
  <tr>
    <td>search</td>
    <td>O(n)</td>
    <td></td>
  </tr>
  <tr>
    <td>access</td>
    <td>O(1)</td>
    <td><code>operator[]</code></td>
  </tr>
</table>

---

## Expanzia vectora

* Vector štandardne narastá v násobkoch
   * 2 gcc a clang
   * 1.5 MSVC
* Operácie `resize`, `erase` a `clear` kapacitu nezmenšujú. `shrink_to_fit` môže kapacitu zmenšiť, ale je to iba nezáväzná požiadavka
* Ak nastane realokácia, každý iterátor (smerník, referencia), ktorý ukazoval na prvok vectora, je neplatný
* Preto je veľmi zlé manipulovať s vectorom počas iterovania cez neho
* Skoro vždy chcete použiť vector

---

## Reportovanie chýb

* Ak zlyhá alokácia pamäte, vector vyhodí výnimku `std::bad_alloc`
* Použitie neplatného iterátora alebo indexovanie mimo hraníc má nedefinované správanie
* Funkcia `at` robí to isté ako `operator[]`, ale vyhadzuje výnimku `std::out_of_range`

```cpp
std::vector<int> vec = { 1, 2, 3, 4, 5 };
vec.resize(100); // may throw bad_alloc

vec.at(200) = 0; // will throw out_of_range
vec[200] = 0; // undefined
```

---

## Preťažovanie funkcií

![vector insert overloads from cppreference.com](./lectures/3_scope_class/insert_overload.png)
<!-- .element: class="stretch" -->

* Môžu existovať funkcie s rovnakým názvom, ale rôznymi parametrami
* Kompilátor potom vyberie správnu funkciu na základe parametrov volania
* V štandarde je veľa preťažených funkcií
* Neodporúča sa to preháňať; štandard tvoria desiatky odborníkov a aj tak sa niekedy pomýlia

---

## Operácie nad vectorom

<table style="font-size: 70%;">
  <tr>
    <th>Metóda</th>
    <th>Zložitosť</th>
    <th>Poznámka</th>
  </tr>
  <tr>
    <td><code>push_back</code></td>
    <td>O(1)</td>
    <td>Vloží prvok na koniec vectora, amortizovaná zložitosť</td>
  </tr>
  <tr>
    <td><code>insert</code></td>
    <td>O(n)</td>
    <td>Vloží prvok(y) na zadanú pozíciu definovanú iterátorom</td>
  </tr>
  <tr>
    <td><code>erase</code></td>
    <td>O(n)</td>
    <td>Zmaže prvok/prvky definované iterátorom</td>
  </tr>
  <tr>
    <td><code>empty</code></td>
    <td>O(1)</td>
    <td>Test, či je vector prázdny</td>
  </tr>
  <tr>
    <td><code>front</code>/<code>back</code></td>
    <td>O(1)</td>
    <td>Vráti prvý/posledný prvok; ak je vector prázdny, správanie je nedefinované</td>
  </tr>
  <tr>
    <td><code>size</code>/<code>capacity</code></td>
    <td>O(1)</td>
    <td>Vráti veľkosť/kapacitu vectora</td>
  </tr>
  <tr>
    <td><code>resize</code>/<code>reserve</code></td>
    <td>O(n)</td>
    <td>Zmení veľkosť/kapacitu vectora</td>
  </tr>
  <tr>
    <td><code>clear</code></td>
    <td>O(n)</td>
    <td>Odstráni všetky prvky z vectora, ale nezmení jeho kapacitu; štandard garantuje O(n), väčšinou O(1) pre primitívne typy</td>
  </tr>
  <tr>
    <td><code>shrink_to_fit</code></td>
    <td>O(n)</td>
    <td>Požiada o zmenšenie kapacity; požiadavka nemusí byť splnená, zložitosť je najviac O(n), väčšinou O(1) pre primitívne typy</td>
  </tr>
  <tr>
    <td><code>begin</code>/<code>end</code></td>
    <td>O(1)</td>
    <td>Podpora iterátorov</td>
  </tr>
</table>


## Príklad

```cpp
int main(int argc, char* argv[]) {
  std::vector<int> params;
  params.reserve(argc - 1);
  for (int i = 1; i < argc; ++i) {
    params.push_back(strtol(argv[i], nullptr, 0));
  }

  for (size_t i = 0; i < params.size(); ++i) {
    if (params[i] < 0) // abs
      params[i] = -params[i];
  }

  params.resize(5); // if more than 5, shrink, otherwise pad with 0
  params.insert(params.end(), { 43, 44 }); // insert overload with initializer_list

  for (const auto& i : params) {
    std::cout << i << " ";
}
  // vector automatically deallocates memory
}
```


## Nedefinované správanie

```cpp
std::vector<int> v = { 1, 2, 5, 8};
for (int i : v) {
  if (i % 2 != 0) {
    v.push_back(i);
  }
}

for (auto i = v.begin(); i != v.end(); ++i) {
  if (*i % 2 != 0) {
    v.push_back(*i);
  }
}
```

Oba cykly spôsobujú nedefinované správanie, takže ich výsledok ani ukončenie nemožno predpovedať. 

---

## Pamäť vectora

![Memory layout of vector](./lectures/3_scope_class/vector_memory.png)
<!-- .element: class="stretch" -->

---

# string

---

## `std::basic_string<T>`

* Reprezentuje jeden reťazec znakov
* Neexistuje Unicode podpora
* Stále je to STL kontajner, takže na ňom všetky algoritmy pracujú správne
* String vie svoju veľkosť a kapacitu
* V podstate je to taký lepší std::vector<char> s pár funkciami navyše
* `std::string` je `std::basic_string<char>`


## Operácie

* Väčšina toho, čo podporuje `vector`, je prítomná s rovnakou sémantikou
    * `push_back`, `insert`, `resize`, `reserve`, ...
    * Zložitosti sú rovnaké
* Špecifické string operácie často pracujú s indexami a nie iterátormi

<table style="font-size: 70%;">
  <tr>
    <th>Metóda</th>
    <th>Zložitosť</th>
    <th>Popis</th>
  </tr>
  <tr>
    <td><code>substr</code></td>
    <td>O(n)</td>
    <td>Vráti substring, rozsah je definovaný indexami (nie iterátormi)</td>
  </tr>
  <tr>
    <td><code>find</code></td>
    <td>O(n)</td>
    <td>Nájde znak alebo reťazec a vráti pozíciu alebo <code>std::string::npos</code> (-1)</td>
  </tr>
  <tr>
    <td><code>append</code></td>
    <td>O(n+m)</td>
    <td>Pridá na koniec znak alebo celý reťazec</td>
  </tr>
  <tr>
    <td><code>operator+=</code></td>
    <td>O(n+m)</td>
    <td>Alias pre <code>append</code></td>
  </tr>
  <tr>
    <td><code>replace</code></td>
    <td>O(n+m)</td>
    <td>Nahradí podreťazec iným reťazcom</td>
  </tr>
</table>


## Voľne stojace funkcie

<table style="font-size: 70%;">
  <tr>
    <th>Funkcia</th>
    <th>Zložitosť</th>
    <th>Popis</th>
  </tr>
  <tr>
    <td><code>operator+</code></td>
    <td>O(n+m)</td>
    <td>Konkatenácia dvoch stringov, vráti nový string</td>
  </tr>
  <tr>
    <td><code>std::to_string</code></td>
    <td>O(1)</td>
    <td>Konverzia číselných typov na string</td>
  </tr>
</table>

---

## Hľadanie v reťazcoch

```cpp
auto str = std::string("Hello World!");
size_t n = str.find("orl"); // 7
n = str.find("ell", 4); // -1 std::string::npos, start at 4
n = str.rfind("o", 10); // 7, reverse search start at 10 backwards
n = str.rfind("ld", std::string::npos); // 9,  reverse search start at end backwards
n = str.find_first_of("aeiou"); // 1
n = str.find_last_not_of("el", 3); // 0, start at 3 and go backwards
```

* Existuje aj tretí parameter (`_Count`), ale správa sa čudne...

```cpp
str = "aaaabbbccd";
n = str.find("bbb", 2, 2); // 4
n = str.rfind("ccc", std::string::npos, 2); // 7?
n = str.find_first_not_of("abcd", 0, 3); // 9?
```

* `Count` je vlastne veľkosť stringu, ktorý sa hľadá
<!-- .element: class="fragment" -->

---

## Novinky v C++20

* V C++20 pribudli do stringu funkcie, ktoré sa dovtedy nahrádzali vlastným kódom alebo boostom

```cpp
auto str = std::string("This is C++20 string");
bool b = str.starts_with("This"); // true
b = str.ends_with("string."); // false
//b = str.contains("is"); // true C++23 :) 
```

---

## Pamäť stringu

![string memory layout MSVC with SSO](./lectures/3_scope_class/string_memory.png)
<!-- .element: class="stretch" -->

---

## `std::string` a C reťazce

* Neexistuje spôsob, ako iba priradiť C reťazec do stringu; vždy sa vytvorí kópia

```cpp
std::string s;
s = "Null terminated"; // copy string
```

* Použiť string ako reťazec ukončený nulovým znakom je jednoduché

```cpp
char c[100];
std::strcpy(c, s.c_str()); // const char*
std::strcat(c, s.data()); // char*
```

---

## split

```cpp
std::string path = R"(C:\Windows\System32\drivers\etc)";
 
std::vector<std::string> fragments;
 
size_t start = 0;
while (true) {
  auto pos = path.find('\\', start);

  auto length = pos == std::string::npos ? std::string::npos : pos - start;
  fragments.push_back(path.substr(start, length));
  if (pos == std::string::npos)
    break;

  start = pos + 1;
}
```

* `substr` vždy vytvorí kópiu. To je v poriadku pre malé stringy (SSO), ale pri väčších môže ísť o problém


## `ranges::views::split`

* C++20 pridáva knižnicu `ranges`, ktorá obsahuje množstvo užitočných funkcií a adaptér na rozdelenie reťazca na časti podľa zadaného znaku
* Jeho použitie je však trochu zložitejšie a vyžaduje aj lepšie pochopenie konceptov C++20

```cpp
#include <ranges>
#include <string>
#include <vector>

int main() {
  std::string path = R"(C:\Windows\System32\drivers\etc)";
  std::vector<std::string> fragments;

  for (auto&& part : path | std::views::split('\\')) {
    fragments.emplace_back(part.begin(), part.end());
  }
}
```


## join

```cpp
std::vector<std::string> fragments =
{
    "Hello", " ", "C++", "20"
};
 
std::string joined;
for (const auto& i : fragments) {
  if (!joined.empty()) {
    joined += '|'; // or append(1, '|')
  }

  joined.append(i); // or += i
}
```

* Nikdy nepoužívame s = s + a;, ale s += a;
* Zamedzíme tým kopírovaniu

---

## `string` ako buffer

```cpp
std::string s(100, '\0');
strcpy(s.data(), "This is C string");
strcat(s.data(), " even concatenation works!");

std::cout << s.size() << '\n'; // 100

s.resize(strlen(s.c_str())); // update the size

std::cout << s.size() << '\n'; // 42
```


## `resize_and_overwrite`

* C++23 pridáva funkciu, ktorá využívanie bufferu stringu v štýle C ešte viac „zjednoduší“ a najmä ho urobí bezpečnejším

```cpp
std::string s;
s.resize_and_overwrite(100, [](char* buf, std::size_t buf_size) -> std::size_t {
  strcpy(buf, "This is C string");
  strcat(buf, " even concatenation works!");

  return strlen(buf);
});
```

---

# Predávanie parametrov do funkcií

---

## Predávanie hodnotou a smerníkom

```cpp
// by value, can be slow
void f(std::string x) { }

// just pointer, should we check for null?
// callee can modify
void g(std::string* x) { }

// just pointer, should we check for null?
// callee cannot modify
void h(const std::string* x) { }
```

```cpp
std::string s = "Test";
f(s);
g(&s);
h(&s);
```

* Niektorí preferujú verziu so smerníkom, lebo zanecháva stopu pri volaní. Takmer nikto však nepreferuje verziu s konštantným smerníkom; lepšie je ...


## Predávanie cez referenciu

```cpp
// by value, can be slow
void f(std::string x) { }

// reference cannot be null
// callee can modify
void g(std::string& x) { }

// reference cannot be null
// callee cannot modify
void h(const std::string& x) { }
```

```cpp
std::string s = "Test";
f(s);
g(s);
h(s);
```

* Volajúci nevidí rozdiel
* Volaná funkcia sa nemusí obávať hodnoty `nullptr` a k premenným pristupuje pomocou `x.`, nie `x->`

---

## Usmernenia

* Volanie hodnotou nemusí byť zlé
  * Hlavne pre typy bez konštruktorov do pár (desiatok) bajtov
  * Ani inokedy nie je zlé, ale to už musíte niečo vedieť o C++, takže si to necháme na neskôr
* Inak preferujte `const&`
* Bez `const` iba ak potrebujete výstupný parameter, čo by ste veľmi nemali

---

# ĎAKUJEM

## Otázky?
