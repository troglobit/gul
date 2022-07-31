Rough Design Ideas [SWEDISH]
============================

Vi m�ste f� till n�t slags stuk p� det h�r med olika versioner och
plattformar snart.  Vad s�gs om n�t s�nt h�r;

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

Ett kanske inte alltf�r ot�nkbart f�rslag, eller?

Sedan hur/om man genererar makefiler anv�nder rekursiv make, configfiler
eller andra bisarra s�tt �terst�r att t�nka ut. 

Kanske b�st med en separat confg/-katalog som man anv�nder f�rdigskrivna
makefiler ur f�r sin respektive milj�?

                                 ----- < > -----

H�r f�ljer en del saker som jag klippt bort ur s�sen eftersom de mest
bara var i v�gen och var kommentarer som egentligen var f�r�ldrade. /Jocke

					/***************************************************************
					 * Borde l�ggas i egen fil (editor_view.c ??) och heta n�t i
					 * stil med "move_cursor_left()"/"moveToPrevChar()" s� man kan
					 * g�ra egna bindings i n�n fil tex:
					 * ~/.gul/keybindings --> /usr/local/share/gul/WordStar_keybindings
					 * :-)
					 */

    /*** Buffer status information ... ? ************************************
     * Oj, den h�r har jag numer stora planer f�r sedan Jakob introducerade
     * den h�r iden med pos & node. Klart fiffigt (tycker jag) om man l�gger
     * in all "bufferspecifik" information p� ett st�lle inkapslat och fint.
     * N�r jag �nd� h�ller p� s� �r det h�r ett st�lle d�r jag skulle vilja
     * spara olika "vyer" ox�. Dvs att man kan titta p� och �ndra samma
     * buffer fr�n olika st�llen. Men det kr�ver lite mer jobb s� tillsvidare
     * skulle jag vilja l�gga in cursorX & cursorY samt top.
     ************************************************************************
     */
    
    	/*** Mera ideer ******************************************************
    	 * 1) Att dela upp kommandon i "movement" och "insert/delete" commands.
    	 *    P� det viset kan man enklare kapsla in s�dant som har med direkt
    	 *    manipulering av inneh�llet/texten att g�ra. [KLART]
    	 *
    	 * 2) Buffer/Gap-metoden p� resp. nod i listan. Ung. som Joe funkar.
    	 *    istf. GUL-metoden med att dela noder och s�nt...
    	 *    [Se yaei.c]
    	 *
    	 *** Problem *********************************************************
    	 * o Mark�ren glider fortfarande iv�g och lever ett eget liv mot var
    	 *   man egentligen befinner sig i buffern ... /Jocke 990308
    	 * o Samma problem ... /Jocke 990324
    	 * o Samma problem ... samma editork�rna ... /Jocke 990608 *suck*
    	 *********************************************************************
    	 */
    
       
    /*** IDEER!!! ***********************************************************
     * Nu j�klar fick jag en massa ideer!!!
     *
     * Vyn skall hanteras av sk�rmpluginet. Det skall vara ett slags
     * "tittf�nster" in i buffern.
     *
     * displayText() ska ta alla de currentp->scr.,maxY rader text som
     * editorCharacterGenerator() ska rendera fr�n den aktuella buffern
     * och bara skriva ut tecknen p� ... n�e. Dum++ ide!
     *
     * eller v�nta! Om man s�ger att man horisontalskrollar sk�rmen tv�
     * steg �t h�ger s� ska virtuella veta om detta och inte acceptera
     * tecken som skrivs till kolumn 1 och 2 av putPixchar().
     * Vad s�gs om den l�sningen d�?!
     * *r�ddar*
     *
     * �nnu b�ttre: Horisontell scrolling/panning g�rs medelst justering
     * av virutella sk�rmens start-x-v�rde ... dvs: screen.hpos= 4; s� kan
     * bara x > 4 skrivas till sk�rmen av putPixchar() ... Deru! :-)
     *
     ***********************************************************************/
