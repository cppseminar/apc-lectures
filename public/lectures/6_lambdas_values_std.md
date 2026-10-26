# C++

## Lambdy, hodnotová sémantika a algoritmy

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Dedukcia typov (`auto`, `decltype`) a smerníky na funkcie
* Funktory a lambda funkcie `[](){}`
* `std::function` a `std::visit`
* Objekty ako hodnota (hodnotová sémantika)
* Preťažovanie operátorov
* Iterátory, štandardné algoritmy a `std::ranges`

---

# Dedukcia typov a smerníky na funkcie

---

## `decltype` a `std::is_same_v`

* `decltype(expr)` vráti presný typ premennej alebo výrazu počas prekladu (bez vyhodnotenia výrazu)
* `std::is_same_v<T, U>` (z `<type_traits>`) overí, či sú dva typy identické

```cpp
auto i = 3;
decltype(i) j = 4;       // int
decltype(i + 4.9) y = 5; // double

using int32 = int;
static_assert(std::is_same_v<int, int32>); // OK: using creates a type alias

const auto s = std::string("hello");
// static_assert(std::is_same_v<decltype(s), std::string>);    // error
static_assert(std::is_same_v<decltype(s), const std::string>); // OK
```

---

## `auto` návratový typ

```cpp
auto intpower(double base, uint32_t exponent) -> double {
    double result = 1.0;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }

    return result;
}
```

* `-> double` je *trailing return type* a umožňuje písať návratový typ funkcie až za zoznam parametrov


## Vnorené typy

```cpp
class A {
public:
    struct Data {
        int x;
        int y;
    };

    Data getData();
};

A::Data A::getData() { return { 1, 2 }; }
auto A::getData() -> Data { return { 1, 2 }; }
```

* Pri zápise za parametrami je návratový typ už v scope triedy `A`, takže nemusíme opakovať `A::Data`


## Automatická dedukcia návratového typu

```cpp
auto intpower(double base, uint32_t exponent) {
    double result = 1.0;
    // ...
    return result;
}
```

* Od C++14 sa návratový typ vie odvodiť priamo z `return` príkazu, ak všetky `return` vracajú rovnaký typ

```cpp
auto factorial(unsigned n) {
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}
```

* V tomto prípade je to kompilačná chyba, pretože `1` je `int` a `n * factorial(n - 1)` je `unsigned`
* Buď explicitne napíšeme návratový typ (`-> unsigned`), alebo zmeníme `1` na `1u`

---

## Smerníky na funkcie

* Funkcie majú v pamäti svoju adresu, na ktorú môžeme vytvoriť smerník
* Takýto smerník potom môžeme uložiť do premennej, odovzdať ako parameter a zavolať pomocou `()`

```cpp
int add(int a, int b) {
    return a + b;
}

int (*add_ptr)(int, int) = add;
double (*intpow_ptr)(double, uint32_t) = intpower;
```

* Syntax je `return_type (*name)(param1_type, param2_type, ...)`


## Použitie smerníka na funkciu

```cpp
int add(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}

int main() {
    int (*fun)(int, int) = nullptr;
    fun = &add;
    std::println("{}", fun(3, 3)); // 6
    fun = mul;
    std::println("{}", fun(3, 3)); // 9
}
```

* Funkcie sa automaticky konvertujú na smerník na funkciu, takže nemusíme písať `&`
* `nullptr` znamená, že smerník neukazuje na žiadnu funkciu; jeho zavolanie má nedefinované správanie


## `typedef` a `using`

* Pre čitateľnejší zápis si môžeme vytvoriť alias na typ pomocou `typedef` alebo `using`
* `using` (od C++11) je prehľadnejší – meno aliasu je vždy na začiatku a nie uprostred deklarácie

```cpp
typedef std::string (*fun_ptr1)(int, int);
using fun_ptr2 = std::string (*)(int, int);

int main() {
    static_assert(std::is_same_v<fun_ptr1, fun_ptr2>, "The same function type");

    fun_ptr1 f1 = nullptr;
    fun_ptr2 f2 = f1;
}
```

---

# Funktory

---

## Volateľné objekty (*Callables*) a funktory

* Smerník na funkciu vieme zavolať pomocou `()`, preto ho voláme aj *callable*
* Ak potrebujeme, aby si funkcia pamätala vnútorný stav alebo konfiguráciu, použijeme **funktor** (*functor* / *function object*) – objekt triedy, ktorá má preťažený `operator()`

```cpp [|5-7|14-17]
struct Counter {
    explicit Counter(int stride) 
        : stride_(stride) { }

    int operator()() {
        return counter_ += stride_;
    }

    int counter_ = 0;
    const int stride_;
};

int main() {
    Counter cnt(2);
    std::println("{}", cnt()); // 2
    std::println("{}", cnt()); // 4
    std::println("{}", cnt()); // 6
}
```

---

## Funktory v štandardnej knižnici

* Štandardná knižnica (v `<functional>`) obsahuje hotové funktory pre bežné operácie: `std::plus`, `std::minus`, `std::multiplies`, `std::greater`, `std::less`, ...

```cpp
std::multiplies<int> mul;
std::println("{}", mul(6, 7)); // 42
```

* Pre vlastnú operáciu (napr. spájanie reťazcov s oddeľovačom) však musíme napísať celú triedu – v modernom C++ to vieme zapísať oveľa kratšie pomocou **lambda funkcií**.

---

# Lambdy

---

## Lambda funkcie

* Občas nazývané aj *anonymous functions* alebo *closures*
* V podstate ide o syntaktický cukor, ktorým kompilátor na pozadí vygeneruje funktor

`[](){}`<!-- .element: style="font-size: 3em;" -->
<!-- .element: style="text-align: center;" -->

