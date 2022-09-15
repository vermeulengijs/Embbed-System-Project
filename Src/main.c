
/**
= Titel van het project: Game of Life =

== door:  Vermeulen Gijs ==

== Korte beschrijving ==
Conway's game of life soms wel life genoemd is een cellulaire automaat.

Deze bevind zich in een 2 dimensionaal raster met vierkante "cellen" die levend of dood kunnen zijn
en die zich volgens vastgelegde regels ontwikkelen en daarbij patronen kunnen vormen.

Dit algoritme volgt 3 hoofd regels:
	Als een levende cel door 2 of 3 levende buurcellen omgeven wordt blijft deze cel leven
	Als een levende cel door 4 of meer levende buurcellen omgeven wordt gaat deze cel dood
	Als een dode cel door precies 3 gekleurde buurcellen omgeven wordt gaat deze cel leven

Dit algoritme heb ik in labview gemaakt.
Het labview programma gaat als server werken en het embedded systeem gaat hier via TCP/IP data van halen en weergeven op zijn LCD.
De client kan ook zelf dingen naar de server sturen.
	- via knoppen kunnen een aantal vaste patronen verstuurd worden
	- via de "eigen "knop" kan men iets in de server applicatie tekenen of als text string ingeven.
Oneindig veel plezier !
== Libs, compiler en apps ==

 - lwip version LwIP V2.0.3 version ( de versie van uit het labo bestand )
 - CubeMX version 4.27.0
 - System workbench (SW4STM32) FW_F7_V1.15.0
 - Labview 2013

== Bestanden aangemaakt/aangepast ==

   Aangepaste bestanden:
	- LCD_LwIP  ( gestart van het bestand op canvas )
   Aangemaakte bestanden:
	- Labview programma ( server )
	- alle afbeeldingen gebruikt in het project ( paint )

== Opmerkingen ==
voor het labview programma te starten :
					- kan op zich werken :
						- zie dat communicatie uit staat
						- zie dat milliseconds to wait minstens op 500 staat
						- begin het programma door via het keuze menu data in de array te sturen.
						( de keuzes :init, next , niks en communicatie versturen geen data in de array !)
						- ( hij zet data altijd in het midden van de array )
						- ( bij eigen zet hij de data in het begin via de eigen array in het programma
						- LET THE GAME BEGIN !
					- als server :
						- zie dat communicatie aan staat ( knop )
						- zie dat milliseconds to wait op 0 staat
						- zie dat grote op 20 ( server verstuurd 1 info pakket en 1 data pakket )tot 30 staat ( server verstuurd 1 info pakket en 2 data pakketten )
						( als we te groot gaan kan LWIP de data niet aan en verliezen we pakketten of worden pakketten random genegeerd )
						- zet de client nu aan ( hij connecteerd beter als je voor op start te klikken eerst is op stop klikt )
						- op de client kan je terwijl het programma bezig is data in de array sturen via de knoppen.
						- De server gaat maar 1 keer rekening houden met de data dat een knop verstuurd ookal blijft de knop actief ( de knop op de LCD licht groen op)
						- je kan maar 1 knop actief hebben tegelijkertijd
						- ( door nog eens op de knop te duwen desactiveer je de knop )
						- soms als je te laat drukt zal er nog een fase tussen zitten ( hangt af van de timing wanneer je klikt en wanneer de pakketten verstuurd worden)
						- LET THE FUN BEGIN

De server loopt altijd 1 fase voor op de client.
( als de client zegt trying to connect kan je best eens op stop drukken en dan terug op start drukken , ik heb ondervonden dat hij dan direct binnen raakt )

== Bronvermeldingen ==
https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

== Optioneel: Extra's ==



  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal.h"
#include "lwip.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <tcp.h>

/* USER CODE BEGIN Includes */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"

