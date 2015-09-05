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

Projekt f�r att vidga vyer flytta berg och skriva den ultimata editorn.  
Med lite tur s� l�r vi oss lite ox� p� v�gen dit. :)


Funktionell beskrivning
-----------------------

N�r vi f�r m�nga m�nar sedan med of�rst�rda sinnen, d� alla samlade i
den perfekta enheten 'Campus', dj�rvt och med en d�res dr�mmar och
visioner gav oss in i detta projekt s� st�llde vi upp vissa m�l. Detta
dokument �r ett f�rs�k att sammanfatta dessa p� ett s� klart och tydligt
s�tt som m�jligt ...

En varning kanske �r p� sin plats: Upprepningar och otydligheter �r
s�llan ovanliga i texten nedan. Den �r dessutom ofantligt l���ng redan.
S� minns denna varning n�r ni mot slutet av dagen utsvultna fl�mtandes
tittar upp fr�n sk�rmen med ett pl�gat uttryck i ansiktet. *he he*


### Krav p� design
H�r f�ljer nu en sammanfattning av punkter som vi talat om. Vissa saker
�r vi r�rande �verens om andra vet vi inte vad vi tycker, utom att det
l�ter ballt och borde vara med...

- Sj�lva editork�rnan skall vara liten.
- Snabb och resurssn�l (i sin enklaste/renaste form).
- Mer lik typ Notepad �n VI (f�r det finns redan s� m�nga vi-
  kloner). Dvs inga separata kommando/editerings-l�gen.
- Modulariserad av bara DEN. Ska tex klara att mappa olika
  tangentkombinationer till olika keysets precis som att man
  enkelt skall kunna kompilera om med en annan display-modul
  och vips kunna ha en editor f�r X (GTK+?!) likv�l som man
  skall kunna k�ra den p� sin favoritterminal (Wyse 50 ;).
- F�r snabba s�kningar har Jakob t�nkt ut att anv�nda hashing.
  (Endast han sj�lv verkar veta hur detta skall funka dock, f�r
  inget mer har h�rts om detta... :) `Jakob skall fylla i h�r!`
- Recovery-filer! Hur? Tja, sv�r och intressant++ fr�ga!
- Swapfiler? Man kan ju bara inte he en hel fil (stor) i minnet
  utan �r ju eg. piskad att ha en swap, kanske i kobination med
  den ovan n�mda recovery-filen?!
- Unicode-support?
- Jag vill ha st�d f�r folding ... /Crash


### Funktionssammanfattning
Fritt tagen ur tidigare maildiskussioner. Andra id�er om hur uppl�gget
av sammanfattningen b�r se ut v�lkomnas som vanligt.

Sammanfattningen ursprungligen gjord av Jakob 23/10 -98 med kommentarer
och till�gg av Jocke.

    Jakob <jakov@quicknet.se> Fri Oct 23 00:22:16 1998
	> Plattformsoberoende.
	>    "Wrap"-funktioner eller "Wrap"-defines till utskrift p�
	>    sk�rmen.
	>    Till�gg av mig [Jakob]: Liknande t�nkade vid access mot
	>                            filsystemet.
	
	Wrappers som g�r att hur �n filsystemet fungerar (alla sjuka
	och konstiga fall!) s� �r editorn oberoende av detta.
	
	> > Liten bin�r.
	>    Mindre �n pico.
	>    pico �r ganska stor, 270 kb p� legolas
	>    och drygt 100kb p� min linux. (Med delade bibliotek.)
	>    Jag tycker v�r design ska vara mycket liten.
	
	Vi skulle visst satsa p� att f�rs�ka komma under 150kB, vilket
	jag personligen inte tror �r n�got som helst problem! :-)
	
	> Multilevel undo. (Som m�l iaf.)
	
	�r eg. ox� avklarat (i teorin) redan. Se vidare nedan.
	
	> Effektiv och snabb minneshantering.
	>    N�gon annan l�sning �n vanlig GAP och LL.
	
	F�r f�rslag, se "Implementationsbeskrivning".
	
	> F�rslag fr�n min sida ang�ende stubbar:
	>    Se om vi inte kan utnyttja #ifdef och #define i r�tt h�g
	>    grad. P� s� vis kan bin�ren h�llas liten.

Vi var �ven inne p� konceptet "tempfiler" Vilket fortfarande nog �r en
r�tt intressant diskussion. F�r som Anders mycket listigt p�pekar i ett
brev, s� anv�nder b�de VI och EMACS denna l�sning och kan d�remd �ven
skryta med "recovery"-funktioner.


