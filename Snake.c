#include "fxlib.h" 

int headloop = 1;

#define SCA 0xD201D002
#define SCB 0x422B0009
#define SCE 0x80010070
typedef void(*sc_vpuc)(unsigned char*);
const unsigned int sc06C4[] = { SCA, SCB, SCE, 0x06C4 };
#define KBD_PRGM_GetKey (*(sc_vpuc)sc06C4)

int PRGM_GetKey(){
unsigned char sysbuffer[12];
        KBD_PRGM_GetKey( sysbuffer );
        return ( sysbuffer[1] & 0x0F ) * 10 + ( ( sysbuffer[2]  & 0xF0 )  >> 4 );
}

typedef int(*sc_iv)(void);
const unsigned int sc003B[] = { SCA, SCB, SCE, 0x003B };
#define RTC_GetTicks (*(sc_iv)sc003B)
const unsigned int sc0541[] = { SCA, SCB, SCE, 0x0541 };
#define itoa (*(sc_viuc)sc0541)

static unsigned int lastrandom=0x12345678;
unsigned int rand(){
    int seed = 0;
    if (seed) lastrandom=seed;
    lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039;
    return ( lastrandom >> 16 );
}
  
int AddIn_main(int isAppli, unsigned short OptionNum)
{
	while (headloop == 1){
	
	int xpos [51];           //Arrays für die X/Y Position/ Richtung der schlange und des Futters
	int ypos [51];
	int direct [51];
	int eatingx [11];
	int eatingy [11];
	
	int sleep = 100;
		
	unsigned int key; //Variable für Tastenabfragen
	
	
	char head = 6;  //Kopfposition der Schlange
	
	int eatcount = 1; //Zahler für die Anzahl des Futters
	
	int count; //Zahler für for Schleifen
	
	int loop = 1; //Schleife aktivieren
	
	int points[5];
	
	rand( RTC_GetTicks());
	
	for (count = 0 ; count <= 7 ; count++)
	{
		direct [count] = 4; //Richtung Rechts an alle Schlangenteile
	}
	for (count = 0 ; count <= 6 ; count++)
	{
		xpos [count] = count -  1;  //xposition festlegen (Startposition)
		ypos [count] = 0;                 //yposition festlegen (Startposition)
	}
	  
	//Menu
	Bdisp_AllClr_DDVRAM(); // Bildschirm loschen
	
	PrintMini(1,20,(unsigned char *)"Geschwindigkeit mit + und - ",MINI_OVER);
	PrintMini(1,30,(unsigned char *)"regeln. Beenden mit EXIT , ",MINI_OVER);
	PrintMini(1,40,(unsigned char *)"steuern mit den Pfeiltasten !",MINI_OVER);
	Bdisp_PutDisp_DD();
	
	GetKey(&key);
	
	Bdisp_AllClr_DDVRAM(); // Bildschirm loschen
	
	while (loop == 1) //endlosschleife, solange bis loop ungleich null
	{
		if (head > 25){
			Bdisp_AllClr_DDVRAM();
			locate(5,4);
			Print("GEWONNEN!!!");
			locate(7,5);
			Print("25 Punkte");
			Bdisp_PutDisp_DD();
			GetKey(&key);
			loop = 2;
		}
		
		itoa(head-6,points);
		PrintMini( 1, 1, &points, MINI_REV);
		Bdisp_PutDisp_DD();
		
		if (eatcount < 10) // 10 mal Futter zufallig auf das Feld verteilen
		{
			eatingx [eatcount] = rand() % 60;
			eatingy [eatcount] = rand() % 30;
			
			for (count = 0; count <= 2; count ++){
				Bdisp_DrawLineVRAM( eatingx [eatcount] * 2 , eatingy [eatcount] * 2 + count , eatingx [eatcount] * 2 - 2, eatingy [eatcount] * 2 + count);
			}
			Bdisp_PutDisp_DD();
		
			eatcount ++; //Futterzähler erhöhen
		}
		for (count = 0; count <= 2; count ++){
			Bdisp_ClearLineVRAM( xpos [1] * 2 , ypos [1] * 2 + count , xpos [1] * 2 - 2 , ypos [1] * 2 + count);
		}
		Bdisp_PutDisp_DD();
	
		for (count = 0; count != head + 1; count ++) //xpositionen aktualisieren und an die evtl veränderte Laufrichtung anpassen
		{switch(direct [count]){
		
			case 1 : 
			ypos [count] = ypos [count] + 1;
			break;
			
			case 2 :
			xpos [count] = xpos [count] - 1;
			break;
			
			case 3 :
			ypos [count] = ypos [count] - 1;
			break;
			
			case 4 :
			xpos [count] = xpos [count] + 1;
			break;}
		}
	
		if (xpos[head]  > 60  || xpos [head] < 0 || ypos [head] < 0 || ypos [head] > 30){
			Bdisp_AllClr_DDVRAM();
			key = 0;
			locate(7,4);
			Print("Verloren.");
			Bdisp_PutDisp_DD();
			GetKey(&key);
		}
		for (count = 0 ; count <= eatcount; count ++){
			if (eatingx [count] == xpos [head] && eatingy [count] == ypos [head]){
			head++;
			eatcount--;
			direct [head + 1] = direct [head];
			switch(direct [head]){
			case 1 : 
			ypos [head] = ypos [head - 1] + 1;
			xpos [head] = xpos [head - 1];
			break;
			
			case 2 :
			xpos [head] = xpos [head - 1] - 1;
			ypos [head] = ypos [head - 1];
			break;
			
			case 3 :
			ypos [head] = ypos [head - 1] - 1;
			xpos [head] = xpos [head - 1];
			break;
			
			case 4 :
	xpos [head] = xpos [head - 1] + 1;
	ypos [head] = ypos [head - 1];
	break;}
	for (count = 0; count <= 2; count ++){
	Bdisp_DrawLineVRAM( xpos [head] * 2 , ypos [head] * 2 + count , xpos [head] * 2 - 2 , ypos [head] * 2 + count);
	}
	Bdisp_PutDisp_DD();
	break;
	}}
	
	for (count = 0; count <= 2; count ++){
	Bdisp_DrawLineVRAM( xpos [head] * 2 , ypos [head] * 2 + count , xpos [head] * 2 - 2 , ypos [head] * 2 + count);
	}
	Bdisp_PutDisp_DD();
	
	for (count = 0 ; count != head + 1; count++)//Richtung ubertragen, von vorne nach hinten, 
	//d. h. Ist vorher der Kopf nach rechts und der 2. Teil nach oben gelaufen, 
	//lauft das 2. Teil jetzt nach oben und der 3. Teil nach rechts 
	{direct [count] = direct [count + 1];}
	
	key = PRGM_GetKey();
	Sleep(sleep);
	
	switch (key){
	case 28 :
	if (direct[head+1] != 1){
	direct[head+1] = 3;//Kopflaufrichtung nach oben
	}
	break;
	case 27 :
	if (direct[head+1] != 2){
	direct[head+1] = 4; //Kopflaufrichtung rechts
	}
	break;
	case 37 :
	if (direct[head+1] != 3){
	direct[head+1] = 1;//runter
	}
	break;
	case 38 :
	if (direct[head+1] != 4){
	direct[head+1] = 2;//links
	}
	break;
	case 47 :
	headloop = 2;
	loop = 2;
	break;
	case 42 :
	if (sleep > 0){
	sleep = sleep - 5;
	}
	break;
	case 32 :
	sleep = sleep + 5;
	break;
	}
	
	}
	
	}
}



//\/ WICHTIG !!!!!! \/ muss da bleiben \/ (warum auch immer)  \/


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