// image data
#include "stop_knop_data.h"
#include "start_KNOP_data.h"
#include "levende_cel_data.h"
#include "dode_cel_data.h"
#include "Block_knop_inactive_data.h"
#include "custom_knop_inactive_data.h"
#include "GLIDER_knop_inactive_data.h"
#include "knop_active_data.h"
#include "LWSS_knop_inactive_data.h"
#include "PULSAR_knop_inactive_data.h"
#include "INIT_knop_inactive_data.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

DMA2D_HandleTypeDef hdma2d;

LTDC_HandleTypeDef hltdc;

UART_HandleTypeDef huart1;

SDRAM_HandleTypeDef hsdram1;



/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

// globale variabelen

char x_tcp_buffer[1024];
uint8_t uc_Closed = 0;
uint8_t uc_Gcounter = 0;

uint8_t uc_start = 1;
uint8_t uc_Data = 0;
uint16_t us_Glen = 0;
int16_t us_Ga = 0;			// als er meerdere data paketten zijn de x waarde bijhouden
int16_t us_Gj = 0;			// als er meerdere data paketten zijn de y waarde bijhouden

// defines

#define x10  10
#define x85  85
#define y20  20
#define y60  60
#define y100 100
#define y140 140
#define y180 180
#define y220 220

#define PORT_SERVER 20
#define PORT_CLIENT 40000

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config( void );
static void MX_GPIO_Init( void );
static void MX_LTDC_Init( void );
static void MX_USART1_UART_Init( void );
static void MX_DMA2D_Init( void );
static void MX_FMC_Init( void );


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void prvdraw( uint16_t, uint8_t );
static uint8_t ucomzetten( uint16_t );

/* ---------------------------------------------------------------------------------*/
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

int16_t _write( int16_t us_file, char *ptr, int16_t us_len1 )
{
    HAL_StatusTypeDef xStatus;
    switch (us_file) {
    case STDOUT_FILENO: /*stdout*/
		xStatus = HAL_UART_Transmit(&huart1, (uint8_t*)ptr, us_len1, HAL_MAX_DELAY);
		if ( xStatus != HAL_OK ) {
			errno = EIO;
			return -1;
		}
        break;
    case STDERR_FILENO: /* stderr */
		xStatus = HAL_UART_Transmit(&huart1, (uint8_t*)ptr, us_len1, HAL_MAX_DELAY);
		if ( xStatus != HAL_OK ) {
			errno = EIO;
			return -1;
		}
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return us_len1;
}
/* ---------------------------------------------------------------------------------*/
static void Close (struct tcp_pcb *pcb )
{

    tcp_close(pcb);
    BSP_LCD_ClearStringLine(0);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) "Disconnected", CENTER_MODE);
	// globale waardes goed zetten voor de volgende connectie
    uc_Gcounter = 0;
    us_Glen = 0;
    us_Ga = 0;
    us_Gj = 0;
    uc_start = 1;
}
/* ---------------------------------------------------------------------------------*/
static err_t Ontvangen( void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err )
{
	uint16_t us_Teller;
    uint16_t us_len;
    char *pc;
    uc_Gcounter++;
    LWIP_UNUSED_ARG( arg );
    if ( err == ERR_OK && p != NULL )			// er is geen error en er zit data in de databuffer
    {
    	tcp_recved( pcb, p->tot_len );
    	printf("stuur ack naar server\r\n");
    	pc = (char *)p->payload;
    	us_len =p->tot_len;
    	for( us_Teller = 0; us_Teller<us_len; us_Teller++ )
    	{
    		x_tcp_buffer[us_Teller] = pc[us_Teller];
    	}
    	prvdraw(us_len, uc_Gcounter);
    }
    else									  // stel er gaat iets mis ( een error bv ) maak de buffer vrij
    {
    	pbuf_free( p );
    }

    pbuf_free( p );

    if( err == ERR_OK && p == NULL )			// er is geen error en de databuffer is leeg gemaakt sluit de connectie
    {
        Close( pcb );
    }
    return ERR_OK;
}
/* ---------------------------------------------------------------------------------*/
static err_t Connected( void *arg, struct tcp_pcb *pcb, err_t err )

