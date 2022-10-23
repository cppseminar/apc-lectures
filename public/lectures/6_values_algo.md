# C++

## Objekty ako hodnota, algoritmy

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Objekty ako hodnota
* Preťažovanie operátorov
* Globálne a statické premenné
* Štandardné algoritmy
    * Iterátory
    * Ranges

---

# Hodnotová sémantika

---

## Kedy sú dva objekty ekvivalentné?

```cpp
int main() {
    std::string a = "Equal string";
    std::string b = "Equal string";
 
    // are those strings equal?
}

```

* Áno aj nie
    * Áno, lebo reprezentujú rovnakú hodnotu
    * Nie, lebo `&a != &b`, takže ich vieme odlíšiť

---

## Objekty ako hodnoty

* V podstate zabudneme na pamäťové adresy a porovnávame objekty na základe ich sémantického významu
* C++ nám poskytuje
    * Kopírovanie pomocou kopírovacích konštruktorov
    * Preťažovanie operátorov
        * `==` pre ekvivalenciu
        * `<` pre usporiadanie
        * `+`, `-`, `*`, ...

---

## Kopírovací konštruktor a operátor priradenia

* Vaše triedy ich majú aj keď o tom neviete
* Sú definované ako kópia všetkých členských premenných
* Automatická generácia je jedna z veľkých chýb C++ ISO výboru

```cpp
class MyClass {
public:
    MyClass(const MyClass&); // copy constructor
    MyClass& operator=(const MyClass&); //[copy] assignment operator
};

```


## Automaticky vygenerovaný kopírovací konštruktor

```cpp
class MyClass {
private:
    int a;
    void* b;
    std::string c;
public:
    MyClass(const MyClass& other)
        : a(other.a)
        , b(other.b)
        , c(other.c) { }
 };

```

* OK pre typy ako `int` alebo `std::string` (v štandarde majú všetky typy dobre definované kopírovacie konštruktory)
* Asi zle pre `void*`


## Automaticky generovaný operátor priradenia

```cpp
class MyClass {
private:
    int a;
    void* b;
    std::string c;
public:
    MyClass& operator=(const MyClass& rhs) {
        a = rhs.a;
        b = rhs.b;
        c = rhs.c;
        return *this;
    }
 };
```

* OK pre typy ako `int` alebo `std::string` (v štandarde majú všetky typy dobre definované operátory priradenia)
* Asi zle pre `void*`


## Čo s tým?

* Ak to má zmysel môžeme definovať vlastné kopírovanie operácie
* Ak nemá kópia rozumnú sémantiku
    * Deklarujeme ich ako privátne bez implementácie (pred C++11)
    * Označíme ich ako `= delete` (po C++11 vrátane)

```cpp
class MyClass {
private:
    MyClass(const MyClass&);
    MyClass& operator=(const MyClass&);
};

```

```cpp
class MyClass {
public:
    MyClass(const MyClass&) = delete;
    MyClass& operator=(const MyClass&) = delete;
};

```


## Ak nám vyhovujú predvolené?

* Môžeme ich označiť = default a napovie to čitateľom, že o nich vieme a vyhovujú nám

```cpp
class MyClass {
public:
    MyClass(const MyClass&) = default;
    MyClass& operator=(const MyClass&) = default;
};

```

---

## Vlastné kopírovacie operácie

<table>
<tr>
<td style="width: 60%">

```cpp [8-13|15-20]
class Buffer {
public: 
    Buffer(size_t n) {
        data_ = new std::byte[n];
        size_ = n;
    }

    Buffer(const Buffer& other)
        : size_(other.size_)
        , data_(new std::byte[size_]) {
        memcpy(data_, rhs.data_, size_);
        return *this;
    }

    Buffer& operator=(const Buffer& rhs) {
        size_ = rhs.size_;
        data_ = new std::byte[size_];
        memcpy(data_, rhs.data_, size_);
        return *this;
    }
private:
    std::byte *data_;
    std::size_t size_;
};

```
<!-- .element: class="showall" -->
</td>
<td>
<div class="fragment">

* Kopirovací konštruktor je viacmenej dobre
* Kopírovací konštruktor už nie
    * Dáta v `this` by nám mohli pri priradený odíjsť bez uvolnenia
    * Nie je dobre