```cpp
auto lambda = []() {}; // lambda which does absolutely nothing
lambda();

auto lambda2 = [] {}; // simplest lambda (empty parameter list can be omitted)
```


## Použitie lambdy na zjednodušenie kódu

```cpp [4,13|6|7-10]
class Joiner {
public:
    explicit Joiner(char delim)
        : delimiter(delim) { }

    std::string operator()(const std::string& a, const std::string& b) const {
        if (a.empty()) {
            return b;
        }
        return a + delimiter + b;
    }
private:
    char delimiter;
};
```

```cpp
char delim = '.';
auto lambda = [delim](const std::string& a, const std::string& b) { 
    return a.empty() ? b : a + delim + b;
};
```
<!-- .element: class="fragment" -->

---

## Lambda definícia

`[capture](params) { body }`<!-- .element: style="font-size: 2em;" -->

* `capture` – zoznam lokálnych premenných (s automatickou dobou uloženia) z okolitého scope-u, ktoré sa majú zachytiť do členskej premennej generovaného funktora:
    * `[]` – žiadne premenné
    * `[=]` – všetky použité lokálne premenné ako kópia
    * `[&]` – všetky použité lokálne premenné ako referencie
    * `[var]` – premenná `var` ako kópia
    * `[&var]` – premenná `var` ako referencia
    * `[=, &var]` – predvolene kópia, ale `var` ako referencia
* Globálne a `static` premenné sa nezachytávajú (v tele sú dostupné priamo)
* `params` – zoznam parametrov volania lambdy
* `body` – telo funkcie


```cpp
int i = 10;
double d = 1.0;

auto lambda = [i, &d](int x) {
    // i += x; // error: variables captured by value are const by default
    d += i;
    d += x;
};

lambda(1);
std::println("{}", d); // 12
i = 100;
lambda(2);
std::println("{}", d); // 24
```

* `i` je zachytené ako kópia v momente vytvorenia lambdy, takže neskoršia zmena `i = 100` ho neovplyvní
* Navyše je `operator()` lambdy predvolene `const`, takže kópiu `i` v tele nemôžeme meniť
* `d` je zachytené ako referencia, takže zmeny sa dejú priamo na pôvodnej premennej `d` (pozor na jej životnosť!)


```cpp
int i = 10;
int j = 20;

auto lambda = [=, &i]() { // default capture must be first
    i += 1;
    return i + j;
};

auto lambda2 = [&]() {
    i += 1;
    j += 1;
    return i + j;
};

std::println("{}", lambda());  // 31 (i=11, copy of j=20)
std::println("{}", lambda2()); // 33 (i=12, j=21)
std::println("{}", lambda());  // 33 (i=13, copy of j=20)
std::println("{}", lambda2()); // 36 (i=14, j=22)
```

---

## Lambdy môžu byť aj inline

* Definovanie samostatného funktora je nepraktické, ak ho chceme použiť iba na jednom mieste
* Lambdu môžeme zapísať priamo ako argument do funkcie alebo algoritmu

```cpp
std::vector<int> vec = { 1, 2, 3, 4, 5 };

auto it = std::ranges::find_if(vec, [](int x) {
    return x > 3;
});
```

---

## Lambda s návratovým typom

* Štandardne sa návratový typ lambdy odvodí z `return` príkazu (pomocou `auto`)
* Ak chceme návratový typ určiť explicitne (napr. pri viacerých `return` s rôznymi typmi), použijeme `-> typ`

```cpp
auto add_backslash = [](const char* x) -> std::string {
    size_t length = std::strlen(x);
    if (length < 1 || x[length - 1] == '\\')
        return x;
    return std::string(x) + '\\';
};

auto s = add_backslash("C:\\Windows");
```


```cpp
class A { public: virtual ~A() = default; };
class B : public A { };
class C : public A { };

auto factory = [](int x) -> std::unique_ptr<A> {
    if (x % 2 == 0)
        return std::make_unique<B>();
    return std::make_unique<C>();
};
```

---

## Generické lambdy

* Od C++14 môžeme v zozname parametrov lambdy použiť `auto` (vygeneruje sa šablónový `operator()`)

```cpp
auto add_to_string = [](auto x, auto y) -> std::string {
    return std::format("{} {}", x, y);
};

std::println("{}", add_to_string(12, 5.0));   // 12 5
std::println("{}", add_to_string(1, 2));      // 1 2
std::println("{}", add_to_string(3.14, 1.0)); // 3.14 1
```

---

## Výraz v capture liste a `mutable`

* V capture liste môžeme vytvoriť novú premennú inicializovanú ľubovoľným výrazom (*init capture* od C++14)
* Kľúčové slovo `mutable` odstráni `const` z `operator()`, takže zachytené kópie môžeme v tele meniť

```cpp
int i = 1;
auto func = [a = i + 10]() mutable {
    a++;
    std::println("{}", a);
};

func(); // 12
func(); // 13
```

* Výraz `i + 10` sa vyhodnotí raz pri vytvorení lambdy a inicializuje členskú premennú `a`
* Hodí sa aj na presun objektov do lambdy, napr. `[ptr = std::move(unique_ptr)]() { ... }`

---

## Zachytávanie členov v metódach (`this` vs. `*this`)

* Ak v metóde triedy vytvoríme lambdu a chceme pristupovať k členským premenným, zachytáva sa smerník `this`:

```cpp
class Worker {
    int id_ = 42;
public:
    auto make_callback() {
        // [this] captures only the pointer to the current Worker!
        // [*this] (since C++17) captures a copy of the whole Worker object
        return [*this]() { std::println("Worker {}", id_); };
    }
};
```

