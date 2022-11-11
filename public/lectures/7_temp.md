# C++

## Move sémantika

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Storage class specifiers
* Čo sú lvalue a rvalue?​
* Rvalue referencie
* Xvalues​
* `std::move​`
* RVO and NRVO​

---

# Storage class specifiers

---

## `auto`

* Klúčové slovo `auto` sa už v C++ nepoužíva na označenie *automatic storage duration*
* Bez označanenia premennej pomocou storage specifier sa použíje *auto*
* Miesto pre premennú je alokované na začiatku scope a dealokované na konci
* Konštruktory a deštruktory sa volajú *auto*maticky
* Miesto sa spravidla alokuje na stacku

---

## `register`

* Kľúčové slovo `register` sa používa na označenie premennej, ktorá sa bude veľa používať a teda kompilátor by ju mal dať prednostne do CPU registrov
* V C sa nedá takejto premennej zobrať adresa v C++ to ide
* Kompilátory ingnorujú, toto použitie je deprecated

```cpp
register int n = 0;
for (register int i = 0; i < 1'000'000; ++i) {
  n += i;
}
```

---

## `static`

* static premenné vo funkciách sú inicializované ak cez nich prejde beh programu (*control flow*)
* Sú potom deštruované po vrátení z `main` funkcie a opačnom poradí ako boli skonštruované
* Ak nejaká statická premenná nebola inicializovaná, tak nebude ani zdeštruovaná (toto je v povinnostiach kompilátora)


```cpp
struct A {
    A() { std::cout << "A"; }
    ~A() { std::cout << "~A"; }
};

void f(int i) {
    if (i == 0) {
        static A a;
    }
}

int main() {
    f(4); // nothing
    f(0); // A
    f(0); // nothing
    std::cout << "M";
} // ~A
```

Konštruktor sa zavolá iba ak sa funkcia `f` zavolá s parametrom `i == 0`. Deštruktor sa "zaregistruje" aby sa zavolal, keď program skončí. 

---

## `thread_local`

Premenná sa inicializuje iba raz pre konkrétny thread, o tomto uvidíme viacej na inej prednáške

---

## `dynamic`

* Dynamická pamäť znamená, že miesto si musíme alokovať ručne
* V C++ sa robí pomocou `new` a `delete`
* My ideálne nepoužívame priamo alokácie pamäte, ale
    * `std::unique_ptr`
    * `std::vector`
    * `std::string`

```cpp
int* i = new int(0);
// no exception can occur here
delete i;
```

---

# Value categories

---

## Globálne premenné

* Sú inicializované ešte pred volaním funkcie `main` and zdeštruované po vrátení z funkcie `main`
* V jednom translation unit je poradie inicializácie rovnaké ako sú napísané v súbore a deštrukcia potom v opačnom poradí


## Nedefinované poradie

```cpp
// main.cpp
#include "Version.h"
 
std::string g_Version = GetAppVersion();
 
int main(int argc, char* argv[]) {
    cout << g_Version;
}

```

```cpp
// Version.cpp
#include "Version.h"
 
uint32_t g_Major = GetMajorNumber();
uint32_t g_Minor = GetMinorNumber();
 
string GetAppVersion() {
    return std::to_string(g_Major) + "." + std::to_string(g_Minor);
}

```

Všetky globálne premenné sú inicializované pred volaním `main`. Tu jedna závisí od druhej v inom súbore (*translation unit*) a to je nedefinované správanie.

---

## Jednoduché globálne a statické premenné

* Kompilátor môže (a aj by mal) staticky (počas kompilácie) inicializovať globálne a statické premenné bez komplexnej inicializácie
* Pamäť pre globálne a statické premenné sa alokuje v dátovej sekcii binárky (*image*)

```cpp
int global_variable = 43;
 
int main(int argc, char* argv[]) {
    static char* static_string = "Static initialization";
 
    std::cout << global_variable;
}
```

---

## Disclaimer lvalue a rvalue

* Koncept lvalue a rvalue prešiel počas života C++ mnohými zmenami
* To čo budeme ďalej požívať nie je úplne presné, je to taký historický zjednodušený model
* Preto ak vám niečo nebude dávať dokonalý zmysel pýtajte sa, alebo konzultujte štandard

---

## lvalue

* V podstate všetko čo môže byť na lavej strane priradenia (l v názve je *left*)​
* Väčšinou pomenované premenné

<table style="width: 80%">
<tr>
<td>

```cpp
int i;
i = 3; 

std::string s;
s = "Lvalue"; 

std::vector<int> v;
v[0] = 0; 
```
</td>
<td>

```cpp
void f(std::string s) {
    s = "Still lvalue";
}

void f(std::string& s) {
    s = "Still...";
}

void f(const std::string& s) {
    // s = "We cannot change, but lvalue";
}
```
<td>
<tr>
<table>

* `T&` sa nazýva *lvalue reference*
* `const T&` sa nazýva *const lvalue reference*

---

## rvalue

* V podstate všetko čo môže byť na pravej strane priradenia (r v názve je *right*)​
* Väčšinou premenné bez mena, alebo *temporaries*

<table style="width: 80%">
<tr>
<td>

```cpp
int i;
i = 3; // lvalue OK

int i;
3 = i; // 3 is rvalue
// error C2106: '=': left operand must be l-value
```
</td>
<td>

```cpp
char *c;
c = "String literal";

char *c;
"String literal" = c;

```
<td>
<tr>
<table>

---

## Dočasné premenné

* *Temporaries*
* Všetky premenné na predchádzajúcich slidoch mali mená
* Pomenované premenné zostávajú žiť až po koniec scope-u, v ktorom boli zkonštruované
* Nepomenované premenné (dočasné) sú zdeštruované hneď po tom čo boli použité (na tom istom riadku)

```cpp
int main() {
    std::string("My temporary string");
 
    cout << std::string("File ") + __FILE__ << endl;
}
```


## rvalue a temporaries

* Temporaries sú vo všeobecnosti `rvalue`
* Do dočasnej premennej nemá zmysel priraďovať, lebo jej hodnotu aj tak nebudeme môcť spracovať

```cpp
int() = 4; // error
```

```cpp
std::string s = "lvalue"; 
std::string() = s; // ???
```

<div class="fragment">

String je zaujímavý. Ako objekt má preťažený operátor priradenia. Preto sa nepoužije zabudovaný operátor priradenia, ale funkcia `std::string& operator=(const std::string&)`. Preto sa kód skompiluje, kompilátor nemôže vedieť, že operátor priradenia nemá žiadne vedlajšie účinky. V tomto prípade môže teoreticky vyhodiť výnimku. 
</div>

---

## Adresa v pamäti

* Iný spôsob ako sa pozrieť na lvalue/rvalue je pomocou operátora &
* Objekt je lvalue ak sa dá zobrať jeho adresa 

```cpp
std::string s = "lvalue"; 
&s; // ok
 
&std::string(); //error
```

<small>V starších MSVC (kompilátor vo Visual Studio) toto išlo skompilovať. Od VS2019 to už nejde.</small>