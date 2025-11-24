# C++

## Štandardná knižnica

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Kontajnery
    * Lineárne kontajnery
    * Asociatívne kontajnery
    * Neusporiadané kontajnery
* Štandardné algoritmy
    * Adaptéry
    * Množinové operácie

---

# Kontajnery

---

## Zložitosť operácií

* V štandardnej knižnici nebýva presne definovaná implementácia, ale iba zložitosť operácií
* Zložitosť operácií je v $O$ notácii

$$ f(n)=O(g(n)) \Leftrightarrow \exists c > 0 \exists n_0 \in \mathbb{N} ​ \forall n \geq n_0 \quad f(n) \leq c \cdot g(n) $$

---

## Operácie

* Zaujímajú nás hlavne nasledovné operácie
    * Získanie prvku (*get* / *access*)
    * Vloženie prvku (*insert*)
    * Zmazanie prvku (*erase*)
* V reálnom kóde sa vyskytuje hlavne získanie a vloženie
* Získanie prvku je operácia, ktorá mi vráti hodnotu na základe kľúča alebo indexu
* Pamäťová zložitosť ktorú uvádzame je pre populárne implementácie, v praxi si môžu knižnice implementovať kontajnery rôzne

---

## `std::vector`

* Najpopulárnejší kontajner
* Dynamicky alokované pole
* Najuniverzálnejší kontajner, ktorý by mal byť prvou voľbou
* Je veľmi ohľaduplný k pamäti, konkrétne dobre spolupracuje s cache pamäťou procesora
* Pri vkladaní sa môžu invalidovať iterátory, **ak** sa vektor **musí realokovať**, inak sa invalidujú iba iterátory, ktoré ukazujú **na**, alebo **za** vložený prvok
* Pri mazani sa invalidujú iterátory, ktoré ukazujú **na**, alebo **za** zmazaný prvok

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(1) |
| Vloženie | O(n) amortizovane O(1) pri vkladaní na koniec |
| Zmazanie | O(n) |


* Pamäťová zložitosť je okrem samotných prvkov
    * Tri smerníky na začiatok, koniec a koniec alokovanej pamäte (24B na 64-bitovom systéme)
    * Veľkosť alokovanej pamäte môže byť až dvojnásobok veľkosti vektoru


## Získanie

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

int a = v[2]; // 3
int b = v.at(2); // 3
```

* `[]` a `at` vrátia prvok na základe indexu, `at` kontroluje rozsah
* Oba spôsoby sú veľmi rýchle, pretože vektor je pole


## Vloženie

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

v.push_back(1); // 1, 2, 3, 4, 5, 1

v.insert(v.begin(), 2); // 2, 1, 2, 3, 4, 5, 1

v.insert(v.begin() + 2, { 9, 8 }); // 2, 1, 9, 8, 2, 3, 4, 5, 1
```

* `push_back` vloží prvok na koniec a je veľmi rýchly pri viacnásobnom vkladaní
* `vector` trochu plytvá pamäťou, keďže si alokuje viac pamäte ako je potrebné
* `insert` vloží prvok na základe iterátora, musí ale posunúť všetky prvky za ním, takže je pomalší


## `push_back` vs `emplace_back`

```cpp
std::vector<std::pair<std::string, std::string>> v;

v.push_back({ "Hello", "worlds" }); // pair is constructed and moved

v.emplace_back("Hello", "worlds"); // pair is constructed in place
```

* `push_back` vytvorí objekt a potom ho presunie do vektora
* `emplace_back` vytvorí objekt priamo v pamäti vektora, teda si ušetríme jeden move alebo copy (ak move neexistuje)


## Zmazanie

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

v.pop_back(); // 1, 2, 3, 4

v.erase(v.begin() + 2); // 1, 2, 4
```

* `pop_back` zmazanie posledného prvku
* `erase` zmazanie prvku na základe iterátora, musí ale posunúť všetky prvky za ním


## Hľadanie

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

auto it = std::find(v.begin(), v.end(), 3);
if (it != v.end()) {
    auto index = std::distance(v.begin(), it); // 2
}
```

* Lineárne prehľadáva celý vektor, ak nájde prvok vráti iterátor na prvý výskyt, inak vráti `end`

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

for (auto &i : v) {
    if (i == 3) {
        return &i - &v[0]; // 2
    }
}
```

---

## `std::vector<bool>`

* Špeciálny prípad, ktorý sa správa inak ako ostatné vektory
* Implementovaný ako bitové pole, kde každý bit reprezentuje jeden bool
* Z toho dôvodu nie je možné získať referenciu na prvok, ale iba hodnotu, alebo proxy objekt
* Zaberá teda v pamäti menej miesta, 8x menej ako `std::vector<char>`

```cpp
std::vector<bool> v = { true, false, true };

bool a = v[1]; // false
// bool& b = v[2]; // will not compile
bool&& c = v[2]; // true
c = false;
const bool& d = v[2]; // true???

