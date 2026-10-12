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


## C štýl (trochu) skrýva zámer

```cpp
double price = 12.75;
int whole = (int)price;

long long huge = 9007199254740993LL;
double approx = (double)9007199254740993LL;

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

std::println("{}", stem);
std::cout << stem; // string_view pozná svoju dĺžku
```

* `substr` nekopíruje znaky; `contains` je od C++23
* `data()` používame spolu s dĺžkou, tu za `"report"` nasleduje `.`, nie `\0`


## Pohľad na sekvenciu: `std::span`

```cpp
int sum(std::span<const int> values) {
    return std::accumulate(values.begin(), values.end(), 0);
}

int array[] = {1, 2, 3};
std::vector<int> vector = {4, 5, 6};

assert(sum(array) == 6);
assert(sum(vector) == 15);
```

* Od C++20; pohľad na súvislú sekvenciu prvkov
* Nevlastní pamäť a nekopíruje prvky
* Prijíma napríklad pole, `std::array` aj `std::vector`
* `std::string_view` je pohľad na znaky, `std::span<T>` funguje pre ľubovoľný typ `T`
* Aj pri `span` musí pôvodná sekvencia existovať počas používania pohľadu


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

## Vybrané štandardné atribúty

| Atribút                         | Od    | Správanie                                  |
| ------------------------------- | ----- | ------------------------------------------ |
| `[[noreturn]]`                  | C++11 | Funkcia sa nikdy normálne nevráti          |
| `[[deprecated("správa")]]`      | C++14 | Symbol je zastaraný                        |
| `[[fallthrough]]`               | C++17 | Potlačí upozornenie pri prepade v `switch` |
| `[[nodiscard]]`                 | C++17 | Upozorní na ignorovanú návratovú hodnotu   |
| `[[maybe_unused]]`              | C++17 | Potlačí upozornenie na nepoužitý symbol    |
| `[[likely]]`, `[[unlikely]]`    | C++20 | Označia očakávanú cestu vykonávania        |
| `[[no_unique_address]]`         | C++20 | Člen nemusí zaberať samostatné miesto      |

* C++23 pridáva napríklad `[[assume]]`


## `[[deprecated]]`, `[[noreturn]]` a kontrola podpory

```cpp
#if __has_cpp_attribute(deprecated)
[[deprecated("Use parse_number() instead")]]
#endif
int parse_int(std::string_view text);

#if __has_cpp_attribute(noreturn)
[[noreturn]]
#endif
void fail(std::string_view message) {
    throw std::runtime_error(std::string(message));
}
```

* Volanie `parse_int` môže vyvolať upozornenie kompilátora s našou správou
* `[[noreturn]]` sľubuje, že `fail` sa volajúcemu nevráti
* `__has_cpp_attribute(názov)` vráti nenulovú hodnotu, ak kompilátor atribút podporuje


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

* Chceme načítať prvý riadok zo súboru
* Prázdny riadok je platný výsledok
* Súbor ale nemusí existovať alebo sa z neho nemusí dať čítať

```cpp
std::string read_first_line(std::string_view path);
```

Ako odlíšime úspešný výsledok od neúspechu?


## Špeciálna hodnota

* Vyhradíme jednu hodnotu na reprezentáciu neúspechu
* Často sa používa prázdny reťazec, `0`, `-1` alebo maximálna hodnota

```cpp
std::string read_first_line(std::string_view path) {
    std::ifstream file{std::string(path)};
    std::string line;

    if (!file || !std::getline(file, line))
        return {}; // neúspech alebo prázdny riadok?

    return line;
}
```

* Funguje iba vtedy, keď vyhradená hodnota nemôže byť platným výsledkom


## Použijeme smerník

```cpp
std::string* read_first_line(std::string_view path) {
    // ...
    return new std::string("first line");
}
```

* `nullptr` môže jednoznačne znamenať neúspech
* Z rozhrania nie je jasné, kto má výsledok uvoľniť
* `std::unique_ptr` vyjadrí vlastníctvo, ale stále vyžaduje alokáciu na halde