### Namnkonventioner
Filnamn och standard f�r `#defines` och variabelnamn.

N�r vi diskuterade detta tidigare s� var vi alla s� r�rande
�verens, men en liten rekapitulation av vad som sagts kanske
�r p� sin plats:

Sammanfattning av Anders 11/10 -98 med kommentarer av Jocke.

    Anders Born�s <mail_boran@yahoo.com> Fri, 11 Sep 1998
	> gulMain.c gulMain.h  <- typ main eller s�
	> mem.c mem.h          <- utbytbar minneshantering
	
	memory.*
	
	> io.c io.h            <- nja, filhantering egentligen
	
	vfs.*                  <- Virtual file system == Filhantering?
	io.*                   <- Input==Keyboard, Output==Screen?
	
	> disp.c disp.h        <- sk�rmutskrifter
	
	display.*
	
	> misc.h               <- en favorit jag _m�ste_ ha!
	>                         (TRUE, FALSE, RANDOMVAL mm)

Variabelnamn har jag f�r mig att vi istf. att anv�nda underscore
s� skulle vi h�lla oss till n�got i den h�r stilen:

    filename/fileName, thisNode, previousNode, nodep, nodepp osv.

D�r `p` och `pp` anv�nds till att tala om att det �r pekare
resp. dubbelpekare.  Givetvis s� �r det ju inte hela v�rlden om man
anv�nder underscore d� och d�.

`#define`'s skall givetvis vara VERSALER! :)


### Kort lista �ver anv�nda begrepp
Boran b�rjade ju tuta ihop en lista av tuffa ord som han inte f�rstod,
n�r vi andra f�r l�nge sedan givit upp hoppet om att n�nsin f�rst� vad
det var vi pratade om ... Den h�r listan *b�r* kompletteras.

| **Begrepp**     | **F�rklaring ** |
| GAPS(I)Z(E)     | Konstant betecknande storleken p� buffergapet. |
| Blockmarking     | M�jligheten att kunna markera text i rektangul�ra block. (Se �ven Appendix A) |
| linescroll(ing)  | Dvs inte wrappat som i emacs/vi. (Se �ven \texttt{M-x hscroll-mode} i Emacs.) |
| ll               | Linked list, se editor.101.txt |
| buffer gap       | Se filen buffer-gap.txt i distributionen |
| Folding          | T�nk dig att kunna markera rader, tex alla kommentarer, och sedan kunna v�lja ``Fold out'' i editorn f�r att dessa sedan skall "f�rsvinna" eller snarare vikas undan, d�rav namnet. |
| Virtual keys     | M�jlighet att ha olika "keymaps". Dvs att kunna binda tangentkombinationer till olika funktioner i editorn. |
| Key binding      | Se: Virtual keys. |
| GULPL            | GUL Public License |
| KAFFE            | Fullkomligt n�dv�ndigt f�r att kunna leva. |


Timeline - �ven k�nd som "Fina listan" :)
-----------------------------------------
H�r f�ljer nu en liten hur-vi-skulle-kunna-g�-tillv�ga-lista v�ldigt fint
sammanfattad av Anders!

**Den Fina Listan**
- printa text
- l�sa in fil till allokerat minne
- mark�rf�rflyttning (simpel)
- infoga ta bort
- spara fil
- ordna bra (n�ja) tangentmappningar
- snygga till minneshanteringen
- r�tta till presentationen
- ut�ka funtionaliteten
- goto f�rra punkten
- ... i stort sett.

Efter den fina listan kan jag nu stolt s�ga att 1--4 �r "klart".  En fil
g�r att l�sa in fr�n prompten, in i strukturen av l�nkade rader (se
vidare nedan f�r beskrivning av *exakt* hur det fungerar).  D�r man
sedan tack vare curses b�de kan se b�rjan av filen (f�rsta sk�rmsidan),
flytta runt en mark�r och ta bort och infoga tecken. :-D

Se vidare filen TODO i distributionen. (Det �r *s�* koolt att kunna s�ga
just "distributionen" om det h�r lilla hacket! ;)


Implementationsbeskrivning
--------------------------

![Overview](overview.eps "�versikt av nuvarande design")

