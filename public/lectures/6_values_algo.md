# C++

## Objekty ako hodnota, algoritmy

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Objekty ako hodnota
* Preťažovanie operátorov
* Štandardné algoritmy
    * Iterátory
    * Ranges
* `std::array`
* `std::map`

---

# Hodnotová sémantika

---

## Kedy sú dva objekty ekvivalentné?

```cpp
int main() {
    std::string a = "Equal string";
    std::string b = "Equal string";
 
    // are those strings equal?
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
* Automatická generácia je jedna z chýb C++ ISO výboru (plus minús to opravili move sémantikou)

```cpp
class MyClass{
public:
    MyClass(const MyClass&); // copy constructor
    MyClass& operator=(const MyClass&); // [copy] assignment operator
};
```


## Automaticky vygenerovaný kopírovací konštruktor

```cpp
class MyClass {
private:
    int a;
    void* b;
    std::string c;
public:
    MyClass(const MyClass& other)
        : a(other.a)
        , b(other.b)
        , c(other.c) { }
 };

```

* OK pre typy ako `int` alebo `std::string` (v štandarde majú všetky typy dobre definované kopírovacie konštruktory)
* Asi zle pre `void*`


## Automaticky generovaný operátor priradenia

```cpp
class MyClass {
private:
    int a;
    void* b;
    std::string c;
public:
    MyClass& operator=(const MyClass& rhs) {
        a = rhs.a;
        b = rhs.b;
        c = rhs.c;
        return *this;
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
class MyClass {
private:
    MyClass(const MyClass&);
    MyClass& operator=(const MyClass&);
};
```

```cpp
class MyClass {
public:
    MyClass(const MyClass&) = delete;
    MyClass& operator=(const MyClass&) = delete;
};
```


## Ak nám vyhovujú predvolené?

* Môžeme ich označiť `= default` a napovie to čitateľom, že o nich vieme a vyhovujú nám

```cpp
class MyClass {
public:
    MyClass(const MyClass&) = default;
    MyClass& operator=(const MyClass&) = default;
};
```


## Bez `const`?

```cpp
class MyClass{
public:
	MyClass() = default;
	MyClass(MyClass&);
};
```

```cpp
MyClass a;
MyClass b(a);

const MyClass ac;
MyClass bc(ac);
```

<div class="fragment">

* Copy konštruktor by mal byť `const`, ale štandard umožnuje aj bez
* Preto v tomto prípade sme kopírovací konštruktor definovali my a kompilátor nám nevygeneruje defaultný `MyClass(const MyClass&)`, vieme ho prinútiť pomocou `= default`
* `MyClass bc(ac);` sa nepodarí skompilovať
</div>

Note: <https://www.fluentcpp.com/2019/04/23/the-rule-of-zero-zero-constructor-zero-calorie/>

---

## Vlastné kopírovacie operácie

<table>
<tr>
<td style="width: 60%">

```cpp [8-13|15-20]
class Buffer {
public: 
    Buffer(size_t n) {
        data_ = new std::byte[n];
        size_ = n;
    }

    Buffer(const Buffer& other)
        : size_(other.size_)
        , data_(new std::byte[size_]) {
        memcpy(data_, rhs.data_, size_);
        return *this;
    }

    Buffer& operator=(const Buffer& rhs) {
        size_ = rhs.size_;
        data_ = new std::byte[size_];
        memcpy(data_, rhs.data_, size_);
        return *this;
    }
private:
    std::byte *data_;
    std::size_t size_;
};

```
</td>
<td>
<div class="fragment">

* Kopírovací konštruktor je viacmenej dobre
* Kopírovací operátor priradenia už nie
    * Dáta v `this` by nám mohli pri priradený odíjsť bez uvolnenia
    * Nie je dobre
</div>
</td>
</tr>


## Kopírovací operátor priradenia

```cpp [|2]
Buffer& operator=(const Buffer& rhs) {
    delete[] data_;
    size_ = rhs.size_;
    data_ = new std::byte[size_];
    memcpy(data_, rhs.data_, size_);
    return *this;
}
```

<div class="fragment">

* Lepšie, ale `a = a;` je stále problém, lebo si odstránime vlastný buffer
* Niekto by to ani neopravoval, lebo iba hlupák by robil `a = a;`, ale nie je to také jednoduché...
</div>


## `this != &rhs` trik

* Môžeme zneužiť umiestnenie v pamäti 
* Zrejme ak majú dva objekty rovnakú adresu, tak musia byť rovnaké
* Samo priradenie sa dá odhaliť

```cpp
Buffer& operator=(const Buffer& rhs) {
  if (this != &rhs) {
    delete[] data_;
    size_ = rhs.size_;
    data_ = new std::byte[size_];
    memcpy(data_, rhs.data_, size_);
  }
  return *this;
}

```

Stále nie dokonalé, `new` môže hodiť výnimku a potom máme problém. Mali by sme najprv kopírovať a potom volať delete...
<!-- .element: class="fragment" -->


## Posledná verzia `operator=`

```cpp
Buffer& operator=(const Buffer& rhs) {
  if (this != &rhs) {
    auto* tmp = data_;
    try {
      data_ = new std::byte[rhs.size_];
      delete tmp; // release old buffer
    } catch (const std::exception&) {
      data_ = tmp;
      throw;
    }

    size_ = rhs.size_;
    memcpy(data_, rhs.data_, size_);
  }
  return *this;
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
  MyClass a;

  MyClass b = a;
  b = a; // assigment operator
  b.operator=(a); // explicit call

  MyClass c;
  c = b = a;
}
```

* `MyClass b = a;` je volanie kopírovacieho konštruktoru, je to vlastne okolo `MyClass b(a);`
* Pretože operátor vracia referenciu na seba samého, tak môžeme operátory reťaziť `c = b = a;`


## Navratový typ

* Väčšinou je to referencia na seba samého, tento typ sa volá idiomaticky `MyClass&`
* Ale môže to byť aj `MyClass`, to ešte dáva aký taký zmysel pre typy, ktoré sú zložené z primitívnych typov
* Ostatné návratové typy sú zvyčajne zlý nápad, ale štandard ich podporuje

```cpp
class UInt128 {
public:
  UInt128 operator=(const UInt128& rhs) {
    low = rhs.low;
    high = rhs.high;
    return *this;
  }
private:
  uint64_t low, high;
};
```


## Bez `const`?

```cpp
MyClass& operator=(MyClass&);
```

* Podobne ako v prípade kopírovacieho konštruktora, operátor priradenia by mal mať parameter `const`, ale štandard umožnuje aj bez
* V istých prípadoch to môže byť užitočné, ale je to veľmi zriedkavé


## Bez referencie?

```cpp
MyClass operator=(MyClass);
```

* Aj táto signatúra je podporovaná, existuje copy-and-swap idiom, ktorý ju využíva

```cpp
MyClass& operator=(MyClass rhs) {  // copy will be called
  std::swap(n, rhs.n);
  std::swap(s, rhs.s);
  return *this;
}

MyClass a, b;
a = b;
```

---

## Matematické operácie

```cpp
class UInt128 {
public:
  UInt128 operator+(const UInt128& rhs) const {
    UInt128 result(*this); // we copy this
    result.low += rhs.low;
    uint64_t carry = (result.low < this->low) ? 1 : 0;
    result.high += rhs.high + carry;
    return result;
  }
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
    this->low += rhs.low;
    uint64_t carry = (this->low < rhs.low) ? 1 : 0;
    this->high += rhs.high + carry;
    return *this;
  }
private:
  uint64_t a, b;
};
```

* Znovu operátor nemusí vracať referenciu, ale potom by sa nesprával, ako je očakávané v jazyku
* `std::string` `+=` modifikuje ľavú stranu

```cpp
UInt128 x, y, z;
z = x += y; // we expect x to change
```


## Operátory mimo tried

* Operátory môžu byť definované aj mimo tried.

```cpp
class UInt128 {
public:
  friend UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs);
private:
  uint64_t a, b;
};

UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs) {
    UInt128 result(lhs); // we copy this
    result.low += rhs.low;
    uint64_t carry = (result.low < this->low) ? 1 : 0;
    result.high += rhs.high + carry;
    return result;
}
```

* `friend` v triede je na to aby sme vedeli pristúpiť z operátora aj k privátnym premenným
* Ľavá strana operátora je explicitne uvedená ako prvý parameter


## Štandardná implementácia

```cpp
class UInt128 {
public:
  UInt128& operator+=(const UInt128& rhs) { }
private:
  uint64_t a, b;
};

UInt128 operator+(const UInt128 &lhs, const UInt128 &rhs) {
  auto result(lhs);
  return result += rhs;
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

<div class="fragment">

Správne sú posledné dva. "Najlepší" variant je tretí, kopírovaním prvého parametru môže optimalizátor vykonať optimalizácie (`move`), ktoré inak nemôže. 
</div>

```cpp
UInt128 operator+(UInt128 lhs, const UInt128 &rhs) {
  return lhs += rhs;
}
```
<!-- .element: class="fragment" -->

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
    int& operator[](size_t i) { return *vec_[i]; }
    const int& operator[](size_t i) const { return *vec_[i]; }
private:
    std::vector<int*> vec_;
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
  bool operator<(const LineNumPair &rhs) const { return m_num < rhs.m_num; }

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

* Pre zjednodušenie bol pridaný *three-way comparison* operátor (aka *spaceship* operátor)
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


## ... ručné definovanie návratovej hodnoty ... 

```cpp
class IntBox {
public:
	IntBox(int i) : data(std::make_unique<int>(i)) { }
	std::strong_ordering operator<=>(const IntBox& rhs) const {
		if (*data < *rhs.data)
			return std::strong_ordering::less;
		if (*data == *rhs.data)
			return std::strong_ordering::equal;
		return std::strong_ordering::greater;
	}
private:
	std::unique_ptr<int> data;
};
```

* `std::strong_ordering` je nový typ, ktorý je definovaný v C++20
* Existujú aj `std::weak_ordering` a `std::partial_ordering` pre prípady, keď nie je možné definovať úplné usporiadanie


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

* Dummy `int` slúži iba na určenie, o ktorý operátor `++` ide, tento parameter sa nemá používať
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
std::vector<int> vec = { 0, 5, 4, -1, 2, 10 };
for (int i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << std::endl;
}

// is roughly equivalent to
// it type is std::vector<int>::iterator
for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << std::endl;
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

Príklad vyššie nebude fungovať, lebo v prvom kroku sa v podstate urobí `*b.begin() = *a.begin()` a `b` nemá žiadnu prvky, takže priradenie do `b.begin()` je nedefinované.
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
std::vector<int> v;
std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(), std::back_inserter(v));
```

---

## Výhody iterátorov

* V debug konfigurácií sú zapnute všetky kontroly, zachytí sa veľa problémov
* V release konfigurácií sú potom všetky vypnuté a kód je extrémne rýchly

<table style="width: 80%">
<tr>
<td style="width: 60%">

```cpp
std::string::iterator it;
{
    std::string s = "This is very good string";
    it = s.begin();
}