{
    LWIP_UNUSED_ARG( arg );
    LWIP_UNUSED_ARG( err );
    if ( err == ERR_OK )
    {
    	printf("connected\r\n");
    	BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) "connected to the server", CENTER_MODE);
    	if ( uc_Data == 0 )
    	{
    		tcp_write( pcb, "0", 1, 0 );
    	}
    	if ( uc_Data == 1 )
    	{
    		tcp_write( pcb, "1", 1, 0 );
    	}
    	if ( uc_Data == 2 )
    	{
    		tcp_write( pcb, "2", 1, 0 );
    	}
    	if ( uc_Data == 3 )
    	{
    		tcp_write( pcb, "3", 1, 0 );
    	}
    	if ( uc_Data == 4 )
    	{
    		tcp_write( pcb, "4", 1, 0 );
    	}
    	if ( uc_Data == 5 )
    	{
    		tcp_write( pcb, "5", 1, 0 );
    	}
    	if ( uc_Data == 6 )
    	{
    		tcp_write( pcb, "6", 1, 0 );
    	}
    	if ( uc_Data == 7 )
    	{
    		tcp_write( pcb, "7", 1, 0 );
    	}
    	tcp_sent( pcb, NULL );
    	tcp_recv( pcb, Ontvangen );
    }

    if ( err == ERR_MEM )
    {
    	printf("Oops ran out of memory\r\n");
    	Close( pcb );
    }


    return ERR_OK;
}
/* ---------------------------------------------------------------------------------*/
/* functie draw
 * Gaat alle data op het LCD schrijven
 * pakket 1 is geen data pakket en moet worden verwerkt door de functie omzetten
 * pakket 2 en hoger zijn data pakketten
 * param grootte : hoeveel data er in het pakket zit
 * param pakket : het hoeveelste pakket data we aan zitten
 */
 void prvdraw( uint16_t us_Grootte, uint8_t us_pakket )
{
	 uint16_t us_Counter;
	 uint16_t us_X = 170;			// moet groter zijn dan 8 bit !!
	 uint16_t us_Y = 32;			// moet groter zijn dan 8 bit !!
	 uint16_t us_Volgend = 0;
	 uint8_t uc_start_x = 170;
	 uint8_t uc_start_y = 32;

	 if ( us_pakket == 1)
	 {
		 us_Glen = ucomzetten(us_Grootte);
	 }
	 // het tweede pakket is het DATA pakket
	 if ( us_pakket > 1  )
	 {
 		 // vanaf pakket 3 komt moet hij rekening houden met het punt waar pakket 2 is gestopt met tekenen
 		 //: waarde zit in us_Ga deze gaan we lokaal bijhouden in
		 us_Volgend = us_Ga;
	 	 for (us_Counter = us_Volgend ; us_Counter < us_Grootte + us_Volgend ; us_Counter++)
	 	 {
	 		    if ( ( us_Counter > 0) && (us_Counter % us_Glen == 0 ) )
	 		    {
	 		    	// als de counter deelbaar is door de lengte en groter dan 0 moet er naar de volgende lijn gegaan worden
	 		    	us_Ga = 0;
	 		    	us_Gj++;
	 		    }

	 		    us_X = uc_start_x + us_Ga * 8;	   // X coodinaat
	 		    us_Y = uc_start_y + us_Gj * 8;     // Y coordinaat

	 			if ( x_tcp_buffer[us_Counter - us_Volgend] == '0' )
	 	 		{
	 	 			WDA_LCD_DrawBitmap((void * ) DODE_CEL_DATA,us_X,us_Y,DODE_CEL_DATA_X_PIXEL,DODE_CEL_DATA_Y_PIXEL,DODE_CEL_DATA_FORMAT);
	 	 		}
	 	 		if ( x_tcp_buffer[us_Counter - us_Volgend] == '1' )
	 	 		{
	 	 			WDA_LCD_DrawBitmap((void * ) LEVENDE_CEL_DATA,us_X,us_Y ,LEVENDE_CEL_DATA_X_PIXEL,LEVENDE_CEL_DATA_Y_PIXEL,LEVENDE_CEL_DATA_FORMAT);
	 	 		}
	 		   	us_Ga++;
	 	 }
	 }
}
 /* ---------------------------------------------------------------------------------*/
 /*	Functie omzetten
  * Gaat de string uit het eerste data pakket van de server omzetten in een numerieke waarde
  * param Grootte : Hoeveel data er in het pakket zit
  * ret : in numerieke waarde hoe lang de rijen zijn
  */
