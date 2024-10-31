# C++

## Lambdy `[](){}`

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Kľúčové slovo `auto`
* Funktory
* Lambdy
* `std::function`
* `std::variant`
* `std::cmp_*`

---

# `auto`

---

## Kľúčové slovo `auto`

* V minulosti (*C++03*) to bol *storage class specifier* (podobne ako `static`, `extern` alebo `register`)
* Znamenalo, že premenná je automatická (lokálna) a má svoj lifetime viazaný na blok, v ktorom bola definovaná
* Od *C++11* je to *placeholder type specifier* - kompilátor určí typ premennej podľa použitia


## Nové kľúčové slová

* Pre C++ committee je celkom ťažké pridať nové kľúčové slová do jazyka
    * Musia byť veľmi opatrní, aby sa nestalo, že niekto bude mať kód, ktorý sa správa inak v staršej verzii kompilátora
    * Preto sa snažia použiť existujúce kľúčové slová a rozšíriť ich význam
    * Prípadne zavádzať kontextové kľúčové slová
* `auto` je trochu výnimka, pretože zobrali staré kľúčové slovo a dali mu nový význam
* A ďalšie významy stále pribúdajú<!-- .element class="fragment" -->

---

## *placeholder type specifier*

* `auto` sa používa na deklaráciu premenných
* Kompilátor určí typ premennej podľa priradenia (inicializácie)

```cpp
auto i = 1; // int
auto i(2ll); // long long
auto f{ 3. }; // double
auto i64 = int64_t(4); // int64_t
auto s = "hello"; // const char*
auto* s2 = "world"; // const char*
const auto ci = 7; // const int
```

* Použitím `const`, alebo `*` vieme ovplyvniť, či je premenná konštantná alebo ukazovateľ (inak sa program neskompiluje)

* Dedukcia typov je statická počast kompilácie, nedá sa urobiť niečo ako

```cpp
auto i = 3; // int
i = 3.14; // error
```


## Ako zistíme, aký typ kompilátor určil?

* Použijeme *Visual Studio* (prípadne iné IDE)

![Visual studio type deduction](./lectures/lambdas/visual_studio_deduction.png)


## Použijeme compiler error message

* Vytvoríme templatovú funkcia, ktorej kompilácia vždy zlyhá a tým donútime kompilátor vypísať typ
* Funguje na *GCC*, *Clang* aj *MSVC* 

```cpp
struct InspectType {
    template<typename X>
    explicit InspectType(X) = delete;
};

int main(int argc, const char* argv[]) {
    auto low_precision_pi = 3.0f;
    InspectType x(low_precision_pi);
}
```


## Výstup z kompilátora

```plaintext
1>C:\...\main.cpp(55,15): error C2280: 'InspectType::InspectType<float>(X)': attempting to reference a deleted function
1>C:\...\main.cpp(55,15): error C2280:         with
1>C:\...\main.cpp(55,15): error C2280:         [
1>C:\...\main.cpp(55,15): error C2280:             X=float
1>C:\...\main.cpp(55,15): error C2280:         ]
1>    C:\...\main.cpp(49,2):
1>    see declaration of 'InspectType::InspectType'
1>Done building project "main.vcxproj" -- FAILED.
```

```plaintext
<source>: In function 'int main(int, const char**)':
<source>:8:35: error: use of deleted function 'InspectType::InspectType(X) [with X = float]'
    8 |     InspectType x(low_precision_pi);
      |                                   ^
<source>:3:14: note: declared here
    3 |     explicit InspectType(X) = delete;
      |              ^~~~~~~~~~~
<source>:8:17: warning: unused variable 'x' [-Wunused-variable]
    8 |     InspectType x(low_precision_pi);
      |                 ^
Compiler returned: 1
```

note: Cesty k súborom sú skrátené kvôli prehľadnosti

---

## `std::is_same<T, U>`

* Pomocou `std::is_same<T, U>` z *type_traits* môžeme zistiť, či sú dva typy rovnaké

```cpp
static_assert(std::is_same<int, char>::value); // error

typedef int int32;

static_assert(std::is_same<int, int32>()); // OK
```

* `typedef` nezavádza nový typ, ale len alias, preto sú `int` a `int32` rovnaké
* Podobne to platí aj pre `int32_t`, je to iba `typedef` na `int` (alebo iný typo podľa platformy)


