#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>
#include <math.h>
#include <time.h>
#define PI 3.14159
#define EXPLOSAO_MAX 100
#define EXPLOSAO_VELOCIDADE 1.3
#define TAMANHO_JOGADOR 2
#define ESPERA 15

int loading();
int tela_inicial();
int menu();
int inicio();
int atualizar_placar();
int desenhar_placar();
int final();
void mover_jogador(int n);
void pintar_tela(int n, int cor, int tamanho);
void colisao_jogador(int p);
void piscar_jogador(int p);
void explosao(int n_jogador);
             


struct jogador{
  float o;
  float x;
  float y;
  int cor;
  char tecla[3];
  int morto;
  int inativo;
  int ponto;
  int esperando;
  int ja_piscou;
  float tamanho_explosao;
};

struct jogador jogadores[6];

int v, l, i;
int buraco = 0;
int tamanho_buraco[5]= {0};
int p =0;
int nmorto=0;
int botao=0;
int n,rodada,h =0;
BITMAP *placar = NULL;



inline void init()
{
	allegro_init();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);

	install_timer();
	install_keyboard();
	install_mouse();

}

inline void deinit()
{
	clear_keybuf();
	allegro_exit();
}


int main()
{
    init();
    srand(time(NULL));
    //loading();
    //tela_inicial();

    while (!key[KEY_ESC])
        {

         rest(25);
         jogadores[0].tecla[0] = key[KEY_LEFT];
         jogadores[0].tecla[1] = key[KEY_DOWN];
         jogadores[0].tecla[2] = key[KEY_UP];
         jogadores[1].tecla[0] = key[KEY_1];
         jogadores[1].tecla[1] = key[KEY_Q];
         jogadores[1].tecla[2] = key[KEY_2];
         jogadores[2].tecla[0] = key[KEY_LCONTROL];
         jogadores[2].tecla[1] = key[KEY_ALT];
         jogadores[2].tecla[2] = key[KEY_X];
         jogadores[3].tecla[0] = key[KEY_M];
         jogadores[3].tecla[1] = key[KEY_COMMA];
         jogadores[3].tecla[2] = key[KEY_N];
         jogadores[4].tecla[0] = key[KEY_PLUS_PAD];
         jogadores[4].tecla[1] = key[KEY_MINUS_PAD];
         jogadores[4].tecla[2] = key[KEY_9_PAD];

         jogadores[0].cor = makecol(255, 255, 0);
         jogadores[1].cor = makecol(255, 0, 0);
         jogadores[2].cor = makecol(0, 255,0);
         jogadores[3].cor = makecol(0, 100, 100);
         jogadores[4].cor = makecol(255, 0, 255);

         if(rodada==0) //verifica se È o inicio de uma rodada partidas, se for entra no if.
          {
           menu();
           inicio();

           rodada=1;// da inicio a rodada
          }

         for(p=0;p<5;p++) //executa o codigo 5 vezes, uma vez para cada jogador.
           {

            if ( jogadores[p].inativo == 1 || (jogadores[p].morto == 1 && jogadores[p].esperando < ESPERA) ){
             jogadores[p].esperando++;
             continue; // se o jogador da vez estiver morto

            }
          
            mover_jogador(p);

            buraco=rand() % 150 + 1;//cria um numero aleatÛrio para o desenho dos buracos na cobra.
            int cor ;
            if (buraco==1)
              tamanho_buraco[p]=0;// 0.66% de chance de sair um buraco por ciclo de desenho
            else
               
              cor = (jogadores[p].morto==1 && jogadores[p].tamanho_explosao == 0 ) ? makecol(255, 255, 255) : jogadores[p].cor;
              pintar_tela(p, cor, TAMANHO_JOGADOR);

            if(tamanho_buraco[p]<5)
             {
              circlefill(screen, jogadores[p].x -(2.5*cos(jogadores[p].o)), jogadores[p].y-(2.5*sin(jogadores[p].o)), 2, makecol(0,0,0));//desenha preto para impress„o de ser um buraco durante 5 ciclos de desenho
              tamanho_buraco[p]++;
             }

             colisao_jogador(p);
             piscar_jogador(p);
            
          }

         while(nmorto==n-1 && !key[KEY_ESC]) // entra quando a partida chega ao fim
            {
              clear_keybuf();
              botao = readkey();
              if ((botao >> 8) == KEY_SPACE)
               {
               // nmorto=0; //zera o numero de mortos
                inicio(); // recalcula as posiÁıes iniciais
                              for(p=0;p<5;p++)  //verifica se os jogadores  est„o com os pontos maximos.
                {
                 if(jogadores[p].ponto >= n*10-10) { final(); rodada=0; } //chama a tela dos scores finais
                }                                                         // e da fim a rodada
               }

            }
       } //fecha while =!esc
    deinit();
	return 1;
}//Fecha main

