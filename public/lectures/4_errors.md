# C++

## Riešenie chybových stavov

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* pretypovanie
* `assert`
* atribúty
* `std::optional`
* postupy riešenia chybových stavov
   * Návratové kódy
   * `errno`
   * Výnimky

---

# Pretypovanie (angl. casting)

---

## C štýl pretypovania

* Konvertuje takmer všetko, do takmer všetkého
    * Reinterpretuje smerníky a referencie
    * Odstraňuje konštantnosť
    * Zužovanie a rozširovanie číselných typov
    * Zaokrúhľovanie
    * Navigácia v hierarchií tried

---

## Používanie C pretypovania

* Menenie typov smerníkov

```cpp
int* i = new int(4);
//char* c = i; // will not compile

char* c = (char*)i;

delete i;
```


* Odstránenie konštantnosti (`const`)

```cpp
const char *s = "My const string";

char *p = (char*)s;
// better not to modify content of p
```


* Zužovanie typu (narrowing)

```cpp
int64_t i = 57342059000411;

int c = (int)i; // narrowing
```


* Orezanie desatinej časti (akoby sme zavolali funkciu `floor`).

```cpp
double d = 123.456;

char c = (char)d; // truncating
```

---

## C++ „odradzuje“ od C pretypovania

> **An ugly operation should have an ugly syntactic form**
> – Bjarne Stroustrup

* C pretypovanie sa ťažko hľadá v programe
* Pretože sa jednoducho píšu, programátori ich majú tendenciu používať aj keď netreba
* Niektoré pretypovania môžu byť naozaj nebezpečné (nedefinované správania)
    * Prístup do pamäte iba na čítania
    * Odrezanie dôležitých dát (pri číselných konverziách)

---

## `static_cast<T>`

* Číselné konverzie
    * Konverzie medzi bitovými dĺžkami (zužovanie, rozširovanie)
    * Orezávanie (*truncating*) čísel s desatinnou čiarkou
* Implicitné konverzie (volanie operátorov)
* Pridávanie const
* Navigácia v hierarchií objektov
    * Upcast
    * Downcast

---

## Používanie `static_cast`

Rozširovanie číselného typu (vždy bezpečné).

```cpp
int8_t i = 127;

auto j = static_cast<int16_t>(i); // safe cast, no lost of information
```

Spolu so `static_cast` je najlepšie používať `auto` ako typ, keďže je už typ uvedený pri pretypovaní a nie je tam ani náhodou ďalšia implicitná konverzia.


Zužovanie číselného typu (môžeme príjsť o dáta).

```cpp
int16_t i = 30'000;

auto j = static_cast<int8_t>(i);
```

~~Výsledok vyššie je definovaný implementáciou, na iných platformách môže byť iný.~~ Od C++20 je to už definované ako **x % 2n**.

```cpp
uint32_t i = 300'000;

// always i mod uint16_t max (65536)
auto j = static_cast<uint16_t>(i); // 37856
```

Pre `unsigned` hodnoty to bolo vždy dobre definované. 


Konverzia z integral typu na floating point a späť (môže dôjsť ku strate dát, alebo k orezaniu desatinej časti).

```cpp
uint64_t i = 3'000'111'222'333'444'555;

auto f = static_cast<double>(i);

// 3'000'111'222'333'444'608
auto j = static_cast<uint64_t>(f); // trunc
```

---

## `dynamic_cast<T>`

* Používa na sa pretypovanie polymorfných typov
* Kontroluje sa počas behu programu
* Viac o ňom bude na niektorej budúcej prednáške

---

## `const_cast<T>`

* Odstraňuje const
* Spôsobuje nedefinované správanie

```cpp
const char* str = "Const string!";

// char* s = static_cast<char*>(str); // will not compile
char* s = const_cast<char*>(str);

s[0] = 'B'; // undefined / most likely crash
```

| | |
| --- | --- |
| Používať extrémne opatrne, ak vôbec | ![Bear warning agains const_cast](./lectures/4_errors/disagreeing_bear.png) |

---

## `reinterpret_cast<T>`