## `decltype`

* `decltype` nám vráti typ premennej, alebo výrazu

```cpp
auto i = 3;
decltype(i) j = 4; // int
```

* Ak zmeníme typ `i`, zmení sa aj typ `j`

```cpp
auto i = 3.14;
decltype(i) j = 4; // double
```

* `decltype` sa dá použiť aj na zistenie typu výrazu

```cpp
int x = 3;
decltype(x + 4.9) y = 5; // double
```


## `auto` a `std::is_same`

```cpp
const auto s = std::string("hello");

static_assert(std::is_same<decltype(s), std::string>()); // error
static_assert(std::is_same<decltype(s), const std::string>()); // OK
```

---

## `auto` return type

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

* `-> double` je *trailing return type* a umožňuje nám písať návratový typ funkcie až za zoznam parametrov


## Vnorené typy

```cpp
class A {
public:
    struct Data {
        int x;
        int y;
    };
};

A::Data A::getData() { return { 1, 2 }; }
auto A::getData() -> Data { return { 1, 2 }; }
```

* Návratový typ je už rovno v scope triedy, takže nemusíme písať `A::Data` ako typ


## Automatická dedukcia návratového typu

```cpp
auto intpower(double base, uint32_t exponent) {
    double result = 1.0;
    // ...
    return result;
}
```

* Návratový typ sa dedukuje z `return` príkazu, ak všetky `return` majú rovnaký typ

```cpp
auto factorial(unsigned n) {
    if (n == 0)
        return 1;
    return n * Factorial(n - 1);
}
```

* V tomto prípade je to kompilačná chyba, pretože `1` je `int` a `n * Factorial(n - 1)` je `unsigned`
* Buď explicitne napíšeme návratový typ (`-> unsigned`), alebo zmeníme `1` na `1u`

---

## Smerníky na funkcie

* Funkcie sú v pamäti umožnené podobne ako dáta (akurát majú nastavené, že sú execuatable)
* Môžeme si preto vytvoriť smerník na funkciu
* Potom takýto smerík môžeme zavolať ako funkciu (pomocou `()`)

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

int main(int argc, const char* argv[]) {
    int (*fun)(int, int) = nullptr;
    fun = &add;
    std::cout << fun(3, 3) << std::endl; // 6
    fun = mul;
    std::cout << fun(3, 3) << std::endl; // 9
}
```

* Funkcie sa automaticky konvertujú na smerník na funkciu, takže nemusíme písať `&`
* `nullptr` je hodnota smerníka, ktorá znamená, že neukazuje na žiadnu funkciu, volanie by skončilo nedefinovaným správaním


## `typedef` a `using`

* Môžeme si vytvoriť alias na typ pomocou `typedef` alebo `using`
* `using` je trochu prehľadnejší, meno typu je na začiatku a nie v strede

```cpp
typedef std::string (*fun_ptr1)(int, int);
using fun_ptr2 = std::string (*)(int, int);

int main() {
    static_assert(std::is_same<fun_ptr1, fun_ptr2>::value, "The same function type");

    fun_ptr1 f1 = nullptr;
    fun_ptr2 f2 = f1;
}
```

---

# Funktory

---

## Volateľné objekty

* Smerík na funkciu vieme zavolať pomocou `()`, voláme ho aj *callable*
* Niekedy chceme mať väčšiu flexibilitu, napr. chceme mať stav
* Na to sa používa *functor* - objekt, ktorý má preťažený `operator()`
* Väčšinou nám stačí okrem `operator()` iba konštruktor


```cpp [8|16|]
class ChangeCase
{
public:
    ChangeCase(bool to_upper)
        : to_upper(to_upper) {
    }

    std::string operator()(const std::string& s) {
        std::string ret_val;
        for (const auto& c : s) {
            ret_val.push_back(m_ToUpper ? toupper(c) : tolower(c));
        }
        return ret_val;
    }
private:
    const bool to_upper;
};
```


```cpp
int main()
{
    ChangeCase to_upper(true);
    std::cout << to_upper("Hello World!") << std::endl;
    std::cout << to_upper("lower case string") << std::endl;
    // temporary
    std::cout << ChangeCase(false)("TEST sTRING") << std::endl;
}
```

* `ChangeCase` je *functor* a `to_upper` je *instance* tohto *functoru*
* `ChangeCase(false)` je *temporary instance* tohto *functoru*
* Functory nám umožňujú mať stav (*stateful* funkcie)

---

## Stateful funkcie

```cpp [12|]
class Counter
{
public:
    Counter(int stride) 
        : stride(stride) {  }