auto&& p = v[2]; // true, proxy object
p = false;
const bool& r = v[2]; // false
```

---

## `std::array`

* Staticky alokované pole, ktorého veľkosť sa nemôže meniť
    * Pridávanie a mazanie prvkov preto nedáva zmysel
* Vyhľadávanie a získavanie prvkov je veľmi rýchle, ako pri `std::vector`
* Pamäťová zložitosť je len veľkosť prvkov v poli

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(1) |
| Vloženie | - |
| Zmazanie | - |

```cpp
std::array<int, 5> a = {1, 2, 3, 4, 5};

int b = a[2]; // 3

auto it = std::find(a.begin(), a.end(), 3);
```

---

## `std::map`

* Asociatívny kontajner, ktorý ukladá dvojice kľúč-hodnota
* Kľúč je unikátny, hodnota môže byť ľubovoľná
* Implementovaný ako vyvážený binárny strom
* Pri vkladaní a mazaní sa neinvalidujú iterátory

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(log n) |
| Vloženie | O(log n) |
| Zmazanie | O(log n) |


* Pamäťová zložitosť je okrem samotných prvkov
    * Každý node má smerníky na ľavého a pravého potomka, rodiča a farbu (32B na 64-bitovom systéme)
    * Samotná mapa je jeden smerník na koreň stromu a explicitne veľkosť mapy (16B na 64-bitovom systéme)	


## Získanie

```cpp
std::map<int, std::string> m = {{1, "one"}, {2, "two"}};

std::string a = m[1]; // "one"
std::string b = m[3]; // ""
std::string c = m.at(4); // std::out_of_range exception

auto it = m.find(4); // m.end()
```

* `[]` vráti hodnotu na základe kľúča, ak kľúč neexistuje, vytvorí ho
* `find` vráti iterátor na základe kľúča, ak kľúč neexistuje, vráti `end`
* O(log n) zložitosť, implementácia je vyvážený binárny strom


## Vloženie

```cpp
std::map<int, std::string> m = { {1, "one"}, {2, "two"} };

std::pair<std::map<int, std::string>::iterator, bool> x = m.insert({ 3, "three" });
assert(x.second);

auto[it, inserted] = m.insert({ 3, "four" });
assert(!inserted);
assert(it->second == "three");

auto[it2, inserted2] = m.insert_or_assign(3, "four" );
assert(!inserted2);
assert(it2->second == "four");
```

* `insert` vloží dvojicu, kde prvý prvok je kľúč a druhý hodnota, vráti dvojicu iterátor na vložený prvok a bool, či sa vložil (`true`) alebo už existoval (`false`)
* `insert_or_assign` vloží kľúč s hodnotou, ak kľúč už existuje, prepíše hodnotu, vráti rovnakú dvojicu ako `insert`


## `emplace`

```cpp
std::map<int, S> m = { {1, { 0, 1.1, "one"} } };

m.insert({ 3, { 3, 3.3, "three" } });

// m.insert_or_assign(2, { 2, 2.2, "two" }); // will not compile
m.insert_or_assign(2, S{ 2, 2.2, "two" });

auto [it, inserted] = m.emplace(3, S{ 4, 4.4, "four" });
assert(!inserted);

auto[_, inserted2] = m.try_emplace(3, 4, 4.4, "four");
assert(!inserted2);
```

* `emplace` vloží dvojicu, ktorá sa priamo vykonštruuje ako pár na mieste, ak kľúč už existuje, nič sa nevykoná
* `try_emplace` vykonštruuje aj samotnú hodnotu na mieste
* Návratové hodnoty sú rovnaké ako pri `insert`


## `emplace_hint`

```cpp
std::map<std::string, int> numbers = {{"A", 1}, {"B", 2}, {"C", 3}};

// if element will be constructed just before hint then, this operation
// is fastest possible, because it will be amortized constant if the hint
// is good, however if hint is not good, then it produce correct result,
// but will take some time penalty
numbers.emplace_hint(numbers.end(), "X", -1);

numbers.insert(numbers.end(), {"Y", -2});
```

* `emplace_hint` vloží dvojicu na základe iterátora, ak sme iterátor trafili, tak je operácia amortizovane konštantná
* Rovnako `insert` má overload pre iterátor, ktorý sa dá použiť ako hint
* Iterátor musí ukazovať tesne za prvok, ktorý chceme vložiť


## Zmazanie

```cpp
std::map<int, std::string> m = {{1, "one"}, {2, "two"}};

m.erase(1);
m.erase(m.begin());
```

* `erase` zmaže prvky na základe kľúča alebo iterátora
* Po zmazaní sa strom musí vyvážiť


## Usporiadané kontajnery

* Ako vypíšeme všetky prvky z mapy od najmenšieho po najväčší kľúč?
    
```cpp
std::map<std::string, int> my_map = { // fill up map
    { "One" , 1 },
    { "Two", 2 },
    { "Three" , 3 }
};

std::vector<std::pair<std::string, int>> my_vector;
std::copy(my_map.begin(), my_map.end(), std::back_inserter(my_vector));
std::sort(my_vector.begin(), my_vector.end());