* `[this]` (aj `[&]`) zachytí iba **smerník `this`** – ak objekt `Worker` zanikne skôr, než sa lambda zavolá, vznikne *dangling pointer* (undefined behavior)
* Pozor: `[=]` v metóde takisto zachytával iba smerník `this` (od C++20 je takéto implicitné zachytenie `this` cez `[=]` *deprecated*)
* Ak má lambda prežiť pôvodný objekt, použijeme `[*this]` (kópia celého objektu) alebo init capture `[id = id_]()`

---

## Výhody a nevýhody lambda funkcií

<div style="display: flex;">
<div style="flex: 1;">

**Výhody**
* Ideálne na použitie so štandardnými algoritmami
* Definícia priamo na mieste použitia
* Nulová réžia oproti ručne písanému funktoru (kompilátor ju ľahko inlinuje)
* Podporujú uchovanie stavu
</div>
<div style="flex: 1;">

**Nevýhody**
* Pri zachytávaní referencií (`[&]`) hrozí *dangling reference*, ak lambda prežije zachytené lokálne premenné
* Príliš dlhá vnorená lambda zhoršuje čitateľnosť – ak ju potrebujeme na viacerých miestach, je lepšia pomenovaná funkcia alebo funktor
</div>
</div>

---

# `std::function`

---

## Čo je typ lambdy?

```cpp
int a = 0;
auto lambda = [a](int x) mutable { return ++a + x * x; };
// what to write instead of auto?
```

<ul>
    <li>Každá lambda má vlastný unikátny anonymný typ vygenerovaný kompilátorom, ktorý nevieme priamo napísať</li>
    <li>Lambda bez zachytených premenných (<code>[]</code>) sa vie implicitne skonvertovať na obyčajný smerník na funkciu</li>
    <li class="fragment">
        Ak však chceme ukladať ľubovoľné callable objekty (aj so zachyteným stavom) do kontajnera alebo atribútu triedy, použijeme <code>std::function</code>
    </li>
</ul>

---

## Kolekcia funkcií

```cpp
double f = 1.1;
std::vector<std::string(*)(int)> v;

// the cast on next line is necessary to select the right overload
v.push_back(static_cast<std::string(*)(int)>(std::to_string));
v.push_back([](int i) { return std::string(i, 'a'); });
// v.push_back([f](int i) { return std::to_string(f * i); }); // will not compile!

std::vector<std::function<std::string(int)>> v2;
v2.push_back(static_cast<std::string(*)(int)>(std::to_string));
v2.push_back([](int i) { return std::string(i, 'a'); });
v2.push_back([f](int i) { return std::to_string(f * i); });

for (auto& fn : v2) {
    std::println("{}", fn(10));
}
```

---

## `std::function`

* `std::function<R(Arg1, Arg2, ...)>` (z `<functional>`) je polymorfný obal pre kopírovateľný callable objekt s danou signatúrou
* Dokáže držať voľnú funkciu, funktor aj lambdu so zachytenými premennými
* Ak lambda zachytáva nekopírovateľný objekt (napr. `std::unique_ptr`), od C++23 použijeme `std::move_only_function`
* Používa techniku *type erasure* (podobne ako `std::any`)
* Väčšinou využíva *Small Buffer Optimization* – malé lambdy sa uložia priamo do objektu `std::function`, väčšie sa alokujú na halde
* Môže byť prázdna (`nullptr`); jej zavolanie v prázdnom stave vyhodí výnimku `std::bad_function_call`
* Užitočné pre callbacky, event handlery a uloženie akcií v triedach

---

## Rekurzia v lambdách

* Bežná `auto` lambda nemôže v tele priamo volať samu seba, pretože jej typ ešte nie je známy počas prekladu tela:

```cpp
auto lambda = [](int x) {
    if (x == 0)
        return 1;
    return x * lambda(x - 1); // error: cannot use 'lambda' in its own initializer
};
```


## Riešenia rekurzie

<div style="display: flex;">
<div style="flex: 1;">

**1. Cez `std::function` (C++11)**
```cpp
std::function<int(int)> fact = [&](int x) {
    if (x == 0)
        return 1;
    return x * fact(x - 1);
};
```
* Typ `fact` je známy pred telom lambdy
* Má však miernu réžiu nepriameho volania a zachytáva `fact` referenciou
</div>
<div style="flex: 1;">

**2. Cez `this auto` (C++23)**
```cpp
auto fact = [](this auto self, int x) {
    if (x == 0)
        return 1;
    return x * self(x - 1);
};
```
* *Explicit object parameter* (*deducing `this`*) v C++23 sprístupní samotnú lambdu ako prvý parameter `self` bez réžie `std::function`
</div>
</div>

---

# `std::variant` a `std::visit`

---

## `std::monostate`

* V minulej prednáške sme videli `std::variant<Ts...>` ako typovo bezpečnú alternatívu k polymorfizmu
* Štandardne `std::variant` nemôže byť „prázdny“ – jeho predvolený konštruktor skonštruuje prvý typ v zozname
* Ak prvý typ nemá default konštruktor (alebo chceme reprezentovať aj prázdny stav), dáme na prvé miesto `std::monostate`

```cpp
class Connection {
public:
    explicit Connection(int port) { } // no default constructor
};

int main() {
    std::variant<std::monostate, Connection> conn; // starts in monostate (empty)
    assert(std::holds_alternative<std::monostate>(conn));
    conn = Connection(8080);
}
```

---

## `std::visit`

* Namiesto ručného vetvenia cez `if (auto* p = std::get_if<T>(&v))` môžeme použiť `std::visit`
* Prijíma *callable* objekt (*visitor*) a jeden alebo viac `std::variant` objektov
* Zavolá `visitor` s hodnotou, ktorá je aktuálne uložená vo variante
* Kompilátor skontroluje, že `visitor` dokáže spracovať **všetky** možné typy variantu!
* Ak majú všetky typy rovnaké rozhranie (napr. `AnySequence` z minulej prednášky), stačí generická lambda:

```cpp
using AnySequence = std::variant<ConstSeq, ArithmeticSeq>;
std::vector<AnySequence> seqs = { ArithmeticSeq{1, 10}, ConstSeq{0} };

for (auto& seq : seqs) {
    int64_t val = std::visit([](auto& s) { return s.Next(); }, seq);
    std::println("{}", val);
}
```


## Overloaded pattern s lambdami

* Pre rôznorodé typy je najčitateľnejšie zložiť niekoľko lámbd do jedného funktora pomocou šablóny `overloaded` (od C++20 stačia tieto 2 riadky):

```cpp
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
```

```cpp
std::variant<int, std::string, std::vector<int>> v = "Hello";

std::visit(overloaded {
    [](int i) { std::println("int: {}", i); },
    [](const std::string& s) { std::println("string: {}", s); },
    [](const std::vector<int>& vec) { std::println("vector of size: {}", vec.size()); }
}, v);
```

* Ak do `std::variant` pridáme nový typ a zabudneme preň dopísať vetvu, program sa neskompiluje
* Od C++26 pribudla aj členská metóda `v.visit(...)`, takže netreba písať `std::visit(..., v)`

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
    * Áno, lebo reprezentujú rovnakú hodnotu (`a == b`)
    * Nie, lebo sídlia na rôznych adresách v pamäti (`&a != &b`)

---

## Objekty ako hodnoty

* Pri hodnotovej sémantike sa nezaujímame o identitu (adresu v pamäti), ale porovnávame a kopírujeme objekty na základe ich hodnoty
* C++ je na hodnotovej sémantike priamo postavené a poskytuje:
    * Kopírovanie pomocou kopírovacieho konštruktora a operátora priradenia
    * Preťažovanie operátorov
        * `==` pre ekvivalenciu
        * `<` a `<=>` pre usporiadanie
        * `+`, `-`, `*`, ...

---

## Kopírovací konštruktor a operátor priradenia

* Vaše triedy ich majú, aj keď ich sami nenapíšete
* Kompilátor ich vygeneruje ako kopírovanie všetkých členských premenných po jednotlivých členoch
* Automatická generácia aj pre triedy so surovými smerníkmi je historická chyba návrhu C++

```cpp
class MyClass {
public:
    MyClass(const MyClass&);            // copy constructor
    MyClass& operator=(const MyClass&); // copy assignment operator
};
```


## Automaticky vygenerované operácie

<div style="display: flex;">
<div style="flex: 1;">