</div>
</td>
</tr>


## Kopírovací operátor priradenia

```cpp [|2]
Buffer& operator=(const Buffer& rhs) {
    delete[] data_;
    size_ = rhs.size_;
    data_ = new std::byte[size_];
    memcpy(data_, rhs.data_, size_);
    return *this;
}
```

<div class="fragment">

* Lepšie, ale `a = a;` je stále problém, lebo si odstránime vlastný buffer
* Niekto by to ani neopravoval, lebo iba hlupák by robil `a = a;`, ale nie je to také jednoduché...
</div>


## `this != &rhs` trick

* Môžeme zneužiť umiestnenie v pamäti 
* Zrejme ak majú dva objekty rovnakú adresu, tak musia byť rovnaké
* Samo priradenie sa dá odhaliť

```cpp
Buffer& operator=(const Buffer& rhs) {
  if (this != &rhs) {
    delete[] data_;
    size_ = rhs.size_;
    data_ = new std::byte[size_];
    memcpy(data_, rhs.data_, size_);
  }
  return *this;
}

```

Stále nie dokonalé, new môže hodiť výnimku a potom máme problém. Mali by sme najprv kopírovať a potom volať delete...
<!-- .element: class="fragment" -->


## Posledná verzia `operator=`

```cpp
Buffer& operator=(const Buffer& rhs) {
  if (this != &rhs) {
    auto* tmp = data_;
    try {
      data_ = new std::byte[rhs.size_];
    } catch (const std::exception&) {
      data_ = tmp;
      throw;
    }

    size_ = rhs.size_;
    memcpy(data_, rhs.data_, size_);
  }
  return *this;
}
```

---

## Hodnotová sémantika a `std::`

* Kontainery zo `std::` predpokladajú, že sa prvky správajú ako hodnoty
* `std::find` používa štandardne `==`
* Vector predpokladá, že sa dajú hodnoty kopírovať, alebo presúvať (move)
* Binárne vyhľadávanie očakáva funkčnosť `<`

---

# Preťažovanie operátorov

---

## Preťažovanie operátorov

* Väčšina operátorov môže byť preťažená
    * Priradenie sme už videli
    * Aritmetické operácie (`+`, `-`, `*`, `/`, ...)
    * Bitové operácie (`|`, `&`, `^`, `~`, ...)
    * Logicke operácie (`!`, `&&`, `||`, ...)
    * Ďalšie operátory ako `,`, ...
* Väčšinou veľmi zlý nápad


## Priradenie

```cpp
class MyClass {
public:
  MyClass& operator=(const MyClass&);
}
```

```cpp
int main() {
  MyClass A;

  MyClass B = A;
  B = A; // assigment operator
  B.operator=(A); // explicit call

  MyClass C;
  C = B = A;
}
```

* `MyClass B = A;` je volanie kopírovacieho konštruktoru, je to vlastne cukor okolo `MyClass B(A);`
* Pretože operátor vracia referenciu na seba samého, tak môžeme operátory reťaziť `C = B = A;`


## `const` je dôležité

Operátory sú iba vlania funkcií, takže je veľmi dôležité, aby sme mali signatúry presné. 

```cpp
MyClass& operator=(MyClass&);
```

Toto nie je operátor priradenia a preto nám kompilátor automaticky vygeneruje ten s `const`. 

---

## Matematické operácie

```cpp
class UInt128 {
public:
  UInt128 operator+(const UInt128& rhs) {
    UInt128 result(*this); // we copy this
    result.a += rhs.a;
    result.b += rhs.b;
    // TODO: we should handle carry !!!
    return result;
  }
private:
  uint64_t a, b;
};
```

* `this` je implicitný parameter ľavej strany operátora `+`
* Všimnime si, že `operator+` vracia nový objekt, je to konvencia (nemusí to tak byť), lebo tak to robia aj ostatné objeky a je to prirodzenejšie.

```cpp
UInt128 x, y, z;
z = x + y; // we do not expect x or y to change
```


## `operator+=`

```cpp
class UInt128 {
public:
  UInt128& operator+=(const UInt128& rhs) {
    this->a += rhs.a;
    this->b += rhs.b;
    // TODO: we should handle carry !!!
    return *this;
  }
private:
  uint64_t a, b;
};
```