for (const auto& i : my_vector) {
    std::cout << i.first << " " << i.second << std::endl;
}
```


* Zbytočne komplikované, mapa je už utriedená
* Obyčajnou iteráciou sa dá vypísať od najmenšieho po najväčší kľúč

```cpp
for (const auto& i : my_map) {
    std::cout << i.first << " " << i.second << std::endl;
}
```


## Ako zlúčime dve mapy?

```cpp
std::map<std::string, int> a, b;

a.insert(b.begin(), b.end());
```

Tento postup reallokuje všetky prvky z mapy `b` do mapy `a`.
<!-- .element class="fragment" -->

```cpp
a.insert(std::make_move_iterator(b.begin()), std::make_move_iterator(b.end()));
```
<!-- .element class="fragment" -->

Rýchlejší postup, pretože aspoň hodnoty, ktoré nie sú `const` sa presunú.
<!-- .element class="fragment" -->

```cpp
a.merge(b);
```
<!-- .element class="fragment" -->

Najrýchlejší postup, pretože sa presunú všetky prvky z mapy `b` do mapy `a`, v podstate sa vytrhajú vnútornosti, využijú sa celé alokácie nodov stromu. Od C++17.
<!-- .element class="fragment" -->

---

## `std::set`

* Asociatívny kontajner, ktorý ukladá unikátne hodnoty, nemá kľúče
* Implementovaný ako vyvážený binárny strom (v MSVC zdieľa implementáciu so `std::map`)

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(log n) |
| Vloženie | O(log n) |
| Zmazanie | O(log n) |

* Pamäťová zložitosť je rovnaká ako u `std::map`


## Operácie

```cpp
std::set<int> s = {1, 2, 3, 4, 5};

s.insert(6);

s.erase(3);

auto it = s.find(4);
```

* V podstate rovnaké operácie ako u `std::map`, ale bez kľúčov
* `try_emplace` neexistuje, ale `emplace` sa práva ako `try_emplace` pre `std::map`

---

## `std::multimap`

* Asociatívny kontajner, ktorý ukladá dvojice kľúč-hodnota, kľúč nie je unikátny
* Implementovaný rovnako ako `std::map`, na MSVC zdieľa implementáciu s `std::map`
* Všetky operácie sú rovnaké ako u `std::map`, ale pri vkladaní sa nekontroluje unikátnosť kľúča

```cpp
std::multimap<int, std::string> m = { {1, "one"}, {2, "two"}, {2, "two2"} };

m.insert({ 2, "two3" });
m.insert({ 3, "three" }, { 3, "three2" });
```

* Existuje aj `std::multiset`, ktorý je rovnaký ako `std::multimap`, ale bez hodnôt, resp. ako `std::set` ale s duplicitnými hodnotami 


## Špeciálne operácie

```cpp
std::multimap<int, std::string> m = { {1, "one"}, {2, "two"}, {2, "two2"} };

m.insert({ 2, "two3" });
m.insert({ { 3, "three" }, { 3, "three2" } });

auto it = m.find(2);
std::cout << it->second << '\n'; // "two" or "two2" or "two3"

std::cout << m.count(3) << '\n'; // 2

auto range = m.equal_range(2);
for (auto it = range.first; it != range.second; ++it) {
    std::cout << it->second << " "; // "two two2 two3"
}
```

* Tieto metódy existujú aj pre `std::map`, ale tam sú menej užitočné

---

## Hashovacia funkcie

* Transformuje objekt na `size_t` (hash)
* Ekvivaletné objekty (`==`) musia vracať rovnaký hash
* Nerovnaké objekty musia s vysokou pravdepodobnosťou vracať rozdielne hash-e
* Deterministická
* Veľmi rýchla (inak nezískame veľa rýchlosti)
* `std::hash` pre preddefinované objekty, viac sa dá nájsť v `boost::hash`


## Hash pre preddefinované typy

```cpp
std::cout << std::hash<int>()(42) << '\n'; // 10203658981158674303

std::hash<std::string> hash_fn;
std::cout << hash_fn("Hello, World!") << '\n'; // 7993990320990026836
std::cout << hash_fn("Bjarne") << '\n'; // 17285228561709254915

std::string s = "One wil have capacity set, other not";
std::string s2 = "One wil have capacity set, other not";
s2.reserve(1000000);
assert(hash_fn(s) == hash_fn(s2));
```

* `std::hash` je funktor, ktorý sa vytvára s typom, ktorý chceme hashovať
* Volanie s hodnotou potom vráti hash tejto hodnoty
    * Výsledok je závislý od implementácie, ale musí byť deterministický
    * Väčšina typov zo štandardnej knižnice má definovaný hash, ktorý hashuje iba hodnotu


## Hash pre vlastné typy

* Definujeme si triedu, ktorú chceme hashovať
* `operator==` nie je nutný pre hashovanie, ale je odporúčaný, keďže hashovacia tabuľka ho používa na porovnávanie
* `std::hash` je šablóna, ktorú musíme špecifikovať pre náš typ, ak máme privátne členy, môžeme definovať `friend std::hash` pre náš typ

```cpp
struct MyClass {
    std::string s;
    uint64_t i;

