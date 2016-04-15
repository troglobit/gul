Rough Design Ideas [SWEDISH]
============================

Vi måste få till nåt slags stuk på det här med olika versioner och
plattformar snart.  Vad sägs om nåt sånt här;

    editor/
        +--- doc/
        |     +--- blue/
        |     +--- trivia/
        +--- src/
              +--- plugin/
              |     +--- vt100/
              |     +--- ncurses/
              |     +--- slang/
              |     +--- amiga/
              |     +--- svi/
              +--- core/
              |     +--- buffer-gap/
              |     +--- linked-list/
              +--- io/
              |     +--- vms/
              |     +--- unix/
              |     +--- amiga/
              |     +--- svi/
              \--- misc/

Ett kanske inte alltför otänkbart förslag, eller?

Sedan hur/om man genererar makefiler använder rekursiv make, configfiler
eller andra bisarra sätt återstår att tänka ut. 

Kanske bäst med en separat confg/-katalog som man använder färdigskrivna
makefiler ur för sin respektive miljö?

                                 ----- < > -----

Här följer en del saker som jag klippt bort ur såsen eftersom de mest
bara var i vägen och var kommentarer som egentligen var föråldrade. /Jocke

					/***************************************************************
					 * Borde läggas i egen fil (editor_view.c ??) och heta nåt i
					 * stil med "move_cursor_left()"/"moveToPrevChar()" så man kan
					 * göra egna bindings i nån fil tex:
					 * ~/.gul/keybindings --> /usr/local/share/gul/WordStar_keybindings
					 * :-)
					 */

    /*** Buffer status information ... ? ************************************
     * Oj, den här har jag numer stora planer för sedan Jakob introducerade
     * den här iden med pos & node. Klart fiffigt (tycker jag) om man lägger
     * in all "bufferspecifik" information på ett ställe inkapslat och fint.
     * När jag ändå håller på så är det här ett ställe där jag skulle vilja
     * spara olika "vyer" oxå. Dvs att man kan titta på och ändra samma
     * buffer från olika ställen. Men det kräver lite mer jobb så tillsvidare
     * skulle jag vilja lägga in cursorX & cursorY samt top.
     ************************************************************************
     */
    
    	/*** Mera ideer ******************************************************
    	 * 1) Att dela upp kommandon i "movement" och "insert/delete" commands.
    	 *    På det viset kan man enklare kapsla in sådant som har med direkt
    	 *    manipulering av innehållet/texten att göra. [KLART]
    	 *
    	 * 2) Buffer/Gap-metoden på resp. nod i listan. Ung. som Joe funkar.
    	 *    istf. GUL-metoden med att dela noder och sånt...
    	 *    [Se yaei.c]
    	 *
    	 *** Problem *********************************************************
    	 * o Markören glider fortfarande iväg och lever ett eget liv mot var
    	 *   man egentligen befinner sig i buffern ... /Jocke 990308
    	 * o Samma problem ... /Jocke 990324
    	 * o Samma problem ... samma editorkärna ... /Jocke 990608 *suck*
    	 *********************************************************************
    	 */
    
       
    /*** IDEER!!! ***********************************************************
     * Nu jäklar fick jag en massa ideer!!!
     *
     * Vyn skall hanteras av skärmpluginet. Det skall vara ett slags
     * "tittfönster" in i buffern.
     *
     * displayText() ska ta alla de currentp->scr.,maxY rader text som
     * editorCharacterGenerator() ska rendera från den aktuella buffern
     * och bara skriva ut tecknen på ... näe. Dum++ ide!
     *
     * eller vänta! Om man säger att man horisontalskrollar skärmen två
     * steg åt höger så ska virtuella veta om detta och inte acceptera
     * tecken som skrivs till kolumn 1 och 2 av putPixchar().
     * Vad sägs om den lösningen då?!
     * *räddar*
     *
     * Ännu bättre: Horisontell scrolling/panning görs medelst justering
     * av virutella skärmens start-x-värde ... dvs: screen.hpos= 4; så kan
     * bara x > 4 skrivas till skärmen av putPixchar() ... Deru! :-)
     *
     ***********************************************************************/