* Znovu operátor nemusí vracať referenciu, ale potom by sa nesprával, ako je očakávané v jazyku
* `std::string` `+=` modifikuje lavú stranu


## Operátory mimo tried

Operátory môžu byť definované aj mimo tried.

```cpp
class UInt128 {
public:
  friend UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs);
private:
  uint64_t a, b;
};

UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs) {
    UInt128 result(lhs); // we copy this
    result.a += rhs.a;
    result.b += rhs.b;
    // TODO: we should handle carry !!!
    return result;
}
```

* `friend` v triede je na to aby sme vedeli pristúpiť z operátora aj k privátnym premenným
* Ľavá strana operátora je explicitne uvedená ako prvý parameter


## Štandardná implementácia

```cpp
class UInt128 {
public:
  UInt128& operator+=(const UInt128& rhs) {
    this->a += rhs.a;
    this->b += rhs.b;
    // TODO: we should handle carry !!!
    return *this;
  }

  friend UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs);
private:
  uint64_t a, b;
};

UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs) {
    return lhs += rhs;
}
```

V operátore `+` si môžem dovoliť `return` operátora `+`, lebo operátor `+=` vracia vlastne samého seba a keďže operátor vracia hodnotu a nie referenciu, tak sa mi automaticky vyrobí kópia. 


## Ktoré preťaženie je “správne”?

```cpp
Foo& operator+(const Foo& lhs, const Foo& rhs);

Foo& operator+(Foo lhs, const Foo& rhs);

Foo operator+(Foo lhs, const Foo& rhs);

Foo operator+(Foo lhs, Foo rhs);
```

Správne sú posledné dva. “Najlepší” variant je tretí, kopírovaním prvého parametru môže optimalizátor vykonať optimalizácie (`move`), ktoré inak nemôže. 

---

## Umožnenie `<<` do stream-u

```cpp
class Time {
public:
  Time(time_t t) : m_time(t) {}

  friend std::ostream &operator<<(std::ostream &lhs, const Time &dt);
private:
  time_t m_time;
};

std::ostream &operator<<(std::ostream &lhs, const Time &rhs) {
  lhs << rhs.m_time << "s";
  return lhs;
}

int main() {
  Time t(5);
  std::cout << t << std::endl;
}
```

---

## Index operátor

* Subscript operator
* Nemôže byť mimo triedy definovaný
* Nové operátory nemôžu byť definované, takže `a[i, j]` nie je dosiahnuteľné

```cpp
class PtrVector {
public:
    int& operator[](size_t i) { return *m_vec[i]; }
    const int& operator[](size_t i) const { return *m_vec[i]; }
private:
    std::vector<int*> m_vec;
};
```

* Spravidla sa preťažuje aj `const` aj nie `const` verzia
    * `const` sa použije ak sa operátor volá na `const` objektom
    * a naopak

---

## Operátory porovnania

* Užitočné ak chceme používať so štandardnými algoritmami
* Alebo ak chceme definovať ekvivalenciu objektov

```cpp
struct LineNumPair {
public:
  bool operator<(const LineNumPair &rhs) { return m_num < rhs.m_num; }

private:
  std::string m_line;
  size_t m_num;
};
```


## Starý spôsob ako definovať porovnanie

```cpp
class IntBox {
public:
    IntBox(int i) : data(std::make_unique<int>(i)) { }
    bool operator==(const IntBox& rhs) const { return *(this->data) == *(rhs.data); }
    bool operator!=(const IntBox& rhs) const { return *(this->data) != *(rhs.data); }
    bool operator<(const IntBox& rhs) const { return *(this->data) < *(rhs.data); }
    bool operator<=(const IntBox& rhs) const { return *(this->data) <= *(rhs.data); }
    bool operator>(const IntBox& rhs) const { return *(this->data) > *(rhs.data); }
    bool operator>=(const IntBox& rhs) const { return *(this->data) >= *(rhs.data); }
private:
    std::unique_ptr<int> data;
};
```

* Musíme definovať všetky operátory
* Môže nám pomôcť `std::rel_ops`
* Prípadne definujeme iba `==`  a `<` ostatné nie sú až také potrebné