## Výstupný parameter

```cpp
bool read_first_line(std::string_view path, std::string& line) {
    std::ifstream file{std::string(path)};
    if (!file || !std::getline(file, line))
        return false;

    return true; 
}
```

* Návratová hodnota oznamuje úspech, výsledok sa zapisuje cez referenciu
* Volajúci musí vytvoriť objekt ešte pred volaním
* Nie je zrejmé, akú hodnotu má `line` po neúspechu


## `std::pair<bool, std::string>`

```cpp
std::pair<bool, std::string> read_first_line(std::string_view path) {
    std::ifstream file{std::string(path)};
    std::string line;

    if (!file || !std::getline(file, line))
        return {false, {}};

    return {true, std::move(line)};
}
```

* Výsledok aj informácia o úspechu sa vracajú spolu
* `std::string` existuje aj pri neúspechu
* Typ povoľuje aj nezmyselné kombinácie, napríklad `{false, "data"}`

---

## `std::optional<T>`

* Od C++17 reprezentuje hodnotu typu `T` alebo neprítomnosť hodnoty
* Hodnota `T` pri prázdnom `optional` vôbec neexistuje
* Nevyžaduje samostatnú alokáciu na halde
* Neuchováva dôvod neúspechu

```cpp
std::optional<std::string> read_first_line(std::string_view path) {
    std::ifstream file{std::string(path)};
    std::string line;

    if (!file || !std::getline(file, line))
        return std::nullopt;

    return line;
}
```


## Vytvorenie a kontrola `std::optional`

```cpp
std::optional<std::string> line;

assert(!line.has_value());
assert(!line);

line = "first line";
assert(line.has_value());

line.reset(); // alebo line = std::nullopt
assert(!line);
```

* V podmienke sa `optional` konvertuje na `bool`
* `has_value()` vyjadruje rovnakú kontrolu explicitne
* `reset()` zničí uloženú hodnotu


## Prístup k hodnote `std::optional`

```cpp
auto line = read_first_line("notes.txt");

if (line) {
    line->append("!");
    std::println("{}", *line);
}

auto text = line.value_or("<no line>");
auto checked = line.value();
auto unchecked = *line;
```

* `line->` pracujem priamo s hodnotou, ak neexistuje je to nedefinované správanie
* `value()` pri prázdnom stave vyhodí `std::bad_optional_access`
* `value_or()` vráti uloženú hodnotu alebo zadanú náhradnú hodnotu
* `*line` tiež nekontroluje prázdny stav a môže spôsobiť nedefinované správanie

---

## `std::expected`

* Pridaný v C++23
* `std::expected<T, E>` obsahuje buď hodnotu `T`, alebo chybu `E`
* Podobné ako `union` v tom, že obsahuje buď jednu hodnotu, alebo druhú, ale nie obe súčasne

```cpp
enum class read_error { cannot_open, cannot_read };

std::expected<std::string, read_error>
read_first_line(std::string_view path) {
    std::ifstream file{std::string(path)};
    if (!file)
        return std::unexpected(read_error::cannot_open);

    std::string line;
    if (!std::getline(file, line))
        return std::unexpected(read_error::cannot_read);

    return line;
}
```


## Použitie `std::expected`

* Kontrola a prístup k hodnote fungujú podobne ako pri `std::optional`
* Chybu získame cez `error()`

```cpp
auto result = read_first_line("notes.txt");

if (result) {
    std::println("{}", *result);
} else {
    switch (result.error()) {
    case read_error::cannot_open:
        std::println("Cannot open file");
        break;
    case read_error::cannot_read:
        std::println("Cannot read first line");
        break;
    }
}
```

---

# Chyby počas behu programu

---

## Chybové stavy

* Očakávané zlyhania nie sú automaticky bugy
* Program s nimi musí počítať a zvoliť ďalší postup
    * Súbor neexistuje alebo používateľ nemá oprávnenie
    * Na disku nie je dosť miesta
    * Sieťová operácia prekročila časový limit
    * Proces alebo služba, od ktorej závisíme, zlyhala
    * Nepodarilo sa alokovať pamäť


