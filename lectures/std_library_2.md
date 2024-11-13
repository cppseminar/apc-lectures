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
* Zložitosť operácií je v O notácii

`$$ f(n)=O(g(n)) \Leftrightarrow \exists c > 0 \exists n_0 \in \mathbb{N} ​ \forall n \geq n_0 \quad f(n) \leq c \cdot g(n) $$`

---

## Operácie

* Zaujímajú nás hlavne nasledovné operácie
    * Vloženie prvku (*insert*)
    * Zmazanie prvku (*erase*)
    * Hľadanie prvku (*find*)
    * Získanie prvku (*get* / *access*)
* V reálnom kóde sa vyskytuje hlavne získanie a vloženie
* Získanie prvku je operácia, ktorá mi vráti hodnotu na základe kľúča alebo indexu
* Hľadanie prvku je operácia, ktorá mi vráti index alebo kľúč na základe hodnoty

---

## `std::vector`

* Najpopulárnejší kontajner
* Dynamicky alokované pole
* Najuniverzálnejší kontajner, ktorý by mal byť prvou voľbou
* Je veľmi ohľaduplný k pamäti, konkrétne dobre spolupracuje s cache pamäťou procesora

| Operácia | Zložitosť |
|----------|-----------|
| `Vloženie` | O(n) amortizovanie O(1) pri vkladaní na koniec |
| `Zmazanie` | O(n) |
| `Hľadanie` | O(n) |
| `Získanie` | O(1) |


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
```

* Lineárne prehľadáva celý vektor, ak nájde prvok vráti iterátor na prvý výskyt, inak vráti `end`

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

for (auto &i : v) {
    if (i == 3) {
        return &i;
    }
}
```

---

# ĎAKUJEM

## Otázky?