## `<=>` operátor

* Pre zjednodušenie bol pridaný three-way comparison operator (aka spaceship operator)
* Bežné operátory (`==`, `<`, ...) sa vygenerujú automaticky

```cpp [|5|13-14]
class IntBox {
public:
    IntBox(int i) : data(std::make_unique<int>(i)) { }
    auto operator<=>(const IntBox& rhs) const { 
        return *(this->data) <=> *(rhs.data);
    }
private:
    std::unique_ptr<int> data;
};

int main() {
    assert(IntBox(3) < IntBox(5));
    assert(IntBox(5) <= IntBox(5));
}
```


## ... ak sa všetky členy dajú porovnať ...

```cpp
class Person {
public:
    Person(std::string name, uint8_t age)
        : name(name), age(age) { }
    auto operator<=>(const Person& rhs) const = default;
private:
    std::string name;
    uint8_t age;
    std::vector<int> privileges;
};

int main() {
    assert(Person("Bjarne", 70) < Person("Richard", 68));
}
```


## ...alebo iba konkrétne operácie.

```cpp
class Person {
public:
    Person(std::string name, uint8_t age)
        : name(name), age(age) { }
    bool operator==(const Person& rhs) const = default;
private:
    std::string name;
    uint8_t age;
};

int main() {
    assert(Person("Bjarne", 70) != Person("Richard", 68));
}
```

Operátor `==` mám umožní volať aj `!=`.

---

## Ako preťažiť ++?

Postfix, alebo prefix?

```cpp
struct IncString {
  IncString& operator++() { // prefix increment
    s = std::to_string(stoul(s) + 1);
    return *this;
  }

  IncString operator++(int) { // postfix increment, dummy int
    IncString tmp(*this);
    operator++(); // call prefix inc
    return tmp;
  }

private:
  std::string s = "0";
};
```
<!-- .element: class="showall" -->

* Dummy `int` sa použije iba ako rozdelenie o ktorý operátor `++` ide, tento parameter sa nemá používať
* Prefix `++` vracia seba samého a postfix zase novú kópiu, je to znovu štandardné správanie


```cpp
int main() {
  IncString s;
  std::cout << s << std::endl; // 0
  std::cout << (s++) << std::endl; // 0 
  std::cout << (++s) << std::endl; // 2
}
```

---

## Používanie

* Netreba to preháňať s preťažovaním
    * Preťaženie `&` (adresa) je zlý nápad
    * Používanie operátora `,` je zlý nápad, preťažovanie najhorší
* Operátory by sa mali správa ako ich štandardné varianty 
* Toto sa nie úplne dá s `&&` a `||` (kvôli vlastnosti short circuit)
* Vedie k nepríjemným problémom a ťažko nájditelným chybám

---

# Algoritmy

---

## Algoritmy v štandardnej knižnici

* Bežné algoritmy na hľadanie
* Triedenie
* Set operations
* Heap operations
* Všetky algortimy sú veľmi rýchle a takmer isto neobsahujú žiadne chyby

---

## Iterátory

* Zovšeobecnenie smerníkov
* Každý smerník je iterátor, ale nie naopak

![Triedy iterátorov](./lectures/6_values_algo/iterators.png)

---

## `begin` a `end`

```cpp
std::vector<int> vec = { 0, 5, 4, -1, 2, 10 };
for (int i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << std::endl;
}

// is roughly equivalent to
// it type is std::vector<int>::iterator
for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << std::endl;
}
```

* `begin` vracia iterátor na začiatok
* `end` vracia iterátor jeden za koniec, je to kvôli tomu aby sa to dobre používalo 😎

---

## Výstupné iterátory

* Dá sa do nich zapisovať, nie čítať
* Vedia "ísť" len dopredu
* Všetky bežné iterátory sú aj výstupné iterátory


## `std::copy`

* Štandardný spôsob na kopírovanie
* "1,5-range" algoritmus, teda výstup nekontroluje či má miesto

```cpp
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b;
std::copy(a.begin(), a.end(), b.begin());
```

Priklad vyššie nebude fungovať, lebo v prvom kroku sa v podstate urobí `*b.begin() = *a.begin()` a `b` nemá žiadnu prvky, takže priradenie do `b.begin()` je nedefinované.
<!-- .element: class="fragment" -->