    friend std::hash<MyClass>;
    bool operator==(const MyClass& rhs) const = default;
};
```


## Špecializácia `std::hash`

* V rámci `std` musíme špecializovať šablónu `std::hash` pre náš typ
* Je to jeden z mála prípadov, kedy môžeme otvoriť `std` namespace a definovať v ňom nové veci

```cpp
namespace std {
    template<> struct hash<MyClass> {
        size_t operator()(const MyClass& keyval) const {
            return std::hash<std::string>()(keyval.s)
                ^ std::hash<uint64_t>()(keyval.i);
        }
    };
}
```


## Použitie vlastného hashu

```cpp
MyClass m{ "Herb", 22 };

std::cout << std::hash<MyClass>()(m) << '\n'; // 7972177731861517081
```

* Použitie je rovnaké ako pre preddefinované typy


## Kombinovanie hashov

* Ak máme viacero členov, ktoré chceme zahrnúť do hashu, musíme ich nejako kombinovať
* XOR je jednoduchý spôsob, ako zabezpečiť, že sa zmení hash, ak sa zmení jeden z členov
* Nie je ale úplne ideálny (`a ^ b == b ^ a`)
* `boost` má `boost::hash_combine`, ktorý je lepší

```cpp
// roughly current boost implementation
template<class T>
void hash_combine(size_t& seed, T const& v) {
	auto mix = [](size_t x) {
		x ^= x >> 33;
		x *= 0xff51afd7ed558ccd;
		x ^= x >> 33;
		x *= 0xc4ceb9fe1a85ec53;
		x ^= x >> 33;
		return x;
	};

	seed ^= mix(seed + 0x9e3779b9 + std::hash<T>()(v));
}
```

note: <https://www.boost.org/doc/libs/1_86_0/libs/container_hash/doc/html/hash.html#notes_hash_combine>


## Použitie `hash_combine`

```cpp
namespace std {
	template<> struct hash<MyClass> {
		size_t operator()(const MyClass& keyval) const {
			size_t seed = 0;
			hash_combine(seed, keyval.i);
			hash_combine(seed, keyval.s);
			return seed;
		}
	};
}
```

---

## `std::unordered_map`

* Asociatívny kontajner, ktorý ukladá dvojice kľúč-hodnota
* Kľúč je unikátny, hodnota môže byť ľubovoľná
* Implementovaný ako hashovacia tabuľka, ktorá rieši kolízie pomocou zreťazenia (štandard toto nepriamo vyžaduje)
* Pri vkladaní a mazaní sa neinvalidujú iterátory
* Ako názov napovedá, iterovanie cez prvky nemusí byť (a ani nebýva) usporiadané
* Bodobne ako pri `std::map` existuje aj `std::unordered_set`, `std::unordered_multimap` a `std::unordered_multiset`

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(n), priemerne O(1) |
| Vloženie | O(n), priemerne O(1) |
| Zmazanie | O(n), priemerne O(1) |


## `std::unordered_map` implementácia

* Žiadna implemenetácii nie je povinná, ale major implementácie používajú vector na reprezentáciu hashovacej tabuľky a ďalej zreťazený zoznam pre samotné prvky

Zdroj: <https://stackoverflow.com/questions/67435837/implementation-of-bucket-in-unordered-map>


### libstdc++

![libstdc++ unordered_map](http://i.sstatic.net/qfxPD.png)


### Microsoft Visual C++

![MSVC unordered_map](https://i.sstatic.net/sNl7p.png)

* Pamäťová zložitosť je okrem samotných prvkov
    * Pár smerníkov pre každý prvok, ktorým sa zreťazuje zoznam (16B na 64-bitovom systéme a MSVC)
    * Vector, ktorý obsahuje iterátory do zozname a hash hodnoty


## Získanie

```cpp
std::unordered_map<int, std::string> m = { {1, "one"}, {2, "two"} };

std::string a = m[1]; // "one"
std::string b = m[3]; // ""

assert(m.find(4) == m.end());

std::string c = m.at(3); // can throw
```

* Interface v podstate rovnaký ako u `std::map`, ale zložitosť je priemerne O(1)


## Vloženie

```cpp
std::unordered_map<int, std::string> m = { {1, "one"}, {2, "two"} };

m.insert({ 3, "three" });
m.insert({ { 4, "four" }, { 5, "five" } });

m.emplace(6, "six");

auto[_, inserted] = m.insert_or_assign(6, "six2");
assert(!inserted);
```

* Interface je rovnaký ako u `std::map`, ale zložitosť je priemerne O(1)
* Rovnako aj pre mazanie prvkov
* Iterátory sa neinvalidujú


## Bucked interface

```cpp
std::unordered_set<std::string> m;
std::generate_n(std::inserter(m, m.end()), 100, [i = 0]() mutable {
    return std::to_string(i++);
});

