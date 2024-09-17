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

* C++ má svoje muchy, ale stále sa vo veľkom používa
* Na týchto prednáškach sa pokúsime ukázať, že aj v C++ sa dá programovať moderne
    * Bez skrytých bezpečnostných problémov
    * Bez množstva boilerplate
    * Bez manuálnej správy pamäte
    * Pomocou moderných princípov
* Zameriame sa na implementácie riešení problémov v C++, nie na návrh riešení
* Chceme dosiahnuť aby programovanie v C++ nevyzeralo ako boj s kompilátorom a hodiny strávene v debuggeri

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

* Stránka predmetu <https://cppseminar.eset.sk>
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
    * C++ Now <https://www.youtube.com/@BoostCon/videos>
    * Meeting C++ <https://www.youtube.com/@MeetingCPP/videos>
* Ostatné
    * STL intro <https://learn.microsoft.com/en-us/shows/c9-lectures-stephan-t-lavavej-standard-template-library-stl-/>


### Prednášky

* Dostupné online <https://cppseminar.github.io/apc-lectures/> ([sources](https://github.com/cppseminar/apc-lectures))
* Pár extra textov <https://cppseminar.github.io/>

---

## Výsledok nášho snaženia

* C++ v roku 2024
* Beautiful code
* Fun! (sort of 😀)

![Compile and works first time, what did I do wrong?](./lectures/1_intro/joke-sort-of.png)

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
    <a href="https://cppseminar.eset.sk">Stránka predmetu</a>
    <ul>
      <li>Oznamy</li>
      <li>Užitočné linky</li>
      <li>Úlohy</li>
    </ul>
  </li>
</ul>

---

## Rozvrh

* Prednášky budú každý týždeň 
    * Utorok 16:00, trvanie 1,5 hod
    * Miestnosť -1.65 (Aula Minor) na FIIT
* Cvičenia budú každý týždeň
    * 1. skupina utorok 18:00
    * 2. skupina streda 16:00
    * 3. skupina streda 18:00
    * ESET Lab (miestnosť 4.46 na FIIT), maximálna kapacita +-16 ľudí 
    * Prvý krát môžete prísť na ľubovoľné cvičenie, ak bude problém s kapacitou nejak to vyriešime

---

## Dochádzka

* Prednášky aj cvičenia sú nepovinné, nebudeme kontrolovať dochádzku
* Na niektorých cvičeniach ale budú bodované úlohy a testy, tak tam odporúčame príjsť

![Travolta looking very confused](./lectures/1_intro/travolta.gif)

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
* Bude sa dať získať 20 bodov, plus nejaký bonus

---

## Cvičenia

* Občas budú úlohy a testy (vždy dopredu oznámime)
* Teoretické testy 
   * 2 x 10 bodov
   * Desať otázok so 4 možnosťami
   * Na papier
* Praktické úlohy
   * 2 x 10 bodov
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
* Letmo sa týchto tém dotkneme na prvých prednáškach, ale určite sa im nebude venovať do hĺbky
* V podstate by ste mali byť schopný "čítať" program v C

---

## Platforma

* Budeme používať MS Windows a na ňom najnovšie Visual Studio 2022 (úplne stačí [Community edition](https://visualstudio.microsoft.com/vs/community/))
* Všetko by malo fungovať aj na iných platformách (Linux, Mac)
* C++20 kompatibilný kompilátor (možno pridáme aj trochu C++23)
* Pre VS2022 máme pripravený aj template, ktorý všetko nastaví ako má
* Niečo existuje aj pre VS Code

---

## Algoritmy

* Na cvičeniach ani skúške nebudeme priamo od vás chcieť vymýšľať/študovať algoritmy (maximálne ako bonus), no veľmi odporúčam algoritmy dátové štruktúry poznať
* Ak ste také predmety nemali, tak odporúčam si ich dať
* Môžeme od vás ale chcieť niektoré algoritmy naimplementovať

---

<!-- .slide: data-background-image="./lectures/1_intro/DALL·E 2023-07-24 00.32.08 - elephant in the barely lit room.png" data-background-opacity="0.2" -->

<img class="fragment" src="./lectures/1_intro/ChatGPT_logo.svg" alt="Chat-GPT logo" width="400" />

---

## Chatboty, copiloty

* Všetky moderné LLM sú celkom schopní programátori v C++
* Problém je, že robia občas chyby a dosť často robia bezpečnostné chyby
* O to dôležitejšie je poznať C++, aby sme mohli kontrolovať vygenerovaný kód

---

## AI a tento predmet

* Na skúške a cvičeniach je používanie AI zakázané
    * Keďže tieto aktivity sú pomerne krátke, veľmi ťažko sa hľadá rovnováha medzi rozsahom a komplexnosťou
    * Ideálne by sme chceli vedieť či C++ rozumiete
* Na aktivitách mimo školy (projekt a iné úlohy) je požívanie AI povolené

---

# ĎAKUJEM

## Otázky?