```cpp
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b(a.size());
std::copy(a.begin(), a.end(), b.begin());
```
<!-- .element: class="fragment" -->

Toto je OK, lebo `b` má dostatočnú veľkosť (pozor iba kapacita nestačí).
<!-- .element: class="fragment" -->


## `std::back_inserter`

* Pravý výstupný iterátor, nedá sa použiť na nič iné iba na zápis

```cpp
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b;
std::copy(a.begin(), a.end(), std::back_inserter(b));
```

Kód vyššie je približne ekvivalentý kódu

```cpp
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b;
for (auto it = a.begin(); it != a.end(); ++it) {
  b.push_back(*it);
}
```

---

## Druhy vstupných iterátorov

* Vstupný (input) iterátor
    * `++` pre posun na ďalší prvok
    * `*` na získanie dát
* Forward iterátor
    * Môžeme čítať viac krát
    * linked list
* Bidirectional iterátor 
    * `--` pre posun o jeden prvok vzad
    * bidirectional linked list
* Random access iterátor
    * `+/-` pre posun o viacero prvkov v jednom kroku (dá sa simulovať pomocou viacnásobného volania `++`, ale tu máme zaručenú rýchlosť)
    * vector

---

## `std::istream_iterator`

* Vstupný iterátor, ktorý pri iterácií číta zo vstupu.
* Ako end-of-stream sa používa default vykonštruovaný `std::istream_iterator`
* end-of-stream iterátor nastane ak je vo vstupnom streame chyba (`bad`, `fail`, alebo `eof`)

```cpp
std::vector<int> v;
std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(), std::back_inserter(v));
```

---

## Výhody iterátorov

* V debug konfigurácií sú zapnute všetky kontroly, zachytí sa veľa problémov
* V release konfigurácií sú potom všetky vypnuté a kód je extrémne rýchly

<table style="width: 80%">
<tr>
<td style="width: 60%">

```cpp
std::string::iterator it;
{
    std::string s = "This is very good string";
    it = s.begin();
}

// next line seems to be working in release mode
// but is actually undefined
// will be catch in debug builds by assertions
std::cout << *it;
```
</td>
<td>

![Iterator assert](./lectures/6_values_algo/iterator-assert.png)
</td>
</tr>

---

## foreach

Range based for cyklus v C++11

```cpp
for (const auto& i : vec) {
    std::cout << i;
}

auto __end = vec.end();
for (auto __begin = vec.begin(); __begin != __end; ++__begin) {
    std::cout << *__begin;
}
```


## Range based for pre užívateľom definované typy

* Musíme zadefinovať typ, ktorý bude slúžiť ako iterátor
* Vytvoríme metódy vracajúce iterátor
    * `begin()` a `end()` ako členské funkcie
    * Alebo ako samostatne stojace funkcie
* Iterátor môže byť aj typ samotný, ale nebýva to zvykom


## Range based for cyklus pre polia

Dá sa použiť priamo s C poliami

```cpp
std::string s[] = { "Zero", "One", "Two" };
for (const auto& i : s) {
    std::cout << i << std::endl;
}
```

```cpp
std::string* b = std::begin(s);
std::string* e = std::end(s);
```

Niekde v štandardnej knižnici existuje špecializácia funkcie `begin` na `T[]`. Rovnako pre funkciu end, preto potom `for` v takejto forme existuje.

---

## Vyhľadávanie `std::find`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5 };
auto it = std::find(vec.begin(), vec.end(), 3);

assert(*it == 3);
assert(it == vec.begin() + 2);

it = std::find(vec.begin(), vec.end(), 4);

// *it is undefined
assert(it == vec.end());
```

* Vracia iterátor na prvý hľadaný prvok
* Alebo `end` iterátor ak sa tam prvok nenachádza
* Na hľadanie od konca musíme použiť `rbegin()` a `rend()`


## Vyhľadávanie s podmienkou

```cpp
bool is_even(const int& i) {
    return i % 2 == 0;
}

std::vector<int> vec = { 1, 2, 3, 0, 5 };
auto it = std::find_if(vec.begin(), vec.end(), is_even);