uint8_t ucomzetten( uint16_t us_Grootte )
{
	 // het eerste pakket zit de grootte van de array in
	 // deze staat in ASCI code dus moeten we omzetten in een numeriek waarde
	 uint16_t us_i =0 ;
	 uint8_t uc_a = 0;
	 uint8_t uc_ASCI = 0;
	 uint8_t uc_waardes[10] = {0,1,2,3,4,5,6,7,8,9};
	 //0: ASCI 48
	 //1: ASCI 49
	 //2: ASCI 50
	 //3: ASCI 51
	 //4: ASCI 52
	 //5: ASCI 53
	 //6: ASCI 54
	 //7: ASCI 55
	 //8: ASCI 56
	 //9: ASCI 57
	 for ( us_i = 0 ; us_i < us_Grootte; us_i++ )
	 {
	 	 uc_ASCI =  (uint8_t)(x_tcp_buffer[us_i]);
	 	 switch (uc_ASCI)
	 	{
	 		 case 48 :	//0
	 		 if ( us_i == 0 && us_Grootte > 1 )
	 		{
	 			 uc_a += 10 * uc_waardes[0];
	 		 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a +=  + uc_waardes[0];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					uc_a += 1 * uc_waardes[0];
	 				 }
	 				 break;
	 			 case 49 :	//1
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[1];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a += + uc_waardes[1];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[1];
	 				 }
	 				 break;
	 			 case 50 :	//2
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[2];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a += + uc_waardes[2];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[2];
	 				 }
	 				 break;
	 			 case 51 :	//3
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[3];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a += + uc_waardes[3];
	 				 }
	 				 if ( us_i == 1 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[3];
	 				 }
	 				 break;
	 			 case 52 :	//4
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[4];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a += + uc_waardes[4];
	 				 }
	 				 if ( us_i == 1 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[4];
	 				 }
	 				 break;
	 			 case 53 :	//5
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[5];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a =  + uc_waardes[5];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[5];
	 				 }
	 				 break;
	 			 case 54 :	//6
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[6];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a +=  + uc_waardes[6];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[6];
	 				 }
	 				 break;
	 			 case 55 :	//7
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[7];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a +=  + uc_waardes[7];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[7];
	 				 }
	 				 break;
	 			 case 56 :	//8
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[8];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a += + uc_waardes[8];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[8];
	 				 }
	 				 break;
	 			 case 57 :	//9
	 				 if ( us_i == 0 && us_Grootte > 1 )
	 				 {
	 					 uc_a += 10 * uc_waardes[9];
	 				 }
	 				 if ( us_i == 0 && us_Grootte <= 1 )
	 				 {
	 					 uc_a +=  + uc_waardes[9];
	 				 }
	 				 if ( us_i == 1 )
	 				 {
	 					 uc_a += 1 * uc_waardes[9];
	 				 }
	 				 break;
	 			 }
	 }
	 return uc_a;
 }
