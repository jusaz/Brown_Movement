/*********************************************************
 *                                                       *
 *      gtk3 - Cairo: Movimento Browniano                *
 *                                                       *
 *      GTK+ 3                                           *
 *      Versao: 1.0                                      *
 *      Pedro Moura  2016                                *
 *                                                       *
 *********************************************************/

#define BG_MASSA_DFT      5
#define BG_VEL_DFT        5
#define BP_NUM_BOLAS_DFT  5
#define BP_MASSA_DFT      5
#define BP_VEL_DFT        5

#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

const gchar  *winTitle    = "Movimento Browniano" ;
glong   win_xlen    = 800 ;
glong   win_ylen    = 600 ;
gint    flag_sc     = 1   ;

typedef struct _config_bg
{
  guint8   bg_massa;
  guint8   bg_velocidade;
  gboolean bg_ver_sozinha;
  gboolean bg_ver_velocidade;
}config_bg;

typedef struct _config_bp
{
  guint8 bp_massa;
  guint8 bp_velocidade;
  guint8 bp_num_bolas;
}config_bp;

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
cb_sb_bg_massa (GtkWidget *widget ,
             gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_massa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bg massa: %d\n", ((config_bg*)data)->bg_massa);
  return false;
}

gboolean
cb_sb_bg_velocidade (GtkWidget *widget ,
                  gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_velocidade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bg velocidade: %d\n", ((config_bg*)data)->bg_velocidade);
  return false;
}

gboolean
cb_chkb_bg_ver_sozinha (GtkWidget *widget ,
                        gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_ver_sozinha = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  g_print("bg ver sozinha: %d\n", ((config_bg*)data)->bg_ver_sozinha ? 1 : 0);
  return false;
}

gboolean
cb_chkb_bg_ver_velocidade (GtkWidget *widget ,
                           gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_ver_velocidade = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  g_print("bg ver velocidade: %d\n", ((config_bg*)data)->bg_ver_velocidade ? 1 : 0);
  return false;
}

gboolean
cb_sb_bp_massa (GtkWidget *widget ,
             gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_massa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp massa: %d\n", ((config_bp*)data)->bp_massa);
  return false;
}

gboolean
cb_sb_bp_num_bolas (GtkWidget *widget ,
                 gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_num_bolas = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp num bolas: %d\n", ((config_bp*)data)->bp_num_bolas);
  return false;
}

gboolean
cb_sb_bp_velocidade (GtkWidget *widget ,
                  gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_velocidade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp velocidade: %d\n", ((config_bp*)data)->bp_velocidade);
  return false;
}

int main(int argc, char *argv[])
{
  //criar a janela
  GtkWidget *window, *draw_area, *frame, *main_box, *button_box, *button;

  GtkWidget *label, *spin_button, *check_box;

  config_bg bg_param;
  config_bp bp_param;

  bg_param = {(guint8)BG_MASSA_DFT, (guint8)BG_VEL_DFT, false, false};
  bp_param = {(guint8)BP_MASSA_DFT, (guint8)BP_VEL_DFT, (guint8)BP_NUM_BOLAS_DFT};

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
  button = gtk_button_new_with_label ("Iniciar");
  gtk_widget_set_size_request (button, 170, 20);
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cb_stop_continue), NULL);
  gtk_box_pack_end (GTK_BOX(button_box), button, FALSE, TRUE, 3);

  label = gtk_label_new("Bola Grande");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 6);

  label = gtk_label_new("Massa:");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 3);

  spin_button = gtk_spin_button_new_with_range(1, 100, 1);
  g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(cb_sb_bg_massa), &bg_param);
  gtk_box_pack_start(GTK_BOX(button_box), spin_button, FALSE, TRUE, 0);

  label = gtk_label_new("Velocidade:");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 3);

  spin_button = gtk_spin_button_new_with_range(1, 10, 1);
  g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(cb_sb_bg_velocidade), &bg_param);
  gtk_box_pack_start(GTK_BOX(button_box), spin_button, FALSE, TRUE, 3);

  check_box = gtk_check_button_new_with_label("Ver só bola grande");
  g_signal_connect(G_OBJECT(check_box), "toggled", G_CALLBACK(cb_chkb_bg_ver_sozinha), &bg_param);
  gtk_box_pack_start(GTK_BOX(button_box), check_box, FALSE, TRUE, 6);

  check_box = gtk_check_button_new_with_label("Ver Velocidade");
  g_signal_connect(G_OBJECT(check_box), "toggled", G_CALLBACK(cb_chkb_bg_ver_velocidade), &bg_param);
  gtk_box_pack_start(GTK_BOX(button_box), check_box, FALSE, TRUE, 6);

  label = gtk_label_new("Bolas Pequenas");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 6);

  label = gtk_label_new("Massa:");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 3);

  spin_button = gtk_spin_button_new_with_range(1, 100, 1);
  g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(cb_sb_bp_massa), &bp_param);
  gtk_box_pack_start(GTK_BOX(button_box), spin_button, FALSE, TRUE, 0);

  label = gtk_label_new("Nº Bolas:");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 3);

  spin_button = gtk_spin_button_new_with_range(1, 30, 1);
  g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(cb_sb_bp_num_bolas), &bp_param);
  gtk_box_pack_start(GTK_BOX(button_box), spin_button, FALSE, TRUE, 0);

  label = gtk_label_new("Velocidade Inicial:");
  gtk_widget_set_hexpand (label, FALSE);
  gtk_widget_set_halign (label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(button_box), label, FALSE, TRUE, 3);

  spin_button = gtk_spin_button_new_with_range(1, 10, 1);
  g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(cb_sb_bp_velocidade), &bp_param);
  gtk_box_pack_start(GTK_BOX(button_box), spin_button, FALSE, TRUE, 0);

  g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);
  gtk_main ();

  return 0;
}