void pintar_tela(int n_jogador, int cor, int tamanho=2){
 circlefill(screen, jogadores[n_jogador].x , jogadores[n_jogador].y  ,tamanho , cor);// desenha cobra normal
}

void mover_jogador(int n_jogador){
 float velocidade = 2.5;
  if(jogadores[n_jogador].morto==1){
      velocidade = velocidade/2;
  }
                                                      // ou inativo,n„o faz nada, ou seja ,
  if (jogadores[n_jogador].tecla[0]) jogadores[n_jogador].o-=0.07;           // pula a execuÁ„o para o prÛximo jogador
  if (jogadores[n_jogador].tecla[1]) jogadores[n_jogador].o+=0.07;//----> --> Caso o jogador estiver pressionando sua tecla
  if (jogadores[n_jogador].tecla[2] || jogadores[n_jogador].tamanho_explosao > 0){
     explosao(n_jogador);
     velocidade=0;
  } 
                                                         //incrementa o valor de "o" em 4 graus
  jogadores[n_jogador].x+=velocidade*cos(jogadores[n_jogador].o); //faz com que a curva seja possivel,alterando apenas o
  jogadores[n_jogador].y+=velocidade*sin(jogadores[n_jogador].o); //‚ngulo "o" da direÁ„o da cobra

}

void explosao(int n_jogador){
  if (jogadores[n_jogador].tamanho_explosao > EXPLOSAO_MAX || jogadores[n_jogador].morto == 0 )
    return;

  pintar_tela(n_jogador, jogadores[n_jogador].cor, jogadores[n_jogador].tamanho_explosao);
  jogadores[n_jogador].tamanho_explosao+= EXPLOSAO_VELOCIDADE;

}

void colisao_jogador(int p){
  if (jogadores[p].morto == 1)
    return;
  int preto = makecol(0, 0, 0);
  int branco = makecol(255, 255, 255);

  int pixel_a_frent = getpixel(screen,jogadores[p].x+3.7*cos(jogadores[p].o) ,jogadores[p].y+3.7*sin(jogadores[p].o));
  int pixel_a_esq = getpixel(screen,jogadores[p].x+3.65*cos(jogadores[p].o+0.7854) ,jogadores[p].y+3.65*sin(jogadores[p].o+0.7854));
  int pixel_a_dir = getpixel(screen,jogadores[p].x+3.65*cos(jogadores[p].o-0.7854) ,jogadores[p].y+3.65*sin(jogadores[p].o-0.7854));
    if (pixel_a_frent != preto && pixel_a_frent != branco)
          //Detector de colisao verifica se existe um pixel preto imediatamente na frente da cabeÁa da cobra
   {
    jogadores[p].morto = 1;//mata o jogador se estiver
    nmorto++;
    atualizar_placar();
   }
   // se n„o tiver na frente
  else if(pixel_a_esq != preto &&  pixel_a_esq != branco)
         //verifica 45∫ ‡ esquerda
        {
         jogadores[p].morto = 1;
         nmorto++;
         atualizar_placar();
        }

  else if(pixel_a_dir != preto && pixel_a_dir != branco )
        { //direita
         jogadores[p].morto = 1;
         nmorto++;
         atualizar_placar();
        }
}

