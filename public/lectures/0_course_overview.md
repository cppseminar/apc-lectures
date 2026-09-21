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

* C++ sa nehodí na všetku prácu, v minulosti sa ale často používal, takže občas je aj tam, kde by nemusel (UI, web, scripting, ...)
* C++ je nie ani zďaleka najlepší programovací jazyk na svete (taký ani neexistuje)
* C++ je málokedy správna odpoveď na problém, ale pri špecifických problémoch môže byť veľmi vhodný (real-time systems)

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
    * Miestnosť 1.40 (U40) na FIIT


* Cvičenia budú každý týždeň
    * 1. skupina utorok 14:00 (pred prednáškou)
    * 2. skupina utorok 18:00
    * 3. skupina streda 16:00
    * ESET Lab (miestnosť 4.46 na FIIT), maximálna kapacita +-16 ľudí 
    * Študenti FMFI môžu chodiť na cvičenia podľa vlastného výberu (pošlite mail, ktoré ste si vybrali)
    * Ak chcete zmeniť skupinu, dajte nám vedieť čím skôr

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

---

## Projekt

* Historicky sme skúšali zaradiť aj jeden väčší projekt, ktorý študenti riešili počas semestra
* Tento rok nebude, keďže sa z neho stal taký "pay to win" a nevieme ako túto vlastnosť odstrániť

---

## Cvičenia

* Občas budú testy a nejaké bonusové úlohy (vždy dopredu oznámime)
* Teoretické testy 
   * 3 x 10 bodov
   * Otázky s možnosťami a krátke odpovede
   * Na papier
* Programovacie úlohy
   * 3 x 10 bodov
   * Programovanie "na papier"
   * Naprogramovanať jednoduchú úloh na papier, kde má byť plus mínus dobre syntax a logika
   * Samozrejme za drobné typo a jedny chýbajúce zátvorky sa body nestrácajú, ide hlavne o pochopenie logiky a akej takej syntaxe C++
* Inak sa budeme venovať tomu čo sa prebralo na prednáške


## Bonusové úlohy

* Budú sa objavovať počas semestra
* Buď priamo programovanie na cvičeniach, alebo úlohy na doma
* Bodovanie bude 1-2 body, dokopy možno 6 bodov za celý semester

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
* Máme aj pripravený github codespace

---

## CMake

* Tento rok plánujeme používať CMake
* CMake nám pomôže zjednodušiť proces kompilácie a správy závislostí
* Aby sme mali jednotné prostredie a nestalo sa nám, že niekomu niečo nefunguje a ostatným to ide

---

## Algoritmy

* Na cvičeniach ani skúške nebudeme priamo od vás chcieť vymýšľať/študovať algoritmy (maximálne ako bonus), no veľmi odporúčam algoritmy dátové štruktúry poznať
* Úlohy budú mať dosť triviálne algoritmické riešenie, vaša úloha ho bude iba nepokaziť, alokáciami a prílišným kopírovaním pamäte.
* Vždy sa budeme prikláňať k jednoduchým a efektívnym riešeniam

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

* Na predmete je AI zakázané
* Skúšali sme používanie AI na predmete a zistili sme, že to vedie k problémom s porozumením a bezpečnosťou kódu
* Navyše je veľká medzera medzi modelmi zadarmo a frontier modelmi, do vedie k tomu, že sa dajú "kúpiť" body na úkor porozumenia.
* Ak má tento predmet niečo priniesť, tak musíme porozumieť C++ sami, inak nebudeme vedieť odhaliť chyby, ktoré môžu vzniknúť pri používaní AI.


* Od AI určite neodradzujeme, v budúcnosti programovania má určite svoje nespochybniteľné miesto.
* Problém je, ak vlastne nemáme čo priniesť sami, tak nás to AI nahradí veľmi rýchlo. 

---

# ĎAKUJEM

## Otázky?
