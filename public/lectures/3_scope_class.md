# C++

## Scope, triedy a základy std::

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Konštanty
* Scope
* Triedy a objekty
* Životnosť premenných
* Základné objekty zo štandardnej knižnice
   * `std::vector`
   * `std::string`


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

## `#define`

* Preprocesorové makrá môžu byť použité ako konštanty
* Makrá sú expandované ešte pred samotnou kompiláciou, fungujú preto mimo typového systému v podstate iba textovo 
* Číselné konštanty sa lepšie vyjadrujú pomocou `constexpr`, alebo `enum`-u
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

# Scope

---

## Naujužitočnejší kus kódu

# `}`

---

## Životnosť lokálnych premenných

* Lokálne premenné vo funkciách začínajú život pri deklarácií a sú zničené na konci scope-u, v ktorom boli deklarované

```cpp
void f() {
    std::string s = "Normal variable";
} // s is destroyed here
```

```cpp
void f() {
    auto *s = new std::string("Leak?");
} // s is destroyed here
// !!! but because s is just a pointer
// !!! there will be a resource leak

```

```cpp
void f() {
    auto *s = new std::string("OK");
    // resource associated with *s 
    // is released by calling delete 
    delete s; 
} // s is destroyed here

```

---

## Život objektov

* (Skoro) každý objekt začína svoj život volaním konštruktora
* (Skoro) každý objekt končí svoj život volaním deštruktora

---

## Pravidlo

* Pokiaľ sa premenná dá použiť, tak je nažive.

---

## Scope

* Scope je abstraktný koncept definovaný v štandarde, ale v princípe je to všetko ohraničené {}

```cpp
void f() {
  // scope A
  int i = 0;
  if (i == 4) {
    // scope B
  } else {
    // scope C
    {
      // scope D
    }
  }
  for (auto i : {"a"}){
    // scope E
  }
}
```

---

## Viditeľnosť

* Premenné sú viazané na scope, v ktorom boli deklarované, každý vnorený scope deklarovaný neskôr môže tiež túto premennú používať 
* Kompilátor to vynucuje 
* Pravidlo
* Vždy deklarujte premenné v najvnorenejšom scope ako sa dá. 

---

## Prečo?

* Kompilátor vie lepšie optimalizovať 
   * Konštruktory a deštruktory sa volajú iba keď je to potrebné
* Program sa jednoduchšie číta
   * Premenné sa nepoužívajú viackrát na rôzne úlohy (toto odrádza používanie veľmi všeobecných mien)
   * Súvisiace časti programu sú viac lokalizované, tak nie je potrebne scrolovať hore dole

---

## for cyklus

```cpp
for (int i = 0; i < 100; ++i) {

}
```

```cpp
{
    int i = 0;
    for (; i < 100; ++i) {

    }
}
```

* Kusy kódu vyššie sú ekvivalentné
* Prvý je silno preferovaný a nie je dôvod používať druhý

```cpp
for (;;) {

}
```

```cpp
while (true) {

}
```

---

# Objektovo orientované programovanie

---

## Trieda (class)

* Šablóna pre vytváranie nových objektov (inštancií)
* Statické počas kompilácie, nové triedy sa nedajú vytvárať počas behu programu
* Obsahujú dáta (fields) a metódy (methods)
* Dáta sú zapuzdrené, teda ich životnosť je naviazaná na životnosť objektu, v ktorom sú definované
* operator. sa používa na prístup k členom 
* V podstate to isté ako struct
* Hlavný cieľ tried je stráženie invariantov

---

## Hlavičkový súbor / interface

```cpp
class Widget {
public:
    Widget(const char* s);
    ~Widget();
 
    int Observe() const;
    void Mutate(int x); 
 
private:
    int data = 0;
    std::string str;
};
```

Meno triedy
Modifikátor prístupu
Konštruktor
Deštruktor
Metódy
Dáta / inštančné premenné

---

## Zdrojový súbor / implementácia

```cpp
Widget::Widget(const char* s)
    : data(10)
    , str(s) {
}
 
Widget::~Widget() {
    std::cout << "~Widget" << '\n';
}
 
int Widget::Observe() const {
    // data++; // error
    std::cout << data << '\n';
    return data;
}
 
void Widget::Mutate(int x) {
    data += x; // OK
    std::cout << data << '\n';
}
```

* Inicializačný list v konštruktore
* Konštantné metódy nemôžu meniť stav objektu.

---

## Inicializačný list v konštruktore

* Miesto na volanie konštruktorov členských premenných
* Môže závisieť od parametrovo konštruktora