void piscar_jogador(int n_jogador){
   if (jogadores[p].inativo == 0 && jogadores[p].ja_piscou == 0)
     {                                            //”””””i o pisca pisca!!!!!
       for(h=0; h<10; h++)
          {
           circlefill(screen, jogadores[p].x , jogadores[p].y , 2, makecol(0,0,0));
           rest(50);
           circlefill(screen, jogadores[p].x , jogadores[p].y , 2, jogadores[p].cor);
           rest(50);
          }
       jogadores[p].ja_piscou = 1;
      }
}

int loading()
{

    int xm = 150;
    int ym = 220;
    //float matriz[805][605];

    BITMAP *bmp = NULL;
    bmp = load_bitmap("loading1.bmp",NULL );
    blit(bmp, screen, 0, 0, 0, 0, bmp->w, bmp->h);
    rest(2000);

    rectfill(screen, 140, 410, 665, 518, makecol(255, 255, 0));
    rectfill(screen, 150, 420, 655, 508, makecol(0, 0, 0));

    for(xm=154;xm<651;xm++)
      {
       for(ym=424;ym<504;ym++)
         {
          //matriz[xm][ym] = 0;
          if((ym>424 && ym<504) && (xm>154 && xm<651)) putpixel(screen, xm, ym,makecol(255, 255, 0) );
         }
       rest(5);
}
return 1;
}

int tela_inicial()
{
    int i=1;

    while (!keypressed()){
          i++;

          BITMAP *tela = NULL;
	      tela = load_bitmap("tela.bmp",NULL );
          blit(tela, screen, 0, 0, 0, 0, tela->w, tela->h);

          BITMAP *press = NULL;
          press = load_bitmap("comspaceinit.bmp",NULL );

          if(i%2 == 0){
                   blit(press, screen, 0, 0, 175, 380, tela->w, tela->h);
                   rest(700);
                   }
          else destroy_bitmap(press);
          rest(300);
    }

    return 1;
}

int menu()
{
    clear_bitmap(screen);
    BITMAP *menu = NULL;
    menu = load_bitmap("menu.bmp",NULL );
    blit(menu, screen, 0, 0, 0, 0, menu->w, 150);
    blit(menu, screen, 0, 150, 0, 575, menu->w, menu->h);

    textout_ex(screen, font, "<left , down>", 100, 200, jogadores[0].cor, -1);
    textout_ex(screen, font, "<1 , Q>", 100, 240, jogadores[1].cor, -1);
    textout_ex(screen, font, "<left Control , left Alt>", 100, 280, jogadores[2].cor, -1);
    textout_ex(screen, font, "<M , ,>", 100, 320, jogadores[3].cor, -1);
    textout_ex(screen, font, "<+ , ->", 100, 360, jogadores[4].cor, -1);


    for(p=0;p<6;p++)  jogadores[p].inativo = 1;

    while(n<=1 || !key[KEY_SPACE])
        {
          if(key[KEY_ESC]) break;
          if(key[KEY_LEFT] && jogadores[0].inativo ==1 )
            {
              textout_ex(screen, font, "PRONTO", 350, 200,
              jogadores[0].cor, -1);
		      jogadores[0].inativo =0;
		      n++;
            }
         if(key[KEY_DOWN] && jogadores[0].inativo == 0)
            {
              textout_ex(screen, font, "PRONTO", 350, 200,
		      makecol(0, 0, 0), -1);
		      jogadores[0].inativo =1;
		      n--;
            }
         if(key[KEY_1] && jogadores[1].inativo == 1 )
            {
              textout_ex(screen, font, "PRONTO",350, 240,
              jogadores[1].cor, -1);
              jogadores[1].inativo =0;
		      n++;
            }

         if(key[KEY_Q]&& jogadores[1].inativo ==0)
            {
              textout_ex(screen, font, "PRONTO", 350, 240,
		      makecol(0, 0, 0), -1);
		      jogadores[1].inativo =1;
		      n--;
            }

         if(key[KEY_LCONTROL] && jogadores[2].inativo ==1)
            {
              textout_ex(screen, font, "PRONTO", 350, 280,
		      jogadores[2].cor, -1);
		      jogadores[2].inativo =0;
              n++;
            }

         if(key[KEY_ALT] && jogadores[2].inativo ==0)
            {
              textout_ex(screen, font, "PRONTO", 350, 280,
		      makecol(0, 0, 0), -1);
		      jogadores[2].inativo =1;
		      n--;
            }

         if(key[KEY_M] && jogadores[3].inativo ==1)  //4
            {
              textout_ex(screen, font, "PRONTO", 350, 320,
		      jogadores[3].cor, -1);
	   	      jogadores[3].inativo =0;
              n++;
            }

         if(key[KEY_COMMA] && jogadores[3].inativo ==0)
            {
              textout_ex(screen, font, "PRONTO", 350, 320,
		      makecol(0, 0, 0), -1);
		      jogadores[3].inativo =1;
		      n--;
            }//baixo

         if(key[KEY_PLUS_PAD] && jogadores[4].inativo ==1) //5
            {
              textout_ex(screen, font, "PRONTO", 350, 360,
		      jogadores[4].cor, -1);
		      jogadores[4].inativo =0;
              n++;
            }

         if(key[KEY_MINUS_PAD] && jogadores[4].inativo ==0)
            {
              textout_ex(screen, font, "PRONTO", 350, 360,
	   	      makecol(0, 0, 0), -1);
		      jogadores[4].inativo =1;
		      n--;
            }

}

return 1;
}

