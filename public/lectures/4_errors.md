# C++

## Riešenie chybových stavov

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* pretypovanie
* `assert` a `[[assume]]`
* `[[nodiscard]]`
* `std::optional` a `std::expected`
* postupy riešenia chybových stavov
   * Návratové kódy
   * `errno`
   * Výnimky
* diagnostika

---

# Pretypovanie (angl. casting)

---

## Konverzia nemusí byť explicitná

```cpp
int count = 7;
double exact = count; // 7

long long hugenum = 9007199254740993LL;
double approximate = hugenum; // 9007199254740992

double price = 12.75;
int truncated = price; // 12
```

* Konverzie sa dejú aj pri priradení, odovzdaní argumentu a návrate z funkcie
* Implicitná konverzia môže stratiť informáciu
* Explicitné pretypovanie vyjadruje zámer

note: IEEE definuje pre double cca 53bit presnosti v inte.

---

## C štýl (trochu) skrýva zámer

```cpp
double price = 12.75;
int whole = (int)price;

long long huge = 9007199254740993LL;
double approx = (double)9007199254740993LL

const int limit = 10;
int* writable = (int*)&limit;
```

* Rovnaká syntax skrýva číselnú konverziu aj odstránenie `const`
* Zápis cez `writable` by mal nedefinované správanie
* C++ casty sa ľahšie hľadajú a presnejšie pomenúvajú operáciu

---

## Ktoré operácie to sú?

| Operácia | Nástroj |
| --- | --- |
| Konverzia hodnoty | `static_cast<T>` |
| Kontrolovaná konverzia v polymorfnej hierarchii | `dynamic_cast<T>` |
| Zmena `const` kvalifikácie | `const_cast<T>` |
| Nízkoúrovňová konverzia smerníkov | `reinterpret_cast<T>` |
| Kopírovanie bitovej reprezentácie do iného typu | `std::bit_cast<T>` |

* Žiadny z týchto nástrojov nenahrádza kontrolu vstupu
* Až na `dynamic_cast` nevedia povedať že zlyhali

---

## `static_cast<T>` konvertuje hodnotu

```cpp
double price = 12.75;
auto whole = static_cast<int>(price);
```

* Výsledok je celé číslo `12`, nie bity reprezentujúce `12.75`
* Nekontroluje počas behu, či sa číselná hodnota zmestí do cieľového typu
* `auto` zabráni ďalšej implicitnej konverzii pri uložení výsledku

---

## Celé čísla a ich rozsah

```cpp
std::int8_t small = 127;
auto wide = static_cast<std::int16_t>(small);

std::int16_t large = 30'000;
auto narrow = static_cast<std::int8_t>(large);
```

* Prvá konverzia zachová hodnotu: cieľový typ pokrýva celý rozsah zdrojového typu
* Druhá v C++23 dá `48`, nie `30'000`
* Od C++20 je výsledok celočíselnej konverzie jednoznačná hodnota cieľového typu kongruentná modulo $2^n$, kde $n$ je počet bitov cieľového typu
* Definované správanie ešte neznamená správny výsledok pre našu aplikáciu

---

## Signed a unsigned

```cpp
int negative = -1;
auto positive = static_cast<std::uint32_t>(negative);

bool ordinary = negative < 1u;
bool mathematical = std::cmp_less(negative, 1u);
```

* `positive` je `4'294'967'295`
* `ordinary` je `false`: pri porovnaní sa `negative` konvertuje na `unsigned int`
* `mathematical` je `true`: `std::cmp_less` porovná celé čísla bez tejto pasce
* Väčší počet bitov sám osebe nezaručuje zachovanie zápornej hodnoty

---

## Najprv kontrola, potom cast

```cpp
std::optional<std::uint16_t> try_to_u16(int value) {
    if (!std::in_range<std::uint16_t>(value))
        return std::nullopt;

    return static_cast<std::uint16_t>(value);
}
```

* `std::in_range` je od C++20; kontroluje reprezentovateľnosť pre podporované celočíselné typy
* `-1` a `70'000` sa nezmestia, `300` áno
* K návratovému typu `std::optional` sa ešte vrátime

---

## Desatinné číslo na celé

```cpp
auto positive = static_cast<int>(12.75);
auto negative = static_cast<int>(-12.75);
```

* Výsledky sú `12` a `-12`: orezávame smerom k nule
* Nie je to `std::floor`, ktorý by pre `-12.75` vrátil `-13.0`
* Ak sa orezaná hodnota nezmestí do cieľového typu, správanie je nedefinované
* Rovnako nemôžeme takto konvertovať NaN ani nekonečno na celé číslo
* `std::in_range` nie je určené pre floating-point vstup

---

## Celé číslo na desatinné

```cpp
static_assert(std::numeric_limits<double>::radix == 2);
static_assert(std::numeric_limits<double>::digits == 53);

std::uint64_t original = 9'007'199'254'740'993;
auto rounded = static_cast<double>(original);
auto restored = static_cast<std::uint64_t>(rounded);
// restored is 9'007'199'254'740'992
```

* Bežný `double` má 53 bitov presnosti, nie 64 bitov celočíselnej presnosti
* `rounded` už nemôže presne reprezentovať `original`
* Konverzia späť neobnoví stratenú informáciu
* Predpoklady o reprezentácii sme tu explicitne overili cez `static_assert`

---

## Pretypovanie v hierarchii tried