    int operator()() {
        return counter += stride;
    }

private:
    int counter = 0;
    const int stride;
};

int main() {
    Counter cnt(2);
    std::cout << cnt() << std::endl; // 2
    std::cout << cnt() << std::endl; // 4
    std::cout << cnt() << std::endl; // 6
}
```

* `Counter` je *functor*, ktorý si drží stav, takže pokaždom volaní nám vráti inú hodnotu

---

## Iterátory (rýchlokurz)

* Iterátor je objekt, ktorý nám umožňuje prechádzať kontajner

```cpp
std::vector<int> v = { 1, 2, 3, 4, 5 };
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << std::endl;
    *it *= 2;
}
```

* `v.begin()` vráti iterátor na začiatok kontajnera
* `v.end()` vráti iterátor za koniec kontajnera
* `*it` vráti hodnotu, na ktorú ukazuje iterátor (ako smerník), dá sa aj modifikovať
* `++it` posunie iterátor na ďalší prvok


## `std::back_inserter`

* `std::back_inserter` je špeciálny iterátor, ktorý nám umožňuje pridávať prvky na koniec kontajnera

```cpp
// std::back_insert_iterator<std::vector<int>> bi = std::back_inserter(v);
auto bi = std::back_inserter(v);
*bi = 10;
++bi;
*bi = 20;
*bi = 30;
```

* `++bi` je no-op, je tam iba preto aby sa dal použiť ako iterátor
* `*bi = 10` pridá prvok na koniec kontajnera (ako keby sme použili `push_back(10)`)

---

## Použitie callable objektov

* Veľa algoritmov v štandardnej knižnici (`<algorithm>`) očakáva *callable* objekt
* Napr. `std::sort`, `std::transform`, `std::for_each`, `std::find_if`, ...
    * Vyhľadávanie, transformácia, zoradenie, ...
* Posúva nás bližšie k funkcionálnemu programovaniu

```cpp
int func(int x) { return x * 2; }

std::vector<int> v = { 1, 2, 3, 4, 5 };
std::transform(v.begin(), v.end(), v.begin(), func);
```

* Parametre do `std::transform` sú iterátory na začiatok a koniec na vstup, potom začiatok výstupu a *callable* objekt, ktorý sa zavolá na každý prvok


## `std::generate_n`

```cpp
std::vector<int> v(100);
std::generate_n(v.begin(), v.size(), Counter(3));
```

```cpp
std::vector<int> v;
std::generate_n(std::back_inserter(v), 100, Counter(3));
```

* `std::generate_n` očakáva *callable* objekt, ktorý sa zavolá `n` krát (druhý parameter) a výsledok sa uloží do kontajnera (prvý parameter)
* Musíme sa vždy presvedčiť, že máme dostatočne veľký kontajner, inak môžeme skončiť s nedefinovaným správaním
* Výsledok bude `3, 6, 9, 12, 15, ...` až po `300`


## `std::for_each`

```cpp
void to_upper(char& c) {
    c = toupper(c);
}

int main() {
    std::string s("MiXed Case String.");
    std::for_each(s.begin(), s.end(), to_upper);
    std::cout << s << std::endl; // MIXED CASE STRING.
}
```

* `std::for_each` očakáva *callable* objekt, ktorý sa zavolá na každý prvok kontajnera, reprezentovaného iterátormi
* V tomto prípade je to funkcia `to_upper`, ktorá zmení každý znak na veľké písmeno
* Wrapper funkcia je potrebná, pretože `std::toupper` nemá správnu signatúru (je potrebná `void (*)(char&)`, ale `std::toupper` má `int (*)(int)`)
* Poradie aplikácie funkcie je garantované v poradí, v akom sú prvky v kontajneri


## `std::transform`

```cpp
std::string s("MiXed Case String.");
std::transform(s.begin(), s.end(), s.begin(), ::tolower);
std::cout << s << std::endl; // mixed case string.

