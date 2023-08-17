# C++

## Riešenie chybových stavov

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

# Konštanty

---

## Konštanty

* Z istého pohľadu programovanie je o udržiavaný invariantov a konštantnosť istých hodnôt a premenných môže pri tom veľmi pomôcť
* Konštantné dáta môžu byť pristupované z viacerých vlákien bez obavy o nedefinované správanie (data race)
* V C++ existuje viacero spôsobov ako definovať koncept konštanty 
   * `const`
   * `constexpr`
   * `#define`
   * `enum`

---

## `const`

* `const` znamená, že "objekt" sa nesmie meniť
* Dá sa obísť pomocou `const_cast`-u
* PROTIP: Nikdy nezahadzujte z objektov const
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

* Globálne premenné sú vždy inicializované na `0` (*zero initialized*)
* Výnimkou sú const objekty, ktoré musia byť inicializované hodnotou
* Väčšinou sú umiestnené do pamäti iba na čítanie a preto pokus o zápis spôsobí access violation

```cpp
int v;
const int c = 1;
 
void main() {
  std::cout << v << " " << c << std::endl; // 0 1
  
  //c = 2; // will not compile
  *const_cast<int*>(&c) = 4; // will compile
}
```

---

## `constexpr`

* Konštanty počas kompilácie
* Užitočné ak potrebujeme compile time constatnt (napríklad veľkosť pola)
* V niektorých kontextoch constexpr implikuje const

```cpp
const int compute_size_1(int a) { return 2 * a*a; }
constexpr int compute_size_2(int a) { return 2 * a*a; }
 
int main() {
  int x[compute_size_1(7)]; // error

  int y[compute_size_2(7)]; // OK
}
```

---

## `#define`

* Preprocesorové makrá môžu byť použité ako konštanty
* Makrá sú expandované ešte pred samotnou kompiláciou, fungujú preto mimo typového systému v podstate iba textovo 
* Číselné konštanty sa lepšie vyjadrujú pomocou constexpr, alebo enum-u
* Občas užitočné pri reťazcoch 
   * Zreťazovanie (concatenation) počas kompilácie

```cpp
#define DIRECTORY "C:"
#define FILENAME "log.txt"
#define SEPARATOR "\\"
#define PATH DIRECTORY SEPARATOR FILENAME
 
void main() {
  std::cout << PATH << std::endl; // "C:\log.txt"
}

```

---

## Čím viac const tým lepšie

* Uľahčí to programovanie
* Treba ale vybrať správny postup

```cpp
typedef WORD ECP_RET_ERROR;
 
static const ECP_RET_ERROR ECP_OK = 0x0000;
static const ECP_RET_ERROR ECP_ERROR = 0x0001;
```

* Kód vyššie kričí, že máme použiť enum
* Ak použijeme enum class ani nebudeme musieť prefixovať hodnoty (ostane nám OK, Error, ...)

---