* Konverzia smerníka z odvodenej triedy na verejnú jednoznačnú bázu je zvyčajne implicitná
* `static_cast` smerom nadol nekontroluje skutočný typ objektu
* `dynamic_cast` môže pri polymorfných objektoch skontrolovať konverziu počas behu
* Neúspešný `dynamic_cast` smerníka vráti `nullptr`, referencie vyhodí `std::bad_cast`
* Konkrétne príklady si ukážeme pri dedičnosti a polymorfizme v nasledujícich prednáškach

---

## `const_cast` a odstránenie `const`

```cpp
int value = 10;
const int* view = &value;
*const_cast<int*>(view) = 20;

const int fixed = 10;
int* forbidden = const_cast<int*>(&fixed);
// *forbidden = 1; // ub

const char* string = "Immutable";
char* dangerous = const_cast<char*>(string);
// dangerous[0] = '/'; //ub
```

* Prvý zápis je definovaný: pôvodný objekt `value` nie je konštantný
* Samotné vytvorenie smerníka `forbidden` nie je undefined až zápis cezeň by bol
* Reťazcový literál tiež nesmieme meniť
* Ideálne je nájsť spôsob ako náš program urobiť korekne bez `const_cast`, jeho použitie je posledná možnosť

---

## `reinterpret_cast` nemení objekt na iný typ

```cpp
float value = 2.5f;
auto* wrong_type = reinterpret_cast<int*>(&value);
```

* V pamäti je stále objekt typu `float`, nevznikol tam `int`
* Čítanie cez `*wrong_type` by porušilo pravidlá prístupu, takže je nedefinované
* Reprezentáciu objektu možno prezerať cez `char`, `unsigned char` alebo `std::byte`
* Cast nevyrieši ani problémy so zarovnaním a životnosťou objektu
* Je to nástroj pre nízkoúrovňový kód, nie bežná číselná konverzia

note: Co myslim tou reprezentaciou typy. Ak máme objekt std::string, tak ho môžeme čítať iba cez smerník na std::string a nie cez int*, výnimku majú tie tri typy uvedené na slide. Smernikom na char môžeme čítať všetko. Súvisí to aj so strict aliasing rule.

---

## `std::bit_cast` reprezentácia ako hodnota

```cpp
static_assert(sizeof(float) == sizeof(std::uint32_t));
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<float>::digits == 24);

auto bits = std::bit_cast<std::uint32_t>(1.0f);
auto restored = std::bit_cast<float>(bits);
```

* Od C++20; oba typy musia byť *trivially copyable* a rovnako veľké
* V tomto príklade `bits == 0x3f800000` a `restored == 1.0f`
* Vzniká nová hodnota, nepristupujeme k pôvodnému objektu cez nesprávny smerník
* Nie je to prenosný formát serializácie, veľmi záleží na reprezentácii typov na konkrétnej platforme

---

## Návrat z `void*`

```cpp
int value = 123;
void* erased = &value;
auto* restored = static_cast<int*>(erased);
```

* Konverzia na `void*` je implicitná, späť stačí `static_cast`
* Musíme poznať pôvodný typ a objekt musí stále existovať
* Tu `restored == &value` a `*restored == 123`
* Podobné rozhrania stretneme v C API (napr. kontext do threadu); v novom C++ kóde preferujeme zachovanie typu

---

## Kedy pretypovať?

* Najprv overme, či správne typy a rozhranie neodstránia potrebu castu
* Pre úmyselnú konverziu hodnoty použime `static_cast`
* Ak môže dôjsť k strate dát, najprv overme rozsah a pravidlá aplikácie
* Bitovú reprezentáciu a číselnú hodnotu riešime rozdielnymi nástrojmi
* `const_cast` a `reinterpret_cast` vyžadujú konkrétne zdôvodnenie
* Cast potláčajúci chybu kompilátora nie je automaticky oprava programu

---

# Chyby v programoch

---

## Bugy

![Bug in my code? It is a features](./lectures/4_errors/bug-feature.png)

* Bug je chyba v programe: vypočíta zlý výsledok, uloží nesprávne dáta alebo spadne
* Neplatný vstup alebo nedostupný súbor sú chybové stavy, s ktorými musíme počítať
* Bug opravujeme; chybový stav spracujeme a zrozumiteľne oznámime

---

## Pády v C++ programoch

* Typické príčiny
    * Prístup cez `nullptr` alebo smerník na už zaniknutý objekt
    * Prístup mimo poľa alebo reťazca
    * Celočíselné delenie nulou
* Pri neplatnom prístupe do pamäte sa často stretneme s hlásením *segmentation fault* alebo *access violation*
* Prevencia: používame `std::string`, `std::vector` a kontrolujeme vstupy pred použitím
* Pád hľadáme pomocou debuggera; k diagnostike sa ešte vrátime

---

## Vždy kontrolujte vstup od užívateľa

* Vstup môže byť prázdny, príliš dlhý alebo v nesprávnom formáte
* Aj správne zapísaná hodnota môže byť mimo povoleného rozsahu
* Platí to aj pre dáta zo súboru, siete a argumentov programu
* Kontrolujeme ich skôr, než ich použijeme na indexovanie, výpočet alebo pretypovanie
* Pri chybe oznámime problém; nepokračujeme s neplatnými dátami
* Poškodený vstup môže byť omyl aj úmyselný útok

---

## Buffer overflow

```cpp
#include <cstdio>

int main() {
    char name[128];
    std::printf("Enter your name: ");
    if (std::scanf("%s", name) == 1) {
        std::printf("Hello %s!", name);
    }
}
```