### Dubbell�nkad lista av rader
*Kort beskrivning:* Enl. den funktionella beskrivningen ovan s� byggs
filen upp av en datastruktur av dubbell�nkade rader. D�r varje rad inte
n�dv�ndigtvis m�ste vara just en fysisk rad i filen utan �r ett block om
256 bytes n�gonstans ur filen.

Den mycket bisarra konstruktionen nedan enl.

    3 * (2 * sizeof(struct dllSt *) + 2 * sizeof(int))

till f�r att p� ett snyggt s�tt aligna hela *dllSt* till en j�mn
tv�potens.  I det h�r fallet 64 bytes (om pekare och *int* x �r fyra
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

Detta uppl�gg har valts pga fr�mst tv� orsaker:
- Enkel id�, l�tt att implementera.
- Ger sm� datablock som �r enklare/snabbare att hantera.
- Snygg alignment till j�mna block om 16 bytes (notera att NFS alltid
  cache:ar block om 16 bytes, vad nu det kan vara bra till?)

### Buffer-Gap
Efter att ha lagt sis�d�r ett par-tre m�nader p� att f�rs�ka f� till
implementationen av DLL, ovan, s� satte sig Jocke ner och tog tag
Buffer-Gap-metoden.  Det var dags att leka med de stora pojkarna!

Det visade sig att Buffer-Gap faktiskt var v�ldigt enkel.  Buffern som
var s� omtalad var bara en minnesarea med hela filinneh�llet i och det
bisarra gapet var bara mark�ren som man l�t glida omkring i buffern.

    |<----- first half -----><----- gap -----><------ second half ------>|

N�r man vill flytta mark�ren �t h�ger s� kopierar man teckent till
h�ger om gapet till gapets position noll och flyttar sedan gapet ett
steg till h�ger:

    thisp->gap[0] = thisp->gap[thisp->gap_size];
    thisp->gap++;
	
    Efter att ha lagt sis�d�r ett par-tre m�nader p� att f�rs�ka f� till
	implementationen av DLL, ovan, [<---G-A-P--->]s� satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!
	
	Efter att ha lagt sis�d�r ett par-tre m�nader p� att f�rs�ka f� till
	implementationen av DLL, ovan, s[<---G-A-P--->]� satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!
	
	Efter att ha lagt sis�d�r ett par-tre m�nader p� att f�rs�ka f� till
	implementationen av DLL, ovan, s�[<---G-A-P--->] satte sig Jocke ner
	och tog tag Buffer-Gap-metoden.  Det var dags att leka med de stora
	pojkarna!

Givetvis f�rst med de normala randv�rdeskontrollerna. F�r att flytta
�t v�nster �r det ungef�r likadant:

    thisp->gap--;
    thisp->gap[thisp->gap_size] = thisp->gap[0];

N�r man vill l�gga till tecken till buffern s� skriver man de till
gapets position noll, flyttar gapet ett steg till h�ger och minskar
sedan storleken med ett.

    thisp->gap[0] = character;
    thisp->gap++;
    thisp->gap_size--;

�vriga f�rflyttningskommandon och kommandon f�r att radera tecken till
v�nster och h�ger om gapet �r bara variationer p� ovanst�ende.


### Inladdningen av filen

Allting b�rjas med att man tex l�ser in en fil, vilket nog �nd� �r det
garanterat vanligaste man g�r ... det �r ju s�llan man skapar en ny fil
och skriver och skriver och helt pl�tsligt �r allt klart, f�rdigt.

F�rvisso finns det ju de som vill �ppna filen ifr�ga fr�n editorn
... men i dagens drag'n'drop-v�rld s� �r dessa puckon en
utrotningshotatd art. ;)


#### DLL -- Konvertering till l�nkade rader

Vid inladdningen sker f�ljande:

- F�niga kontroller om filen finns och annat tjafs.
- Allokera nod.
- Fyll noden mha fread (anv�nd `MAX_LINE_LENGTH`).
- S�tt begin till noll och end till det fread returnerar.
- Upprepa 2-4 tills fread returnerar `EOF` eller `fread()`
  returnerar n�got annat �n `MAX_LINE_LENGTH`.

Efter inladdningen s� blir f�nster/utskrivnings-servern anmodad
att visa den inladdade texten.


#### Buffer-Gap -- Ladda fil till Buffer

Under Buffer-Gap �r det extremt l�tt att ladda in filen eftersom
allt man beh�ver g�ra �r:

- Ta reda p� filstorleken.
- Allokera tillr�ckligt stor buffer plus `GAP_SIZE` bytes.
- S�tt gap att peka p� buffer
- L�s in allt data fr�n filen till `buffer[GAP_SIZE]`
- Mark�ren (Gap) st�r nu p� f�rsta tecknet i filen.
- Klart.


### F�rflyttning
All f�rflyttning sker precis som man �r van vid, eller som man
har valt att sj�lv mappa funktionerna f�r f�rflyttning.

F�r varje f�rflyttningskommando som kommer in till h�ndelsehanteraren
(`while(1)`-loopen) s� skickas det vidare till
f�nster/utskrivnings-servern som flyttar mark�ren och/eller uppdaterar
texten p� sk�rmen.

F�nster/utskrivnings-servern har hand om allt vad f�nsterstorlek, wrap,
och annat larv. Den h�ller �ven reda p� var i filen/buffern man befinner
sig. Tex: `Col=45, Row=16`. Det �r d� viktigt att den �r synkad med var
editor-k�rnan befinner sig ... Det h�r �r bara en fr�n min sida en
flummig ide som uppstod mitt i natten.  Alla andra l�sningar (f�r att
h�lla det s� modulariserat som m�jligt) �r som vanligt v�lkomna, dvs ni
*m�ste* hitta p� n�t b�ttre!!

Eftersom filen lagras i minnet i en `dllSt` struktur s� inneb�r det att
man m�ste t�nka till lite n�r man flyttar sig runt i den. Att g�ra "pil
ned" �r inte bara att s�tta `currentRow` till `currentRow->next` utan
att man m�ste s�ka upp n�sta `verb:\:n` i `currentRow->text[]` eller
efterf�ljande noder ...

Detta �r en smula trist och just f�r tilf�llet s� �r det inte s�
kul, fr�gan �r hur mycket man eg. tj�nar p� en s�n h�r minnesstruktur?

### Editering (Notera att det inte �r Buffer-Gap)

#### Grundl�ggande editering
N�r en anv�ndare skall ta bort tecken till v�nster s� delas noden upp i
tv� och end justeras allt eftersom i den v�nstra av de tv� noderna. N�r
tecken tas bort till h�ger sker i princip samma sak med det enda
undantaget att det �r i den h�gra av de tv� och det �r begin som
flyttas.

�ven vid skrivning av tecken s� skapas en ny nod som fylls med de tecken
som skrivs f�r att sedan l�nkas in p� l�mpligt st�lle n�r mark�ren
f�rflyttas. Detta sker genom att den aktuella noden i vilken mark�ren
befinner sig delas i tv� exakt d�r mark�ren �r i arrayen. Den enda
skillnaden �r att s� l�nge man befinner sig inne i noden och skriver och
mark�ren befinner sig i/p� 'end' utan att vara vid `MAX_LINE_LENGTH` s�
forts�tter man att jobba i den nuvarande noden!

Snurrigt?  Gissa om det �r snurrigt f�rsta veckan man t�nker p� det,
gissa! :-)

#### Undo/Redo (B�r vara konfigureringsbart f�r anv�ndaren)
En enkel f�rklaring skulle kunna vara att man "spelar in" vad anv�ndaren
trycker p� f�r tangenter och sedan spelar upp det hela bakl�nges (utan
att radera) n�r man g�r undo.  Redo i sin tur skulle bara betyda att man
spelar framl�nges fr�n d�r man st�r p� tapen. (Tapen �r givetvis
o�ndligt l�ng �t h�ger ;)

Aprop� konfigureringsbarheten (undo-djupet tex) s� kom Jakob p�
n�got kul d�r: "P� en 64:a f�r man poke:a en adress ...;-)"

#### Kompaktering
Rent genrellt sett �r den h�r implementationen av en editor v�ldigt
enkel.  Dess st�rsta problem dock �r att den �r enormt kladdig!  Detta
avhj�lps genom att man med j�mna intervall, n�r det inte finns n�gra
tecken/kommandon att utf�ra, kompakterar de noder man �ndrat i.

Detta kan l�sas p� m�nga s�tt. Ett kan vara att man har en lista av
vilka noder man varit inne och pillat i s� att man inte b�rjar fr�n
b�rjan av filen med att f�rs�ka kompaktera, d� detta �r ganska on�digt.
T�nk bara p� att kompaktering b�r ske i sm� steg och inte rekursivt/allt
p� en g�ng.

