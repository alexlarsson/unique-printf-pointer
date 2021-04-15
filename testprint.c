#include <printf.h>
#include <stdio.h>
#include <glib.h>

int
main (int argc, char *argv[])
{
  char a, b, c;


  printf ("printf:\n");
  for (int i = 0; i < 2; i++)
    {
      printf ("'%-20p' '%20p' %p\n", &a, &b, &c);
    }

  /* Works with g_print too */
  printf ("g_print:\n");
  for (int i = 0; i < 2; i++)
    {
      g_print ("'%-20p' '%20p' %p\n", &a, &b, &c);
    }

}