* Pole má miesto na 127 znakov a ukončovaciu nulu
* `%s` bez obmedzenia dĺžky nezohľadňuje veľkosť poľa
* Príliš dlhé slovo spôsobí zápis za koniec poľa: nedefinované správanie, poškodenie pamäte alebo pád
* Kontrola návratovej hodnoty tu nestačí; potrebujeme aj bezpečný spôsob načítania

---

## V C++ použijeme `std::getline`

```cpp
int main() {
    std::cout << "Enter your name: ";

    std::string name;
    if (!std::getline(std::cin, name)) {
        std::cerr << "Cannot read name.\n";
        return 1;
    }

    if (name.empty() || name.size() > 127) {
        std::cerr << "Name must contain 1 to 127 bytes.\n";
        return 1;
    }

    std::cout << "Hello " << name << '\n';
}
```

* `std::string` spravuje pamäť a `std::getline` načíta celý riadok vrátane medzier
* Úspešné načítanie ešte neznamená platný vstup: pravidlá aplikácie kontrolujeme zvlášť
* Rovnako si musíme ujasniť, aké vstupy smú dostať naše vlastné funkcie

---

# Kontrola parametrov funkcií

---

## Neplatné parametre

Zjednodušený príklad: spojíme adresár a názov súboru pomocou `\`.

```cpp
std::string join_path(const char* dir, const char* file) {
    std::string result = dir;

    if (result.back() != '\\') {
        result.push_back('\\');
    }
    result += file;

    return result;
}
```

* `dir` alebo `file` môže byť `nullptr`: nemáme platný reťazec
* Prázdny `dir` vytvorí prázdny `result`; volať na ňom `back()` je chyba
* Podobne ako pri externom vstupe si musíme určiť, ktoré hodnoty funkcia prijíma

---

## Aké vstupy funkcia prijíma?

* `dir` môže byť prázdny: výsledkom bude iba názov súboru
* `file` musí byť neprázdny a nesmie začínať `\`
* Oba parametre musia ukazovať na platné, nulou ukončené reťazce

```cpp
bool valid_path_parts(const char* dir, const char* file) {
    return dir != nullptr && file != nullptr
        && file[0] != '\0' && file[0] != '\\';
}
```

* `&&` vyhodnocuje zľava doprava a skončí pri prvej nepravdivej podmienke
* Preto k `file[0]` pristúpime až po kontrole `file != nullptr`
* Takáto kontrola samozrejme neodhalí, či sú reťazce ukončené nulou a či sú vôbec platná pamäť
* `strlen` napríklad nekontroluje `str != nullptr`

---

## `assert`

* Pripomenutie: kontrolujeme predpoklady a programátorské chyby
* Z `<cassert>`; pri definovanom `NDEBUG` sa výraz nevyhodnotí
* Zlyhanie vypíše diagnostiku a zavolá `std::abort`
* Výraz nesmie mať vedľajšie účinky potrebné pre správny beh programu
* Pozor `import std;` neimportuje toto makro

```cpp
#include <cassert>

int divide_positive(int numerator, int denominator) {
    assert(denominator > 0);
    return numerator / denominator;
}
```

* `assert` nie je náhrada validácie vstupu, slúžia výhradne na kontrolu počas developmentu


## Externý vstup najprv overíme

```cpp
std::optional<int> checked_divide(int numerator, int denominator) {
    if (denominator <= 0)
        return std::nullopt;

    return numerator / denominator;
}
```

* Neplatný vstup je tu očakávaný neúspech
* Kontrola funguje aj pri definovanom `NDEBUG`
* `std::nullopt` znamená, že výsledok nemáme; `std::optional` si vysvetlíme v ďalšej kapitole

---

## Neočakávaná vetva `assert(false)`

```cpp
const char* direction_name(int direction) {
    switch (direction) {
    case 0: return "left";
    case 1: return "right";
    default:
        assert(false && "Unexpected direction");
        return "unknown";
    }
}
```

* Očakávame, že náš kód odovzdá iba `0` alebo `1`
* Iná hodnota pri aktívnom `assert` vyvolá diagnostiku a ukončenie programu
* Pri `NDEBUG` sa `assert` vynechá; tu funkcia vráti `"unknown"`
* Vstup od používateľa overíme zvlášť, nie cez `assert`


## Nedosiahnuteľné miesto

```cpp
#include <utility>

const char* direction_name(int direction) {
    switch (direction) {
    case 0: return "left";
    case 1: return "right";
    default: std::unreachable();
    }
}
```

* Volajúci musí zaručiť, že `direction` je iba `0` alebo `1`
* Od C++23 takto oznámime kompilátoru, že vetva `default` je nedosiahnuteľná
* Na rozdiel od `assert` nič nekontrolujeme: iná hodnota spôsobí nedefinované správanie aj pri `NDEBUG`
* Nie je to spracovanie neplatného vstupu ani zaručené ukončenie programu

---

## Vhodnejší typ parametra: `std::string_view`

* Od C++17; pohľad na reťazec, ktorý si pamätá začiatok a dĺžku
* Nekopíruje znaky a nevlastní ich pamäť
* Prijíma napríklad `std::string` aj reťazcový literál
* Má `empty()`, `size()`, `front()`, `back()` a vyhľadávanie
* `front()` a `back()` používame iba na neprázdnom pohľade
* Má `data()`, ale nie `c_str()`: reťazec nemusí byť ukončený nulou


## Používanie `std::string_view`

```cpp
std::string_view name = "report.txt";
assert(!name.empty() && name.size() == 10);
assert(name.front() == 'r' && name.back() == 't');
assert(name[0] == 'r');
assert(name.starts_with("report") && name.ends_with(".txt"));
assert(name.contains("port"));