std::string output;
std::transform(s.begin(), s.end(), std::back_inserter(output), ::toupper);
std::cout << output << std::endl; // MIXED CASE STRING.
```

* Transformuje prvky z jedného kontajnera do druhého
* `std::transform` negarantuje poradie tranformácie, nedá sa na neho spoliehať
* Ak je ale výstupný iterátor `std::back_inserter`, tak potom implementáciu v podstate prinútime robiť transformáciu v poradí

note: `::tolower` a `::toupper` sú funkcie z C knižnice, ktoré pracujú s `int` hodnotami a preto toto warninguje ako divé

---

## Funktory v štandardnej knižnici

* Štandardná knižnica obsahuje veľa *functorov*, ktoré sú pripravené na použitie
* Napr. `std::plus`, `std::minus`, `std::multiplies`, `std::divides`, `std::modulus`, `std::negate`, ...

```cpp
std::vector<int> v1 = { 1, 2, 3, 4, 5 };
std::vector<int> v2 = { 0, 1, 0, 2, 1 };

std::vector<int> result;
std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(result), std::multiplies<int>());
```

---

## `std::accumulate`

```cpp
std::string s("MiXed Case String.");
char c = std::accumulate(s.begin(), s.end(), 0, std::bit_xor<char>());
// c == 0x52

std::vector<int> nums = { 1, 2, 3, 4, 5 };
int product = std::accumulate(nums.begin(), nums.end(), 1, std::multiplies<int>());
// product == 120
```

* `std::accumulate` je funkcia, ktorá nám umožňuje zredukovať kontajner na jednu hodnotu
* Nenachádza sa v `<algorithm>`, ale v `<numeric>`
* Operácia, ktorú urobí je vlastne *start (op) *begin (op) *(begin+1) (op) ...*


## Join stringov pomocou `std::accumulate`

```cpp
class Joiner{
public:
    Joiner(char delim)
        : delimiter(delim) { }

    std::string operator()(const std::string& a, const std::string& b) {
        if (a.empty()) {
            return b;
        }
        return a + delimiter + b;
    }
private:
    char delimiter;
};

std::vector<std::string> s = { "Hello", "world", "!" };
auto joined = std::accumulate(s.begin(), s.end(), std::string(), Joiner('.'));
// Hello.world.!
```

* `Joiner` je *functor*, ktorý zreťazí dva stringy pomocou zadaného delimitéra
* Je to celkom veľký kódu na takúto jednoduchú vec, ale vieme lepšie

---

# Lambdy

---

## Lambda funkcie

* Občas nazývané aj *anonymous functions* alebo *closures*
* V podstate je to syntaktický cukor pre vytvorenie *functoru*

`[](){}`<!-- .element: style="font-size: 3em;" -->
<!-- .element: style="text-align: center;" -->

```cpp
auto lambda = []() {}; // lambda which does absolutely nothing
lambda();

auto lambda2 = [] {}; // simplest lambda
```


## Použitie lambdy na zjednosušenie kódu

```cpp [4,13|6|7-10]
class Joiner{
public:
    Joiner(char delim)
        : delimiter(delim) { }

    std::string operator()(const std::string& a, const std::string& b) {
        if (a.empty()) {
            return b;
        }
        return a + delimiter + b;
    }
private:
    char delimiter;
};
```

```CPP
char delim = '.';
auto lambda = [delim](const std::string& a, const std::string& b) { 
    return a + delim + b;
};
```
<!-- .element class="fragment" -->

---

## Lambda definícia

`[capture](params) { body }`<!-- .element: style="font-size: 2em;" -->

* `capture` zoznam premenných, ktoré sa majú zachytiť zo scope, kde je lambda definovaná
    * `[]` - žiadne premenné
    * `[=]` - všetky premenné ako kópia
    * `[&]` - všetky premenné ako referencie
    * `[var]` - premenná `var` ako kópia
    * `[&var]` - premenná `var` ako referencia
    * `[=, &var]` - všetky premenné ako kópia, `var` ako referencia
* `params` zoznam parametrov do volania lambdy
* `body` telo lambdy


```cpp
int i = 10;
double d = 1.0;

