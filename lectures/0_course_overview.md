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

* Autori prednášok (a cvičení) nepoužívajú C++ na všetku svoju prácu
* Tiež si nemyslia, že C++ je najlepší programovací jazyk na svete 
* C++ je málokedy správna odpoveď na problém

---

## Programovanie v C++

* Hlavný cieľ je naučiť sa programovať v C++ s dôrazom na moderné a bezpečné konštrukcie
* Pokiaľ sa bude dať, budeme používať štandardnú knižnicu

<div style="display: flex; align-items: center;">
<div style="flex: 7;">

A Tour of C++  
Bjarne Stroustrup  
Addison-Wesley Professional; (September 24, 2022)  
ISBN-13: 978-0136816485  
<https://www.amazon.com/Tour-C-Bjarne-Stroustrup-dp-0136816487/dp/0136816487/>  
Časti prístupné online - <https://isocpp.org/tour>
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

* C++ v roku 2025
* Beautiful code
* Fun! (sort of 😀)

![Compile and works first time, what did I do wrong?](./lectures/0_course_overview/joke-sort-of.png)

---

# Organizácia štúdia

---

## Kontakt

<ul>
  <li>
    Kontaktná e-mail adresa je
    <p style="font-size: larger; text-align: center;">
      <a href="mailto:cpp@eset.sk">cpp@eset.sk</a>
    </p>
  </li>
  <li>
    Teams skupina predmetu
    <p style="font-size: larger; text-align: center;">
      <a href="https://teams.microsoft.com/l/team/19%3AHAzrmcaDkHF7Wn86KPePnZmlGIoPQHy4GucoeCtgmLc1%40thread.tacv2/conversations?groupId=4c104ceb-267c-4744-bb8f-0bd774673b03&tenantId=25733538-6b16-4aa3-8ed6-297eb79b8e06">FIIT APC_B</a>
    </p>
  </li>
</ul>

---

## Rozvrh

* Prednášky budú každý týždeň 
    * Utorok 16:00, trvanie cca 1,5 hodiny
    * Miestnosť -1.65 (Aula Minor) na FIIT


* Cvičenia budú každý týždeň
    * 1. skupina utorok 18:00
    * 2. skupina streda 12:00
    * 3. skupina streda 16:00
    * ESET Lab (miestnosť 4.46 na FIIT), maximálna kapacita +-16 ľudí 
    * Študenti FMFI majú zapísané cvičenia v utorok 18:00
    * Ak chcete zmeniť skupinu, dajte nám vedieť čím skôr
    * Prvá skupina sa pravdepodobne bude presúvať do 1.31a (uvidíte v rozvrhu, resp. dám vedieť na Teams)


**Cvičenia budú začínať až budúci týždeň, teda 30. septembra, tento týždeň nie sú**

---

## Dochádzka

* Prednášky aj cvičenia sú nepovinné, nebudeme kontrolovať dochádzku
* Na niektorých cvičeniach ale budú bodované úlohy a testy, tak tam odporúčame príjsť

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
* Žiadna časť predmetu nie je povinná, takže ak budete mať zo semestra viacej ako 55 bodov, na skúšku ani nemusíte chodiť a máte E

---

## Skuška

* Pozostáva z dvoch častí
   * Test na preskúšanie teórie (ako rozumiete C++)
   * Programovanie
* Na konci semestra bude "testovacia" skúška, aby ste si to mohli vyskúšať

* Teoreticky sa toto rozdelenie ešte môže trochu zmeniť

---

## Projekt

* Počas semestra budeme mať aj jeden väčší projekt
* Predstavený bude niekedy koncom októbra
* Bude sa dať získať 30 bodov, plus nejaký bonus

---

## Cvičenia

* Občas budú testy a nejaké bonusové úlohy (vždy dopredu oznámime)
* Teoretické testy 
   * 3 x 10 bodov
   * Otázky s možnosťami a krátke odpovede
   * Na papier
* Programovacie úlohy
   * Menšie programovacie úlohy, za pár bonusových bodov
   * Odovzdávanie bude cez náš portál
   * Na vlastných PC, alebo na tých čo sú v učebni
* Inak sa budeme venovať tomu čo sa prebralo na prednáške
* Zadanie a konzultácie projektu budú tiež na cvičeniach

---
 
# Prerekvizity

---

## Trochu skúseností s programovaním

<div style="display: flex; align-items: center;">
<div style="flex: 1;">
  
* Predpokladáme aspoň základnú znalosť programovania v C, alebo rovno C++
* Pravdepodobne sa dá predmet zvládnuť aj keď poznáte skôr iné jazyky
* Musíte ale poznať základné koncepty z programovania
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
* V podstate by ste mali byť schopný "čítať" program v C

---

## Platforma

* Kedže jedna z výhod C++ je prenositeľnosť kódu, budeme podporovať všetky rozšírené platformy
   * MS Windows a na ňom najnovšie Visual Studio 2022 (úplne stačí [Community edition](https://visualstudio.microsoft.com/vs/community/))
   * Linux a Mac budeme kompilovať cez `g++`, template pre Visual Studio Code
* Môžete používať aj iné IDE, resp. kompilátor, ale tieto dve riešenia budeme vedieť najlepšie podporiť
* C++23 kompatibilný kompilátor (možno pridáme aj trochu novšieho C++26)

---

## CMake

* Tento rok plánujeme používať CMake na správu projektu
* Bude relevantné hlavne pre projekt
* CMake nám pomôže zjednodušiť proces kompilácie a správy závislostí

---

## Algoritmy

* Na cvičeniach ani skúške nebudeme priamo od vás chcieť vymýšľať/študovať algoritmy (maximálne ako bonus), no veľmi odporúčam algoritmy dátové štruktúry poznať
* Ak ste také predmety nemali, tak odporúčam si ich dať
* Môžeme od vás ale chcieť niektoré algoritmy naimplementovať

---

<!-- .slide: data-background-image="./lectures/0_course_overview/DALL·E 2023-07-24 00.32.08 - elephant in the barely lit room.png" data-background-opacity="0.2" -->

<img class="fragment" src="./lectures/0_course_overview/ChatGPT_logo.svg" alt="Chat-GPT logo" width="400" />

---

## Chatboty, copiloty

* Všetky moderné LLM sú celkom schopní programátori v C++
* Problém je, že robia občas chyby a dosť často robia bezpečnostné chyby
* O to dôležitejšie je poznať C++, aby sme mohli kontrolovať vygenerovaný kód

---

## AI a tento predmet

* Na skúške bude zakázané používanie AI 
* Na ostatných aktivitách (projekt a iné úlohy) je používanie AI povolené

---

# ĎAKUJEM

## Otázky?