auto dot = name.find('.');
assert(dot == 6);
assert(name.find('/') == std::string_view::npos);

auto stem = name.substr(0, dot); // report
const char* data = stem.data();
assert(data == name.data()); // same memory
assert(data[stem.size()] == '.'); // no nul terminator

std::println("{}", stem); // with cout it is complicated
```

* `substr` nekopíruje znaky; `contains` je od C++23
* `data()` používame spolu s dĺžkou, tu za `"report"` nasleduje `.`, nie `\0`


## Pohľad musí mať na čo pozerať

```cpp
std::string name = "report.txt";
std::string_view view = name;
std::string_view empty;

assert(view == "report.txt");
assert(empty.empty());
```

* `name` musí existovať počas používania `view`
* Zmeny v `name` môžu pohľad zneplatniť, napríklad pri presune dát do novej pamäte
* Prázdny pohľad je platný vstup, `nullptr` ako reťazec ním v C++23 nie je
* Lepší typ zjednoduší rozhranie, ale nevyrieši životnosť dát za nás

---

## Rozhranie a predpoklady spolu

```cpp
std::string join_path(std::string_view dir, std::string_view file) {
    assert(!file.empty());
    assert(file.front() != '\\');

    std::string result(dir);
    if (!result.empty() && result.back() != '\\') {
        result.push_back('\\');
    }

    result += file;
    return result;
}
```

* `join_path("docs", "report.txt")` vráti `"docs\\report.txt"`
* `join_path("", "report.txt")` vráti `"report.txt"`
* Volajúci zodpovedá za platný názov súboru; assertions pomáhajú odhaliť chybu v jeho kóde
* Funkcia vracia výsledok, v ďalšej časti ukážeme, ako upozorniť na jeho ignorovanie

---

# Atribúty

---

## Atribúty

* Štandardizované anotácie kódu so syntaxou od C++11
* Podľa atribútu sa zapisujú k deklarácii alebo príkazu
* Slúžia napríklad na upozornenia a optimalizáciu; každý má vlastné pravidlá
* Neznámy atribút môže kompilátor ignorovať; podporu overíme cez `__has_cpp_attribute`
* Syntax je `[[attribute]]`

---

## Atribúty dostupné v C++17

| Attribút                 | Správanie                             |
| ------------------------ | ------------------------------------- |
| `[[noreturn]]`           | Funkcia nikdy neskončí                |
| `[[deprecated]]`         | Symbol bol označený ako zastaraný     |
| `[[deprecated("msg")]]`  | Ako predchádzajúci s vlastnou správou |
| `[[fallthrough]]`        | Potlaćenie upozornení v `switch`      |
| `[[nodiscard]]`          | Navrátová hodnota sa musí použiť      |
| `[[maybe_unused]]`       | Premenná môže byť nepoužitá.          |
| `[[carries_dependency]]` | 😮 Aj ja by som rád vedel 😊         |

* C++20 pridáva napríklad `[[likely]]` a `[[unlikely]]`, C++23 `[[assume]]`

---

## `[[likely]]` a `[[unlikely]]`

```cpp
std::optional<int> checked_divide(int numerator, int denominator) {
    if (denominator > 0) [[likely]] {
        return numerator / denominator;
    } else [[unlikely]] {
        return std::nullopt;
    }
}
```

* Od C++20; označujú očakávanú častejšiu alebo zriedkavejšiu cestu
* Sú to odporúčania pre optimalizátor, nie kontroly ani záruka zrýchlenia
* Obe vetvy zostávajú platné: aj zriedkavý prípad sa normálne spracuje
* Používame ich na základe merania, nie automaticky pri každom `if`

---

## `[[assume]]` náš sľub optimalizátoru

```cpp
int divide_positive(int numerator, int denominator) {
    [[assume(denominator > 0)]];
    return numerator / denominator;
}
```

* Štandardný atribút od C++23, zapísaný na prázdnom príkaze
* Podmienka sa nevyhodnocuje; negeneruje runtime kontrolu ani hlásenie chyby
* Kompilátor môže optimalizovať s predpokladom, že je podmienka pravdivá
* Ak pri dosiahnutí tohto miesta neplatí, správanie je nedefinované
* Nepoužívame vedľajšie účinky; podporu overíme cez `__has_cpp_attribute(assume)`

---

## Kontrola alebo predpoklad?

| Nástroj | Ak podmienka neplatí |
| --- | --- |
| `if` a spracovanie chyby | Program vykoná zvolenú chybovú vetvu |
| `assert`, bez `NDEBUG` | Diagnostika a ukončenie cez `abort` |
| `assert`, s `NDEBUG` | Žiadna kontrola ani vyhodnotenie výrazu |
| Podporované `[[assume]]` | Nedefinované správanie |

* `[[assume]]` nezávisí od `NDEBUG` a nie je rýchlejší `assert`
* Na rozdiel od `[[unlikely]]` nehovorí „zriedka“, ale „toto musí platiť“
* Ak ho kompilátor ignoruje, nemôžeme počítať s optimalizáciou
* Korektnosť programu nesmie závisieť od podpory tohto atribútu

---

## Posledný pokus na `join_path`

```cpp [|1|3-4|7-9|10-15]
[[nodiscard]]
std::string join_path(std::string_view path, std::string_view fragment) {
    assert(fragment.length() > 0);
    assert(fragment[0] != '\\');
    
    std::string result;
    bool add_separator = !path.empty() && path.back() != '\\';
    
    result.reserve(path.length() + fragment.length() + (add_separator ? 1 : 0));
    result.assign(path);
    if (add_separator) {
        result += '\\';
    }

    result += fragment;
    return result;
}
```
<!-- .element: class="showall" -->

* Path môže byť ľubovoľný, ale súbor nie, tam musí byť neprázdny a nechceme aby začínal separátorom.
* Ešte si v rámci funkcie aj predalokujeme pamäť aby to bolo ćo najrýchlejšie

---

# Návratové hodnoty

---

## Voliteľná návratová hodnota

* Predstavme si, že máme funkciu, ktorá vracia reťazec
* Ak ale niečo zlyhá, tak chceme reportovať chybu

```cpp
std::string get_string() {
    // will return string if exists or nothing...
    // the thing is how to represent nothing?
}
```


## Špeciálna hodnota na reprezentáciu chyby

* Prázdny reťatec pre `std::string`
* `0`, `-1`, `MIN_INT`, `MAX_INT`
* V poriadku, ak si môžeme dovoliť takto vyčleniť hodnotu

```cpp
std::string get_string() {
    if (/* something bad happened */)
        return std::string(); // no value

    // ...
}
```


## Použijeme smerník

```cpp
std::string* get_string() {
    if (/* something bad happened */)
        return nullptr; // no value

    std::string* s = new std::string();
    // ...
    return s;
}
```

* Surové smerníky sú zlé
* Aj keď použijeme `std::unique_ptr`, stále sa nevyhneme alokácií na heap-e


## Výstupný parameter

```cpp
bool get_string(std::string& s) {
    if (/* something bad happened */)
        return false; // failed

    s = "output"; // compute output

    return true;
}
```

* Nie veľmi moderné
* Lepšie je keď funkcia nemá žiadne výstupné parametre
* Použite je komplikovanejšie
* Návratový typ sa musí dať default skonštruovať


## `std::pair<bool, std::string>`

```cpp
std::pair<bool, std::string> get_string() {
    if (/* something bad happened */)
        return { false, std::string() }; // no value
    
    std::string s;
    // ...
    return { true, s };
}
```

* Hodnota musí byť default skonštruovateľná
* Hodnota sa vytvorí aj keď je nie je treba, teda ak je výsledok `false`

---

## Skutočný optional

* Pridaný v C++17
    * `boost::optional` nie je na 100% kompatibilný
    * Existuje Abseil optional, ktorý funguje od C++11

```cpp
std::optional<std::string> get_string() {
    if (/* something bad happened */)
        return std::nullopt; // no value

    std::string s;
    // ...
    return s;
}
```


## Interface `std::optional` 

```cpp [1|3|5-6|8-9|11-12|14|16-19|21-23|25]
std::optional<std::string> o;