// next line seems to be working in release mode
// but is actually undefined
// will be catch in debug builds by assertions
std::cout << *it;
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
for (const auto& i : vec) {
    std::cout << i;
}

auto __end = vec.end();
for (auto __begin = vec.begin(); __begin != __end; ++__begin) {
    std::cout << *__begin;
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
std::string s[] = { "Zero", "One", "Two" };
for (const auto& i : s) {
    std::cout << i << std::endl;
}
```

```cpp
std::string* b = std::begin(s);
std::string* e = std::end(s);
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
std::vector<int> vec = { 1, 2, 3, 5 };
assert(std::is_sorted(vec.begin(), vec.end()));

bool found = std::binary_search(vec.begin(), vec.end(), 4);
std::cout << std::boolalpha << found; // false

auto it = std::lower_bound(vec.begin(), vec.end(), 4);
if (it != vec.end())
  std::cout << *it; // 5
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
auto it = std::find(vec.begin(), vec.end(), 0);
if (it != vec.end()) {
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
void erase_all(std::vector<int>& vec, int value) {
    for (size_t i = vec.size() - 1; i >= 0; --i) {
        if (vec[i] == value) {
            vec.erase(vec.begin() + i);
        }
    }
}
```

* Nie je dobre, lebo je tam nekonečný cyklus, size_t nikdy nebude menší ako 0.  
<!-- .element: class="fragment" -->


## Mazanie viacerých prvkov

```cpp
void erase_all(std::vector<int>& vec, int value) {
  for (int i = vec.size() - 1; i >= 0; --i) {
    if (vec[i] == value) {
      vec.erase(vec.begin() + i);
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

* `erase` vracia iterátor na ďalší prvok, ktorý nasleduje po zmazanom


## Erase-remove idiom

* Najlepšie riešenie je použiť tento štandardný postup
* Algoritmus `remove` reorganizuje prvky, tak že prvky rovné zadanej hodnote sa presunú na koniec

![erase remove idiom](./lectures/6_values_algo/erase-remove.png)

```cpp
void erase_remove(std::vector<int>& vec, int value) {
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}
```

---

# Array

---

## Staré dobré C polia

* Vložené priamo do funkcií, objektov
* Nulové mrhanie pamäťou a rýchlosťou

```cpp
struct MyClass {
    int ints[3];
    char character;
};
 
int main(int argc, const char* argv[]) {
    MyClass arr[50];
 
    static_assert(sizeof(arr) == 50 * sizeof(MyClass), 
        "Not the same size.");
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
int main(int argc, const char** argv) {
    std::array<int, 4> arr = { 3, 1, 2 };
    std::sort(arr.begin(), arr.end()); // 0, 1, 2, 3

    for (const auto& i : arr)  {
        std::cout << i << " ";
    }
    std::cout << arr.size() << '\n'; // 4
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

Od C++14/17 existujú voľne stojace funkcie ako `std::begin`, `std::end` a `std::size` aj pre `T[]` .

---

## Operátor priradenia =

* Nefunguje pre C polia
* Funguje správne pre `std::array`
* Nie je preto veľa dôvodov prečo neuprednostniť `std::array`

```cpp
int main(int argc, const char** argv) {
    int arr[] = { 3, 1, 2 };
    //int arr2[3] = arr; will not compile
    int arr2[3];
    memcpy(arr2, arr, sizeof(arr));
    // hope they have the same size

    std::array<int, 3> std_arr = { 3, 1, 2 };
    std::array<int, 3> std_arr2 = std_arr;
}
```

---

# `std::map`

---

## Asociatívne pole

* Asociatívne pole je dátová štruktúra, ktorá mapuje kľúče na hodnoty
* Nazýva sa aj mapa alebo slovník
* Kľúče musia byť unikátne a porovnateľné
* Ak sú kľúče čísla, tak môžeme použiť aj `std::vector`

```cpp
std::vector<std::string> dict; // "mapping" size_t to std::string
dict.push_back("A"); // index 0
dict.resize(100);
dict[5] = "B"; // index 5
dict.push_back("C"); // index 100
```

---

## `std::string` ako kľúč

* Predstavme si, že chceme mať mapu, ktorá mapuje reťazce na celé čísla (napr. mená na vek)
* Chceme podporu pre pridávanie, mazanie, hľadanie a získanie hodnôt
* Skúsme to urobiť s `std::vector` (ak si nič iné z celého predmetu nezapamätáte, tak si zapamätajte, že `std::vector` by mal byť vaša prvá voľba na skoro všetko)


## `std::pair`

* Súčasť štandardnej knižnice (v headri `<utility>`)
* V podstate iba štruktúra s dvoma verejnými členmi
* Má preťažené porovnania

```cpp
std::pair<std::string, std::string> p;
p.first = "Index";
p.second = "Value";

p = { "Index", "Value" };

// pair std::string, int
auto r = std::make_pair(std::string("Index"), 16);

// since C++17 deduction guides
using namespace std::literals::string_literals;
std::pair p = { "Index"s, 16u };
```


## `std::vector` ako mapa

* Prvky vectoru budú `std::pair`
   * Prvý prvok bude kľúč
   * Druhý prvok bude hodnota
* Pridávanie a mazanie bude jednoduché pridávanie do vectora, ale bude treba zabezpečiť, aby sa kľúče neopakovali
* Hľadanie bude lineárne


## `assoc_vector`

```cpp
class assoc_vector {
public:
  using key = std::string;
  using value = size_t;
  using node = std::pair<key, value>;
  using array = std::vector<node>;
  using iterator = array::iterator;

  iterator find(const key& key);
  void erase(const iterator& position);
  void insert(const node& what);
  iterator end() { return _array.end(); }
private:
  array _array;
};
```


## `find`

```cpp
assoc_vector::iterator assoc_vector::find(const key& key) {
  return std::find_if(_array.begin(), _array.end(), [&key](const node& n) {
    return n.first == key;
  });
}
```


## `insert` a `erase`

```cpp
void assoc_vector::insert(const node& what) {
  auto it = find(what.first);
  if (it == end()) {
    _array.push_back(what); // insert value
  } else {
    *it = what; // replace value
  }
}

void assoc_vector::erase(const iterator& position) {
  _array.erase(position);
}
```


## Použitie `assoc_vector`

```cpp
int main() {
  assoc_vector people;
  people.insert({ "Bjarne", 68 });
  people.insert({ "Brian", 77 });
  people.insert({ "Ken", 76 });
  auto it = people.find("Ken");
  if (it != people.end()) {
    std::cout << it->second;
  }
}
```


## Zložitosť operácií

<table>
<tr>
  <th>Operácia</th>
  <th>Zložitosť</th>
  <th>Poznámka</th>
</tr>
<tr>
  <td>Vyhľadanie</td>
  <td>O(n)</td>
  <td>Sekvenčné hľadanie</td>
</tr>
<tr>
  <td>Vkladanie</td>
  <td>O(n)</td>
  <td>Spúšta vyhľadanie</td>
</tr>
<tr>
  <td>Mazanie</td>
  <td>O(n)</td>
  <td>Spúšta vyhľadanie</td>
</tr>
</table>

* Lineárne zložitosti sú v zásadé zlé
* Hlavne vyhľadávanie, ktoré sa zvyčajne robí veľmi často
* Ak by sme chceli zlepšiť vyhľadávanie, tak by sme mohli použiť triedený vector a binárne vyhľadávanie


## Utriedený vector

```cpp
bool assoc_vector::compare_keys(const MyPair& lhs, const Key& rhs) {
  return lhs.first < rhs;
}

assoc_vector::iterator assoc_vector::find(const Key& key) {
  auto it = std::lower_bound(
    _array.begin(),
    _array.end(),
    key,
    compare_keys
  );

  if (it != m_array.end() && it->first == key) {
    return it;
  }
  return m_array.end();
}
```

* Ešte je potrebné zabezpečiť, aby bol vector utriedený
* Zmena v `insert` a `erase` je tiež potrebná, ale celkom jednoduchá
* `std::lower_bound` je binárne vyhľadávanie, ktoré nám vráti miesto, kde by sa prvok mal nachádzať
* O(log n) zložitosť vyhľadávania


## `boost::flat_map`

* `boost` knižnica má implementovaný `flat_map`, čo je v podstate utriedený vector
* V C++23 `std::flat_map` je to isté

<table>
<tr>
  <th>Operácia</th>
  <th>Zložitosť</th>
  <th>Poznámka</th>
</tr>
<tr>
  <td>Vyhľadanie</td>
  <td>O(log n)</td>
  <td>Binárne vyhľadávanie</td>
</tr>
<tr>
  <td>Vkladanie</td>
  <td>O(n)</td>
  <td>Niekedy musí urobiť zväčšenie vectora</td>
</tr>
<tr>
  <td>Mazanie</td>
  <td>O(n)</td>
  <td>Prvky sa musia poposúvať</td>
</tr>
</table>

---

## `std::map`

* Asociatívne pole z knižnice
* Implementované ako vyvážený binárny strom
* Kľuče sú unikátne a porovnateľné pomocou operátora `<`
* Druhá najpopulárnejšia dátová štruktúra (po vectoroch)

<table>
<tr>
  <th>Operácia</th>
  <th>Zložitosť</th>
  <th>Poznámka</th>
</tr>
<tr>
  <td>Vyhľadanie</td>
  <td>O(log n)</td>
  <td>Hľadanie v binárnom strome</td>
</tr>
<tr>
  <td>Vkladanie</td>
  <td>O(log n)</td>
  <td>Musí urobiť vyváženie stromu</td>
</tr>
<tr>
  <td>Mazanie</td>
  <td>O(log n)</td>
  <td>Musí urobiť vyváženie stromu</td>
</tr>
</table>


## Implementácia

* Zložitostné ohraničenia v podstate diktujú samovyvažujúci sa binárny strom
* MSVC používa červeno-čierny strom
   * Samotný objekt `std::map` obsahuje iba pointer na koreň stromu a veľkosť (pre rýchle zistenie počtu prvkov)
   * Uzly v strome sú alokované dynamicky, obsahujú kľúč, hodnotu, ukazatele na deti a rodiča a farbu
* Mapa potrehuje aby kľúče boli porovnateľné (strict weak ordering)
   * Ak A je menšie ako B, potom *(A < B == true) && (B < A == false)*
   * Ak A je ekvivalentné B, potom *(A < B == false) && (B < A == false)*
   * Ak A je väčšie ako B, potom *(A < B == false) && (B < A == true)*


## Operácie

<table>
<tr>
  <th>Operácia</th>
  <th>Efekt</th>
</tr>
<tr>
  <td>size</td>
  <td>Veľkost kontajnera</td>
</tr>
<tr>
  <td>empty</td>
  <td>true ak je kontajner prázdny</td>
</tr>
<tr>
  <td>operator[]</td>
  <td>Nájde zadaný prvok</td>
</tr>
<tr>
  <td>clear</td>
  <td>Zmaže celý kontajner</td>
</tr>
<tr>
  <td>insert</td>
  <td>Vloží nový prvok, ak už ekvivalentný v mape existuje, tak ho prepíše, návratová hodnota obsahuje informáciu, či došlo k prepisu</td>
</tr>
<tr>
  <td>erase</td>
  <td>Zmaže prvok</td>
</tr>
<tr>
  <td>begin / end</td>
  <td>Podpora iterátorov</td>
</tr>
</table>


## `std::map` príklad

```cpp
std::map<std::string, int> ages;
ages["Bjarne"] = 68;
ages["Brian"] = 77;
ages["Ken"] = 76;

auto res = ages.insert({ "Ken", 77 }); // replace value
std::cout << res.second; // 0
res = ages.insert({ "Gabriel", 60 }); // add value
std::cout << res.second; // 1
ages.erase("Brian");

auto it = ages.find("Ken");
if (it != ages.end()) {
  std::cout << it->second;
}
```


## `operator[]`

* Index operátor má definované správanie ak kľúč nie je v mape
   * Vloží kľúč do mapy
   * Hodnota sa default zkonštruuje

```cpp
std::map<std::string, int> ages;
ages["Bjarne"] = 68;
ages["Brian"] = 77;
ages["Ken"] = 76;

std::cout << people["Herb"]; // print 0
std::cout << people.size(); // print 4
```


## Iterácia

* Iterátory nie sú zneplatnené, keď vložíme nový prvok
* Pri mazaní je zneplatnený iba iterátor na zmazaný prvok
* Modifikovanie kontajnera počas iterácie
   * Veľmi zlé pre `std::vector`
   * OK pre `std::map`


## Mazanie viacerých prvkov

* Mapa nemá podporu na mazanie viacerých kľúčov
* Musíme si ju naprogramovať sami (C++ idiom)

```cpp
for (auto it = people.begin(); it != people.end(); ) {
  if (it->first[0] == 'B') {
    it = people.erase(it);
  } else {
    ++it;
  }
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
// second parameter is the value that should be at the new front
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

## Algoritmy a adaptéry

* Knižnica `ranges` obsahuje algoritmy a adaptéry
   * Algoritmy sú funkcie, ktoré fungujú podobne ako algoritmy z `<algorithm>`
   * Adaptéry rozsahov sú funkcie, ktoré transformujú alebo filtrujú rozsahy a vytvárajú z nich nový rozsah bez toho, aby pôvodné dáta modifikovali
* Adaptéry sa volajú až keď si vyžiadame ich hodnoty

```cpp
std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
auto even = v | std::views::filter([](int i) { return i % 2 == 0; });
auto x = even | std::views::drop(1) 
  | std::views::transform([](int i) { return i + 2; })
  | std::views::reverse
  | std::views::take(2);

// alternatively we can use transform_view object directly, instead of pipe
auto y = std::ranges::transform_view(x, [](int i) { return std::to_string(i); });
auto z = y | std::views::join; // will flatten the ranges
for (auto i : z) {
  std::cout << i << ' '; // 1 0 8
}
```

---

# ĎAKUJEM

## Otázky?