auto lambda = [i, &d](int x) {
    // i += x; error, captured variables are const
    d += i;
    d += x;
};

lambda(1);
std::cout << d << std::endl; // 12
i = 100;
lambda(2);
std::cout << d << std::endl; // 24
```

* `i` je zachytené ako kópia, takže sa nemení zmenou `i` v scope, kde bola lambda definovaná
* Rovnako sa nedá zmeniť v tele lambdy, pretože by default sú zachytené ako `const`
* `d` je zachytené ako referencia, takže sa zmení zmenou `d` v scope, kde bola lambda definovaná


```cpp
int i = 10;
int j = 20;

auto lambda = [=, &i]() { // = must be first
    i += 1;
    return i + j;
};

auto lambda2 = [&]() {
    i += 1;
    j += 1;
    return i + j;
};

std::cout << lambda() << std::endl; // 31
std::cout << lambda2() << std::endl; // 33
std::cout << lambda() << std::endl; // 33
std::cout << lambda2() << std::endl; // 36
```

---

## Lambdy môžu byť aj inline

* S funktormi musíme definovať triedu, potom konštruktor a nakoniec `operator()`
* Definovanie funktoru môže byť veľmi nepraktické, ak ho chceme použiť iba raz
* Lambdy nám umožňujú definovať *"functor"* priamo tam, kde ho chceme použiť

```cpp
std::vector<int> vec = { 1, 2, 3, 4, 5 };
std::vector<int> result;

std::transform(vec.begin(), vec.end(), std::back_inserter(result), [](const int& i) {
    return i * i * i;
});
```

---

## Lambda s návratovým typom

* Štadardne je návratový typ lambdy určený podľa `return` príkazu (akoby bol `auto`)
* Ak chceme explicitne definovať návratový typ, môžeme to urobiť pomocou `-> typ`

```cpp
auto add_backslash = [](const char* x) -> std::string {
    size_t length = strlen(x);
    if (length < 1 || x[length - 1] == '\\')
        return x;
    return std::string(x) + '\\';
};

auto s = add_backslash("C:\\Windows");
```


```cpp
class A { };
class B : public A { };
class C : public A { };

auto lambda = [](int x) -> std::unique_ptr<A> {
    if (x % 2 == 0)
        return std::make_unique<B>();
    return std::make_unique<C>();
};
```

---

## Generické lambdy

* Znovu použije kľúčové slovo `auto` na dedukciu typu parametrov

```cpp
auto add_to_string = [](auto x, auto y) -> std::string {
    return std::to_string(x) + " " + std::to_string(y);
};

std::cout << add_to_string(12, 5.0) << std::endl;
std::cout << add_to_string(1, 2) << std::endl;
std::cout << add_to_string(3.14, 1.0) << std::endl;
```

---

## Premenovanie v capture liste

* Môžeme premenovať premenné v capture liste

```cpp
int i = 1;
auto func = [a = i]() mutable {
    a++;
    std::cout << a;
};

func(); // 2
func(); // 3
```

* `mutable` umožňuje meniť hodnoty zachytených premenných, nie sú `const`

---

## Výhody lambda funkcií

* Veľmi jednoduché na použitie so štandardnými algoritmami
* Definícia priamo tam, kde je použitá (jednoduhšia orientácia v kóde)
* Nulová (alebo minimálna) režia pri vytváraní a volaní
* Môžeme použiť všetky výhody *functorov* (napr. stav)
* Viac približujú C++ k funkcionálnemu programovaniu


## Nevýhody lambda funkcií

* Od C++11, niekroré pokročilé vlastnosti pribúdajú pomaly v štandardoch
* Veľa kódu je generované implicitne, čo môže byť zložité na debugovanie
* Ak niektorú lambda funkciu potrebujeme použiť viackrát, môže byť lepšie definovať ju ako *functor*, alebo funkciu

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
    <li>Typ lambdy nie je špecifikovaný, preto ho nevieme ani priamo použiť</li>
    <li>Čo keď chceme lamdy uložiť do kontajnera, alebo ich poslať ako parameter?</li>
    <li class="fragment">
        Štandardná knižnica nám ponúka riešenie v podobe wrappera <code>std::function</code>
    </li>
</ul>

---

## Kolekcia funkcií

```cpp
double f = 1.1;
std::vector<std::string(*)(int)> v;

