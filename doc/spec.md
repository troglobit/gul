<!-- 
  The complete source specification for the GUL Editor

  Revision history
  1998-12-20: LaTeX formatting done by Joachim Nilsson.
  1999-01-19: Some small fixing up and additions made /Jocke
  1999-06-08: Small fixes and additions. /Jocke
  2015-09-05: Convert to Markdown format

-->

Specifikation Editorprojekt - GUL Editor
========================================

1998-10-15  
Joachim Nilsson

Projekt för att vidga vyer flytta berg och skriva den ultimata editorn.  
Med lite tur så lär vi oss lite oxå på vägen dit. :)


Funktionell beskrivning
-----------------------

När vi för många månar sedan med oförstörda sinnen, då alla samlade i
den perfekta enheten 'Campus', djärvt och med en dåres drömmar och
visioner gav oss in i detta projekt så ställde vi upp vissa mål. Detta
dokument är ett försök att sammanfatta dessa på ett så klart och tydligt
sätt som möjligt ...

En varning kanske är på sin plats: Upprepningar och otydligheter är
sällan ovanliga i texten nedan. Den är dessutom ofantligt lååång redan.
Så minns denna varning när ni mot slutet av dagen utsvultna flämtandes
tittar upp från skärmen med ett plågat uttryck i ansiktet. *he he*


### Krav på design
Här följer nu en sammanfattning av punkter som vi talat om. Vissa saker
är vi rörande överens om andra vet vi inte vad vi tycker, utom att det
låter ballt och borde vara med...

- Själva editorkärnan skall vara liten.
- Snabb och resurssnål (i sin enklaste/renaste form).
- Mer lik typ Notepad än VI (för det finns redan så många vi-
  kloner). Dvs inga separata kommando/editerings-lägen.
- Modulariserad av bara DEN. Ska tex klara att mappa olika
  tangentkombinationer till olika keysets precis som att man
  enkelt skall kunna kompilera om med en annan display-modul
  och vips kunna ha en editor för X (GTK+?!) likväl som man
  skall kunna köra den på sin favoritterminal (Wyse 50 ;).
- För snabba sökningar har Jakob tänkt ut att använda hashing.
  (Endast han själv verkar veta hur detta skall funka dock, för
  inget mer har hörts om detta... :) `Jakob skall fylla i här!`
- Recovery-filer! Hur? Tja, svår och intressant++ fråga!
- Swapfiler? Man kan ju bara inte he en hel fil (stor) i minnet
  utan är ju eg. piskad att ha en swap, kanske i kobination med
  den ovan nämda recovery-filen?!
- Unicode-support?
- Jag vill ha stöd för folding ... /Crash


### Funktionssammanfattning
Fritt tagen ur tidigare maildiskussioner. Andra idéer om hur upplägget
av sammanfattningen bör se ut välkomnas som vanligt.

Sammanfattningen ursprungligen gjord av Jakob 23/10 -98 med kommentarer
och tillägg av Jocke.

    Jakob <jakov@quicknet.se> Fri Oct 23 00:22:16 1998
	> Plattformsoberoende.
	>    "Wrap"-funktioner eller "Wrap"-defines till utskrift på
	>    skärmen.
	>    Tillägg av mig [Jakob]: Liknande tänkade vid access mot
	>                            filsystemet.
	
	Wrappers som gör att hur än filsystemet fungerar (alla sjuka
	och konstiga fall!) så är editorn oberoende av detta.
	
	> > Liten binär.
	>    Mindre än pico.
	>    pico är ganska stor, 270 kb på legolas
	>    och drygt 100kb på min linux. (Med delade bibliotek.)
	>    Jag tycker vår design ska vara mycket liten.
	
	Vi skulle visst satsa på att försöka komma under 150kB, vilket
	jag personligen inte tror är något som helst problem! :-)
	
	> Multilevel undo. (Som mål iaf.)
	
	Är eg. oxå avklarat (i teorin) redan. Se vidare nedan.
	
	> Effektiv och snabb minneshantering.
	>    Någon annan lösning än vanlig GAP och LL.
	
	För förslag, se "Implementationsbeskrivning".
	
	> Förslag från min sida angående stubbar:
	>    Se om vi inte kan utnyttja #ifdef och #define i rätt hög
	>    grad. På så vis kan binären hållas liten.