for (size_t i = 0; i < m.bucket_count(); ++i) {
    std::cout << "bucket #" << i << " has " << m.bucket_size(i) << " elements.\n";
}
```

* `bucket_count` vráti počet bucketov
* `bucket_size` vráti počet prvkov v danom buckete
* Iterovať bucket by sme vedeli cez `begin(i)` a `end(i)`, kde i je index bucketu
* Načo je to dobré? ... Zo *standard proposal*
    * *...it lets users investigate how well their hash function performs...*
    * *...if the iterators have an underlying segmented structure (as they do in existing singly linked list implementations), algorithms that exploit that structure, with an explicit nested loop, can be more efficient than algorithms that view the elements as a flat range*


## Hash policy

* `load_factor` - pomer počtu prvkov k počtu bucketov
* `max_load_factor` - maximálny pomer počtu prvkov k počtu bucketov, ktorý je ešte akceptovateľný, môže byť nastavený užívatelom

```cpp
std::unordered_map<int, std::string> m;
// fill up m
std::cout << m.load_factor() << '\n'; // 0.762939
std::cout << m.max_load_factor() << '\n'; // 1
```


## Rehash

* `rehash(n)` - nastaví počet bucketov na `n`, alebo viac, aby sa zmestili všetky prvky a `max_load_factor` sa dodržal
* `reserve(n)` - nastaví počet bucketov, aby sa zmestilo aspoň `n` prvkov a `max_load_factor` sa dodržal, zavolá rehash
* Ako zvyčajne, kontajnerom sa nechce moc zmenšovať

```cpp
std::unordered_set<std::string> m;
m.max_load_factor(7);

m.max_load_factor(0.5);
m.rehash(0); // without this it will leave the hash table as is

std::cout << m.load_factor() << '\n'; // 0.38147
std::cout << m.max_load_factor() << '\n'; // 0.5
```

---

## `std::list`

* Obojsmerne zreťazený zoznam
* Explicitne si uchováva veľkosť (`size()` je *O(1)*)
* Kazdý uzol je alokovaný na heape
* Pamäťová zložitosť je okrem samotných prvkov
    * Dva smerníky na predchádzajúci a nasledujúci prvok (16B na 64-bitovom systéme)
    * Veľkosť zoznamu (8B na 64-bitovom systéme)

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(n) |
| Vloženie | O(1) |
| Zmazanie | O(1) |


## Operácie

* Iterátory sa neinvalidujú, jedine ak sa zmazáva prvok samozrejme sa invalidujú iterátory na zmazaný prvok
* Iterátory sú bidirectional, teda sa dajú posúvať v oboch smeroch, ale nie náhodne prístupovať k prvkom

```cpp
std::list<int> l = {1, 2, 3, 4, 5};

l.push_back(6);
l.insert(l.begin(), 0);
```

* *O(1)* zložitosť vkladania a mazania vyzerá dobre, ale treba si uvedomiť, že sa jedná o zoznam, ktorý je lineárne prehľadávaný, takže zložitosť získania je *O(n)*


## `reverse`, `sort` a `unique`

* Tieto algoritmy sú špeciálne pre zoznamy ako členovia triedy `std::list`
* `sort` je stále *O(n log n)*, ale pomalší ako pre vektor

```cpp
std::list<int> l = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 };

l.sort();
//std::sort(l.begin(), l.end()); // will not compile
```


## `splice` a `merge`

* `splice` presunie prvky z jedného zoznamu do druhého
* `merge` zlúči dva 

```cpp
std::list<int> l1 = { 1, 2, 3, 4, 5 };
std::list<int> l2 = { 6, 7 };

l1.splice(l1.begin(), l2); // 6, 7, 1, 2, 3, 4, 5
l2.splice(l2.begin(), l1, l1.begin(), std::next(l1.begin(), 2)); // 6, 7

l1.merge(l2); // 1, 2, 3, 4, 5, 6, 7
```


## Rýchlosť zoznamu

* Zoznam je určite užitočná štruktúra, ale treba si uvedomiť, že nie veľmi rýchla
* Hlavne užitočná, ak potrebujeme mať stabilné iterátory, alebo veľmi často vkladať a mazať prvky zo začiatku
* Inak použite `std::vector`

---

## `std::forward_list`

* Jednosmerne zreťazený zoznam
* Funkcie a operácie sú podobné ako pri `std::list`
* Iterátory sú len Forward, teda sa dajú posúvať len vpred
* Stále existure `sort` v *O(n log n)*
* Používa menej pamäte ako `std::list`, pretože nemá druhý smerník
* Neukladá si veľkosť, takže `size()` ani neexistuje


## `before_begin`

* `before_begin` je špeciálny iterátor, ktorý ukazuje pred prvý prvok
* Je potrební kvôli jednosmernej povaze zoznamu, aby sa dalo vkladať na začiatok
* Pri iných kontajneroch sa dá vkladať na začiatok cez `begin`, štandardne pozícia označuje miesto pred ktoré sa vkladá

```cpp
std::forward_list<int> l = { 1, 2, 3, 4, 5 };

l.insert_after(l.before_begin(), 0);