// the cast on next line necessary to select to right overload
v.push_back(static_cast<std::string(*)(int)>(std::to_string));
v.push_back([](int i) { return std::string(i, 'a'); });
//v.push_back([f](int i) { return std::to_string(f * i); }); // will not compile
std::vector<std::function<std::string(int)>> v2;
v2.push_back(static_cast<std::string(*)(int)>(std::to_string));
v2.push_back([](int i) { return std::string(i, 'a'); });
v2.push_back([f](int i) { return std::to_string(f * i); });

for (auto& i : v2) {
    std::cout << i(10) << std::endl;
}
```

---

## `std::function`

* `std::function` je wrapper pre ľubovoľný callable objekt
* Môže byť použitý na uloženie lambdy, funkcie, funktoru, ...
* Používa tehcniku *type erasure* na to, aby mohol byť použitý na ľubovoľný typ callable objektu
* Aká je veľkosť `std::function`?
   * Definovaná implementáciou<!-- .element: class="fragment" -->
   * Väčšinou používa small buffer optimization a malý callable sa zmestí priamo do buffera, inak alokácia<!-- .element: class="fragment" -->


* `std::function` je definovaný v `<functional>`
* `std::function<R(Arg1, Arg2...)>` je typ callable objektu, ktorý má návratový typ `R` a parametre `Arg1, Arg2,...`
* Má preťažený `operator()`, takže sa dá použiť ako funkcia
* Môže byť `nullptr`
* Užitočné pre callbacky, eventy, ...


## `std::bind`

* Substitúcia parameterov
* Zmenenie poradia parametrov

```cpp
int f(int a, int b) {
    return a * (b + 1);
}

int main() {
    std::cout << f(1, 2) << std::endl; // 3
    using namespace std::placeholders; // for _1, _2, etc.
    std::function<int(int, int)> f2 = std::bind(f, _2, _1);
    std::cout << f2(1, 2) << std::endl; // 4
    std::function<int(int)> f3 = std::bind(f2, _1, _1);
    std::cout << f3(1) << std::endl; // 2
    std::function<int(int)> f4 = std::bind(f, _1, 5);
    std::cout << f4(1) << std::endl; // 6
}
```


## Môžeme radše použiť lambdu

```cpp
int f(int a, int b) {
    return a * (b + 1);
}

int main() {
    std::cout << f(1, 2) << std::endl; // 3
    auto f2 = [](int a, int b) { return f(b, a); };
    std::cout << f2(1, 2) << std::endl; // 4
    auto f3 = [](int a) { return f(a, a); };
    std::cout << f3(1) << std::endl; // 2
    auto f4 = [](int a) { return f(a, 5); };
    std::cout << f4(1) << std::endl; // 6
}
```

---

# `std::variant`

---

## `union`

* Zdedené z C
* Hodnoty zdieľajú pamäťové miesto

```cpp
union A {
    double f;
    int64_t i;
};

int main() {
    A a;
    a.f = 1;
    std::cout << a.i; // 4607182418800017408
}
```

* V skutočnosti je to nedefinované správanie, pretože povolé je čítať iba poslednú hodnotu, ktorá bola do unionu zapísaná

---

## Typy s konštrutorom

```cpp
union A {
    int64_t i;
    std::string s;
};

int main() {
    A a;
}
```

* Nepodarí sa skonštruovať, chýba konštruktor


## Vlastný konštruktor

```cpp
union A {
    int64_t i;
    std::string s;

    A() { }
    ~A() { }
};

int main() {
    A a;
    a.i = 1;
    new (&a.s) std::string("Ahoj!");
    // a.i = 2; // undefined
    a.s.~basic_string(); // we need explicitly call
}
```

* `new` a  parametrom adresa je *placement new*, ktorý vytvorí objekt na danej adrese
* `~basic_string()` je volanie deštruktora, ktorý sa musí volať explicitne

---

## Variant

```cpp
std::variant<int64_t, double, std::string> a;
a = int64_t(5); // holds int64_t
a = 5.0; // holds double
a = "5"; // holds string