assert(*it == 2);
assert(it == vec.begin() + 1);
```

* Správa sa rovnako ako `std::find`, akurát nehľadá konkrétny prvok, ale prvok, ktorý spĺňa predikát
* Rovnako existuje aj `std::find_if_not`


## Binárne vyhľadávanie

* Na utriedenom vectore môžeme vykonať binárne vyhľadávanie
* Naša starosť aby bol vector utriedený

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5 };
assert(std::is_sorted(vec.begin(), vec.end()));

bool found = std::binary_search(vec.begin(), vec.end(), 4);
std::cout << std::boolalpha << found; // false

auto it = std::lower_bound(vec.begin(), vec.end(), 4);
if (it != vec.end())
  std::cout << *it; // 5
```

* `binary_search` iba vráti, či sa nachádza
* `lower_bound` vráti miesto, kde ak by sme prvok vložili, tak bude sekvecia stále utriedená, lower preto lebo je to miesto najbližšie k začiatku 
* `upper_bound` by bolo miesto najďalej
* Treba si zapamätať, že ak tam prvok nie je, tak to mi to stále môže vrátiť platný iterátor (teda nie `end`). Preto to musíme kontrolovať.

---

## Filtrácia

* `std::filter` neexistuje, ale môžeme použiť `std::copy_if`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5, 4 };

std::vector<int> res;
std::copy_if(vec.begin(), vec.end(), std::back_inserter(res), is_even);

assert(res[0] == 2);
assert(res[1] == 0);
assert(res[2] == 4);
```

---

## Transformácia

* `std::transform` slúži na mapovanie hodnôt na nové

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5, 4, 2 };

std::vector<bool> res;
std::transform(vec.begin(), vec.end(), std::back_inserter(res), is_even);
// false, true, false, true, false, true, true
```

---

## Triedenie

* Triedenie sa robi pomocou algoritmu `std::sort`
* Zložitosť je *n log n* (použiva sa väčšinou variácia quick sortu)
* Triedenie sa robí pomocou operátora `<`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5, 4, 2 };

std::sort(vec.begin(), vec.end());

std::sort(vec.begin(), vec.end(), std::greater<int>());
```

Treti parameter je predikát (ak nám nevyhovuje `<`). 

---

## Mazanie prvkov

* Nemáme na to jeden algoritmus, každý kontajner má vlastný `erase`
* Mazanie jedného prvku je jednoduché
* `erase` invaliduje všetky iterátory na mazaný prvok a všetky nasledujúce prvky

```cpp
auto it = std::find(vec.begin(), vec.end(), 0);
if (it != vec.end()) {
    vec.erase(it);
}
```

---

## Mazanie viacerých prvkov

```cpp
void erase_all(std::vector<int>& vec, int value) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it == value) {
            vec.erase(it);
        }
    }
}
```

* Nie je dobre, z dôvodu, že ak nájdeme prvok, tak vlastne druhý preskočíme.
<!-- .element: class="fragment" -->


## Mazanie viacerých prvkov

```cpp
void erase_all(std::vector<int>& vec, int value) {
    for (size_t i = vec.size(); i >= 0; --i) {
        if (vec[i] == value) {
            vec.erase(vec.begin() + i);
        }
    }
}
```

* Nie je dobre, lebo je tam nekonečný cyklus, size_t nikdy nebude menší ako 0.  
<!-- .element: class="fragment" -->


## Mazanie viacerých prvkov

```cpp
void erase_all(std::vector<int>& vec, int value) {
    for (size_t i = vec.size() - 1; i > 0; --i) {
        if (vec[i - 1] == value) {
            vec.erase(vec.begin() + i - 1);
        }
    }
}
```

* Nie je dobre, lebo size - 1 môže podtiecť a máme skoro nekonečný cyklus.  
<!-- .element: class="fragment" -->


## Mazanie viacerých prvkov

```cpp
void erase_all(std::vector<int>& vec, int value) {
  for (int i = vec.size() - 1; i >= 0; --i) {
    if (vec[i] == value) {
      vec.erase(vec.begin() + i);
    }
  }
}
```

* Ako tak dobre, ale používame `int` a náš algoritmus je `O(n*n)`


## Štandardné mazanie viacerých

```cpp
void erase_all(std::vector<int>& vec, int value) {
  for (auto it = vec.begin(); it != vec.end();) {
    if (*it == value) {
      it = vec.erase(it);
    } else {
      ++it;
    }
  }
}
```

* `erase` vracia iterátor na ďalší prvok, ktorý nasledoval po zmazanom


## Erase-remove idiom

* Najlepšie riešenie je použiť tento štandardný postup
* Algoritmus `remove` reorganizuje prvky, tak že prvky rovné zadanej hodnote sa presunú na koniec

![erase remove idiom](./lectures/5_values_algo/erase-remove.png)

```cpp
void erase_remove(std::vector<int>& vec, int value) {
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}
```

---

# Array

---

## Staré dobré C polia

* Vložené priamo do funkcií, objektov
* Nulové mrhanie pamäťou a rýchlosťou

```cpp
struct MyClass {
    int ints[3];
    char character;
};
 