std::forward_list<int> l2 = { 6, 7 };
l.splice_after(l.before_begin(), l2);
```

---

## `std::deque`

* Double-ended queue
* Rýchle vkladanie a mazanie na začiatku a konci
* Rýchle získanie prvkov na začiatku a konci
* Nemá lineárnu pamäť (sú to pospájané bloky pamäte)

| Operácia | Zložitosť |
|----------|-----------|
| Získanie | O(1) |
| Vloženie | O(n), O(1) z oboch koncov |
| Zmazanie | O(n), O(1) z oboch koncov |


## Invalidácia iterátorov

* Iterátory sa neinvalidujú, ak sa pridáva alebo maže na koniec alebo začiatok
* Ak sa pridáva alebo maže v strede, tak sa invalidujú všetky iterátory


## Reprezentácia

![std::deque internal representation](https://i.sstatic.net/SthOW.png)

Zdroj: <https://stackoverflow.com/a/6292437>


## Operácie

```cpp
std::deque<int> d = { 1, 2, 3, 4, 5 };

d.push_back(0);
d.push_front(6);

std::sort(d.begin(), d.end());

d.pop_back();
d.pop_front();
```

* Interface je bodobný ako u `std::vector`, neexistuje `reserve` a ani `capacity`


## Problémy

* `std::deque` vyzerá ako dobrá voľba, ale má svoje problémy
* Pamäťová alokácia je zložitejšia ako pri `std::vector`, pretože sa musia alokovať bloky pamäte
* Dosť komplikovaná implementácia, hlavne oproti `std::vector`
* Deštruovanie nie je jeden `free`, ale viacero
* Indexovanie je pomalšie (dvojitá dereferencia)
* [Porovnanie](https://www.codeproject.com/Articles/5425/An-In-Depth-Study-of-the-STL-Deque-Container)

---

# Adaptéry

---

* Adaptéry uprajujú rozhranie kontajnerov
    * Pridávajú funkcie
    * Skrývajú implementáciu

---

## `std::stack`

* LIFO štruktúra
* Implementovaná nad iným kontajnerom 
    * štandardne `std::deque`
    * dáva zmysel zmeniť na `std::vector`


## Interface

```cpp
std::stack<int, std::vector<int>> s; // standard is std::deque
s.push(1);
s.push(2);
std::cout << s.top(); // 2
s.pop();
std::cout << s.top(); // 1
s.pop();
// std::cout << s.top(); // undefined
```

* `push` pridá prvok na vrchol
* `pop` odstráni prvok z vrcholu
* `top` vráti referenciu na vrchný prvok
* `size` vráti veľkosť

---

## `std::queue`

* FIFO štruktúra
* Implementovaná nad iným kontajnerom 
    * štandardne `std::deque`
    * dáva zmysel zmeniť na `std::list`, ak veľmi chceme


## Interface

```cpp
std::queue<int, std::list<int>> q; // standard is std::deque
q.push(1);
q.push(2);
std::cout << q.front(); // 1
std::cout << q.back(); // 2
q.pop();
std::cout << q.front(); // 2
q.pop();
// std::cout << q.front(); // undefined
```

* `push` pridá prvok na koniec
* `pop` odstráni prvok z začiatku
* `front` vráti referenciu na prvý prvok, `back` na posledný

---

## `std::priority_queue`

* Prioritná rada
* Implementovaná nad iným kontajnerom 
    * štandardne `std::vector`
    * môžeme zmeniť na `std::deque`
* Prvky sú uložené aby boli podľa priority
* Priorita je definovaná pomocou `<`
* Vnútorne je implementovaná ako halda (heap)


## Interface

```cpp
std::priority_queue<int> s; // standard is std::vector
s.push(1);
s.push(2);
s.push(0);
s.push(3);
std::cout << s.top(); // 3
s.pop();
std::cout << s.top(); // 2
s.pop();
std::cout << s.top(); // 1
s.pop();
```

---

## `std::flat_map` a `std::flat_set`

* C++23 (trochu iné ako v Boost `boost::flat_map`)
* Implementované nad dvoma `std::vector`mi (dá sa zmeniť)
    * V jednom sú triedené kľúče
    * V druhom sú hodnoty
    * Platí, že kľúče a prislúchajúce hodnoty sú na rovnakom indexe
* V princípe zaberá menej miesta ako `std::map` a `std::set` (nemá toľko smerníkov) 


## Interface

* V podstate rovnaký ako u `std::map` a `std::set`, ale
    * Iteratory sa invalidujú pri vkladaní a mazaní (závisí to od implementačného kontajneru)
    * Vkladanie a mazanie je *O(n)*

```cpp
std::flat_map<int, std::string> m = { {1, "one"}, {2, "two"} };

