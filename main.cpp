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
gint    flag_sc     = 0   ;

gboolean
cb_stop_continue (GtkButton  *widget ,
                  gpointer    data   )
{
  if (flag_sc)
    gtk_button_set_label (GTK_BUTTON(widget), "Parar");
  else
    gtk_button_set_label (GTK_BUTTON(widget), "Continuar");

  flag_sc = (flag_sc + 1) % 2;

  return FALSE;
}

gboolean
cb_restart (GtkWidget *widget ,
            gpointer   data   )
{
  return false;
}

gboolean
cb_options_clicked (GtkWidget *widget ,
                    GtkWidget *win    )
{
  GtkWidget    *op_win, *vbox, *label ;

  glong   xy_len    = 400 ;

  GtkDialogFlags flags = GtkDialogFlags(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT);

  op_win = gtk_dialog_new_with_buttons("Opções",
                                       GTK_WINDOW(widget),
                                       flags,
                                       GTK_STOCK_OK,
                                       GTK_RESPONSE_ACCEPT,
                                       GTK_STOCK_CANCEL,
                                       GTK_RESPONSE_REJECT,
                                       NULL);

  gtk_window_set_position (GTK_WINDOW(op_win), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (op_win, xy_len, xy_len);
  gtk_window_set_title (GTK_WINDOW (op_win), "Opcões");
  g_signal_connect (G_OBJECT(op_win), "destroy", G_CALLBACK(gtk_widget_destroy), op_win);


  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
  gtk_container_add (GTK_CONTAINER (op_win), vbox);

  label = gtk_label_new("Bola Grande:");
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 3);

  label = gtk_label_new("Bolas Pequenas:");
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 3);

  gtk_widget_show_all (op_win);

  /* Create the dialog as modal and destroy it when a button is clicked. */
  gtk_dialog_run (GTK_DIALOG (op_win));
  gtk_widget_destroy (op_win);

  return FALSE;
}

int main(int argc, char *argv[])
{
  //criar a janela
  GtkWidget *window, *draw_area, *frame, *main_box, *button_box, *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position (GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW(window), win_xlen, win_ylen);
  gtk_widget_set_size_request (window, win_xlen - 50, win_ylen - 50);
  gtk_window_set_title (GTK_WINDOW(window), winTitle);

  //criação da box que suporta a drawing area e a frame de configuração
  main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (main_box), 5);
  gtk_container_add (GTK_CONTAINER (window), main_box);

  //criação da frame para a drawing area
  frame = gtk_frame_new ("");
  gtk_frame_set_label_align (GTK_FRAME(frame), 0.5, 0.5);
  gtk_box_pack_start (GTK_BOX(main_box), frame, TRUE, TRUE, 3);

  //criação da drawing area
  draw_area = gtk_drawing_area_new ();
  gtk_container_add (GTK_CONTAINER (frame), draw_area);

  //criação da frame de configuração
  frame = gtk_frame_new (" Configuração ");
  gtk_frame_set_label_align (GTK_FRAME(frame), 0.5, 0.5);
  gtk_box_pack_start (GTK_BOX(main_box), frame, FALSE, TRUE, 3);

  //criação da button_box
  button_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 10);
  gtk_container_add (GTK_CONTAINER (frame), button_box);

  //criação do botão reiniciar
  button = gtk_button_new_with_label ("Reiniciar");
  gtk_widget_set_size_request (button, 170, 20);
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cb_restart), NULL);
  gtk_box_pack_end (GTK_BOX(button_box), button, FALSE, TRUE, 3);

  //criação do botão parar
  button = gtk_button_new_with_label ("Parar");
  gtk_widget_set_size_request (button, 170, 20);
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cb_stop_continue), NULL);
  gtk_box_pack_end (GTK_BOX(button_box), button, FALSE, TRUE, 3);

  //criação do botão Opcões
  button = gtk_button_new_with_label ("Opções");
  gtk_widget_set_size_request (button, 170, 20);
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cb_options_clicked), window);
  gtk_box_pack_end (GTK_BOX(button_box), button, FALSE, TRUE, 3);

  g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);
  gtk_main ();

  return 0;
}