* Určite nevygeneruje žiadne CPU inštrukcie
* Dajú sa reinterpretovať smerníky na iné smerníky 
* Smerníky na čísla a opačne (ak majú dosť bitov)
* Nedá sa ním odstrániť const
* Môže viesť k nedefinovanému správaniu
* C++ má [type alias rule](https://en.cppreference.com/w/cpp/language/reinterpret_cast) a preto iba malá sada pretypovania je definovné správanie (napr. konverzia z `int*` na `char*`)

---

## Používanie `reinterpret_cast`

```cpp
float f = 2.5f;

auto i = static_cast<int>(f); // 2;

//auto j = reinterpret_cast<int>(f); // will not compile
auto* j = reinterpret_cast<int*>(&f);

*j; // undefined, 0x0099fcc0
```

Reinterpretovanie smerikov na iné typy. Môže byť nedefinované. 


```cpp
int* p = new int(0);
auto i = reinterpret_cast<uintptr_t>(p);

auto* pp = reinterpret_cast<int*>(i);
```

Môžeme konvertovať aj smerníky na číselné typy. Tie musia mať dostatok bitoch (`intptr_t` a `uintptr_t` majú vždy dosť).


```cpp
int i = 123;

void* p = &i; // implicit

auto* pi = static_cast<int*>(p);
```

Konverzia na `void*` je implicitná a z `void*` na typový smerník stačí `static_cast`.

---

## `typeid`

* Základné informácie o type sa dajú získať pomocou operátora `typeid`
` Pre polymorfné typy sa deje počas behu programu, inak počas kompilácie

```cpp
#include <typeinfo>

int main() {
    const type_info& t = typeid(int);

    // will print something like int
    std::cout << t.name() << '\n';
}
```


## `type_info`

* Trieda reprezentujúca výsledok operátora `typeid`
* Obsahuje veľmi základné informácie
    * Meno typu
    * Hash hodnotu
    * Užitočnejšia s *RTTI*

Notes: RTTI je runtime type inference, použivané dynamic_cast a polymorfnými triedami.

---

## Pretypovanie a C++

<ul>
<li class="fragment">

Nepoužívajte C štýl
</li>
<li class="fragment">

Nepoužívajte `const_cast`
</li>
<li class="fragment">

Nepoužívajte `reinterpret_cast` 
</li>
<li class="fragment">

Nepoužívajte `dynamic_cast` 
</li>
<li class="fragment">

Nepoužívajte `static_cast` 
</li>
<li class="fragment">

Je najlepšie vyhnúť sa akémukoľvek pretypovaniu, ale keď sa inak nedá, tak zdola nahor
</li>
</ul>
 
---

## `std::bit_cast`

* `reinterpret_cast` sa nedá použiť na bitové reinterpretácie
* Od C++20, použiť sa dá iba ak oba typy sú triviálne a majú rovnakú veľkosť

```cpp
#include <bit>
#include <iostream>

int main() {
    float f = 1;
    int i = std::bit_cast<int>(f);

    std::cout << i << '\n'; // 1065353216
}
```

---

# Chyby v programoch

---

## Bugy

![Bug in my code? It is a features](./lectures/4_errors/bug-feature.png)

* Logické chyby, naša aplikácia nerobí to čo má
* Abnormálne ukončenie („pád aplikácie“)

---

## Pády v C++ programoch

* Často výsledkom nedefinovaného správania
    * Dereferencia nullptr smerníka
    * Delenie nulou
    * Porušenie ochrany pamäte (*access violation*, *segmentation fault*)

* Ťažko sa diagnostikujú, niekedy máme k dispozícií iba výpis (dump)
* Oveľa lepšia je prevencia
    * Nespoliehať sa na správanie, o ktorom sme si nie na 100% istý, že je definované
    * Nepoužívať zastarané techniky a konštrukcie (surové smerníky)

---

## Vždy kontrolujte vstup od užívateľa

* Žiadnemu užívateľovi sa nedá veriť
* Neúmyselné poškodenie
    * Užívatelia nevedia, čo robia a čo sa od nich očakáva
    * Pripraviť sa na najhoršie a vždy zobrazovať zmysluplné chyby
* Úmyselné poškodenie
    * Niektorí užívatelia budú chcieť spôsobiť nedefinované správanie aby využili náš program na získanie prístupu, prezradenie tajomstva
    * Náš proces nesmie spadnúť za žiadnych okolností

---

## Buffer overflow

```cpp
#include <stdio.h>

int main() {
    char name[128];
    printf("Enter your name:");
    scanf("%s", name);
    printf("Hello %s!", name);
}
```

* Ak je zadaný reťazec 127 znakov, alebo menší, tak je všetko v poriadku
* Väčšie hodnoty vedú k nedefinovanému správaniu


## Kompilátor sa nám snaží pomôcť

> error C4996: 'scanf': This function or variable may be unsafe. Consider using scanf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS.

![Failed debug abort](./lectures/4_errors/debug-abort.png)


## `scanf_s` (C only)

```cpp
int main() {
    char name[128];
    printf("Enter your name:");
    if (scanf_s("%s", name, (unsigned)std::size(name)) == 1) {
        printf("Hello %s!", name);
    }
}
```

* Po každom `%s` nasleduje veľkosť buffra, ak je buffer malý, tak zlyhá
* Problém je, že nie je dostupné na C++, iba na C11


## Fixing scanf for C++

```cpp
int read_string(char* buffer, size_t buflen) {
    char format[32];
    if (buflen == 0)
        return 0;

    snprintf(format, std::size(format), "%%%zus", buflen - 1);
    return scanf(format, *buffer*);
}
```

Tu musíme veľmi pozorne riešiť ak sa nám vstup nezmestí do buffra. Lebo nevieme rozlíšiť medzi nedostatkom pamäti a reťazcom presne takej dĺžky ako máme buffer.

```cpp
int main() {
    printf("Enter your name:");

    char name[16];
    read_string(name, std::size(name));
}
```


## ... alebo rovno `std::getline`

```cpp
#include <iostream>

int main()
{
    std::cout << "Enter your name: ";

    std::string name;
    if (std::getline(std::cin, name)) {
        std::cout << "Hello " << name << '\n';
    }
}
```

* Tento prístup je možno pomalší
* Ale zaručene spracováva správne všetky chybové stavy

---

# Kontrola parametrov funkcií

---

## Neplatné parametre

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

* Problémy
    * `dir` môže byť `nullptr`
    * dir môže byť prázdny reťazec
    * `file` môže byť `nullptr`

---

## parameter != nullptr

* Môžeme na začiatku funkcie skontrolovať parametre a ak je niečo zlé, tak vrátiť chybu

```cpp
std::string join_path(const char* dir, const char* file)
{
    if (dir == nullptr || file == nullptr)
        return {};
    // ...
}
```

* Prečo je `nullptr` taký špeciálny?
* `strlen` neoveruje či je jeho vstup `nullptr`
* Je prázdny reťazec najlepší spôsob ako hlásiť chybu?

---

## `assert`

* Runtime kontrola zadaných podmienok
* Aktívne iba ak `NDEBUG` **nie je** definovaný (zvyčajne debug buildy)
* Veľmi užitočné pre skorom zachytávaní problémov
* Zlyhaný `assert` vedie k okamžitému ukončeniu programu

```cpp
std::string join_path(const char* dir, const char* file) {
    assert(dir != nullptr);
    assert(file != nullptr);
    assert(file[0] != '\0');
    // ...
}
```


## Zlyhaný assert

![Failed assert](./lectures/4_errors/failed-assert.png)

* Hlási podmienku, súbor a číslo riadku
* Pozor aby sa nám nedostali do produkčného buildu


## assert(false)

```cpp
void print_message(int option) {
switch (option) {
case 0:
    std::cout << "A";
    break;
case 1:
    std::cout << "B";
    break;
default:
    assert(false);
}
```

* `assert(false)` nám má označovať nedosiahnuteľný kód
* Dá sa ale aj lepšie
    * Platný smerník je vždy `true`
    * `assert(!"Unsupported option!");`

---

## std::string_view 

* Môžeme použiť `std::string_view` (od C++17)
    * Obálka okolo `char*` (dáta reťazca) a `size_t` (dĺžka reťazca)
    * Môže byť `nullptr`, potom je ale dĺžka 0 (to sa dá dosiahnuť iba konštruktorom bez parametrov)
    * Vykonštruovať sa dá aj zo stringu a `char*`
    * Skoro celý read-only interface `std::string` je k dispozícií
    * Nemá `c_str()`, iba `data()`, lebo nemusia byť ukončené nulou


## Použivanie `std::string_view`

```cpp
std::strings_view s1;
assert(s1.data() == nullptr && s1.length() == 0);
assert(s1.find_first_of("a") == std::string_view::npos);

// std::string_view s2(nullptr); // undefined
std::string_view s3("String");
assert(s3.find_first_of("i") == 3);

std::string str("String");

// will not own the string, so we need to make sure
// the memory is still accessible, so it string_view is
// more like non owning pointer
std::string_view s4(str);
assert(s4.find_first_of("i") == 3);
```

---

## Riešenie so std::string_view

```cpp
std::string join_path(std::string_view dir, std::string_view file) {
    std::string result(dir);
    if (result.empty() || result.back() != '\\') {
        result.push_back('\\');
    }

    result += file;
    return result;
}
```

* Pri dvoch prázdnych reťazcoch vyrobí ("\\")
* Ale funkcia je stále dobre definovaná pre všetky vstupy
* Môžeme prípadne použiť pár `assert`-ov, ak nám to nevyhovuje

---

# Atribúty

---

## Atribúty

* Štandardizované anotácie kódu
* Spätne kompatibilné, staré kompilátory ich budú ignorovať (od C++17)
* Slúžia na správu upozornení a optimalizáciu, zvyčajne neovplyvňujú samotné správanie kódu
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

* V C++20 mnohé ďalšie (`[[likely]]`, `[[no_unique_adress]]`, ...)

---

## Posledný pokus na `join_path`

```cpp [|1|3-4|7-9|10-15]
[[nodiscard]]
std::string join_path(std::string_view path, std::string_view fragment) {
    assert(fragment.length() > 0);
    assert(fragment[0] != '\\');
    
    std::string result;
    bool add_separator = path.empty() || path.back() != '\\';
    
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
    * Abseil optional je a funguje v C++11

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

* Predvolene ich ignorujeme, dá sa k nim pridať [[nodiscard]], ale aj tak
* Aj keď sa neignorujú, tak často programátori používajú iba error/success
    * Efektívne používajú ako bool
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

* Alebo použijeme rovno std::stoll, čo je `strtoll` s výnimkami

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

# ĎAKUJEM

## Otázky?