if (std::holds_alternative<int64_t>(a)) {
    // these access are equivalent
    std::cout << std::get<0>(a);
    std::cout << std::get<int64_t>(a);
}
```

* `std::variant` je štruktúra, ktorá môže držať jeden z viacerých typov
   * Vždy drží iba jeden typ a pamätá si, ktorý to je
   * Automaticky volá konštruktor a deštruktor ak treba

---

## `std::monostate`

* Štandardne `std::variant` nemôže byť prázdny, po default konštruktore drží prvý typ
* Ak chceme mať prázdny `std::variant`, môžeme použiť `std::monostate`

```cpp
class A {
public:
    // no default constructor
    A(int) { }
};

int main() {
    std::variant<std::monostate, A> a; // allow empty
    a = A(0);
}
```

---

## Prístup k hodnote

```cpp
std::variant<int, std::string> a;
try {
    std::get<1>(a) = "String";

    auto* x = std::get_if<1>(a); // can be nulltpr, never throws
} catch (const std::bad_variant_access&) {
    std::cout << "String not there";
}
// nullptr if not there
if (auto * str = std::get_if<1>(&a)) {
    std::cout << *str;
}
```

---

## Podľa typu

* K prvkom môžeme pristupovať aj podľa typu, nie iba indexu
* Toto je v poriadku, lebo jeden typ sa nemôže v `std::variant` opakovať

```cpp
std::variant<int64_t, std::string> v;

// throw if not there
auto& s = std::get<std::string>(v);

// nullptr if not there
auto* p = std::get_if<std::string>(&v);
```

---

## Výnimky z konštruktorov

```cpp
struct A {
    A() = default;
    A(const A&) { // always throws
        throw std::exception();
        }
};

std::variant<std::string, A> a;
a = "Hello";
try {
    a = A();
} catch (const std::exception&) { /* noop */ }

std::cout << a.index() << '\n'; // 4294967295
std::cout << a.valueless_by_exception() << '\n'; // 1
std::get<A>(a); // will throw
std::get<std::string>(a); // will throw
```

---

# Porovnávanie čísel

---

## Typy čísel v C++

* C++ má priveľa typov pre čísla
* Niektoré sú rovnaké iba sa inak volajú
   * `int` na väčšine platform je 32bit
   * `long` na väčšine platform je 64bit (ale môže byť aj 32bit)
   * `int32_t` je vždy 32bit, ale teoreticky nemusí existovať
   * `int_least32_t` je aspoň 32bit, ale môže byť aj viac, musí existovať

---

## Promotion a conversion

* Promotion
   * Zmena typu na väčší
   * Nikdy sa hodnota nezmení
   * `char` -> `int`, alebo `long` -> `long long`
* Conversion
   * Zmena typu na iný
   * Môže sa niektorá hodnota zmeniť, ak sa nedajú reprezentovať v novom type

---

## V praxi

```cpp
unsigned int n = std::numeric_limits<int>::max() + 1;
int i = -1;
if (i < n) {
    std::cout << "TRUE" << std::endl;
}
```

* Nevypíše nič<!-- .element class="fragment" -->

---

## Promotion

<a style="display: block;" class="r-stretch" href="https://en.cppreference.com/w/cpp/language/implicit_conversion#Integral_promotion">
  <img src="./lectures/lambdas/integer_promotion.png" alt="Integer promotion from cppreference" />
</a>


## Conversion

<a style="display: block;" class="r-stretch" href="https://en.cppreference.com/w/cpp/language/implicit_conversion">
  <img src="./lectures/lambdas/numeric_conversions.png" alt="Numeric conversions from cppreference" />
</a>


## Operátory

<a style="display: block;" class="r-stretch" href="https://en.cppreference.com/w/c/language/conversion">
  <img src="./lectures/lambdas/numeric_operators.png" alt="Numeric operators from cppreference" />
</a>

---

## `std::cmp_less`

```cpp
int main() {
    unsigned n = std::numeric_limits<int>::max() + 1;
    int i = -1;
    if (std::cmp_less(i, n)) {
        std::cout << "TRUE\n";
    }
}
```

* Funguje správne, pretože `std::cmp_less` porovnáva hodnoty, najprv robí scount, potom
* Funkcie nerobia konverziu

---

# ĎAKUJEM

## Otázky?