/* ---------------------------------------------------------------------------------*/
 /* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main( void )
{
  /* USER CODE BEGIN 1 */
	uint8_t uc_keuze = 0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LTDC_Init();
  MX_USART1_UART_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */

  /* LCD Initialization */
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);
    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS+(480*272*4));
    /* Enable the LCD */
    BSP_LCD_DisplayOn();
    /* Select the LCD Background Layer  */
    BSP_LCD_SelectLayer(0);
    /* Clear the Background Layer */
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SelectLayer(1);
    /* Clear the foreground Layer */
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    /* Some sign */
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) "Welcome", CENTER_MODE);

    BSP_TS_Init(480,272);

    // TEKEN DE KNOPPEN
   WDA_LCD_DrawBitmap((void * ) START_KNOP_DATA,x10,y20,START_KNOP_DATA_X_PIXEL,START_KNOP_DATA_Y_PIXEL,START_KNOP_DATA_FORMAT);	// startknop
   WDA_LCD_DrawBitmap((void * ) STOP_KNOP_DATA,x10,y60,STOP_KNOP_DATA_X_PIXEL,STOP_KNOP_DATA_Y_PIXEL,STOP_KNOP_DATA_FORMAT);		// stopknop
   WDA_LCD_DrawBitmap((void * ) BLOCK_KNOP_INACTIVE_DATA,x10,y100,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 1
   WDA_LCD_DrawBitmap((void * ) GLIDER_KNOP_INACTIVE_DATA,x10,y140,GLIDER_KNOP_INACTIVE_DATA_X_PIXEL,GLIDER_KNOP_INACTIVE_DATA_Y_PIXEL,GLIDER_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 2
   WDA_LCD_DrawBitmap((void * ) CUSTOM_KNOP_INACTIVE_DATA,x10,y180,CUSTOM_KNOP_INACTIVE_DATA_X_PIXEL,CUSTOM_KNOP_INACTIVE_DATA_Y_PIXEL,CUSTOM_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 3
   WDA_LCD_DrawBitmap((void * ) LWSS_KNOP_INACTIVE_DATA,x10,y220,LWSS_KNOP_INACTIVE_DATA_X_PIXEL,LWSS_KNOP_INACTIVE_DATA_Y_PIXEL,LWSS_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 4
   WDA_LCD_DrawBitmap((void * ) PULSAR_KNOP_INACTIVE_DATA,x85,y100,PULSAR_KNOP_INACTIVE_DATA_X_PIXEL,PULSAR_KNOP_INACTIVE_DATA_Y_PIXEL,PULSAR_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 5
   WDA_LCD_DrawBitmap((void * ) INIT_KNOP_INACTIVE_DATA,x85,y140,INIT_KNOP_INACTIVE_DATA_X_PIXEL,INIT_KNOP_INACTIVE_DATA_Y_PIXEL,INIT_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 6

    // LWIP init
    struct tcp_pcb *tcp_connectie;
    ip_addr_t ip_addr_serverIp;
    ip_addr_t ip_addr_clientIp;

    IP4_ADDR(&ip_addr_serverIp,192,168,69,11);
    IP4_ADDR(&ip_addr_clientIp,192,168,69,10);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  MX_LWIP_Process();

	  TS_StateTypeDef TS_State;
	  BSP_TS_GetState(&TS_State);
	  // startknop
	  if ( ( ( uc_start == 1) && ( uc_Closed == 1) ) || ( ( ( ( ( ( uc_start == 1 ) &&  ( TS_State.touchDetected ) ) &&  ( TS_State.touchX[0] >= 10  ) ) && ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 20 ) ) && ( TS_State.touchY[0] <= 55) ) )
	 	{
	 		printf("start\r\n");
	 		BSP_LCD_ClearStringLine(0);
	 		BSP_LCD_DisplayStringAt(0, 0, ( uint8_t* ) "Trying to connect", CENTER_MODE);
			uc_start = 0;		// het mogelijk maken dat we maar 1 verbinding tegelijkertijd kunnen hebben
			uc_Closed = 1;		// het mogelijk maken om de verbinding automatisch opnieuw te laten starten
			tcp_connectie = tcp_new();	// nieuwe verbinding initialiseren
			tcp_bind(tcp_connectie,&ip_addr_clientIp , PORT_CLIENT);
			tcp_arg(tcp_connectie,NULL);
			tcp_connect(tcp_connectie, &ip_addr_serverIp, PORT_SERVER, Connected);
	 	 }

	  // stopknop
	  if ( ( ( ( ( ( uc_Closed == 1 ) && (TS_State.touchDetected ) ) && ( TS_State.touchX[0] >= 10  ) ) && ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 60 ) ) && ( TS_State.touchY[0] <= 95 )  )
	 	{
		  // om de connectie op een mooie manier te sluiten
		  // gaan we nog 1 keer connecteren en 1 keer data krijgen
		  	  uc_Closed = 0;
	 		  printf("stop\r\n");
		      tcp_connectie = tcp_new();
		      tcp_bind(tcp_connectie,&ip_addr_clientIp , PORT_CLIENT);
		      tcp_arg(tcp_connectie,NULL);
	 		  Close(tcp_connectie);

	 		 BSP_LCD_ClearStringLine(0);
	 		 BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) "trying to disconnect", CENTER_MODE);

	 	 }
	  // keuze knop Block
	  if ( ( ( uc_keuze == 0 ) || ( uc_Data == 1 ) ) && ( ( ( ( TS_State.touchDetected ) && ( TS_State.touchX[0] >= 10  ) ) &&  ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 100 ) ) && ( TS_State.touchY[0] <= 135 ) )
	  {
		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if (( TS_State.touchDetected ) == 0 )
		  {
			  if ( uc_keuze == 0)
			  {
				  uc_keuze = 1;
				  uc_Data =  1;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x10,y100,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT); // keuzeknop 1
			  }
			  else if ( uc_keuze == 1)
			  {
				  uc_Data = 0;
				  uc_keuze = 0;
				  WDA_LCD_DrawBitmap((void * ) BLOCK_KNOP_INACTIVE_DATA,x10,y100,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 1
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }

	  // keuze knop GLIDER
	  if ( ( ( uc_keuze == 0 ) || ( uc_Data == 3 ) ) && ( ( ( (TS_State.touchDetected ) && ( TS_State.touchX[0] >= 10  ) ) && ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 140 ) ) && ( TS_State.touchY[0] <= 175 ) )
	  {
		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if ( ( TS_State.touchDetected ) == 0 )
		  {
			  if ( uc_keuze == 0 )
			  {
				  uc_keuze = 1;
				  uc_Data =  3;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x10,y140,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT);
			  }
			  else if ( uc_keuze == 1 )
			  {
				  uc_Data = 0;
				  uc_keuze = 0;
				  WDA_LCD_DrawBitmap((void * ) GLIDER_KNOP_INACTIVE_DATA,x10,y140,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 1
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }
	  // keuze knop EIGEN
	  if ( ( ( uc_keuze == 0 ) || ( ( uc_Data == 2 ) || ( uc_Data == 7 ) ) )  && ( ( ( ( (TS_State.touchDetected ) && ( TS_State.touchX[0] >= 10 ) ) && ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 180 ) ) && ( TS_State.touchY[0] <= 215 ) ) )
	  {
		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if ( ( TS_State.touchDetected ) == 0 )
		  {
			  if ( uc_keuze == 0 )
			  {
				  uc_keuze = 1;
				  uc_Data =  2;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x10,y180,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT); // keuzeknop 1
			  }
			  else if ( uc_keuze == 1 )
			  {
				  uc_keuze = 0;
				  uc_Data = 7;	// speciale in de server
				  WDA_LCD_DrawBitmap((void * ) CUSTOM_KNOP_INACTIVE_DATA,x10,y180,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT); // keuzeknop 1
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }

	  // keuze knop LWSS
	  if ( ( ( uc_keuze == 0 ) || ( uc_Data == 5 ) ) && ( ( ( ( ( TS_State.touchDetected ) && ( TS_State.touchX[0] >= 10  ) ) && ( TS_State.touchX[0] <= 75 ) ) && ( TS_State.touchY[0] >= 220) ) && ( TS_State.touchY[0] <= 255 ) ) )
	  {
		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if ( (TS_State.touchDetected ) == 0 )
		  {
			  if ( uc_keuze == 0 )
			  {
				  uc_keuze = 1;
				  uc_Data =  5;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x10,y220,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT);
			  }
			  else if ( uc_keuze == 1 )
			  {
				  uc_keuze = 0;
				  uc_Data = 0;
				  WDA_LCD_DrawBitmap((void * ) LWSS_KNOP_INACTIVE_DATA,x10,y220,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT);
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }

	  // keuze knop PULSAR
	  if ( ( ( uc_keuze == 0 ) || ( uc_Data == 4 ) ) && ( ( ( ( ( TS_State.touchDetected ) && ( TS_State.touchX[0] >= 85  ) ) && ( TS_State.touchX[0] <= 145 ) )  && ( TS_State.touchY[0] >= 100 ) ) && ( TS_State.touchY[0] <= 135 ) ) )
	  {

		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if ( ( TS_State.touchDetected ) == 0 )
		  {

			  if ( uc_keuze == 0 )
			  {
				  uc_keuze = 1;
				  uc_Data =  4;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x85,y100,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT);
			  }
			  else if ( uc_keuze == 1 )
			  {
				  uc_keuze = 0;
				  uc_Data =  0;
				  WDA_LCD_DrawBitmap((void * ) PULSAR_KNOP_INACTIVE_DATA,x85,y100,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT);
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }

	  // keuze knop INIT
	  if ( ( ( uc_keuze == 0 ) || ( uc_Data == 6 ) ) && ( ( ( ( TS_State.touchDetected ) && ( TS_State.touchX[0] >= 85  ) ) &&  ( TS_State.touchX[0] <= 145 ) && ( TS_State.touchY[0] >= 140 ) ) && ( TS_State.touchY[0] <= 175 ) ) )
	  {
		  HAL_Delay(100);
		  BSP_TS_GetState(&TS_State);
		  if ( ( TS_State.touchDetected ) == 0 )
		  {
			  if ( uc_keuze == 0 )
			  {
				  uc_keuze = 1;
				  uc_Data =  6;
				  WDA_LCD_DrawBitmap((void * ) KNOP_ACTIVE_DATA,x85,y140,KNOP_ACTIVE_DATA_X_PIXEL,KNOP_ACTIVE_DATA_Y_PIXEL ,KNOP_ACTIVE_DATA_FORMAT);
			  }
			  else if ( uc_keuze == 1 )
			  {
				  uc_Data = 0;
				  uc_keuze = 0;
				  WDA_LCD_DrawBitmap((void * ) INIT_KNOP_INACTIVE_DATA,x85,y140,BLOCK_KNOP_INACTIVE_DATA_X_PIXEL,BLOCK_KNOP_INACTIVE_DATA_Y_PIXEL ,BLOCK_KNOP_INACTIVE_DATA_FORMAT);
			  }
			  else
			  {

			  }
		  }
		  HAL_Delay(100);
	  }
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config( void )
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* DMA2D init function */
static void MX_DMA2D_Init( void )
{

  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* LTDC init function */
static void MX_LTDC_Init( void )
{

  LTDC_LayerCfgTypeDef pLayerCfg;
  LTDC_LayerCfgTypeDef pLayerCfg1;

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 40;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 53;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 533;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 565;
  hltdc.Init.TotalHeigh = 285;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 255;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB1555;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 480;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 272;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB1555;
  pLayerCfg1.Alpha = 255;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 480;
  pLayerCfg1.ImageHeight = 272;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init( void )
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* FMC initialization function */
static void MX_FMC_Init( void )
{
  FMC_SDRAM_TimingTypeDef SdramTiming;

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init( void )
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_Port, LCD_BL_CTRL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, LED_Pin|LCD_DISP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LCD_BL_CTRL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin LCD_DISP_Pin */
  GPIO_InitStruct.Pin = LED_Pin|LCD_DISP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler( char *file, int line )
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed( uint8_t* file, uint32_t line )
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