int main(int argc, const char* argv[]) {
    MyClass arr[50];
 
    static_assert(sizeof(arr) == 50 * sizeof(MyClass), 
        "Not the same size.");
}
```

V poliach nikdy nie je padding medzi prvkami.
<!-- .element: class="fragment" -->


## Problémy s C poliami

* Neposkytujú iterátory
* Nekontrolujú prístup mimo hraníc ani v debug móde
* Veľkosť sa nedá ľahko získať priamo z poľa, bez templatových hackov
* Nevyzerajú dosť C++


## `std::array<T, N>`

* C pole ozdobné pár funkciami
* Nezaberá nič navyše v pamäti, iba `N * sizeof(T)`

```cpp
int main(int argc, const char** argv) {
    std::array<int, 4> arr = { 3, 1, 2 };
    std::sort(arr.begin(), arr.end()); // 0, 1, 2, 3

    for (const auto& i : arr)  {
        std::cout << i << " ";
    }
    std::cout << arr.size() << '\n'; // 4
}
```


## ...ale to sa dá aj C polami

```cpp
int main(int argc, const char** argv) {
    int arr[4] = { 3, 1, 2 };
    std::sort(std::begin(arr), std::end(arr)); // 0, 1, 2, 3

    for (const auto& i : arr) {
        std::cout << i << " ";
    }
    std::cout << std::size(arr) << '\n'; // 4
}
```

Od C++14/17 existujú voľne stojace funkcie ako `begin`, `end` a `size` aj pre `T[]` .

---

## Operátor priradenia =

* Nefunguje pre C polia
* Funguje správne pre `std::array`
* Nie je preto veľa dôvodov prečo neuprednostniť std::array

```cpp
int main(int argc, const char** argv) {
    int arr[] = { 3, 1, 2 };
    //int arr2[3] = arr; will not compile
    int arr2[3];
    memcpy(arr2, arr, std::size(arr));
    // hope they have the same size

    std::array<int, 3> std_arr = { 3, 1, 2 };
    std::array<int, 3> std_arr2 = std_arr;
}
```

---

# Ranges

---

## Ranges

* V C++20 pribudli ranges
* Jeden z ich prínosov je, že pri štandardných algoritmoch už nemusíme používať iterátory
* Takmer všetky algoritmy zo `std::` majú dvojičku aj v `std::ranges::`

---

## Príklady

```cpp
std::vector<int>vec = { 1,2,3,0,5 };
std::ranges::sort(vec);
auto it = std::ranges::find(vec, 3);
if (it == vec.end())
    std::cout << "Not found\n";

std::cout << std::distance(vec.begin(), it);
std::ranges::reverse(vec);
// second parameter is the value that should be at the new front
std::ranges::rotate(vec, vec.begin() + 2);
```

Ak môžete radšej používajte tieto algoritmy zo `std::ranges`.

---

## Erase remove idiom ++

C++20 za nás všetko vyriešilo

```cpp
bool is_even(int x) { return x % 2 == 0; }

int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 1, 2, 4, 5 };
    std::erase(v, 1);
    std::erase_if(v, is_even);
    for (auto i : v) {
        std::cout << i << ' '; // 3, 5, 5
    }
}
```

---

# ĎAKUJEM

## Otázky?