Vi var även inne på konceptet "tempfiler" Vilket fortfarande nog är en
rätt intressant diskussion. För som Anders mycket listigt påpekar i ett
brev, så använder både VI och EMACS denna lösning och kan däremd även
skryta med "recovery"-funktioner.


### Namnkonventioner
Filnamn och standard för `#defines` och variabelnamn.

När vi diskuterade detta tidigare så var vi alla så rörande
överens, men en liten rekapitulation av vad som sagts kanske
är på sin plats:

Sammanfattning av Anders 11/10 -98 med kommentarer av Jocke.

    Anders Bornäs <mail_boran@yahoo.com> Fri, 11 Sep 1998
	> gulMain.c gulMain.h  <- typ main eller så
	> mem.c mem.h          <- utbytbar minneshantering
	
	memory.*
	
	> io.c io.h            <- nja, filhantering egentligen
	
	vfs.*                  <- Virtual file system == Filhantering?
	io.*                   <- Input==Keyboard, Output==Screen?
	
	> disp.c disp.h        <- skärmutskrifter
	
	display.*
	
	> misc.h               <- en favorit jag _måste_ ha!
	>                         (TRUE, FALSE, RANDOMVAL mm)

Variabelnamn har jag för mig att vi istf. att använda underscore
så skulle vi hålla oss till något i den här stilen:

    filename/fileName, thisNode, previousNode, nodep, nodepp osv.

Där `p` och `pp` används till att tala om att det är pekare
resp. dubbelpekare.  Givetvis så är det ju inte hela världen om man
använder underscore då och då.

`#define`'s skall givetvis vara VERSALER! :)


### Kort lista över använda begrepp
Boran började ju tuta ihop en lista av tuffa ord som han inte förstod,
när vi andra för länge sedan givit upp hoppet om att nånsin förstå vad
det var vi pratade om ... Den här listan *bör* kompletteras.

| **Begrepp**     | **Förklaring ** |
| GAPS(I)Z(E)     | Konstant betecknande storleken på buffergapet. |
| Blockmarking     | Möjligheten att kunna markera text i rektangulära block. (Se även Appendix A) |
| linescroll(ing)  | Dvs inte wrappat som i emacs/vi. (Se även \texttt{M-x hscroll-mode} i Emacs.) |
| ll               | Linked list, se editor.101.txt |
| buffer gap       | Se filen buffer-gap.txt i distributionen |
| Folding          | Tänk dig att kunna markera rader, tex alla kommentarer, och sedan kunna välja ``Fold out'' i editorn för att dessa sedan skall "försvinna" eller snarare vikas undan, därav namnet. |
| Virtual keys     | Möjlighet att ha olika "keymaps". Dvs att kunna binda tangentkombinationer till olika funktioner i editorn. |
| Key binding      | Se: Virtual keys. |
| GULPL            | GUL Public License |
| KAFFE            | Fullkomligt nödvändigt för att kunna leva. |


Timeline - Även känd som "Fina listan" :)
-----------------------------------------
Här följer nu en liten hur-vi-skulle-kunna-gå-tillväga-lista väldigt fint
sammanfattad av Anders!

**Den Fina Listan**
- printa text
- läsa in fil till allokerat minne
- markörförflyttning (simpel)
- infoga ta bort
- spara fil
- ordna bra (nåja) tangentmappningar
- snygga till minneshanteringen
- rätta till presentationen
- utöka funtionaliteten
- goto förra punkten
- ... i stort sett.

Efter den fina listan kan jag nu stolt säga att 1--4 är "klart".  En fil
går att läsa in från prompten, in i strukturen av länkade rader (se
vidare nedan för beskrivning av *exakt* hur det fungerar).  Där man
sedan tack vare curses både kan se början av filen (första skärmsidan),
flytta runt en markör och ta bort och infoga tecken. :-D

Se vidare filen TODO i distributionen. (Det är *så* koolt att kunna säga
just "distributionen" om det här lilla hacket! ;)


Implementationsbeskrivning
--------------------------

![Overview](overview.eps "Översikt av nuvarande design")