## Čo môžeme pri chybe urobiť?

* Zotaviť sa a operáciu zopakovať alebo použiť náhradný postup
* Oznámiť chybu volajúcemu
    * návratový kód, `std::expected`, výnimka
* Zalogovať problém a ukončiť aktuálnu operáciu
* Ukončiť celý program, ak bezpečne nemôže pokračovať
* Ignorovať iba chybu, o ktorej vieme, že nie je podstatná

---

## Ignorovanie návratovej hodnoty

```cpp
bool save_configuration();

int main() {
    // dp stuhhh

    save_configuration(); // if it fails, what should we do? 
}
```

* Ignorovanie chyby môže viesť k strate dát alebo nekonzistentnému stavu
* Ak chybu ignorujeme zámerne, má to byť z kódu a komentára zrejmé

---

## Spôsoby ukončenia programu

| Nástroj | Čo sa uprace |
| --- | --- |
| návrat z `main` | lokálne objekty v `main`, potom ako `std::exit` |
| `std::exit` | statické objekty a funkcie z `std::atexit` |
| `std::quick_exit` | iba funkcie z `std::at_quick_exit` |
| `std::_Exit` | žiadne C++ upratovanie |
| `std::abort` | žiadne bežné C++ upratovanie |

* `std::exit` nezničí automatické lokálne objekty preskočených funkcií
* Výber závisí od toho, či ide o riadené alebo núdzové ukončenie


## Registrácia handlera cez `std::atexit`

```cpp
void report_exit() {
    std::println("Program is ending");
}

int main() {
    if (std::atexit(report_exit) != 0) {
        std::println("Cannot register exit handler");
        return EXIT_FAILURE;
    }

    std::println("Program is running");
    return EXIT_SUCCESS;
}
```

* Handler nemá parametre ani návratovú hodnotu
* Zavolá sa pri návrate z `main` alebo cez `std::exit`
* Nevolá sa pri ukončení cez `std::abort`


## `std::terminate`

* Volá sa napríklad pri neodchytenej výnimke
* Zavolá sa aj vtedy, keď výnimka opustí funkciu označenú `noexcept`
* Handler môžeme nastaviť cez `std::set_terminate`
* Predvolený handler zavolá `std::abort`

```cpp
void write_required_audit_log(std::string_view message) {
    std::ofstream file("audit.log", std::ios::app);
    if (!file)
        std::terminate();

    file << message << '\n';
    if (!file)
        std::terminate();
}
```

Ak bez auditného záznamu nesmieme pokračovať, zlyhanie ukončí celý program.

---

## Chybové kódy `bool`

```cpp
[[nodiscard]]
bool read_first_line(std::string_view path, std::string& line) {
    std::ifstream file{std::string(path)};
    if (!file || !std::getline(file, line))
        return false;

    return true;
}
```

* `bool` jednoducho odlíši úspech od neúspechu
* Nevie však oznámiť, prečo operácia zlyhala
* Výsledok musíme vrátiť cez výstupný parameter


## Chybové kódy `enum`

```cpp
enum class read_error {
    success,
    cannot_open,
    cannot_read,
};

[[nodiscard]]
read_error read_first_line(std::string_view path, std::string& line) {
    std::ifstream file{std::string(path)};
    if (!file)
        return read_error::cannot_open;

    if (!std::getline(file, line))
        return read_error::cannot_read;

    return read_error::success;
}
```

* `enum class` pomenuje viacero možných chýb
* Volajúci však stále musí výsledok skontrolovať a propagovať
* `std::expected<T, E>` spojí chybový stav priamo s výslednou hodnotou

---

## `errno`