m.insert({ 3, "three" }); // may invalidate iterators
```

---

## `std::inplace_vector`

* C++26
* Hybrid medzi `std::vector` a `std::array`
* Musíme definovať maximálnu veľkosť pri konštrukcii
* Prvky potom vkladáme na miesto, ktoré sme si rezervovali
* Interface ako u `std::vector`, ale nemôžeme meniť veľkosť

---

## Boost a spol.

* Knižnice ako boost alebo Abseil ponúkajú ďalšie kontajnery, z ktorých niektoré sa dostanú aj do štandardu
* Boost
    * `boost::flat_map` a `boost::flat_set` - podobné ako `std::flat_map` a `std::flat_set`
    * `boost::multi_index` - kontajner, ktorý umožňuje viacero indexov
    * `boost::intrusive` - kontajnery, ktoré sa dajú použiť v iných štruktúrach
    * `boost::small_vector` - podobné ako `std::vector`, ale ak je veľkosť malá, tak sa alokuje na stacku
* Abseil
    * `absl::flat_hash_map` - `std::unordered_map` implementovaná s otvoreným adresovaním
    * `absl::btree_map` - `std::map` implementovaná nad B-stromom

---

# Algoritmy

---

## Množiny

* Utriedené sekvencie sú množiny z hladiska niektorých algoritmov z `<algorithm>`
    * `std::set` je utriedená sekvencia
    * Aj utriedený `std::vector` je utriedená sekvencia
    * `std::unordered_set` nie je
* Fungujú nad nimi množinové operácie

```cpp
std::set<int> primes = { 2, 3, 5, 7 };
std::vector<int> even = { 2, 4, 6, 8, 10 };
std::set<int> inter, merged;

std::set_intersection(primes.begin(), primes.end(), even.begin(), even.end(), std::inserter(inter, inter.end()));
// inter : 2
std::merge(primes.begin(), primes.end(), even.begin(), even.end(), std::inserter(merged, merged.end()));
// merged : 2, 3, 4, 5, 6, 7, 8, 10
```

---

## Halda

* Máme síce adaptér `std::priority_queue`, ale môžeme si spravovať aj sami haldu

```cpp
std::vector<int> heap = { 5, 7, 51, 8, 7, 9, 6 };
std::make_heap(heap.begin(), heap.end());
heap.push_back(100); // (..., 100)

// assume [begin, end - 1) is heap, push end - 1
std::push_heap(heap.begin(), heap.end()); // (100, ...)
// move first to last and reheap
for (int i = 0; i < 3; ++i) {
    std::pop_heap(heap.begin(), heap.end());
    std::cout << heap.back() << ", "; // 100, 51, 9
    heap.pop_back();
}
std::sort_heap(heap.begin(), heap.end()); // 5, 6, 7, 7, 8
```

---

## Triedenie

* Triedenie je jedna z najčastejších operácií, ktorú potrebujeme
* `std::sort` je implementovaný (MSVC) ako intro-sort, teda kombinácia quick-sort, heap-sort a insertion-sort
    * Pre malé sekvenice používa insertion-sort
    * Pre veľké používa quick-sort
    * Ak je veľa rekurzií, tak prejde na heap-sort
* `std::stable_sort` je sort, ktorý zachováva poradie rovnakých prvkov


## `unique`

* `std::unique` odstráni duplikáty z utriedenej sekvencie

```cpp
std::vector<int> v = { 1, 2, 2, 3, 3, 3, 4, 5, 5, 5, 5 };
v.erase(std::unique(v.begin(), v.end()), v.end());
```

---

## `nth_element`

* `std::nth_element` posunie `n`-ty prvok na svoje mieste, akoby bol utriedený
    * Všetky prvky naľavo od neho sú menšie
    * Všetky prvky napravo od neho sú väčšie
* Prvky sa poposúvajú, ale nie sú úplne utriedené
* Zložitosť je *O(n)* prierne (robí to podobne ako quick-sort)

```cpp
std::vector<int> v = { 5, 7, 51, 8, 7, 9, 6 };
std::nth_element(v.begin(), v.begin() + 2, v.end());
```


## a mnoho ďalších

* `std::rotate` - posunie prvky v sekvencii
* `std::reverse` - obráti sekvenciu
* `std::shuffle` - zamieša sekvenciu
* `std::minmax_element` - vráti iterátory na najmenší a najväčší prvok
* ...

---

# Paralelné spracovanie

---

## `std::execution`

* Od C++17
* `std::execution` je enum, ktorý definuje spôsob, akým sa majú algoritmy vykonávať
* `std::execution::seq`
    * sekvenčné vykonanie
    * default
* `std::execution::par`
    * paralelné vykonanie
    * niektoré algoritmy môžu byť paralelizované a dáva to zmysel
    * MSVC má napríklad `std::sort` paralelizovaný
* `std::execution::par_unseq`
    * paralelné vykonanie s vektorizáciou
    * nie je implementované asi v žiadnom kompilátore

---

## `std::sort`

```cpp
int rnd() {
    static std::mt19937 rng; // random number generator
    return rng();
}

