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

#define BALL0_X  200
#define BALL0_Y  30

#define BALL1_X  500
#define BALL1_Y  30

#define DFLT_NR_SMALL_BALLS   10
#define MAX_NR_SMALL_BALLS    10
#define DFLT_MASS_SMALL_BALLS 2.

#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

const gchar  *winTitle    = "Movimento Browniano" ;
glong   win_xlen    = 800 ;
glong   win_ylen    = 600 ;
gint    flag_sc     = 1   ;

glong   xlen        = 450 ;
glong   ylen        = 200 ;
gdouble dt          = 1.  ;
gdouble dt_max      = 10. ;

typedef struct _vector
{
  gdouble x;
  gdouble y;
}vector;

typedef struct Ball_
{
  gdouble raio;
  gdouble massa;
  vector  posicao;
  vector  velocidade;
}Ball;

typedef struct Components_
{
  Ball* bigBall;
  Ball* smallBall;
}Components;

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

void actualizar(Ball* ptr)
{
  ptr->posicao.x += ptr->velocidade.x;
  ptr->posicao.y += ptr->velocidade.y;
}

void colisoes_janela(Ball* ptr, GtkAllocation *alloc1)
{
  if(ptr->posicao.x > alloc1->width - ptr->raio)
    {
      ptr->posicao.x = alloc1->width - ptr->raio;
      ptr->velocidade.x *= -1;
    }
  else if(ptr->posicao.x < ptr->raio)
    {
      ptr->posicao.x = ptr->raio;
      ptr->velocidade.x *= -1;
    }
  else if(ptr->posicao.y > alloc1->height - ptr->raio)
    {
      ptr->posicao.y = alloc1->height - ptr->raio;
      ptr->velocidade.y *= -1;
    }
  else if (ptr->posicao.y < ptr->raio)
    {
      ptr->posicao.y = ptr->raio;
      ptr->velocidade.y *= -1;
    }
}

void desenhar(Ball* ptr, cairo_t *cr)
{
  cairo_set_source_rgb (cr, 1., 0., 0.);
  cairo_set_line_width (cr, 4.0);
  cairo_arc (cr, ptr->posicao.x, ptr->posicao.y, ptr->raio, 0, 2. * M_PI);
  cairo_stroke_preserve (cr);

  // Nota: ver o que acontece se se comentar a linha anterior e descomentar a seguinte
  //cairo_stroke (cr);

  cairo_set_source_rgb (cr, 0, 0, 1);
  cairo_fill (cr);
  cairo_stroke(cr);
}