* Niektoré funkcie C knižnice ním oznamujú podrobnosti o chybe
* Každé vlákno má vlastnú hodnotu `errno`
* Úspešné volanie ho nemusí nastaviť späť na nulu
* Čítame ho iba vtedy, keď dokumentácia danej funkcie určuje jeho význam
* Ak potrebujeme odlíšiť starú hodnotu, nastavíme `errno = 0` tesne pred volaním


## Konverzia cez `strtoll`

`strtoll` dostane vstup, výstupný smerník na koniec konverzie a číselnú sústavu.

```cpp
bool try_to_int64(const char* input, std::int64_t& output) {
    if (input == nullptr)
        return false;

    char* end = nullptr;
    output = std::strtoll(input, &end, 10);

    return end != input && *end == '\0';
}
```

* `end == input` znamená, že sa neskonvertoval ani jeden znak
* `*end != '\0'` znamená, že za číslom zostali ďalšie znaky
* Funkcia akceptuje úvodné biele znaky aj znamienka `+` a `-`


## Rozsah výsledku cez `errno`

```cpp
bool try_to_int64(const char* input, std::int64_t& output) {
    if (input == nullptr)
        return false;

    errno = 0;
    char* end = nullptr;
    output = std::strtoll(input, &end, 10);

    return end != input && *end == '\0' && errno != ERANGE;
}
```

* Pri príliš veľkej alebo malej hodnote nastaví `strtoll` `errno` na `ERANGE`
* Pred volaním ho nulujeme, aby sme nečítali chybu po staršej operácii
* Výstupný parameter môže byť zmenený aj pri neúspechu


## `std::from_chars` od C++17

```cpp
std::int64_t value;
auto begin = input.data();
auto end = begin + input.size();
auto [ptr, ec] = std::from_chars(begin, end, value);
```

* Neignoruje úvodné biele znaky a nepovoľuje úvodné `+`
* `std::errc::invalid_argument`: nezačalo sa platné číslo
* `std::errc::result_out_of_range`: výsledok sa nezmestí
* `ptr != end`: za číslom zostali ďalšie znaky
* Nepoužíva `errno`, nealokuje a nevyhadzuje výnimky


## Moderné rozhranie konverzie

```cpp
enum class parse_error { invalid, out_of_range, trailing };

std::expected<std::int64_t, parse_error>
parse_int64(std::string_view input) {
    std::int64_t value;
    auto end = input.data() + input.size();
    auto [ptr, ec] = std::from_chars(input.data(), end, value);

    if (ec == std::errc::invalid_argument)
        return std::unexpected(parse_error::invalid);
    if (ec == std::errc::result_out_of_range)
        return std::unexpected(parse_error::out_of_range);
    if (ptr != end)
        return std::unexpected(parse_error::trailing);

    return value;
}
```

* `from_chars` vykoná konverziu, `expected` pomenuje výsledok rozhrania
* Volajúci dostane buď číslo, alebo konkrétny dôvod neúspechu

---

# Výnimky

---

![Revelation why exceptions are great](./lectures/4_errors/revelation.png)

---

## Ako oznámiť chybu z konštruktora

* Konštruktor nemá návratovú hodnotu pre chybový stav
* Objekt môže mať metódu `IsOK()`, ale potom existuje aj v nepoužiteľnom stave
* Dvojfázová inicializácia cez `Init()` má rovnaký problém
* Factory funkcia môže vrátiť `std::expected`
* Konštruktor môže vyhodiť výnimku


## Objekt, ktorý nemusí byť platný

```cpp
class File {
public:
    explicit File(const char* path)
        : stream_(path) {}

    bool is_open() const { return stream_.is_open(); }

private:
    std::ifstream stream_;
};
```

* Každé použitie musí najprv skontrolovať `is_open()`
* Trieda si musí pamätať neplatný stav a deštruktor s ním musí počítať


## Konštruktor, ktorý zlyhá výnimkou

```cpp
class File {
public:
    explicit File(const char* path)
        : stream_(path) {
        if (!stream_)
            throw std::runtime_error("Cannot open file");
    }

private:
    std::ifstream stream_;
};
```