int main() {
    using namespace std::chrono;
    std::vector<int> v(100'000'000);
    std::generate(v.begin(), v.end(), rnd); // will generate numbers
    auto start = high_resolution_clock::now();
    std::sort(std::execution::par, v.begin(), v.end());
    auto duration = high_resolution_clock::now() - start;
    auto millisec = duration_cast<milliseconds>(duration).count();
    std::cout << millisec << "ms\n";
}
```

Note: u mňa to trvalo cca 2.1x rýchlejšie


## `std::for_each`

```cpp
std::vector<int> v(100'000'000);
std::generate(v.begin(), v.end(), rnd); // will generate numbers
auto start = high_resolution_clock::now();
std::for_each(std::execution::par, v.begin(), v.end(), [](int& i) { i *= 2; });
auto duration = high_resolution_clock::now() - start;
auto millisec = duration_cast<milliseconds>(duration).count();
std::cout << millisec << "ms\n";
```

* Strácame vlastnosť, že `for_each` pôjde v poradí
* Na VS 2022, asi 2x pomalšie ako sekvenčné spracovanie


## `std::transform`

```cpp
bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

std::vector<int> v(10000);
std::vector<bool> res(v.size());
std::iota(v.begin(), v.end(), 0);

std::transform(std::execution::par, v.begin(), v.end(), res.begin(), [](int i) { return is_prime(i); });
```

* Na VS 2022, pri 100k prvkov je `seq` rovnako rýchle ako `par`, pri viac je `par` rýchlejší a pri menej zase `seq`


## Nástrahy paralelného spracovania

* Paralelné spracovanie nie je zadarmo
* Môže byť pomalšie, ako sekvenčné spracovanie
* Môže byť nepredvídateľné, používajú sa nové vlákna
* Môže byť ťažké debugovať
* Niektoré algoritmy sú ťažko paralelizovateľné (`std::reverse`)
* Implementácia je celkom chytrá
    * Ak máme len jeden procesor, tak sa nepoužije paralelné spracovanie
    * Pre malé sekvencie sa nepoužije paralelné spracovanie
* Netreba by default používať všade
* Merať, merať, merať

---

# Ostatné

---

## `std::bitset<N>`

* Akoby `std::array<bool, N>`, ale používa menej pamäte, podobne ako `std::vector<bool>`
* Nie je to kontajner, takže nemá iterátory
* Operácie s bitmi ako `&`, `|`, `^`, `<<`, `>>` sú definované
* Funkcie ako `count`, `size`, `test`, `set`, `reset`, `flip`

```cpp
std::bitset<8> b;
b.set(2);
b.count(); // 1
b.to_string(); // "00000100"
b.to_ulong(); // 4
```

---

## `std::source_location`

* Umožňuje získať informácie o mieste, kde sa nachádzame v kóde

```cpp
void log(const std::source_location location = std::source_location::current())
{
    std::cout << "file: "
              << location.file_name() << '('
              << location.line() << ':'
              << location.column() << ") "
              << location.function_name() << "\n";
}
 
int main() {
    log();
}
```

---

## Matematické konštanty

* Konečne nemusíme používať `M_PI` a podobne
* V hlavičkovom súbore `<numbers>` sú definované konštanty ako `pi`, `e`, `log2e`, `sqrt2` a podobne

```cpp
std::cout << std::numbers::pi << '\n'; // 3.14159
```

---

## Random numbers

* Starý `rand` je už pomaly dosluhuje (a nikdy nebol dobrý)
* **Nie sú vhodné na kryptografické účely**

```cpp
std::random_device rd;

std::mt19937 gen(rd()); // std::default_random_engine

std::uniform_int_distribution<int> dis(1, 6); // fair dice

for (size_t i = 0; i < 30; ++i) {
    std::cout << dis(gen); // e.g. 223646345444643431415515513225
}
```


## Replacement for `rand`

```cpp
int rnd() { // 0..maxint
	static std::random_device rd;
	static std::default_random_engine gen(rd());
	static std::uniform_int_distribution<int> dis;

	return dis(gen);
}
```

---

## Čas

* `<chrono>` obsahuje všetko potrebné na meranie a prácu s časom
* `std::chrono::system_clock`, `std::chrono::steady_clock`, `std::chrono::high_resolution_clock`

```cpp
auto start = std::chrono::high_resolution_clock::now();

// do something

auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

---

## Regular expressions

* `<regex>` obsahuje triedy na prácu s regulárnymi výrazmi

```cpp
std::regex r(R"((\d{4})-(\d{2})-(\d{2}))");

std::string s = "2024-11-17";

std::smatch m;
if (std::regex_match(s, m, r)) {
    std::cout << m[0] << '\n'; // 2024-11-17

    for (size_t i = 1; i < m.size(); ++i) {
        std::cout << m[i] << '\n';
    }
}

std::cout << std::regex_replace(s, r, "$2/$3/$1"); // 11/17/2024
```

---

## `std::filesystem`

* Práca s súborovým systémom
* Pracuje aj s POSIX aj s Windows

```cpp
std::filesystem::path p = "/";

std::recursive_directory_iterator it(p);
for (const auto& entry : it) {
    std::cout << entry.path() << '\n';
}

std::filesystem::copy("file.txt", "file2.txt");
std::filesystem::remove("file2.txt");
```

---

# ĎAKUJEM

## Otázky?