**Ex)**
Om man har en l�nkad lista av noder som �r editerade och man alltid
l�nkar in nya noder f�rst i den listan. S� kan det vara en bra ide att
titta bakifr�n den listan f�r att se om den n�st sista och sista kan
sl�s ihop. Om detta inte g�r forts�tter man med den n�st n�st sista och
den n�st sista etc. :-)


### Avklarade funktioner

Hit kan nog bara r�knas en delsm�ngd av "Grundl�ggande editering" pga
problemen med curses (att det �r puckat och inte kan mappa alla
tangenter tex BACKSPACE) s� sitter vi just nu med en hop fina funktioner
utan keybindings. HOME, END, BACKSPACE, DEL och annat finns redan
implementerat ...

Ett finfint pluginsystem har b�rjat ta form. Se vidare filen
`virtual_screen.txt` f�r lite info om det. Annars �r s�sen nog b�sta
informationen just nu.

Se filen TODO f�r vidare info.


Delaktiga
---------

Delaktiga i detta projekt �r f�ljande tveksamma individer:

| *Person*                          | *Email*                          | *Current employer*        |
|:----------------------------------|:---------------------------------|:--------------------------|
| Anders "Boran" Born�s             | <mailto:boran@telia.com>         | Ericsson Radar Systems AB |
| Jakob  "Jakov af Wallby" Eriksson | <mailto:jakob@quicknet.se>       | Quicknet AB               |
| Joachim "Crash Override" Nilsson  | <mailto:joachim.nilsson@home.se> | ABB AB                    |


Kommentarer
-----------

* **2015-09-05** Konverterat till Markdown. "Vem som helst kan skriva
  Markdown!" --Jocke
* **2004-03-30** Fiffig �versiktsbild och en del omstrukturering av
  dokumentet. /Jocke
* **1999-04-23** Lade till insert & peek till
  kommandodefinitionslistan. Skrev lite om pluginsystemet.
* **1999-03-27** Massor av fixxar gjorda och en hel del
  omstrukturering/modularisering.  Viss uppdatering av spec.tex, se
  vidare ChangeLog och TODO i distributionen.
* **1999-01-19** Sm� fixar och �ndringar gjorda. Tog bla bort kapitlet
  *RFC* och lade det ist. som en parentes under *Delaktiga*.  Den senare
  blev dessutom omd�pt till *Delaktiga samt deras kommentarer*.
* **1999-01-01** Fylleri alla luckor och st�dar upp i LaTeX-koden.
  Skriver en liten fin README som en handledning till hur man ska bete
  sig f�r att skriva ut, l�sa och "kompilera" LaTeX.  Kommentarer kan ju
  nu med f�rdel skrivas direkt i LaTeX-koden som

        % [Jokke] -- Lade till bla bla bla...

  Eller n�got liknande.

* **1998-12-20** Konvertering till LaTeX

* **1998-10-23** H�r kan man ju f�rslagsvis l�gga sina kommentarer, om
  man nu inte vill l�gga dem direkt i texten. Om man g�r detta kan det
  vara bra att tala om att man gjort det med n�n liten f�nig remark i
  v�nstra kolumnen typ som detta:
  
        [Jokke]  S� h�r kan en s�n d�r f�nig inlinekommentar se ut. :P


Editorkommandon
---------------

Anv�ndbart i allt fr�n makron till keybindings. Infogat fr�n
`internal_commands.txt`.  En v�ldigt bra sammanfattning gjord av Jakov
af Wallby 26/10 -98.

### Different styles for marking text

Two ways of marking text are possible. An example follows where marked
text is displayed in upper case and unmarked text is displayed in lower
case. In both examples the same commands are pressed, just different
marking styles are used.

**NOTEPAD.EXE style**

hello all fri<p style="color:white; background-color:black;">ends, is this not a  
cool editor or what, jag provar att</p> skriva  
lite p� svenska ocks�  
hej.  

**COOL.EXE style**

                  ______________________
    hello all fri|ends, is this not a   |
	             |                      |
	cool editor o|r what, jag provar att| skriva
	             `----------------------'
	lite p� svenska ocks�.
	
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


hitSearch - Hur s�kning efter text b�r g� till
----------------------------------------------

Infogat fr�n `hitSearch.txt`. �nnu en v�ldigt bra sammanfattning
gjord av Jakov af Wallby 26/11 -98.

Lite pseudokod i C-format f�r enklast m�jliga texts�kning om
dokumentet ligger i en str�ng. H�ll till godo:

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