```cpp
class Foo {
public:
  Foo(int x) {
    // s is default constructed 
    i = x;
    s = std::to_string(x);
    f = x;
  }
private:
  std::string s;
  int i;
  float f;
};
```

```cpp
class Foo {
public:
  Foo(int x)
    : i(x)
    , s(std::to_string(x))
    , f(x) {
    // everything inside class is constructed
    // add other steps required for class to
    // function properly
  }
private:
  std::string s;
  int i;
  float f;
};

```

---

## Metódy objektov

* Obyčajné funkcie s jedným implicitným parametrom this (smerník na inštanciu objektu)
* Vnútri metódy sa dajú referencovať všetky členské premenné

```cpp
class StringI {
public:
    void SetIndex(int i) {
        this->i = i;
        s = std::to_string(i);
    }

    void Print() const {
        // i = 0; or whatever is error
        std::cout << s << '\n';
    }
private:
    int i = 0;
    std::string s;
};
```

* Nejednoznačnosť je vyriešená explicitným použitím this smerníka.
* Konštantné funkcie majú konštantný this smerník, preto nevedia meniť členské premenné.

---

## Get a Set metódy

* Tiež sa volajú mutator a accessor
* Zapuzdrujú členské premenné
* V iných jazykoch sa volajú aj properties

```cpp
class Person {
public:
    Person(const std::string& name) 
        : m_Name(name) { }
 
    int GetAge() const { return m_Age; }
    void SetAge(int age) { 
        assert(age >= 0); 
        m_Age = age; 
    }
 
    const std::string& GetName() const { return m_Name; }
private:
    int m_Age = 0;
    std::string m_Name;
};
```

* Ak nedovolíme priamy prístup k premenným, tak môžeme vynútiť kontroly, alebo závislosti.
* Premenná iba na čítanie.

const std::string&?

Na predchádzajúcom slide je uvedená konštantná referencia na štandardný string
Zatiaľ nás nemusí trápiť, čo to presne je, iba si môžete zapamäť, že ak do funkcie dávam std::string (resp. akýkoľvek zložitý objekt), tak ho predám ako const&, kód bude výrazne rýchlejší

---

## Vytváranie nových inštancií 

* Ak aspoň jeden konštruktor úspešne skončí (nevyvolá výnimku), potom je objekt považovaný za skonštruovaný

```cpp
class Object {
public:
    Object();
    Object(int i);
    Object(const std::string& s);
};
 
int main() {
    Object a; // call to Foo::Foo()
    Object b(0); // call to Foo::Foo(int)
    Object c("string"); // call to Foo::Foo(const std::string&)
}
```

---

## Deštruovanie inštancií

* Akonáhle má byť inštancia zdeštruovaná (skončila sa jej životnosť), tak sa zavolá deštruktor
* Trieda môže mať ľubovoľne veľa konštruktorov, ale iba jeden deštruktor

```cpp
class Obj {
public:
    Obj() { std::cout << "Obj" << '\n'; }
    Obj(int) { std::cout << "Obj(int)" << '\n'; }
    ~Obj() { std::cout << "~Obj" << '\n'; }
};
 
int main() {
    Obj a, b{ 1 }, c(1);
    // destructors are called in reverse b, a
}
```

---

## C++ most vexing parse

Scott Meyers, 2001

```cpp
class A {
public:
  A() { std::cout << "A"; }
  ~A() { std::cout << "~A"; }
};
int main() { A a(); }
```

Nová premenná typu A s menom a je skonštruovaná volaním A::A().

Vlastne je to deklarácia funkcie s menom a, ktorá nemá žiaden parameter a vracia objekt typu A.

Väčšina programátorov očakáva nový objekt, štandard ale vyžaduje deklaráciu funkcie. 

A a{}; funguje správne. 

---

## Explicitné konštruktory

* Ak konštruktor triedy C obsahuje iba jeden parameter typu T, potom tento konštruktor sa môže použiť ba implicitnú konverziu z T na C

```cpp
class Convert {
public:
    Convert(int i) { std::cout << i << '\n'; }
    explicit Convert(std::string f) { std::cout << f << '\n'; }
};
 
int f(const Convert&);
 
int main() {
    f(15); // OK, Foo::Foo(int) is used as conversion
    f(5.4); // OK (warning), :(
    // f(std::string("abc")); will not compile
}
```

Pravidlo: Všetky jednoparametrové konštruktory označíme automaticky slovom explicit.

---

## Modifikátory prístupu

public Každý môže pristupovať k danej premennej, alebo metóde.
protected Iba zdedené triedy a priatelia môžu pristupovať.
private Pristupovať môže iba samotná trieda, alebo priatelia.