* Ak vznikne objekt `File`, je vždy otvorený a použiteľný
* Ak konštruktor vyhodí výnimku, objekt typu `File` nevznikne
* Jeho deštruktor sa nezavolá, ale už skonštruované členy a bázy sa upracú

---

## `throw`, `try` a `catch`

```cpp
void load_configuration() {
    throw std::runtime_error("Invalid configuration");
}

int main() {
    try {
        load_configuration();
        return EXIT_SUCCESS;
    } catch (const std::runtime_error& error) {
        std::println("Configuration error: {}", error.what());
        return EXIT_FAILURE;
    } catch (const std::exception& error) {
        std::println("Error: {}", error.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::println("Unknown error");
        return EXIT_FAILURE;
    }
}
```

* Handlery sa skúšajú zhora nadol, preto dávame konkrétnejšie typy prvé
* Výnimky spravidla chytáme ako `const&`; súvisí to s tým aby sme náhodou nekopírovali výnimku


## Výnimka sa propaguje

```cpp
void read_configuration() {
    throw std::runtime_error("Cannot read configuration");
}

void start_application() {
    read_configuration();
}

int main() {
    try {
        start_application();
    } catch (const std::exception& error) {
        std::println("Startup failed: {}", error.what());
        return EXIT_FAILURE;
    }
}
```

* Hľadanie vhodného `catch` pokračuje cez volajúce funkcie
* Medziľahlé funkcie nemusia chybu manuálne preposielať


## Ak výnimka opustí `main`

```cpp
int main() {
    load_configuration(); // vyhodí výnimku
}
```

* Ak sa nenájde vhodný handler, zavolá sa `std::terminate`
* Predvolený terminate handler zavolá `std::abort`
* Nie je zaručené, že sa pred `terminate` vykoná stack unwinding
* Na upratanie lokálnych objektov sa preto pri neodchytenej výnimke nespoliehame

---

## Stack unwinding

```cpp
struct ScopeLog {
    std::string_view name;
    ~ScopeLog() { std::println("Leaving {}", name); }
};

void read_data() {
    ScopeLog log{"read_data"};
    throw std::runtime_error("Read failed");
}

void load_data() {
    ScopeLog log{"load_data"};
    read_data();
}
```

* Pri propagácii sa opúšťajú scope až po nájdený `catch`
* Deštruktory lokálnych objektov sa volajú v opačnom poradí


## Surový zdroj sa neuprace sám

```cpp
void process() {
    auto* data = new int[1000];

    validate_input(); // môže vyhodiť výnimku
    use(data);

    delete[] data;
}
```

* Ak `validate_input()` vyhodí výnimku, k `delete[]` sa nedostaneme
* Stack unwinding pozná objekty, nie význam surového smerníka


## RAII pripraví kód na výnimky

```cpp
void process() {
    std::vector<int> data(1000);

    validate_input(); // môže vyhodiť výnimku
    use(data);
}
```

* `std::vector` vlastní pamäť a jeho deštruktor ju uvoľní
* Rovnaký princíp používajú `std::string`, smart pointery, streamy a zámky
* Každý zdroj má vlastniť objekt, ktorého deštruktor ho uvoľní


## `noexcept`

```cpp
void close_connection() noexcept {
    // výnimka nesmie opustiť funkciu
}
```

* `noexcept` je záväzok rozhrania, že výnimka funkciu neopustí
* Ak výnimka napriek tomu unikne, zavolá sa `std::terminate`
* Operácie `move` a `swap` označujeme `noexcept`, keď to naozaj garantujú


## Deštruktory a výnimky

* Deštruktory sú spravidla `noexcept`
* Výnimka unikajúca z deštruktora môže zavolať `std::terminate`
* Obzvlášť nebezpečné je to počas spracovania inej výnimky
* Cleanup preto používame v nevyhadzujúcej forme, ak ju API ponúka

```cpp
TemporaryFile::~TemporaryFile() noexcept {
    std::error_code error;
    std::filesystem::remove(path_, error);
}
```