```cpp
class MyClass {
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
</div>
<div style="flex: 1;">

```cpp
class MyClass {
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
</div>
</div>

* Správne pre typy ako `int`, `std::string`, `std::vector` (Rule of Zero!)
* Pravdepodobne zlé pre vlastnený surový smerník `void*` (plytká kópia / *shallow copy* vedie k dvojitému `delete`)
* Ak kópia pre náš typ nedáva zmysel, zakážeme obe operácie pomocou `= delete`; ak nám predvolené vyhovujú, môžeme ich označiť `= default`

---

## Vlastné kopírovacie operácie

<table>
<tr>
<td style="width: 60%">

```cpp [7-11|13-18]
class Buffer {
public: 
    explicit Buffer(size_t n)
        : data_(new std::byte[n]), size_(n) { }
    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other)
        : data_(new std::byte[other.size_])
        , size_(other.size_) {
        std::copy_n(other.data_, size_, data_);
    }

    Buffer& operator=(const Buffer& rhs) {
        size_ = rhs.size_;
        data_ = new std::byte[size_];
        std::copy_n(rhs.data_, size_, data_);
        return *this;
    }
private:
    std::byte* data_;
    std::size_t size_;
};
```
</td>
<td>
<div class="fragment">

* Kopírovací konštruktor je v poriadku (vytvára nový objekt)
* Kopírovací operátor priradenia však **nie je správny**:
    * Pôvodné `data_` v `this` sme pred prepísaním neuvoľnili – máme memory leak!
</div>
</td>
</tr>
</table>


## Pokus č. 2: uvoľnenie starej pamäte

```cpp [|2]
Buffer& operator=(const Buffer& rhs) {
    delete[] data_;
    size_ = rhs.size_;
    data_ = new std::byte[size_];
    std::copy_n(rhs.data_, size_, data_);
    return *this;
}
```

<div class="fragment">

* Čo ak spravíme samopriradenie `a = a;`?
    * Najprv si cez `delete[] data_` zmažeme vlastný buffer a potom z neho kopírujeme (`rhs.data_` je to isté ako `data_`) – undefined behavior!
* Čo ak `new` vyhodí výnimku `std::bad_alloc`?
    * `data_` už ukazuje na zmazanú pamäť a deštruktor zavolá `delete[]` druhýkrát!
</div>


## Bezpečný `operator=`

<div style="display: flex;">
<div style="flex: 1;">

**Najprv alokácia, potom výmena**
```cpp
Buffer& operator=(const Buffer& rhs) {
    if (this != &rhs) {
        auto* new_data = new std::byte[rhs.size_];
        std::copy_n(rhs.data_, rhs.size_, new_data);
        delete[] data_;
        data_ = new_data;
        size_ = rhs.size_;
    }
    return *this;
}
```
</div>
<div style="flex: 1;">

**Copy-and-swap idiom**
```cpp
Buffer& operator=(Buffer rhs) { // copy made in param
    std::swap(data_, rhs.data_);
    std::swap(size_, rhs.size_);
    return *this;
} // rhs destructor frees old data_!
```
</div>
</div>

* V oboch prípadoch platí **silná garancia výnimiek**: ak alokácia zlyhá, pôvodný objekt `this` ostane neporušený.

---

# Preťažovanie operátorov

---

## Preťažovanie operátorov

* Väčšina operátorov v C++ môže byť preťažená pre vlastné typy:
    * Priradenie (`=`)
    * Aritmetické (`+`, `-`, `*`, `/`, `+=`, ...)
    * Bitové (`|`, `&`, `^`, `~`, `<<`, `>>`, ...)
    * Relačné (`==`, `!=`, `<`, `>`, `<=>`, ...)
    * Volanie `()`, indexovanie `[]`, dereferencia `*`, `->`, inkrement `++`, ...
* **Pravidlo:** Operátory preťažujeme iba vtedy, keď je ich význam pre daný typ úplne prirodzený a zrejmý.


## Operátor priradenia `=`

```cpp
int main() {
    MyClass a;

    MyClass b = a;   // copy constructor! (same as MyClass b(a);)
    b = a;           // copy assignment operator
    b.operator=(a);  // explicit call to the same operator

    MyClass c;
    c = b = a;       // chaining works because operator= returns MyClass&
}
```

* Pozor na rozdiel: `MyClass b = a;` vytvára nový objekt, takže volá **kopírovací konštruktor**, nie `operator=`
* Operátor priradenia vracia referenciu na ľavý operand (`return *this;`), čo umožňuje reťazenie `c = b = a;`

---

## Aritmetické operácie (`+=` a `+`)

```cpp
class UInt128 {
public:
    UInt128& operator+=(const UInt128& rhs) {
        low_ += rhs.low_;
        uint64_t carry = (low_ < rhs.low_) ? 1 : 0;
        high_ += rhs.high_ + carry;
        return *this;
    }

    friend UInt128 operator+(UInt128 lhs, const UInt128& rhs) {
        lhs += rhs;
        return lhs;
    }
private:
    uint64_t low_ = 0;
    uint64_t high_ = 0;
};
```

* `operator+=` mení ľavý operand priamo na mieste a vracia `UInt128&` (`*this`)
* `operator+` nemení pôvodné operandy (`z = x + y`), ale vracia novú hodnotu `UInt128`
* Idiomaticky implementujeme `operator+` ako voľnú (často `friend`) funkciu pomocou `operator+=`, pričom prvý parameter `lhs` prijmeme hodnotou

---

## Výpis do streamu `operator<<`

```cpp
class Time {
public:
    explicit Time(time_t t) : time_(t) {}
    time_t seconds() const { return time_; }

    friend std::ostream& operator<<(std::ostream& os, const Time& dt) {
        return os << dt.time_ << "s";
    }
private:
    time_t time_;
};

int main() {
    Time t(5);
    std::cout << t << '\n'; // 5s
}
```

* Keďže ľavým operandom pri `std::cout << t` je `std::ostream`, tento operátor **musí** byť voľnou funkciou (nemôže byť členskou metódou triedy `Time`)


## Podpora pre `std::format` a `std::println` (C++20/23)

* Pre moderný výpis cez `std::format` a `std::println` sa nepreťažuje operátor, ale špecializuje sa šablóna `std::formatter<T>`:

```cpp [|2-4|6-8|13]
template <>
struct std::formatter<Time> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin(); // parses format specifiers inside {}
    }

    auto format(const Time& dt, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}s", dt.seconds());
    }
};

int main() {
    std::println("Cas: {}", Time(5)); // Cas: 5s
}
```

* `parse()` spracúva formátovacie príznaky za dvojbodkou (napr. `{:02}`) – ak ich nepotrebujeme, stačí vrátiť `ctx.begin()`
* `format()` zapíše naformátovaný výsledok do výstupného iterátora `ctx.out()` pomocou `std::format_to`

---

## Indexovací `operator[]`

* Musí byť členskou metódou triedy
* Spravidla preťažujeme obe verzie – nekonštantnú (pre zápis aj čítanie) aj `const` (pre čítanie z `const` objektu)

```cpp
class PtrVector {
public:
    int& operator[](size_t i) { return *vec_[i]; }
    const int& operator[](size_t i) const { return *vec_[i]; }
private:
    std::vector<int*> vec_;
};
```

* Od C++23 môže `operator[]` prijímať aj viacero argumentov, napr. `matrix[row, col]`

---

## Operátory porovnania

* Užitočné pre vyhľadávanie, triedenie a ukladanie do usporiadaných kontajnerov (`std::map`, `std::set`)
* Pred C++20 bolo pre plnú podporu porovnávania potrebné ručne napísať všetkých 6 operátorov (`==`, `!=`, `<`, `<=`, `>`, `>=`)


## Spaceship operátor `<=>` (C++20)

* *Three-way comparison* operátor `<=>` porovná dva objekty naraz a vráti výsledok porovnania (`< 0`, `== 0`, `> 0`)
* Kompilátor z neho automaticky sprístupní `<`, `<=`, `>` a `>=`
* Pri ručnej implementácii `<=>` však kompilátor automaticky **nevygeneruje** `operator==` (treba ho dopísať, z neho sa potom odvodí aj `!=`)

```cpp [|4-9|15-17]
class IntBox {
public:
    explicit IntBox(int i) : data_(std::make_unique<int>(i)) { }
    auto operator<=>(const IntBox& rhs) const { 
        return *data_ <=> *rhs.data_;
    }
    bool operator==(const IntBox& rhs) const {
        return *data_ == *rhs.data_;
    }
private:
    std::unique_ptr<int> data_;
};

int main() {
    assert(IntBox(3) < IntBox(5));
    assert(IntBox(5) <= IntBox(5));
    assert(IntBox(3) != IntBox(5));
}
```


## Kategórie usporiadania a `= default`

* Návratovým typom `<=>` je jedna z kategórií v `<compare>`:
    * `std::strong_ordering` (`less`, `equal`, `greater`) – rovnaké hodnoty sú nerozlíšiteľné (napr. `int`, `std::string`)
    * `std::weak_ordering` (`less`, `equivalent`, `greater`) – ekvivalentné hodnoty sa môžu v niečom líšiť (napr. case-insensitive reťazce)
    * `std::partial_ordering` (navyše `unordered`) – niektoré hodnoty sa nedajú porovnať (napr. `float` a `NaN`)
* Ak nám vyhovuje lexikografické porovnanie všetkých členov v poradí deklarácie, stačí napísať `= default` (tým sa automaticky vygeneruje aj `operator==`!):


```cpp
struct Person {
    std::string name;
    uint8_t age;

    auto operator<=>(const Person& rhs) const = default;
};

int main() {
    assert(Person{"Bjarne", 70} < Person{"Richard", 68});
    assert(Person{"Bjarne", 70} != Person{"Richard", 68});
}
```

---

## Ako preťažiť `++` (prefix vs. postfix)?

```cpp
struct Counter {
    Counter& operator++() { // prefix: ++c
        ++value;
        return *this;
    }

    Counter operator++(int) { // postfix: c++ (dummy int parameter)
        Counter tmp(*this);
        operator++(); // call prefix increment
        return tmp;
    }

    int value = 0;
};
```

* Nepomenovaný parameter `int` slúži iba pre kompilátor na odlíšenie postfixovej verzie od prefixovej
* Prefix `++c` zväčší hodnotu a vráti referenciu na seba (`Counter&`)
* Postfix `c++` si musí odložiť pôvodnú kópiu, zvýšiť stav a vrátiť starú kópiu (`Counter`) – preto v cykloch preferujeme `++it`

---

## Ktoré operátory radšej nepreťažovať?

* `&&` a `||` – preťažená verzia je volanie funkcie, takže **stráca skrátené vyhodnocovanie** (*short-circuit evaluation*) a poradie vyhodnotenia operandov
* `,` (čiarka) a unárny `&` (address-of) – zmena ich správania mätie čitateľa aj knižnice
* Ak operácia nemá jednoznačný matematický alebo doménový zmysel, vždy radšej použijeme pomenovanú metódu

---

# Iterátory a štandardné algoritmy

---

## Algoritmy v štandardnej knižnici

* Hlavičky `<algorithm>` a `<numeric>` obsahujú desiatky hotových algoritmov:
    * vyhľadávanie (`find`, `find_if`, `binary_search`, `lower_bound`, ...)
    * transformácie a kopírovanie (`copy`, `copy_if`, `transform`, `generate_n`, ...)
    * triedenie a usporiadanie (`sort`, `reverse`, `rotate`, ...)
    * redukcie (`accumulate`, `count_if`, ...)
* Algoritmy nepracujú priamo s konkrétnymi kontajnermi, ale s **iterátormi** (od C++20 aj priamo s **ranges**)

---

## Iterátory

* Zovšeobecnenie smerníkov, ktoré prepája kontajnery a algoritmy
* Každý smerník je iterátor, ale nie každý iterátor je smerník

![Triedy iterátorov](./lectures/6_values_algo/iterators.png)

---

## `begin` a `end`

```cpp
std::vector<int> vec = { 0, 5, 4, -1, 2, 10 };

// it has type std::vector<int>::iterator
for (auto it = vec.begin(); it != vec.end(); ++it) {
    std::println("{}", *it);
}
```

* `begin()` vracia iterátor na prvý prvok
* `end()` vracia iterátor **jeden prvok za koniec** (*past-the-end*) – nesmie sa dereferencovať!
* Polootvorený interval `[begin, end)` zjednodušuje reprezentáciu prázdneho rozsahu (`begin == end`) aj ukončovaciu podmienku cyklu (`it != end`)

---

## Kategórie iterátorov

* **Input / Output iterátor** – jednosmerný prechod (`++`), prvok možno prečítať (`*it`) alebo zapísať (`*it = val`) iba raz (napr. stream iterátory, `back_inserter`)
* **Forward iterátor** – jednosmerný prechod (`++`), ale cez ten istý rozsah môžeme prejsť viackrát (napr. `std::forward_list`, minimálna garancia pre `std::unordered_map`)
* **Bidirectional iterátor** – pohyb dopredu (`++`) aj dozadu (`--`) (napr. `std::list`, `std::map`, `std::set`)
* **Random access / Contiguous iterátor** – skok o ľubovoľný počet prvkov v $O(1)$ (`it + n`, `it[n]`, `it2 - it1`), vyžaduje ho napr. `std::sort` (napr. `std::vector`, `std::array`, `std::string`)

---

## `std::copy` a výstupné iterátory

* Algoritmy zapisujúce do výstupného iterátora samy **nezväčšujú cieľový kontajner**!

```cpp
std::vector<int> a = { 1, 2, 3 };
std::vector<int> b;
// std::copy(a.begin(), a.end(), b.begin()); // UNDEFINED BEHAVIOR: b is empty!
```

<div class="fragment">

Máme dve správne možnosti:

<div style="display: flex;">
<div style="flex: 1;">

**1. Vopred nastaviť veľkosť (`resize`)**
```cpp
std::vector<int> b(a.size());
std::copy(a.begin(), a.end(), b.begin());
```
*(Pozor: samotné `reserve()` nestačí, mení len kapacitu!)*
</div>
<div style="flex: 1;">

**2. Použiť vložný adaptér (`std::back_inserter`)**
```cpp
std::vector<int> b;
std::copy(a.begin(), a.end(),
          std::back_inserter(b));
```
</div>
</div>
</div>

---

## Vložné iterátory (`back_inserter`, `inserter`)

* Špeciálne výstupné iterátory z `<iterator>`, ktoré pri priradení `*it = val` volajú metódu kontajnera:
    * `std::back_inserter(c)` – volá `c.push_back(val)` (amortizovane $O(1)$ pre `vector`)
    * `std::front_inserter(c)` – volá `c.push_front(val)` (pre `deque`, `list`)
    * `std::inserter(c, pos)` – volá `c.insert(pos, val)` a posunie pozíciu za vložený prvok (užitočné aj pre `std::set` / `std::map`)

```cpp
std::vector<int> v = { 1, 2, 3 };
auto bi = std::back_inserter(v);
*bi = 10; // calls v.push_back(10)
*bi = 20; // calls v.push_back(20)

auto ins = std::inserter(v, v.begin());
*ins = 0; // inserts 0 at the beginning -> 0, 1, 2, 3, 10, 20
```

---

## Výhody iterátorov

* V debug konfigurácii knižnice často zapínajú kontroly platnosti iterátorov a odhalia chyby hneď pri behu
* V release konfigurácii sa kontroly vypnú a iterátory sa skompilujú na rovnako rýchly kód ako surové smerníky

<table style="width: 80%">
<tr>
<td style="width: 60%">

```cpp
std::string::iterator it;
{
    std::string s = "This is very good string";
    it = s.begin();
}

// undefined behavior: s was destroyed!
// caught in debug builds by assertions:
std::cout << *it;
```
</td>
<td>

![Iterator assert](./lectures/6_values_algo/iterator-assert.png)
</td>
</tr>
</table>

---

## Vyhľadávanie: `std::find` a `std::find_if`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5 };

auto it = std::find(vec.begin(), vec.end(), 3);
assert(it != vec.end() && *it == 3);
assert(it == vec.begin() + 2);

auto even_it = std::find_if(vec.begin(), vec.end(), [](int x) {
    return x % 2 == 0;
});
assert(even_it != vec.end() && *even_it == 2);
```

* `std::find` hľadá prvok pomocou `==`, `std::find_if` hľadá prvý prvok, pre ktorý predikát (napr. lambda) vráti `true`
* Ak sa prvok nenájde, vrátia `end()` iterátor – pred dereferenciou `*it` ho preto vždy kontrolujeme!

---

## Binárne vyhľadávanie v utriedenom rozsahu

* Ak je sekvencia utriedená, namiesto lineárneho `std::find` ($O(n)$) môžeme použiť binárne vyhľadávanie ($O(\log n)$)

```cpp
std::vector<int> vec = { 1, 2, 3, 5 };
assert(std::is_sorted(vec.begin(), vec.end()));

bool found = std::binary_search(vec.begin(), vec.end(), 4); // false

auto it = std::lower_bound(vec.begin(), vec.end(), 4);
if (it != vec.end()) {
    std::println("{}", *it); // 5 (first element >= 4)
}
```

* `std::binary_search` vráti iba `bool`, či sa hodnota v rozsahu nachádza
* `std::lower_bound` vráti iterátor na prvý prvok $\ge$ hľadanej hodnote (miesto, kam by sa dal vložiť bez porušenia utriedenia)
* Ak chceme overiť, či sme našli presne hľadané číslo `x`, skontrolujeme `it != vec.end() && *it == x`

---

## Generovanie a prechod: `generate_n` a `for_each`

```cpp
std::vector<int> v;
std::generate_n(std::back_inserter(v), 5, [n = 0]() mutable {
    return n += 3;
});
// v contains: 3, 6, 9, 12, 15

std::for_each(v.begin(), v.end(), [](int& x) {
    x *= 2;
});
// v contains: 6, 12, 18, 24, 30
```

* `std::generate_n` zavolá bezparametrický callable objekt $n$-krát a výsledky zapíše do výstupného iterátora
* `std::for_each` zavolá funkciu postupne na každý prvok v rozsahu (pre jednoduchý prechod celým kontajnerom zvyčajne preferujeme obyčajný range-based `for` cyklus)

---

## Filtrácia a transformácia: `copy_if` a `transform`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5, 4 };

// Filter even numbers:
std::vector<int> evens;
std::copy_if(vec.begin(), vec.end(), std::back_inserter(evens), [](int x) {
    return x % 2 == 0;
}); // evens: 2, 0, 4

// Transform elements to their squares:
std::vector<int> squares;
std::transform(evens.begin(), evens.end(), std::back_inserter(squares), [](int x) {
    return x * x;
}); // squares: 4, 0, 16
```

* `std::filter` v `<algorithm>` neexistuje – na filtrovanie používame `std::copy_if` (alebo `std::views::filter` v C++20)
* `std::transform` aplikuje funkciu na každý prvok a výsledok zapíše do výstupného iterátora (môže zapisovať aj priamo do toho istého vektora cez `vec.begin()`)

---

## Redukcia: `std::accumulate`

* Nachádza sa v hlavičke `<numeric>` a zredukuje rozsah na jednu hodnotu
* Štandardne prvky sčítava pomocou `+`, ale môžeme dodať vlastnú binárnu operáciu (funktor alebo lambdu)

```cpp
std::vector<int> nums = { 1, 2, 3, 4, 5 };

int sum = std::accumulate(nums.begin(), nums.end(), 0); // 15
int product = std::accumulate(nums.begin(), nums.end(), 1, std::multiplies<int>()); // 120

std::vector<std::string> words = { "Hello", "world", "!" };
auto joined = std::accumulate(words.begin(), words.end(), std::string{},
    [](const std::string& a, const std::string& b) {
        return a.empty() ? b : a + '.' + b;
    });
// joined == "Hello.world.!"
```

---

## Triedenie: `std::sort`

* Triedi prvky v čase $O(n \log n)$ (zvyčajne *introsort* – kombinácia quicksortu, heapsortu a insertion sortu)
* Vyžaduje *random access* iterátory
* Štandardne porovnáva pomocou `<`, ale ako tretí argument môžeme odovzdať vlastný komparátor

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5, 4, 2 };