assert(!o.has_value());

o = "Now has value";
assert(o.has_value());

// operator -> will not check if value is assigned, undefined if no value
o->append(" and it has overloaded ->operator.");

auto str = o.value_or("empty");
assert(str != "empty");

o.reset(); // or = std::nullopt, will discard the value

std::optional<std::string> o2;

auto str2 = o2.value_or("empty");
assert(str2 == "empty");

[[maybe_unused]] auto x = o2.value(); // will throw exception std::bad_optional_access

*o2; // undefined

o2 = std::make_optional<std::string>("new optional");
```

---

## `std::expected`

* Pridaný v C++23
* Podobný `std::optional`, ale s možnosťou vrátiť chybový stav
* `std::expected<T, E>`, kde `T` je hodnota a `E` je chyba

```cpp
std::expected<std::string, std::error_code> get_string() {
    if (/* something bad happened */)
        return std::make_unexpected(std::error_code());
    
    std::string s;
    // ...
    return s;
}
```


## `std::expected` volanie

* `std::expected` drží buď hodnotu, alebo chybu
* Interface umožňuje jednoduché rozlíšenie chyby od úspechu
* Rovnako sa dá ľahko dostať k platnej hodnote

```cpp
std::expected<std::string, std::error_code> s = get_string();

if (s) {
    std::cout << *s << '\n';
} else {
    std::cout << "Error: " << s.error().message() << '\n';
}
```

---

# Chyby počas behu programu

---

## Chybové stavy

* Nie bugy
* Stavy, v ktorých sa stalo niečo zlé
    * Nedostatok pamäte
    * Málo miesta na disku
    * Užívateľ nemá prístup k nejakej časti systému
    * Time out
    * Pád iného procesu, na ktorom sme závislý

---

## Ošetrovanie chýb

* Ignorácia chýb
* Chybové kódy
* `Errno` prístup
* C++ výnimky
* <small>Iné systémovo špecifické prístupy</small>

---

## Ignorácia chýb

* Niekedy môže mať zmysel
* Ak aplikácií dôjde pamäť, moc sa s tým nedá robiť
* Veľmi zlé, ak sa nepodarí otvoriť súbor, alebo sieťové spojenie
* Bezpečnostná diera

```cpp
int main(int argc, char** argv) {
    char* c = (char*)malloc(1000); // let's roll the dice
    strcpy(c, "Hello");
}
```

---

## `exit`, `abort`, `terminate`

* `std::(quick_)exit`
    * Normálne ukončenie programu
    * Quick preskočí nejaké cleanupy
    * `std::at_(quick_)exit` slúži na registráciu handlerov, ktoré sa pri exit-e majú zavolať
* `std::terminate`
    * Automaticky volané ak sa niečo zlé stane (neodchytená výnimka, výnimka z konštruktora globálneho objektu)
    * `std::set(get)_terminate` nastaví handler na terminate
    * Štandardný handler je volanie `abort`
* `std::abort`
    * Najrýchlejšia cesta ako ukončiť program
    * Žiaden cleanup sa nevykoná


## `abort`

* Zavolať abort je stále lepšie ako neurobiť nič a spoľahnúť sa na to, že sa to pokazí neskôr
* Nie je to veľmi užívateľsky prívetive

```cpp
int main(int argc, char** argv) {
    char* c = (char*)malloc(1000);
    if (c == nullptr)
        abort();

    strcpy(c, "Hello");
}
```

* V C++ exituje `std::terminate`, ktorý C++ volá na mnohých miestach, kde už nemá zmysel pokračovať


## `exit`

* Oveľa lepšie ako `abort` je zavolať `exit(error_code);`
* Prípadne `quick_exit`, `_exit` alebo `_Exit` 😎

<table>
<tr>
<td width="70%">

```cpp
void exit_logger() {
    std::cout << "Exit was called.\n";

}