### Dubbellänkad lista av rader
*Kort beskrivning:* Enl. den funktionella beskrivningen ovan så byggs
filen upp av en datastruktur av dubbellänkade rader. Där varje rad inte
nödvändigtvis måste vara just en fysisk rad i filen utan är ett block om
256 bytes någonstans ur filen.

Den mycket bisarra konstruktionen nedan enl.

    3 * (2 * sizeof(struct dllSt *) + 2 * sizeof(int))

till för att på ett snyggt sätt aligna hela *dllSt* till en jämn
tvåpotens.  I det här fallet 64 bytes (om pekare och *int* x är fyra
bytes).

    typedef struct dllSt{
	    struct dllSt  *previous;
		struct dllSt  *next;
		int           begin;
		int           end;
		char          line[3 * (2 * sizeof(struct dllSt *) +
                           2 * sizeof(int))];
	}text;
	
    #define MAX_LINE_LENGTH 3 * (2 * sizeof(struct dllSt *) + \
                                 2 * sizeof(int))

Detta upplägg har valts pga främst två orsaker:
- Enkel idé, lätt att implementera.
- Ger små datablock som är enklare/snabbare att hantera.
- Snygg alignment till jämna block om 16 bytes (notera att NFS alltid
  cache:ar block om 16 bytes, vad nu det kan vara bra till?)

### Buffer-Gap
Efter att ha lagt sisådär ett par-tre månader på att försöka få till
implementationen av DLL, ovan, så satte sig Jocke ner och tog tag
Buffer-Gap-metoden.  Det var dags att leka med de stora pojkarna!

Det visade sig att Buffer-Gap faktiskt var väldigt enkel.  Buffern som
var så omtalad var bara en minnesarea med hela filinnehållet i och det
bisarra gapet var bara markören som man lät glida omkring i buffern.

    |<----- first half -----><----- gap -----><------ second half ------>|

När man vill flytta markören åt höger så kopierar man teckent till
höger om gapet till gapets position noll och flyttar sedan gapet ett
steg till höger:

    thisp->gap[0] = thisp->gap[thisp->gap_size];
    thisp->gap++;
	
    Efter att ha lagt sisådär ett par-tre månader på att försöka få till
	implementationen av DLL, ovan, [<---G-A-P--->]så satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!
	
	Efter att ha lagt sisådär ett par-tre månader på att försöka få till
	implementationen av DLL, ovan, s[<---G-A-P--->]å satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!
	
	Efter att ha lagt sisådär ett par-tre månader på att försöka få till
	implementationen av DLL, ovan, så[<---G-A-P--->] satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!

Givetvis först med de normala randvärdeskontrollerna. För att flytta
åt vänster är det ungefär likadant:

    thisp->gap--;
    thisp->gap[thisp->gap_size] = thisp->gap[0];

När man vill lägga till tecken till buffern så skriver man de till
gapets position noll, flyttar gapet ett steg till höger och minskar
sedan storleken med ett.

    thisp->gap[0] = character;
    thisp->gap++;
    thisp->gap_size--;

Övriga förflyttningskommandon och kommandon för att radera tecken till
vänster och höger om gapet är bara variationer på ovanstående.


### Inladdningen av filen

Allting börjas med att man tex läser in en fil, vilket nog ändå är det
garanterat vanligaste man gör ... det är ju sällan man skapar en ny fil
och skriver och skriver och helt plötsligt är allt klart, färdigt.

Förvisso finns det ju de som vill öppna filen ifråga från editorn
... men i dagens drag'n'drop-värld så är dessa puckon en
utrotningshotatd art. ;)


#### DLL -- Konvertering till länkade rader

Vid inladdningen sker följande:

- Fåniga kontroller om filen finns och annat tjafs.
- Allokera nod.
- Fyll noden mha fread (använd `MAX_LINE_LENGTH`).
- Sätt begin till noll och end till det fread returnerar.
- Upprepa 2-4 tills fread returnerar `EOF` eller `fread()`
  returnerar något annat än `MAX_LINE_LENGTH`.

Efter inladdningen så blir fönster/utskrivnings-servern anmodad
att visa den inladdade texten.


#### Buffer-Gap -- Ladda fil till Buffer

Under Buffer-Gap är det extremt lätt att ladda in filen eftersom
allt man behöver göra är:

- Ta reda på filstorleken.
- Allokera tillräckligt stor buffer plus `GAP_SIZE` bytes.
- Sätt gap att peka på buffer
- Läs in allt data från filen till `buffer[GAP_SIZE]`
- Markören (Gap) står nu på första tecknet i filen.
- Klart.


### Förflyttning
All förflyttning sker precis som man är van vid, eller som man
har valt att själv mappa funktionerna för förflyttning.

För varje förflyttningskommando som kommer in till händelsehanteraren
(`while(1)`-loopen) så skickas det vidare till
fönster/utskrivnings-servern som flyttar markören och/eller uppdaterar
texten på skärmen.

Fönster/utskrivnings-servern har hand om allt vad fönsterstorlek, wrap,
och annat larv. Den håller även reda på var i filen/buffern man befinner
sig. Tex: `Col=45, Row=16`. Det är då viktigt att den är synkad med var
editor-kärnan befinner sig ... Det här är bara en från min sida en
flummig ide som uppstod mitt i natten.  Alla andra lösningar (för att
hålla det så modulariserat som möjligt) är som vanligt välkomna, dvs ni
*måste* hitta på nåt bättre!!

Eftersom filen lagras i minnet i en `dllSt` struktur så innebär det att
man måste tänka till lite när man flyttar sig runt i den. Att göra "pil
ned" är inte bara att sätta `currentRow` till `currentRow->next` utan
att man måste söka upp nästa `verb:\:n` i `currentRow->text[]` eller
efterföljande noder ...

Detta är en smula trist och just för tilfället så är det inte så
kul, frågan är hur mycket man eg. tjänar på en sån här minnesstruktur?

### Editering (Notera att det inte är Buffer-Gap)

#### Grundläggande editering
När en användare skall ta bort tecken till vänster så delas noden upp i
två och end justeras allt eftersom i den vänstra av de två noderna. När
tecken tas bort till höger sker i princip samma sak med det enda
undantaget att det är i den högra av de två och det är begin som
flyttas.

Även vid skrivning av tecken så skapas en ny nod som fylls med de tecken
som skrivs för att sedan länkas in på lämpligt ställe när markören
förflyttas. Detta sker genom att den aktuella noden i vilken markören
befinner sig delas i två exakt där markören är i arrayen. Den enda
skillnaden är att så länge man befinner sig inne i noden och skriver och
markören befinner sig i/på 'end' utan att vara vid `MAX_LINE_LENGTH` så
fortsätter man att jobba i den nuvarande noden!

Snurrigt?  Gissa om det är snurrigt första veckan man tänker på det,
gissa! :-)

#### Undo/Redo (Bör vara konfigureringsbart för användaren)
En enkel förklaring skulle kunna vara att man "spelar in" vad användaren
trycker på för tangenter och sedan spelar upp det hela baklänges (utan
att radera) när man gör undo.  Redo i sin tur skulle bara betyda att man
spelar framlänges från där man står på tapen. (Tapen är givetvis
oändligt lång åt höger ;)

Apropå konfigureringsbarheten (undo-djupet tex) så kom Jakob på
något kul där: "På en 64:a får man poke:a en adress ...;-)"

#### Kompaktering
Rent genrellt sett är den här implementationen av en editor väldigt
enkel.  Dess största problem dock är att den är enormt kladdig!  Detta
avhjälps genom att man med jämna intervall, när det inte finns några
tecken/kommandon att utföra, kompakterar de noder man ändrat i.

Detta kan lösas på många sätt. Ett kan vara att man har en lista av
vilka noder man varit inne och pillat i så att man inte börjar från
början av filen med att försöka kompaktera, då detta är ganska onödigt.
Tänk bara på att kompaktering bör ske i små steg och inte rekursivt/allt
på en gång.

**Ex)**
Om man har en länkad lista av noder som är editerade och man alltid
länkar in nya noder först i den listan. Så kan det vara en bra ide att
titta bakifrån den listan för att se om den näst sista och sista kan
slås ihop. Om detta inte går fortsätter man med den näst näst sista och
den näst sista etc. :-)


### Avklarade funktioner

Hit kan nog bara räknas en delsmängd av "Grundläggande editering" pga
problemen med curses (att det är puckat och inte kan mappa alla
tangenter tex BACKSPACE) så sitter vi just nu med en hop fina funktioner
utan keybindings. HOME, END, BACKSPACE, DEL och annat finns redan
implementerat ...