## Garancie pri výnimkách

Garancia odpovedá na otázku: **Čo ostane po operácii, ktorá vyhodila výnimku?**

| Garancia | Stav po výnimke |
| --- | --- |
| bez výnimiek (*no-throw*) | Operácia výnimku nevyhodí |
| silná (*strong*) | Stav je rovnaký ako pred volaním |
| základná (*basic*) | Objekty sú platné a zdroje neunikli |
| žiadna (*no guarantee*) | Môžu byť porušené invarianty objektu |

Samotná možnosť vyhodiť výnimku teda ešte neurčuje garanciu.


## Silná garancia: buď všetko, alebo nič

```cpp
void Playlist::load(std::span<const std::string> lines) {
    std::vector<Song> new_songs;

    for (const auto& line : lines)
        new_songs.push_back(parse_song(line)); // môže vyhodiť

    songs_.swap(new_songs);                    // noexcept: commit
}
```

```plain
pred:       songs_ = [A, B]
po úspechu: songs_ = [C, D, E]
po výnimke: songs_ = [A, B]
```

Jednotlivé `push_back` môžu uspieť, kým neskorší krok zlyhá. Nový playlist preto
pripravíme bokom a pôvodný stav zmeníme až operáciou, ktorá nevyhodí.


## Základná garancia: platný, ale zmenený stav

```cpp
void Song::set_title(std::string title) noexcept {
    title_.swap(title);
}

void Playlist::normalize_titles() {
    for (auto& song : songs_) {
        auto title = normalize(song.title()); // môže vyhodiť
        song.set_title(std::move(title));      // noexcept: commit
    }
}
```

```plain
pred:       songs_ = [" hello ", " WORLD ", " cpp "]
po výnimke: songs_ = ["Hello", "World", " cpp "]
```

* Časť práce už prebehla a nevráti sa späť
* `Playlist` je naďalej platný a môžeme ho bezpečne používať
* Žiadne zdroje neunikli


## Žiadna garancia: porušený invariant

```cpp
void Playlist::add(Song song) {
    ++song_count_;
    songs_.push_back(std::move(song)); // môže vyhodiť
}
```

```plain
pred:       song_count_ == songs_.size() == 2
po výnimke: song_count_ == 3, songs_.size() == 2
```

Objekt síce existuje, ale jeho údaje si odporujú. Ďalšie operácie sa naň nemôžu spoľahnúť.


## Porovnanie garancií

Ak `Playlist` pred operáciou obsahuje `[A, B]` a operácia zlyhá:

| Garancia | Možný stav po výnimke |
| --- | --- |
| bez výnimiek | výnimka nenastane |
| silná | presne `[A, B]` |
| základná | napríklad `[A]`, ale stále platný objekt |
| žiadna | napríklad počet `3`, ale iba dve skladby |

Garancia opisuje **následky zlyhania**, nie pravdepodobnosť zlyhania.

---

## Štandardné výnimky

| Typ | Typické použitie |
| --- | --- |
| `std::logic_error` | porušený predpoklad rozhrania alebo logiky |
| `std::invalid_argument` | neplatná hodnota argumentu |
| `std::out_of_range` | index alebo hodnota mimo rozsahu |
| `std::runtime_error` | zlyhanie zistené počas behu |
| `std::system_error` | systémová chyba spolu s `std::error_code` |
| `std::bad_alloc` | zlyhanie alokácie pamäte |

* Tieto typy dedia zo `std::exception` a poskytujú `what()`
* Vlastné výnimky zvyčajne odvodíme od vhodnej štandardnej bázy


## Kde výnimku chytiť?

* Tam, kde sa vieme zotaviť alebo zvoliť náhradný postup
* Tam, kde chceme pridať užitočný kontext a výnimku poslať ďalej
* Na hranici `main`, kde chybu oznámime a zvolíme návratový kód
* Nechytáme ju iba preto, aby sme ju potichu ignorovali
* `catch (...)` používame na hranici, kde musíme zachytiť aj neznáme typy