void colisoes(Components *ptr)
{
  int i;
  for(i = 0; i < DFLT_NR_SMALL_BALLS; i++)
    {
      //calcular a distancia entre as bolas (em x e y)
      vector vec_aux;
      vec_aux.x = ptr->bigBall->posicao.x - ptr->smallBall[i].posicao.x;
      vec_aux.y = ptr->bigBall->posicao.y - ptr->smallBall[i].posicao.y;

      //calcular a magnitude do vector que separa as bolas
      gdouble aux_mag = sqrt((vec_aux.x * vec_aux.x) + (vec_aux.y * vec_aux.y));

      if (aux_mag <= (ptr->bigBall->raio + ptr->smallBall[i].raio))
        {
          //calcular o angulo do vector aux
          gdouble theta  = atan2(vec_aux.y, vec_aux.x);

          //precalcular os valores de seno e coseno
          gdouble seno   = sin(theta);
          gdouble coseno = cos(theta);

          /*Na situação de colisão as posições das bolas são relativas
            uma à outra. Por isso podemos o vector entre elas (vec_aux)
            como ponto de referência para calcular as rotações.
            Assim criamos um vector de referência (vec_ref) com coordenadas
            x e y inicializadas a 0 assumindo que o vector vec_temp irá
            rodar em torno de vec_ref
          */
          vector vec_ref = {.x = 0, .y = 0};
          vector vec_temp;
          vec_temp.x  = coseno * vec_aux.x + seno * vec_aux.y;
          vec_temp.y  = coseno * vec_aux.y - seno * vec_aux.x;

          // rotação das velocidades
          vector vec_vel_temp[2];
          vec_vel_temp[0].x  = coseno * ptr->smallBall[i].velocidade.x + seno * ptr->smallBall[i].velocidade.y;
          vec_vel_temp[0].y  = coseno * ptr->smallBall[i].velocidade.y - seno * ptr->smallBall[i].velocidade.x;
          vec_vel_temp[1].x  = coseno * ptr->bigBall->velocidade.x + seno * ptr->bigBall->velocidade.y;
          vec_vel_temp[1].y  = coseno * ptr->bigBall->velocidade.y - seno * ptr->bigBall->velocidade.x;

          /* Now that velocities are rotated, you can use 1D
           conservation of momentum equations to calculate
           the final velocity along the x-axis. */
          vector vec_final[2];

          /*Lei da Conservação do momento*/
          vec_final[0].x = ((ptr->smallBall[i].massa - ptr->bigBall->massa) * vec_vel_temp[0].x + 2 * ptr->bigBall->massa * vec_vel_temp[1].x) / (ptr->smallBall[i].massa + ptr->bigBall->massa);
          vec_final[0].y = vec_vel_temp[0].y;

          // final rotated velocity for b[0]
          vec_final[1].x = ((ptr->bigBall->massa - ptr->smallBall[i].massa) * vec_vel_temp[1].x + 2 * ptr->smallBall[i].massa * vec_vel_temp[0].x) / (ptr->smallBall[i].massa + ptr->bigBall->massa);
          vec_final[1].y = vec_vel_temp[1].y;

          // hack to avoid clumping
          vec_ref.x  += vec_final[0].x;
          vec_temp.x += vec_final[1].x;

          /* Rotate ball positions and velocities back
           Reverse signs in trig expressions to rotate
           in the opposite direction */
          // rotate balls

          vector b_final[2];
          b_final[0].x = coseno * vec_ref.x - seno * vec_ref.y;
          b_final[0].y = coseno * vec_ref.y + seno * vec_ref.x;
          b_final[1].x = coseno * vec_temp.x - seno * vec_temp.y;
          b_final[1].y = coseno * vec_temp.y + seno * vec_temp.x;

          // update balls to screen position
          ptr->bigBall->posicao.x = ptr->smallBall[i].posicao.x + b_final[1].x;
          ptr->bigBall->posicao.y = ptr->smallBall[i].posicao.y + b_final[1].y;

          ptr->smallBall[i].posicao.x += b_final[0].x;
          ptr->smallBall[i].posicao.y += b_final[0].y;


          // update velocities
          ptr->smallBall[i].velocidade.x = coseno * vec_final[0].x - seno * vec_final[0].y;
          ptr->smallBall[i].velocidade.y = coseno * vec_final[0].y + seno * vec_final[0].x;
          ptr->bigBall->velocidade.x = coseno * vec_final[1].x - seno * vec_final[1].y;
          ptr->bigBall->velocidade.y = coseno * vec_final[1].y + seno * vec_final[1].x;
        }
    }
}

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
  return FALSE;
}

gboolean
cb_sb_bg_massa (GtkWidget *widget ,
                gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_massa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bg massa: %d\n", ((config_bg*)data)->bg_massa);
  return FALSE;
}

gboolean
cb_sb_bg_velocidade (GtkWidget *widget ,
                     gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_velocidade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bg velocidade: %d\n", ((config_bg*)data)->bg_velocidade);
  return FALSE;
}

gboolean
cb_chkb_bg_ver_sozinha (GtkWidget *widget ,
                        gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_ver_sozinha = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  g_print("bg ver sozinha: %d\n", ((config_bg*)data)->bg_ver_sozinha ? 1 : 0);
  return FALSE;
}

gboolean
cb_chkb_bg_ver_velocidade (GtkWidget *widget ,
                           gpointer   data   )
{
  config_bg *bg = (config_bg*)data;
  bg->bg_ver_velocidade = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  g_print("bg ver velocidade: %d\n", ((config_bg*)data)->bg_ver_velocidade ? 1 : 0);
  return FALSE;
}

gboolean
cb_sb_bp_massa (GtkWidget *widget ,
                gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_massa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp massa: %d\n", ((config_bp*)data)->bp_massa);
  return FALSE;
}

gboolean
cb_sb_bp_num_bolas (GtkWidget *widget ,
                    gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_num_bolas = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp num bolas: %d\n", ((config_bp*)data)->bp_num_bolas);
  return FALSE;
}