std::sort(vec.begin(), vec.end()); // ascending: 0, 1, 2, 2, 3, 4, 5

std::sort(vec.begin(), vec.end(), std::greater<int>()); // descending

std::sort(vec.begin(), vec.end(), [](int a, int b) {
    return std::abs(a) < std::abs(b); // custom order by absolute value
});
```

---

## Mazanie prvkov z kontajnera

* Samotné algoritmy nepoznajú kontajner, preto z neho nevedia fyzicky ubrať prvky – na to slúži metóda `erase` kontajnera
* Zmazanie jedného prvku je priamočiare:

```cpp
auto it = std::find(vec.begin(), vec.end(), 0);
if (it != vec.end()) {
    vec.erase(it);
}
```

* Pozor: `vec.erase(it)` zneplatní iterátor `it` aj všetky iterátory za ním!


## Ako (ne)mazať viacero prvkov v cykle?

<div style="display: flex;">
<div style="flex: 1;">

**Zle: preskočenie prvku a neplatný iterátor**
```cpp
for (auto it = vec.begin(); it != vec.end(); ++it) {
    if (*it == value) {
        vec.erase(it); // 'it' is now invalid!
    }
}
```
</div>
<div style="flex: 1;">

**Správne v cykle (ale pre `vector` $O(n^2)$)**
```cpp
for (auto it = vec.begin(); it != vec.end(); ) {
    if (*it == value) {
        it = vec.erase(it); // returns next valid it
    } else {
        ++it;
    }
}
```
</div>
</div>

* Pri `std::vector` každé zavolanie `erase` posúva všetky nasledujúce prvky doľava, takže mazanie v cykle má kvadratickú zložitosť $O(n^2)$


## Erase-remove idiom a `std::erase` (C++20)

* Algoritmus `std::remove` (resp. `std::remove_if`) v jednom lineárnom prechode $O(n)$ presunie všetky ponechané prvky dopredu a vráti iterátor na nový logický koniec
* Zvyšok na konci potom zmažeme jediným volaním `vec.erase(...)`:

![erase remove idiom](./lectures/6_values_algo/erase-remove.png)

```cpp
// Classic C++98/11 erase-remove idiom:
vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
```


```cpp
// Modern C++20 replacement:
std::erase(vec, value);
std::erase_if(vec, [](int x) { return x % 2 == 0; });
```

* Od C++20 používame rovno `std::erase` a `std::erase_if`, ktoré urobia presne to isté za nás (a fungujú správne aj pre `std::map`, `std::set` či `std::list`).

---

# Ranges

---

## `std::ranges` algoritmy (C++20)

* V C++20 pribudla knižnica `<ranges>`, vďaka ktorej už do algoritmov nemusíme posielať dvojicu `vec.begin(), vec.end()`, ale priamo celý kontajner
* Takmer všetky algoritmy zo `std::` majú modernejšiu dvojičku v `std::ranges::`

```cpp
std::vector<int> vec = { 1, 2, 3, 0, 5 };

