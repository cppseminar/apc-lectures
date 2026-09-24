# C++

## Úvod do predmetu

*Peter Koscelanský <cpp@eset.sk>* <!-- .element: class="author" -->

---

## Obsah

* Ciele predmetu
* Organizácia štúdia
* Prerekvizity

---

# Ciele predmetu

---

## Prečo práve tento predmet?

* Hoci má C++ svoje nedostatky, stále patrí medzi široko používané jazyky
* Na prednáškach ukážeme, že aj v C++ je možné programovať moderne:
  * Bez skrytých bezpečnostných rizík
  * Bez nadbytočného kódu (boilerplate)
  * Bez manuálneho spravovania pamäte
  * S využitím moderných princípov a postupov
* Zameriame sa na implementáciu riešení v C++, nie na samotný návrh riešení
* Naším cieľom je ukázať, že programovanie v C++ nemusí byť boj s kompilátorom ani hodiny trápenia v debuggeri

---

## Disclaimer

* C++ sa nehodí na všetku prácu, v minulosti sa však často používal, takže občas je aj tam, kde by nemusel byť (UI, web, scripting, ...)
* C++ nie je ani zďaleka najlepší programovací jazyk na svete (taký ani neexistuje)
* C++ je málokedy správna odpoveď na problém, ale pri špecifických problémoch môže byť veľmi vhodný (real-time systems)

---

## Programovanie v C++

* Hlavným cieľom je naučiť sa programovať v C++ s dôrazom na moderné a bezpečné konštrukcie
* Pokiaľ sa bude dať, budeme používať štandardnú knižnicu

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

A Tour of C++  
Bjarne Stroustrup  
Addison-Wesley Professional; (September 24, 2022)  
ISBN-13: 978-0136816485  
<https://www.amazon.com/Tour-C-Bjarne-Stroustrup-dp-0136816487/dp/0136816487/>  
Časti prístupné online: <https://isocpp.org/tour>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/Tour3English-large.jpg" alt="A Tour of C++ (3rd edition)" style="width: 70%;" />
</div>
</div>

---

## Literatúra