gboolean
cb_sb_bp_velocidade (GtkWidget *widget ,
                     gpointer   data   )
{
  config_bp *bp = (config_bp*)data;
  bp->bp_velocidade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
  g_print("bp velocidade: %d\n", ((config_bp*)data)->bp_velocidade);
  return FALSE;
}

gboolean
cb_draw_event (GtkWidget  *darea ,
               cairo_t    *cr    ,
               gpointer    data  )
{

  gchar          texto[128] ;
  GtkAllocation  alloc1 ;
  int i;

  Components *myComponents = (Components*)data;

  gtk_widget_get_allocation (darea, &alloc1);

  /*
  cairo_move_to (cr, 20, 20);
  cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, 18);
  cairo_set_source_rgb (cr, 0., 0., 1.);
  sprintf (texto, "Exemplo de funcionamento de GTK+ Cairo            Velocidade: %.0lf", dt);
  cairo_show_text (cr, texto);
  cairo_stroke (cr);
  */

  // Small Balls


  for(i = 0; i < DFLT_NR_SMALL_BALLS; i++)
    {
      actualizar(&myComponents->smallBall[i]);
      desenhar(&myComponents->smallBall[i], cr);
      colisoes_janela(&myComponents->smallBall[i], &alloc1);
    }



  //Big Ball
  actualizar(myComponents->bigBall);
  desenhar(myComponents->bigBall, cr);
  colisoes_janela(myComponents->bigBall, &alloc1);

  /* Para cada bola vamos :*/
  /* Fazer o update das posições */
  /* fazer o rendering na drawing area */
  /* verificar se existem colisões com as bordas da drawing area */
  /* verificar se existem colisões entre as bolas peuqenas com a grande*/

  colisoes(myComponents);

  return FALSE;
}

gboolean time_handler (GtkWidget *widget)
{
  if (!gtk_widget_get_window (widget))
    return FALSE;

  gtk_widget_queue_draw(widget);

  return TRUE;
}

int main(int argc, char *argv[])
{
  //criar a janela
  GtkWidget *window, *draw_area, *frame, *main_box, *button_box, *button;

  GtkWidget *label, *spin_button, *check_box;

  int i;

  config_bg bg_param = {(guint8)BG_MASSA_DFT, (guint8)BG_VEL_DFT, FALSE, FALSE};
  config_bp bp_param = {(guint8)BP_MASSA_DFT, (guint8)BP_VEL_DFT, (guint8)BP_NUM_BOLAS_DFT};

  const gdouble radius = 10;

  //Create init Small Ball Array
  Ball *vSmallBall = (Ball*)malloc(MAX_NR_SMALL_BALLS * sizeof(Ball));

  srand ((unsigned) time (NULL));


  for(i = 0; i < DFLT_NR_SMALL_BALLS; i++)
    {
      vSmallBall[i].raio         = radius;
      vSmallBall[i].posicao.x    = rand () % win_xlen + 1;
      vSmallBall[i].posicao.y    = rand () % win_ylen + 1;
      vSmallBall[i].velocidade.x =    3.;
      vSmallBall[i].velocidade.y =    3.;
      vSmallBall[i].massa         =  DFLT_MASS_SMALL_BALLS;
    }

  //create Big Ball
  Ball* theBigBall            = (Ball*)malloc(sizeof(Ball));
  theBigBall->raio            = 2*radius;
  //theBigBall->posicao.x      = rand () % win_xlen + 1;
  //theBigBall->posicao.y      = rand () % win_ylen + 1;
  theBigBall->posicao.x      = 0.;
  theBigBall->posicao.y      = 0.;
  theBigBall->velocidade.x   =   3.;
  theBigBall->velocidade.y   =   3.;
  theBigBall->massa           =  3 * DFLT_MASS_SMALL_BALLS;

  Components* myComponents = (Components*)malloc(sizeof(Components));

  myComponents->bigBall = theBigBall;
  myComponents->smallBall = vSmallBall;

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
  g_signal_connect (G_OBJECT(draw_area), "draw", G_CALLBACK(cb_draw_event), myComponents);
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

  g_timeout_add (10, (GSourceFunc) time_handler, draw_area);

  gtk_widget_show_all(window);
  gtk_main ();

  free(myComponents);
  free(theBigBall);
  free(vSmallBall);

  return 0;
}