int inicio()
  {
      for(p=0;p<5;p++)
        {
          jogadores[p].esperando = 0 ;
          jogadores[p].morto = 0 ;
          jogadores[p].x = rand() % 400 + 100;
          jogadores[p].y = rand() % 400 + 100;
          jogadores[p].o = (rand() % 62831 + 0)/10000;
          jogadores[p].ja_piscou = 0;
          jogadores[p].tamanho_explosao = 0;
          rest(3);
         }
      clear_bitmap(screen);
      rectfill(screen,0,0, 650, 600, makecol(128, 128, 0));
      rectfill(screen, 5, 5, 645, 595, makecol(0, 0, 0));


      placar = load_bitmap("placar.bmp",NULL );
      for(p=0;p<5;p++)
      {
          if (jogadores[p].inativo==1);
          else textprintf_ex(placar, font, 40, 100+(30*p), jogadores[p].cor,
		  -1, "Score: %d", jogadores[p].ponto);
	  }
	  blit(placar, screen, 0, 0, 650, 0, placar->w, placar->h);

      nmorto=0;
      return 1;

}

int atualizar_placar()
{

    placar = load_bitmap("placar.bmp",NULL );
    for(p=0;p<5;p++)
      {
        if(jogadores[p].morto==1 || jogadores[p].inativo==1);
        else jogadores[p].ponto++;
        if (jogadores[p].inativo==1);
        else textprintf_ex(placar, font, 40, 100+(30*p), jogadores[p].cor,
		   -1, "Score: %d", jogadores[p].ponto);
    }
  blit(placar, screen, 0, 0, 650, 0, placar->w, placar->h);
  return 1;
}

int desenhar_placar()
{
    placar = load_bitmap("placar.bmp",NULL );
     for(p=0;p<5;p++)
      {
          if (jogadores[p].inativo==1);
          else textprintf_ex(placar, font, 40, 100+(30*p), jogadores[p].cor,
		    -1, "Score: %d", jogadores[p].ponto);
	  }
	blit(placar, screen, 0, 0, 650, 0, placar->w, placar->h);
	return 1;
}

int final()
   {
   clear_bitmap(screen);
   for(p=0;p<5;p++)
   {

        textprintf_ex(screen, font, 350, 155 +(p*20) , jogadores[p].cor,
		    -1, "Score: %d", jogadores[p].ponto);
		      jogadores[p].ponto = 0;

   }
   rest(5000);
   clear_keybuf();
   n=0;
   readkey();
   return 1;
    }


END_OF_MAIN()