Ett finfint pluginsystem har börjat ta form. Se vidare filen
`virtual_screen.txt` för lite info om det. Annars är såsen nog bästa
informationen just nu.

Se filen TODO för vidare info.


Delaktiga
---------

Delaktiga i detta projekt är följande tveksamma individer:

| *Person*                          | *Email*                          | *Current employer*        |
|:----------------------------------|:---------------------------------|:--------------------------|
| Anders "Boran" Bornäs             | <mailto:boran@telia.com>         | Ericsson Radar Systems AB |
| Jakob  "Jakov af Wallby" Eriksson | <mailto:jakob@quicknet.se>       | Quicknet AB               |
| Joachim "Crash Override" Nilsson  | <mailto:joachim.nilsson@home.se> | ABB AB                    |


Kommentarer
-----------

* **2015-09-05** Konverterat till Markdown. "Vem som helst kan skriva
  Markdown!" --Jocke
* **2004-03-30** Fiffig översiktsbild och en del omstrukturering av
  dokumentet. /Jocke
* **1999-04-23** Lade till insert & peek till
  kommandodefinitionslistan. Skrev lite om pluginsystemet.
* **1999-03-27** Massor av fixxar gjorda och en hel del
  omstrukturering/modularisering.  Viss uppdatering av spec.tex, se
  vidare ChangeLog och TODO i distributionen.
* **1999-01-19** Små fixar och ändringar gjorda. Tog bla bort kapitlet
  *RFC* och lade det ist. som en parentes under *Delaktiga*.  Den senare
  blev dessutom omdöpt till *Delaktiga samt deras kommentarer*.
* **1999-01-01** Fylleri alla luckor och städar upp i LaTeX-koden.
  Skriver en liten fin README som en handledning till hur man ska bete
  sig för att skriva ut, läsa och "kompilera" LaTeX.  Kommentarer kan ju
  nu med fördel skrivas direkt i LaTeX-koden som

        % [Jokke] -- Lade till bla bla bla...

  Eller något liknande.

* **1998-12-20** Konvertering till LaTeX

* **1998-10-23** Här kan man ju förslagsvis lägga sina kommentarer, om
  man nu inte vill lägga dem direkt i texten. Om man gör detta kan det
  vara bra att tala om att man gjort det med nån liten fånig remark i
  vänstra kolumnen typ som detta:
  
        [Jokke]  Så här kan en sån där fånig inlinekommentar se ut. :P


Editorkommandon
---------------

Användbart i allt från makron till keybindings. Infogat från
`internal_commands.txt`.  En väldigt bra sammanfattning gjord av Jakov
af Wallby 26/10 -98.

### Different styles for marking text

Two ways of marking text are possible. An example follows where marked
text is displayed in upper case and unmarked text is displayed in lower
case. In both examples the same commands are pressed, just different
marking styles are used.

**NOTEPAD.EXE style**

hello all fri<p style="color:white; background-color:black;">ends, is this not a  
cool editor or what, jag provar att</p> skriva  
lite på svenska också  
hej.  