```cpp
class A {
public:
    A(int n) : data(n) { }
    int GetData() const { return data; }
    int tag;
private:
    int data = 0;
    friend void Modify(A& a, int n);
};
void Modify(A& a, int n) {
    a.data = n;
}
```

```cpp
int main() {
    A a(10);
    a.tag = 10;
    std::cout << a.GetData(); // 10
    // a.data = 11; // error
    Modify(a, 11);
    std::cout << a.GetData(); // 11
}
```

Friend trieda, alebo funkcia môže pristupovať ak k privátnym dátam. 

---

## Aký je rozdiel medzi class a struct v C++?

```cpp
class MyClass
{
    // ...
};
```

```cpp
struct MyClass
{
    // ...
};
```

Existuje iba jeden malý rozdiel. Všetky členy v štruktúre sú predvolene public, v triede private.

Niektorí programátori používajú stále triedy, iný preferujú štruktúry pre POD typy (plain old data) a urobia všetko public.

---

## Skrývanie informácie

* Information hiding
* Triedy poskytujú interface a skrývajú všetky implementačné detaily (všetky členské premenné by mali byť privátne)
* Používateľov tried by mali zaujímať iba verejné časti a nikdy by sa nemali chytať vnútornosti tried
* Pokiaľ niečo nie je explicitne napísané, treba predpokladať, že je to nedefinované

---

# Triedy a scope

---

## Život objetku

* Vždy keď sa má objekt vytvoriť zavolá sa definovaný konštruktor
* Ak je konštrukcia úspešná, garantovanie sa nám zavolá deštruktor na konci života objektu

NEjaky priklad ako sa to da pouzit. 

---

## Kde je bug v nasledujúcom kuse kódu?

```cpp
int main()
{
    string *ptr = nullptr;
    
    int i;
    if (cin >> i)
    {
        string s = std::to_string(55);
        s += to_string(i);
 
        ptr = &s;
    }
 
    if (ptr != nullptr)
        cout << *ptr;
}
```

Na tomto mieste je reťazec s zdeštruovaný a teda všetky referencie a smerníky sú neplatné a nesmú sa použiť. 

Test na nullptr stále prejde, je na programátorovi aby toto urobil dobre.

---

## Členy v triedach

* Ich život je zviazaný (bound to) so životom objektu, ktorý ich obsahuje
* Aké je poradie deštrukcie členov triedy? (Je to poradie v súbore, alebo v inicializačnom liste, alebo nešpecifikované...)

```cpp
class B {
public:
    B() : m_y("Y"), m_x("X") { 
        std::cout << "B";
    }
    ~B() {
        std::cout << "~B";
    }
private:
    A m_x;
    A m_y;
};
```

```cpp
class A {
public:
    A(const char* s) : m_s(s) {
        std::cout << m_s;
    }
    ~A() { std::cout << "~" << m_s; }
private:
    const char* m_s;
};
```

* X Y B ~B ~Y ~X
* Poradie je rovnaké ako v súbore
* Najprv sa skonštruujú členy, potom sa zavolá samotný konštruktor, deštrukcia je potom v opačnom poradí

---

# Štandardná knižnica

---

# Prehľad

* Štandard má okolo 1500 strán a väčšina je venovaná opisu štandardnej knižnice
* Obsahuje všetko, čo C knižnica
* Vstup a výstup spolu s lokalizáciou
* Podpora vlákien a atomických premenných
* Matematické operácie
* Regulárne výrazy
* ...
* Kontainery a algoritmy

---

## STL

<div style="display: flex; align-items: center;">
<div style="flex: 1;">

* Standard Template Library
* **Alexander Stephanov** (1979)
* Formálny návrh na pridanie STL do C++ knižnice v roku 1994
* Generické programovanie bez najmenšej straty efektívnosti
</div>
<div style="flex: 1;">
<div style="width: 15vw; margin: auto;">
    <img src="./lectures/3_scope_class/Alexander_Stepanov.jpg" style="width: 100%; margin-bottom: 0;" />
    <div style="font-size: 1vh; text-align: right;">
        Source <a href="https://en.wikipedia.org/wiki/Alexander_Stepanov#/media/File:Alexander_Stepanov.jpg">Wiki</a>
    </div>
</div>
</div>
</div>

> STL ≠ C++ Standard Library

---

## Kontainery a algoritmy

Interátory poskytujú jednotný interface na prácu s kontainermi, ten potom algoritmy využívajú 
Ak máme n kontainerov a m algoritmov

O(n+m) vs O(nm)

---

## Kontainery

* vector<T>
* array<T, N>
* map<T>, set<T>
* multimap<T>
* list<T>, forward_list<T>
* unordered_map<T>
* string

---

# vector

---