std::ranges::sort(vec);
auto it = std::ranges::find(vec, 3);
if (it != vec.end()) {
    std::println("Found at index {}", std::distance(vec.begin(), it));
}

std::ranges::reverse(vec);
std::ranges::rotate(vec, vec.begin() + 2);
```

* Ak môžete, v novom kóde uprednostnite algoritmy zo `std::ranges`.


## Projekcie v `std::ranges`

* Algoritmy v `std::ranges` prijímajú okrem komparátora aj **projekciu** – funkciu alebo ukazovateľ na člen, ktorý sa aplikuje na každý prvok pred porovnaním
* Odpadá písanie opakujúcich sa lámbd len kvôli výberu jedného atribútu štruktúry:

```cpp
struct Person {
    std::string name;
    int age;
};

std::vector<Person> people = { {"Bjarne", 73}, {"Herb", 59}, {"Alex", 74} };

// Sort by age ascending ({} uses default std::ranges::less):
std::ranges::sort(people, {}, &Person::age);

// Find person by name:
auto it = std::ranges::find(people, "Herb", &Person::name);
assert(it != people.end() && it->age == 59);
```

---

## Pohľady a adaptéry (`std::views`)

* Adaptéry rozsahov (`std::views::*`) umožňujú skladať operácie ako filtrovanie, transformáciu či orezávanie pomocou operátora `|` (*pipe*)
* Sú **lenivé** (*lazy*) – pri vytvorení pohľadu sa nič nekopíruje ani nepočíta, výpočet prebieha až pri iterovaní prvkov

```cpp
std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