int main() {
    if (std::atexit(exit_logger) != 0) {
        std::cout << "At exit not worked\n";
        return EXIT_FAILURE;
    }

    std::cout << "returning from main\n";
    return EXIT_SUCCESS;
}
```
</td>
<td>

`exit_logger` sa zavolá, je to z toho dôvodu, že return z `main`-u, vlastne akoby volal `exit`
</td>
</tr>
</table>

* `exit` volá všetky registované funkcia a deštruktory globálnych objektov...
* `quick_exit` volá iba funkcia zaregistrované pomocou `at_quick_exit`
* `_Exit` nevolá nič

---

## Chybové kódy `bool`

* Použije bool ako návratovú hodnotu
* Nevieme ale komunikovať aký bol problém
* Musíme používať výstupné parametre 😢

```cpp
bool read_line(const std::string& path, std::string& line) {
    std::ifstream ifs(path);
    if (!ifs.is_open())
        return false;

    if (!std::getline(ifs, line))
        return false;

    return true;
}
```

---

## Chybové kódy `enum`

* Ak nám dva stavy nestačia
* Môžeme mať viaceré chybové stavy
    * Súbor sa nepodarilo otvoriť
    * Zlyhalo čítanie zo súboru
    * Nie je dosť pamäte
    * ...
* Historicky sa používali `int`, alebo `ssize_t`
* Dnes hlavne `enum class`


## Príklad enum

```cpp [1-5|7-18]
enum class error {
    success,
    memory_error,
    cannot_open_file,
};

error read_from_file(std::string_view path, char** data) {
    std::ifstream f(path.data(), std::ios::binary);
    if (!f.is_open())
        return error::cannot_open_file;

    *data = static_cast<char*>(malloc(1024));
    if (!*data)
        return error::memory_error;

    // ...
    return error::success;
}
```

---

## Problémy s chybovými kódmi

* Predvolene ich ignorujeme, dá sa k nim pridať `[[nodiscard]]`, ale aj tak
* Aj keď sa neignorujú, tak často programátori používajú iba error/success
    * Efektívne používajú ako `bool`
* Jeden nedôsledný programátor môže pokaziť dômyselný systém, lebo nepropaguje chyby ďalej

---

## `errno`

* Každé vlákno má vlastnú hodnotu
    * Volaný ju nastaví
    * Volajúci si ju potom vie prečítať
* Netreba mať výstupné parametre
* Ešte menej viditeľné, takže sa dá ľahko ignorovať
* *WinApi `GetLastError`*


## Konvertovanie reťazca na číslo

* Máme const char* a chceme funkciu, ktorá sa ho pokúsi konvertovať na `int64_t` v desiatkovej sústave
* Reťazec je od užívateľa, takže musíme kontrolovať všetko
* Štandard poskytuje `strtoll` s troma parametrami
    * smerník na reťazec
    * smerník na znak, kde sa skončila konverzia
    * radix

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    output = strtoll(input, nullptr, 0);
    return true;
}
```


## Radix a neplatný reťazec

* Najprv chceme zadať pevný radix, aby si program neodmýšľal 
    * default 0 znamená zisti podľa prefixu
* Vstup ako `"123abc"` chceme odignorovať
    * hodí sa nám druhý parameter `strtoll`

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    char* endptr = nullptr;
    output = strtoll(input, &endptr, 10);
    if (*endptr != '\0')
        return false;

    return true;
}
```


## Začiatok whitespace-om

* Teraz skonvertujeme aj reťazec ako `"  123"`

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    if (!isdigit(input[0]))
        return false;

    char* endptr = nullptr;
    output = strtoll(input, &endptr, 10);
    if (*endptr != '\0')
        return false;

    return true;
}
```


## Overflow

* Reťazec `"111111111111111111111111111"` vyrobí výsledok `"9223372036854775807"`
* Musíme skontrolovať `errno`

```cpp
bool TryConvertToInt64(const char* input, *int64_t*& output) {
    if (!isdigit(input[0]))
        return false;

    char* endptr = nullptr;
    output = strtoll(input, &endptr, 10);
    if (*endptr != '\0' || errno == ERANGE)
        return false;

    return true;
}
```

Ak nastane pretečenie, tak funkcia nastavý `errno` na `ERANGE`.


## Ešte sme neskončili