**COOL.EXE style**

                  ______________________
    hello all fri|ends, is this not a   |
	             |                      |
	cool editor o|r what, jag provar att| skriva
	             `----------------------'
	lite på svenska också.
	
	hej.


In both examples the user entered "mark mode" (or held shift or
whatever) and pushed: one down, 21 right. In the NOTEPAD.EXE, this marks
the text in a "stream of bytes" fashion.

In GULPL block marking mode, this marks a block of 2 times 21
characters.

All this is high level input stuff not really concerning the
first implementation of the editor.

### Command definitions

Here comes the first definitions.

Regard these commands as allocated for future releases, all need not be
implemented at once.

**Edit**

| *Function*          | *Description* |
|:--------------------|:--------------|
| insert              | Used to insert a charater.  |
| peek                | What is the current character?  |
| delete              |  |
| copy                |  |
| paste-normal        |  |
| paste-block         |  |
| replace-character   |  |
| replace-word        |  |
| replace-line        |  |
| replace-block       |  |
| replace-marked      |  |

**Movement**

| *Function*          | *Description* |
|:--------------------|:--------------|
| row-end             | same as line-end  |
| row-start           | same as line-start  |
| column-end          | cool feature, similar to line-end and line-start  |
| column-start        | Almost the same as page-start. (page-start sets X coord to 0)  |
| word-end            |  |
| word-start          |  |
| column-word-end     | same as word-end, but in a vertical fashion.  |
| column-word-start   | same as word-start, but in a vertical fashion.  |
| |
| file-end            |  |
| file-start          |  |
| page-end            |  |
| page-start          |  |
| block-end           | block: a piece of text separated from any other  |
| block-start         | piece of text by two newlines  |
| |
| up                  |  |
| down                |  |
| left                |  |
| right               |  |
| left-up             |  |
| right-up            |  |
| left-down           |  |
| right-down          |  |

**Block marking movement**

| *Function*          | *Description* |
|:--------------------|:--------------|
| BM_row-end          | same as line-end  |
| BM_row-start        | same as line-start  |
| BM_column-end       | cool feature, similar to line-end and line-start  |
| BM_column-start     | Almost the same as page-start. (page-start sets X coord to 0)  |
| BM_word-end         |  |
| BM_word-start       |  |
| BM_column-word-end  | same as word-end, but in a vertical fashion.  |
| BM_column-word-start| same as word-start, but in a vertical fashion.  |
| BM_file-end         |  |
| BM_file-start       |  |
| BM_page-end         |  |
| BM_page-start       |  |
| BM_block-end        | block: a piece of text separated from any other  |
| BM_block-start      | piece of text by two newlines  |
| |
| BM_up               |  |
| BM_down             |  |
| BM_left             |  |
| BM_right            |  |
| BM_left-up          |  |
| BM_right-up         |  |
| BM_left-down        |  |
| BM_right-down       |  |

**Notepad marking movement**

| *Function*          | *Description* |
|:--------------------|:--------------|
| NM_row-end          | same as line-end  |
| NM_row-start        | same as line-start  |
| NM_column-end       | cool feature, similar to line-end and line-start  |
| NM_column-start     | Almost the same as page-start. (page-start sets X coord to 0)  |
| NM_word-end         |  |
| NM_word-start       |  |
| NM_column-word-end  | same as word-end, but in a vertical fashion.  |
| NM_column-word-start| same as word-start, but in a vertical fashion.  |
| NM_file-end         |  |
| NM_file-start       |  |
| NM_page-end         |  |
| NM_page-start       |  |
| NM_block-end        | block: a piece of text separated from any other  |
| NM_block-start      | piece of text by two newlines  |
| |
| NM_up               |  |
| NM_down             |  |
| NM_left             |  |
| NM_right            |  |
| NM_left-up          |  |
| NM_right-up         |  |
| NM_left-down        |  |
| NM_right-down       |  |


hitSearch - Hur sökning efter text bör gå till
----------------------------------------------

Infogat från `hitSearch.txt`. Ännu en väldigt bra sammanfattning
gjord av Jakov af Wallby 26/11 -98.

Lite pseudokod i C-format för enklast möjliga textsökning om
dokumentet ligger i en sträng. Håll till godo:

    /*
	 * NAME:
	 *    hitSearch
	 *
	 *    hitSearch takes document in 'textstr',
	 *    searched for pattern in 'patternstr' and
	 *    returns position in document where hit first occured or
	 *    -1 on miss.
	 *
	 * BUGS:
	 *    Not known. Not tested for many cases either, like empty
	 *    strings etc.
	 */
	int hitSearch(char *textstr, char *patternstr)
	{
	    int hitPos, tPos, pPos;
		
		tPos = 0;
		while (textstr[tPos]){
		    pPos = 0;
			while (textstr[tPos] == patternstr[pPos] && patternstr[pPos] && textstr[tPos]){
			    pPos++;
			    tPos++;
	        }
			if ('\0' == patternstr[pPos]){
			    if (isspace(textstr[tPos]) || '\0' == textstr[tPos]){
				    tPos-= pPos;
					return tPos;
                } else {
				    tPos-= pPos;
                }
            }
            tPos++;
            /* On miss, "tPos" must be backed as many steps as
             * already compared. (pPos). pPos must then be set
             * to zero.
             */
        }
        return -1; /* For error */
    }