auto pipeline = v
    | std::views::filter([](int i) { return i % 2 == 0; }) // 2, 4, 6, 8
    | std::views::drop(1)                                  // 4, 6, 8
    | std::views::transform([](int i) { return i + 2; })   // 6, 8, 10
    | std::views::reverse                                  // 10, 8, 6
    | std::views::take(2);                                 // 10, 8

for (int x : pipeline) {
    std::print("{} ", x); // 10 8
}
```

* Žiadne pomocné dočasné vektory medzi jednotlivými krokmi nevznikajú!
* Pozor: niektoré pohľady (napr. `filter`, `drop`) si pri iterácii ukladajú pomocný stav, preto premennú `pipeline` neoznačujeme ako `const`


## Materializácia pohľadu: `std::ranges::to` (C++23)

* Samotný pohľad (`view`) dáta nevlastní; ak z výsledku pipeline potrebujeme vyrobiť nový kontajner (napr. `std::vector`), v C++23 použijeme `std::ranges::to`:

```cpp
std::vector<int> v = { 1, 2, 3, 4, 5, 6 };

auto even_squares = v
    | std::views::filter([](int x) { return x % 2 == 0; })
    | std::views::transform([](int x) { return x * x; })
    | std::ranges::to<std::vector>(); // returns std::vector<int>{ 4, 16, 36 }
```

---

# ĎAKUJEM

## Otázky?