## Opätovné vyhodenie

```cpp
void load_user_data() {
    try {
        read_database();
    } catch (const std::exception& error) {
        std::println("Loading user data failed: {}", error.what());
        throw;
    }
}
```

* Samotné `throw;` pokračuje s pôvodnou výnimkou
* `throw error;` vytvorí nový objekt a môže orezať jeho dynamický typ
* Logovanie na každej vrstve môže tú istú chybu zaznamenať viackrát

---

## Výnimka alebo `std::expected`?

| `std::expected` | Výnimka |
| --- | --- |
| neúspech je bežná súčasť rozhrania | chyba preskočí vrstvy, ktoré ju nevedia riešiť |
| volajúci výsledok explicitne kontroluje | úspešná cesta ostáva oddelená od chyby |
| vhodné pre lokálne očakávané chyby | prirodzené pre zlyhanie konštruktora |

* Factory funkcia môže namiesto vyhadzujúceho konštruktora vrátiť `expected`
* V jednom rozhraní používame konzistentnú politiku oznamovania chýb


## Cena a použitie výnimiek

* Bežná cesta bez vyhodenej výnimky býva navrhnutá ako lacná
* Vyhodenie, hľadanie handlera a stack unwinding sú podstatne drahšie
* Trvanie spracovania chyby závisí od počtu opustených scope a deštruktorov
* Výnimky preto nepoužívame ako bežné vetvenie alebo ukončenie slučky


## Rady

* Vyhadzujeme objekty odvodené od `std::exception`
* Chytáme ako `const&` a iba tam, kde vieme reagovať
* Zdroje spravujeme cez RAII, nie cez ručné `new`, `delete`, `malloc` a `free`
* Na hranici `main` prevedieme výnimku na diagnostiku a návratový kód (môžeme použíť `catch (...)` na zachytenie všetkých typov výnimiek)
* Výnimky nepúšťame z deštruktorov ani z funkcií označených `noexcept`

---

## Výnimky nie sú ťažké, ošetrovanie chýb je ťažké.

---

# Diagnostika

---

## Informácie o kóde

* `__FILE__` - názov súboru
* `__LINE__` - číslo riadku
* `__func__` - názov aktuálnej funkcie

```cpp
#define LOG_ERROR(msg) log_error(__FILE__, __LINE__, __func__, msg)

void log_error(const char* file, int line,
               const char* function, const char* message) {
    std::cout << file << ':' << line << " in " << function
              << ": " << message << '\n';
}
```

```plain
/home/user/project/main.cpp:10 in main: Something bad happened
```

Makro vloží informácie o mieste, na ktorom použijeme `LOG_ERROR`.


## `std::source_location`

```cpp
#include <iostream>
#include <source_location>
#include <string_view>

void log_error(
    std::string_view message,
    std::source_location location = std::source_location::current()) {
    std::cout << location.file_name()
              << ':' << location.line()
              << ':' << location.column()
              << " in " << location.function_name()
              << ": " << message << '\n';
}

int main() {
    log_error("Something bad happened");
}
```

* Od C++20; poskytuje súbor, funkciu, riadok a stĺpec
* Predvolený argument zachytí miesto volania `log_error`, nie riadok v jej tele
* Volajúci nemusí použiť makro ani informácie odovzdávať ručne


## Stacktrace

```cpp
#include <iostream>
#include <stacktrace>

void log_stacktrace() {
    std::cout << "Current stack trace:\n"
              << std::stacktrace::current() << '\n';
}

int main() {
    log_stacktrace();
}
```

* `std::stacktrace::current()` je štandardizované od C++23
* Zachytí postupnosť aktívnych volaní; jednotlivé rámce možno aj iterovať
* Dostupnosť závisí od podpory štandardnej knižnice a niekedy aj od nastavení linkera
* Názvy funkcií a riadky závisia od debug symbolov
* Optimalizácia a inlining môžu výsledný stack trace zmeniť

---

# ĎAKUJEM

## Otázky?