* [cppreference](https://en.cppreference.com/w/)
* [CppCoreGuidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
* [C++ Standard Draft](https://eel.is/c++draft/)
* ["Domovská" stránka C++](https://isocpp.org/)


<div style="display: flex; align-items: center;">
<div style="flex: 7;">

C++17 In Detail  
Bartłomiej Filipek  
Independently published (July 18, 2019)  
ISBN-13: 978-1798834060  
<https://www.amazon.com/17-Detail-Exciting-Features-Standard/dp/1798834065>  
<https://www.cppindetail.com/>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/cpp17indetail.png" alt="C++17 In detail" style="width: 70%;" />
</div>
</div>


<div style="display: flex; align-items: center;">
<div style="flex: 7;">

C++20 - The Complete Guide  
Nicolai M. Josuttis  
Independently published (November 7, 2022)  
ISBN-13:  978-3967309201   
<https://www.amazon.de/-/en/Nicolai-M-Josuttis/dp/3967309207/>  
<https://cppstd20.com/>
</div>
<div style="flex: 3;">
  <img src="./lectures/1_intro/covercpp20opt255x317.png" alt="C++20 - The Complete Guide" style="width: 70%;" />
</div>
</div>


### Videá

* Konferencie
  * CppCon <https://www.youtube.com/user/cppcon>
  * C++ Now <https://www.youtube.com/@CppNow>
  * Meeting C++ <https://www.youtube.com/@MeetingCPP/videos>
* Ostatné
  * STL intro <https://learn.microsoft.com/en-us/shows/c9-lectures-stephan-t-lavavej-standard-template-library-stl-/>


### Prednášky

* Dostupné online <https://cppseminar.github.io/apc-lectures/> ([sources](https://github.com/cppseminar/apc-lectures))
* Pár extra textov <https://cppseminar.github.io/>

---

## Výsledok nášho snaženia

* C++ v roku 2026
* Beautiful code
* Fun! (sort of 😀)

![Compile and works first time, what did I do wrong?](./lectures/0_course_overview/joke-sort-of.png)

---

# Organizácia štúdia

---

## Kontakt

<ul>
  <li>
    Kontaktná e-mailová adresa je
    <p style="font-size: larger; text-align: center;">
      <a href="mailto:cpp@eset.sk">cpp@eset.sk</a>
    </p>
  </li>
  <li>
    Teams skupina predmetu
    <p style="font-size: larger; text-align: center;">
      <a href="https://teams.microsoft.com/l/team/19%3AQcm-zZr1HHRQDqjdeLxMtU3m7fnfZDDqIs5tqI0CcdU1%40thread.tacv2/conversations?groupId=efe9b24e-c145-4613-8de5-fc108b0c3539&tenantId=25733538-6b16-4aa3-8ed6-297eb79b8e06">FIIT APC_B</a>
    </p>
  </li>
</ul>

---

## Rozvrh

* Prednášky budú každý týždeň 
  * Utorok 16:00, trvanie cca 1,5 hodiny
  * Miestnosť 1.40 (U40) na FIIT


* Cvičenia budú každý týždeň
  * 1. skupina utorok 14:00 (pred prednáškou)
  * 2. skupina utorok 18:00
  * 3. skupina streda 16:00
  * ESET Lab (miestnosť 4.46 na FIIT), maximálna kapacita približne 16 ľudí
  * Študenti FMFI môžu chodiť na cvičenia podľa vlastného výberu (pošlite nám e-mail, ktorú skupinu ste si vybrali)
  * Ak chcete zmeniť skupinu, dajte nám vedieť čo najskôr

---

## Dochádzka

* Prednášky aj cvičenia sú nepovinné, nebudeme kontrolovať dochádzku
* Na niektorých cvičeniach však budú bodované úlohy a testy, preto na ne odporúčame prísť

![Travolta looking very confused](./lectures/0_course_overview/travolta.gif)

---

## Hodnotenie

* Rozdelenie hodnotenia
  * Počas semestra sa bude dať získať 60 bodov
  * Na skúške potom zvyšných 40 bodov
* Známkovanie
  * A (100-92)
  * B (91-83)
  * C (82-74)
  * D (73-65)
  * E (64-56)
* Žiadna časť predmetu nie je povinná, takže ak budete mať zo semestra viac ako 55 bodov, na skúšku ani nemusíte chodiť a máte E

---

## Skúška

* Pozostáva z dvoch častí
  * Test z teórie (ako rozumiete C++)
  * Programovanie
* Na konci semestra bude "testovacia" skúška, aby ste si to mohli vyskúšať

---

## Projekt

* Historicky sme skúšali zaradiť aj jeden väčší projekt, ktorý študenti riešili počas semestra
* Tento rok nebude, keďže sa z neho stal taký „pay to win“ a nevieme, ako túto vlastnosť odstrániť

---

## Cvičenia

* Občas budú testy a nejaké bonusové úlohy (vždy dopredu oznámime)
* Teoretické testy 
  * 3 × 10 bodov
  * Otázky s možnosťami a krátke odpovede
  * Na papieri
* Programovacie úlohy
  * 3 × 10 bodov
  * Programovanie „na papieri“
  * Naprogramovať jednoduchú úlohu na papieri, pričom syntax a logika majú byť viac-menej správne
  * Samozrejme, za drobné preklepy a niekoľko chýbajúcich zátvoriek sa body nestrácajú, ide hlavne o pochopenie logiky a akej-takej syntaxe C++
* Inak sa budeme venovať tomu, čo sa prebralo na prednáške


## Bonusové úlohy

* Budú sa objavovať počas semestra
* Buď priamo programovanie na cvičeniach, alebo úlohy na doma
* Bodovanie bude 1 – 2 body, dokopy možno 6 bodov za celý semester

---
 
# Prerekvizity

---

## Trochu skúseností s programovaním

<div style="display: flex; align-items: center;">
<div style="flex: 1;">
  
* Predpokladáme aspoň základnú znalosť programovania v C alebo rovno v C++
* Pravdepodobne sa dá predmet zvládnuť, aj keď poznáte skôr iné jazyky
* Musíte však poznať základné koncepty programovania
</div>
  <div style="flex: 1;">
    <img src="./lectures/1_intro/code-works.png" alt="My code doesn't work I have no idea why" />
  </div>
</div>

---

## Čomu by ste mali teoreticky rozumieť

* Existujú nejaké *typy* – `int`, `char`, `string`
* Ako funguje *control flow* – `if`, `for`, `while`
* *Funkcie* a ich volanie, *rekurzia*
* Letmo sa týchto tém dotkneme na nasledujúcej prednáške, ale určite sa im nebude venovať do hĺbky
* V podstate by ste mali byť schopní „čítať“ program v C

---

## Platforma

* Keďže jednou z výhod C++ je prenositeľnosť kódu, budeme podporovať všetky rozšírené platformy
  * MS Windows a na ňom najnovšie Visual Studio 2022 (úplne stačí [Community edition](https://visualstudio.microsoft.com/vs/community/))
  * Na Linuxe a Macu budeme kompilovať pomocou `g++`, pripravíme šablónu pre Visual Studio Code
* Môžete používať aj iné IDE, resp. kompilátor, ale tieto dve riešenia budeme vedieť najlepšie podporiť
* Kompilátor kompatibilný s C++23 (možno pridáme aj niečo z novšieho C++26)
* Máme pripravený aj GitHub Codespace

---

## CMake

* Tento rok plánujeme používať CMake
* CMake nám pomôže zjednodušiť proces kompilácie a správy závislostí
* Cieľom je mať jednotné prostredie a predísť situácii, keď niekomu niečo nefunguje, zatiaľ čo ostatným áno

---

## Algoritmy

* Na cvičeniach ani na skúške nebudeme priamo od vás chcieť vymýšľať ani študovať algoritmy (maximálne ako bonus), no veľmi odporúčame poznať algoritmy a dátové štruktúry
* Úlohy budú mať pomerne triviálne algoritmické riešenie, vašou úlohou bude iba nepokaziť ho zbytočnými alokáciami a nadmerným kopírovaním dát
* Vždy sa budeme prikláňať k jednoduchým a efektívnym riešeniam

---

<!-- .slide: data-background-image="./lectures/0_course_overview/DALL·E 2023-07-24 00.32.08 - elephant in the barely lit room.png" data-background-opacity="0.2" -->

<img class="fragment" src="./lectures/0_course_overview/ChatGPT_logo.svg" alt="Chat-GPT logo" width="400" />

---

## Chatboty, copiloty

* Všetky moderné LLM sú celkom schopné programovať v C++
* Problém je, že občas robia chyby, pričom pomerne často ide o bezpečnostné chyby
* O to dôležitejšie je poznať C++, aby sme mohli kontrolovať vygenerovaný kód

---

## AI a tento predmet

* Používanie AI je na predmete zakázané
* Skúšali sme používanie AI na predmete a zistili sme, že to vedie k problémom s porozumením a bezpečnosťou kódu
* Navyše je veľká medzera medzi modelmi zadarmo a frontier modelmi, čo vedie k tomu, že sa dajú „kúpiť“ body na úkor porozumenia
* Ak má tento predmet niečo priniesť, musíme porozumieť C++ sami, inak nebudeme vedieť odhaliť chyby, ktoré môžu vzniknúť pri používaní AI


* Od používania AI určite neodrádzame; v budúcnosti programovania má nespochybniteľné miesto
* Problém nastáva, ak sami nemáme čo priniesť; vtedy nás AI veľmi rýchlo nahradí

---

## Náhodné číslo LLM

> C99, máme len funkciu rand, ako dosiahnuť, aby sme dostali číslo od 0 do 99 vrátane.


### Výsledok

```c [|5]
// Inicializácia generátora náhodných čísel
srand(time(NULL)); // Inicializácia so súčasným časom

// Generovanie náhodného čísla v rozsahu 0 až 99
int random_number = rand() % 100; // Získa číslo od 0 do 99

// Výpis náhodného čísla
printf("Náhodné číslo: %d\n", random_number);
```


### ...pokračovanie konverzácie

> Je to bezpečné? 


### Výsledok

```c [|7]
srand(time(NULL));

// Generate random number until it falls within the desired range
int random_number;
do {
    random_number = rand();
} while (random_number >= RAND_MAX - (RAND_MAX % 100));

random_number %= 100;

printf("Random number: %d\n", random_number);
```

note: RAND_MAX môže byť teoreticky 32 alebo podobné malé číslo.


### To isté v C++

> C++20, máme len štandardnú knižnicu, ako dosiahnuť, aby sme dostali náhodné číslo od 0 do 99 vrátane.

```cpp
// Inicializácia generátora náhodných čísel
std::random_device rd;  // Získanie náhodného zariadenia
std::mt19937 gen(rd());  // Inicializácia generátora Mersenne Twister
std::uniform_int_distribution<> dis(0, 99);  // Rozdelenie na celé čísla od 0 do 99

// Generovanie náhodného čísla
int random_number = dis(gen);
```

---

# ĎAKUJEM

## Otázky?
