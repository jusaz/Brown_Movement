/*********************************************************
 *                                                       *
 *      gtk3 - Cairo: Movimento Browniano               *
 *                                                       *
 *      GTK+ 3                                           *
 *      Versao: 1.0                                      *
 *      Pedro Moura  2016                                *
 *                                                       *
 *********************************************************/

#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

const gchar  *winTitle    = "Movimento Browniano" ;
glong   win_xlen    = 800 ;
glong   win_ylen    = 600 ;


int main(int argc, char *argv[])
{
  //criar a janela
  GtkWidget *window;
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW(window), win_xlen, win_ylen);
  gtk_window_set_title (GTK_WINDOW(window), "Movimento Browniano");
  gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show (window);
  gtk_main ();

}

