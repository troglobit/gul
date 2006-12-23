#include <stdio.h>
#include <slang.h>

int main (int argc, char *argv[])
{
   int key = 0;

   SLtt_get_terminfo ();
   SLang_init_tty (/* Ctrl-G */ 7, 1, 0);

   SLsmg_init_smg ();
   SLsmg_cls ();

   SLsmg_gotorc (0,0); SLsmg_erase_eos ();
   SLsmg_refresh ();

   while (key != 'q')
   {
      key = SLang_getkey ();
      SLsmg_printf ("%d ", key);
      SLsmg_refresh ();
   }

   SLang_reset_tty ();

   return 1;
}