* Ak prvé volanie tento funkcie vyrobí pretečenie, tak druhé skončí s takou istou chybou
* Žiadna štandardná funkcia nenastavuje `errno` na nulu

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    if (!isdigit(input[0]))
        return false;

    errno = 0; // reset errno
    char* endptr = nullptr;

    output = strtoll(input, &endptr, 10);

    if (*endptr != '\0' || errno == ERANGE)
        return false;
    
    return true;
}
```


## Ešte jeden problém...

* Funkcie môžu modifikovať errno aj keď skončia bez chyby, mali by sme to skontrolovať

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    if (!isdigit(input[0])) // check if it is a number
        return false;

    errno = 0; // reset errno

    char* endptr = nullptr;

    output = strtoll(input, &endptr, 10);
    if (*endptr != '\0')
        return false; // whole string is not a number

    if ((output == LLONG_MAX || output == LLONG_MIN) && errno == ERANGE)
        return false; // underflow or overflow

    return true;
}
```
<!-- .element: class="showall" -->

* v tomto prípade je to trochu overkill, ale bežne je to dobrá prax


## Problémy s errno

* Ťažká propagácia chýb
* Nesmieme zabudnúť nastaviť errno na 0 pre volaním
* Ak chceme vlastnú implementáciu takéhoto systému, tak je to celkom ťažké (C++11 `thread_local` vie pomôcť)


## Konverzia v C++17

```cpp
bool TryConvertToInt64(const char* input, int64_t& output) {
    auto end = input + strlen(input);
    auto result = std::from_chars(input, end, output);
    if (result.ec != std::errc() || result.ptr != end)
        return false;

    return true;
}
```

* Rozdiely
    * from_chars neignoruje biele znaky na začiatku
    * \+ na začiatku nie je povolené
    * Nerobí rozhodovanie o číselnej sústave

---

# Výnimky

---

![Revelation why exceptions are great](./lectures/4_errors/revelation.png)

---

## Ako oznámiť chybu z konštruktora

* Môžeme mať výstupný parameter
* Metódu `Init`
* Metódu `IsOK`
* Pomocou `errno`
* Všetky predchádzajúce ale **skonštruujú** objekt, teda sa na ňom **zavolá** deštruktor
* Preto si objekty často musia niesť, či sú skonštruované dobre


## Príklad

* V konštruktore musíme držať informáciu, či bolo všetko OK
* Niektorí toto považujú za idiomatické, väčšina za škaredé

```cpp [|15|13|4|7-11|20]
class File {
public:
    File(const char* path) {
        m_file = fopen(path, "r");
    }

    ~File() {
        if (IsOK()) {
            fclose(m_file);
        }
    }

    bool IsOK() const { return m_file != nullptr; }

    FILE* m_file = nullptr;
};

int main(int argc, const char** argv) {
    File f(argv[0]);
    if (f.IsOK()) {
        // .. use
    }
}
``` 

---

## Chyby pri konštrukcii

* Hlavný cieľ výnimiek je poskytnúť spôsob akým reportovať chyby z konštruktora
* Ak konštruktor skončí s výnimkou, deštruktor sa nevolá
* Výnimky môžu preskákať cez viacero  =scope-ov a štandard garantuje, že sa všetky uprace

---

## Throw

* Výnimky vznikajú (vyhadzujú) sa kľúčovým slovom throw
* Všetko sa dá vyhodiť

```cpp
File(const char* path) {
    m_file = fopen(path, "r");
    if (m_file == nullptr)
        throw 0;
}

~File() {
    fclose(m_file);
}
```

* V deštruktore netreba kontrolovať platnosť premenných.

---

## Catch

* Výnimky sa zachytávajú kľúčovým slovom `catch`
* Môžeme zachytiť konkrétny typ, alebo všetko

```cpp
int main(int argc, const char** argv) {
    try {
        File f(argv[0]);
        
        // ...
    } catch (int i) {
        std::cout << "Unable to open file " << i;
    } catch (...) {
        std::cout << "WTF!?";
    }
}
```

* V prípade výnimky sa pohľadá konkrétny handler, ktorý ju vie spracovať.

---

## Stack unwinding

* Ak sa urobí `throw`, nájde sa príslušný `catch`, ktorý ho vie zachytiť
* Všetky scope, ktoré sa počas toho preskočia zavolajú deštruktory na objekty (a tie na svoje členy...)

*TODO: stack unwinding image*

---

## Best practice

* Vyhadzovať hocičo je hlúpe
* Mali by sa vyhadzovať iba typy dediace od `std::exception`
* Chytanie všetkého všade je tiež hlúpe
* Výnimky chytáme iba ak vieme niečo urobiť
* V `main`-e *(alebo v entry pointe threadu)* zachytíme všetko a urobíme z toho log, reštart, ...

---

## ConvertToInt64 s výnimkami

```cpp
int64_t ConvertToInt64(const char* input) {
    if (!isdigit(input[0]))
        throw std::invalid_argument("Input must be number!");

    errno = 0;

    char* endptr = nullptr;
    int64_t result = strtoll(input, &endptr, 10);
    if (*endptr != '\0')
        throw std::invalid_argument("Input must be number!");

    if ((result == LLONG_MAX || result == LLONG_MIN) && errno == ERANGE)
        throw std::overflow_error("Input number too big!");

    return result;
}
```
<!-- .element: class="showall" -->

* Alebo použijeme rovno `std::stoll`, čo je `strtoll` s výnimkami

---

## Vnorené `try` ... `catch`

```cpp
std::vector<int> AllocateAndFillCache(size_t max) {
    while (max > 0) {
        try {
            std::vector<int> ret_val(max);

            // fill up cache

            return ret_val;
        } catch (const std::bad_alloc&) {
            max /= 2;
            // log it and continue
        }
    }

    return { };
}
```

* Chytíme to čo vieme spracovať, inak to pustíme ďalej
* Výnimky spravidla chytáme ako `const&`


```cpp
int main() {
    try {
        std::vector<int> cache = AllocateAndFillCache(stoul(argv[1]));
    } catch (const std::exception & e) {
        std::cout << e.what();
    } catch (...) {
        std::cout << "Something strange.";
    }
}
```

---

## Štandardné výnimky

* Všetky sú zdedené zo `std::exception`
* `std::runtime_error`
    * Chyba, ktorá sa môže objaviť iba počas behu programu
    * range_error, underflow_error, ...
* `std::logic_error`
    * Porušenie logických predpokladov alebo invariantov
    * invalid_argument, out_of_range, domain_error
* `std::system_error`
    * Systémovo závislé

---

## Rethrow

* Niekedy chceme iba zistiť, že výnimka nastala, zalogovať chybu a nestarať sa

```cpp
void f() {
    try {
        // do stuff
    } catch (const std::exception& e) {
        // log some error
        // this will rethrow exception
        throw;
    }
}
```

---

## Kód pripravený na výnimky

* Exception safe code
* Nie každý kód je na to pripravený

```cpp
int main() {
    try {
        int *a = new int[1000];
        
        int *b = new int[100000];
    } catch (const std::exception& e) {
        std::cout << e.what();
    }
}
```

* Ak nastane výnimka po alokácií `a`, tak `a` sa neuvoľní a teda máme stratenú pamäť.

---

## Ako to dosiahnuť?

* Každý zdroj (resource) je manažovaný nejakou triedou
* Žiaden explicitný `free`/`delete`/release
* Musíme sa zmieriť s tým, že všetko môže vyhodiť výnimku
    * Znie to strašne, ale je to oslobodzujúce
    * C++ sa bez toho ani nedá robiť 🚀


## Garancie

* No throw
    * Funkcia za žiadnych okolností nevyhodí výnimku
    * `strtoll`, `empty` vo vectore
* Silná garancia (strong)
    * Ak funkcia vyhodí výnimku, tak všetko ostane v stave akoby sa ani nezavolala
    * `push_back` vo vectore
* Základná garancia (basic)
    * Ak funkcia vyhodí výnimku, tak stav je nešpecifikovaný ale stále platný
    * Bez tohto sa nedá programovať, lebo inak sa nedá na nič spoľahnúť

---

## noexcept

* Kľúčové slovo, ktoré špecifikuje, že funkcia nehodí výnimku (neodchytenú)
* Ak by ju hodila, tak sa zavolá `std::terminate` (už sa nesmie pokračovať v programe)

```cpp
void f() noexcept {

}
```

* Nasledujúci kód zavolá `std::terminate` 

```cpp
void g() noexcept {
    throw std::runtime_error("");
}
```

<small>

  V starom C++ sa používalo `throw()`

</small>

---

## Deštruktory

* Deštruktory sú predvolene `noexcept`
    * Hádzať výnimku z deštruktura je nebezpečné
    * Ak by sa akurát spracovávala výnimka a znovu sa nejaká udiala, tak sa zavolá `std::terminate`

* Funkcie ako `move` a `swap` by mali byť `noexcept`
    * Inak sa nevyrobí taký rýchly kód ako by sa mohol

---

## Plusy a mínusy

|    ➕  |  ➖   |
| ---   | ---  |
| Nemôžu byť ignorované | C API na nich nie je pripravené |
| Minimálna cena ak nenastanú | Cena ak nastanú sa nedá dopredu vyčísliť, lebo nevieme koľko sa bude upratovať |
| Ľahko sa propagujú ďalej | |
 

---

## Rady

* V konštruktoroch inicializujme a v deštruktoroch cleanup-ujme
* Nebojme sa výnimiek
* Nepoužívajme `new`/`malloc`
* Výnimky treba používať iba vo výnimočných prípadoch (teda nie na riadenie programu)

---

## Výnimky nie sú ťažké, ošetrovanie chýb je ťažké.

---

# Diagnostika

---

## Informácie o kóde

* `__FILE__` - názov súboru
* `__LINE__` - číslo riadku

```cpp
#define LOG_ERROR(msg) log_error(__FILE__, __LINE__, msg)

void log_error(const char* file, int line, const char* msg) {
    std::cout << "Error in file " << file << " at line " << line << ": " << msg << '\n';
}
```

```plain
Error in file /home/user/project/main.cpp at line 10: Something bad happened
```


## `std::source_location`

* Pridané v C++20
* Umožňuje získať informácie o aktuálnom mieste v kóde

```cpp
#include <iostream>
#include <source_location>

void log_error(const char* msg, const std::source_location& location = std::source_location::current()) {
    std::cout << "Error in file " << location.file_name() 
        << " in function " << location.function_name() 
        << " at line " << location.line() << ": " << msg << '\n';
}

int main() {
    log_error("Something bad happened");
}
```


## Stacktrace

* Získať stacktrace je od C++23 štandardizované
* `std::stacktrace::current()` na získanie aktuálneho stacktrace
* `std::to_string()` na konverziu na reťazec

```cpp
#include <iostream>
#include <stacktrace>

void log_stacktrace() {
    auto stacktrace = std::stacktrace::current();

    std::cout << "Stacktrace: \n" << std::to_string(stacktrace) << '\n'; // auto conversion

    std::cout << "Current stacktrace:\n";
    for (const auto& frame : stacktrace) { // manual iteration through frames
        std::cout << "  " << std::to_string(frame) << '\n';
    }
}

int main() {
    log_stacktrace();
}
```

---

# ĎAKUJEM

## Otázky?
