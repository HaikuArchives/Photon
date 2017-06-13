#include "ColWindow.h"


ColView::ColView(BRect r, uint8 f_mode, share *sh, long the_flags) : 
BView(r, "colview", the_flags, B_WILL_DRAW )
{

shared = sh;
old_pt.Set(0,0);
//--------------------------------------------------------------------------------
//on agit sur la foreground... ou background
mode_fore=f_mode; 

if (mode_fore==ON) color_selector_size = ThePrefs.fore_selector_size;
   else color_selector_size = ThePrefs.back_selector_size;

//----------------------------------------

SetViewColor(216,216,216);
BRect rect;

rect.Set(0,0,(256/color_selector_size)-1,(256/color_selector_size)-1);
col_img = new BBitmap(rect,B_RGB32);

rect.Set(0,0,(24/color_selector_size)-1,(256/color_selector_size)+1);
slide_img = new BBitmap(rect,B_RGB32);

CreateSpectrum(); //dessine le dégradé

rect.Set((4/color_selector_size),(260/color_selector_size),(68/color_selector_size),(266/color_selector_size));
red_box = new BTextControl(rect,"",Language.get("RED"),"0",new BMessage(COL_CHANGED), B_FOLLOW_ALL,B_WILL_DRAW);

rect.OffsetBy(71,0);
green_box = new BTextControl(rect,"",Language.get("GREEN"),"0",new BMessage(COL_CHANGED), B_FOLLOW_ALL,B_WILL_DRAW);

rect.OffsetBy(71,0);
blue_box = new BTextControl(rect,"",Language.get("BLUE"),"0",new BMessage(COL_CHANGED), B_FOLLOW_ALL,B_WILL_DRAW);

rect.OffsetBy(71,0);
alpha_box = new BTextControl(rect,"",Language.get("ALPHA"),"0",new BMessage(COL_CHANGED), B_FOLLOW_ALL,B_WILL_DRAW);

AddChild(red_box);
AddChild(green_box);
AddChild(blue_box);
AddChild(alpha_box);


char t[255];

if (mode_fore==OFF)
	{
	//met la valeur foreground actuelle dans les cases...
	sprintf(t,"%d",shared->fore_color.red);
	red_box->SetText(t);
	sprintf(t,"%d",shared->fore_color.green);
	green_box->SetText(t);
	sprintf(t,"%d",shared->fore_color.blue);
	blue_box->SetText(t);
	sprintf(t,"%d",shared->fore_color.alpha);
	alpha_box->SetText(t);
	
	//retrouve ancienne col en cliquant sur anciennes coord
	ClickedInSpectrum(shared->ori_front_slide); 
	ClickedInSquare(shared->ori_front_square);
	}
else //donc si on agit sur le background color
	{
	sprintf(t,"%d",shared->back_color.red);
	red_box->SetText(t);
	sprintf(t,"%d",shared->back_color.green);
	green_box->SetText(t);
	sprintf(t,"%d",shared->back_color.blue);
	blue_box->SetText(t);
	sprintf(t,"%d",shared->back_color.alpha);
	alpha_box->SetText(t);
	
	//retrouve ancienne col en cliquant sur anciennes coord
	ClickedInSpectrum(shared->ori_back_slide);
	ClickedInSquare(shared->ori_back_square);
	}

pressed=false;
updating=false;	


//CA FAIT QUE LES MESSAGES NE S'entassent pas... du coup ça suit...
//sinon le redraw est trop long pour les millions de mousemoved
SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY); 


}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

ColView::~ColView()  { }

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------


void ColView::Draw(BRect update_rect)
{
DrawBitmap(col_img,BPoint(0,0));
DrawBitmap(slide_img,BPoint(((256+8)/color_selector_size),0));



//------------------------------------------------------------------------------------------------
//DESSIN DE LA CROIX
BPoint point_ori;
if (mode_fore==OFF) point_ori=shared->ori_front_square;
	else  point_ori=shared->ori_back_square;
	SetDrawingMode(B_OP_INVERT);
	StrokeLine(BPoint(point_ori.x,point_ori.y-2),BPoint(point_ori.x,point_ori.y+2),B_SOLID_HIGH); 
	StrokeLine(BPoint(point_ori.x-2,point_ori.y),BPoint(point_ori.x+2,point_ori.y),B_SOLID_HIGH);
	SetDrawingMode(B_OP_COPY);

//------------------------------------------------------------------------------------------------
//DESSIN DE LA LIGNE SUR SPECTRUM
BPoint pt;
if (mode_fore==OFF) pt=shared->ori_front_slide;
	else  			pt=shared->ori_back_slide;
	SetPenSize(1);	SetHighColor(216,216,216);
	FillRect(BRect((258/color_selector_size),0,(263/color_selector_size),(260/color_selector_size)),B_SOLID_HIGH); //efface anciennes lignes
	
	
	SetHighColor(0,0,0);
	StrokeLine(BPoint((258/color_selector_size),pt.y),BPoint((263/color_selector_size),pt.y),B_SOLID_HIGH);

}



void ColView::MouseDown(BPoint point)
{
SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY); 

BPoint pt=point;
uint32 buttons;

GetMouse(&pt, &buttons); 
//en attendant de trouver comment intercepter le "Zoom window" (Maximize) on fait avec le right button
if (buttons==B_SECONDARY_MOUSE_BUTTON) 
	{
	Window()->PostMessage(RESIZE_CSEL); 
	}

else
{
	pressed=true;
	MouseMoved(point,B_ENTERED_VIEW,NULL);
}
 //toujours la pic activée ...


}

void ColView::MouseUp(BPoint point)
{
pressed=false;
}

//------------------------------------------------------------------------------------------------

void ColView::UpdatePanel(rgb_color col)
{
updating = true;

uint8 *bits,*temp;
bits= (uint8*)col_img->Bits();

int32 pos=0;
int32 line_ct=0;

float a=0;
float r=0;
float g=0;
float b=0;


rgb_color base_col = col;

uint8 maxi=255;

int16 r_base,g_base,b_base; //base désaturée au fur et à mesure

r_base = base_col.red;
g_base = base_col.green;
b_base = base_col.blue;

while (line_ct!=(256/color_selector_size))
{

r = maxi;	g = maxi;	b = maxi;		

pos=(255/color_selector_size);
while (pos!=-1)
{
		if (r > r_base) 	r -= 1/(pos/(r-r_base)); 
		if (g > g_base)		g -= 1/(pos/(g-g_base)); 
		if (b > b_base) 	b -= 1/(pos/(b-b_base)); 

		//copie dans RGB+A
		temp=bits;  *temp = (uint8) b; //on arrondi le float
		temp++;		*temp = (uint8) g;
		temp++;		*temp = (uint8) r;
		temp++;		*temp = (uint8) a;

		bits+=4; pos--;
		
		}
line_ct++;
maxi-=1*color_selector_size;

		if (r_base > maxi) 	r_base --; 
		if (g_base > maxi)	g_base --; 
		if (b_base > maxi) 	b_base --; 

}
Draw(Bounds());
updating = false;


}//Fin updatepanel


//------------------------------------------------------------------------------------------------

void ColView::CreateSpectrum()
{

uint8 *bits,*temp;

BRect rect;
rect.Set(0,0,(24/color_selector_size)-1,(256/color_selector_size)+1);
slide_img = new BBitmap(rect,B_RGB32);

bits= (uint8*)slide_img->Bits();

int32 line_ct=0;
int16 a=0;
int16 r=0;
int16 g=0;
int16 b=0;

//1. Step one, red to green+red
r=255;
while (g<255)
{
line_ct=0;
while (line_ct!=(24/color_selector_size) )
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
g+=(6*color_selector_size);
}
g=255;//pour assurer

//2. Step two, green+red to green

while (r>0)
{
line_ct=0;
while (line_ct!=(24/color_selector_size) )
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
r-=(6*color_selector_size);
}
r=0;//pour assurer


//3. Step three, green to green+blue

while (b<255)
{
line_ct=0;
while (line_ct!=(24/color_selector_size) )
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
b+=(6*color_selector_size);;
}
b=255;//pour assurer

//4. Step Four, green+blue to blue

while (g >0)
{
line_ct=0;
while (line_ct!=(24/color_selector_size) )
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
g-=(6*color_selector_size);;
}
g=0;//pour assurer

//5. Step Five, blue to blue+red
while (r < 255)
{
line_ct=0;
while (line_ct!=(24/color_selector_size) )
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
r+=(6*color_selector_size);;
}
r=255;//pour assurer

//5. Step Five, blue to blue+red
int32 to_reach;

if (color_selector_size==2) to_reach=2; //necessary with small version to reach the bottom.. some rounding prob..
else to_reach = 0;

while (b > 0)
{
line_ct=0;

while (line_ct!=(24/color_selector_size)-to_reach)
{
	temp=bits;  *temp=b;
	temp++;		*temp=g;
	temp++;		*temp=r;
	temp++;		*temp=a;
	bits++;bits++;bits++;bits++;
	line_ct++;
}
b-=(6*color_selector_size);;
}

b=0;//pour assurer



}//fin create spectrum


//------------------------------------------------------------------------------------------------

rgb_color ColView::SelectColor(BPoint pix) //DANS LA BARRE
{
unsigned long pos_bits;
pos_bits=(unsigned long)(0+((24/color_selector_size)*pix.y)*4); //seul y compte c'est pareil sur toute la ligne

uint8	*bits;
bits = (uint8 *)slide_img->Bits(); //pour accèder aux pixels directement
bits+=pos_bits;//avance jusqu'au pixel en question


rgb_color col;
col.blue=*bits;  	bits++;
col.green=*bits;  	bits++;
col.red=*bits;  	bits++;
col.alpha=*bits;  

col_ori=col;
return col;
}

rgb_color ColView::PickColor(BPoint pix) //DANS LE CARRE
{

unsigned long pos_bits;
pos_bits=(unsigned long)((pix.x+((256/color_selector_size)*pix.y))*4); //seul y compte c'est pareil sur toute la ligne
uint8	*bits;
bits = (uint8 *)col_img->Bits(); //pour accèder aux pixels directement
bits+=pos_bits;//avance jusqu'au pixel en question


rgb_color col;
col.blue=*bits;  	bits++;
col.green=*bits;  	bits++;
col.red=*bits;  	bits++;
col.alpha=*bits;  

return col;
}


void ColView::ClickedInSquare(BPoint pt) //DANS LE CARRE
{

	if (pt.x <0) pt.x =0;
	if (pt.y <0) pt.y =0;

	//très important pour quand on est en petit!!! sinon le premier init crashe la fenetre... (le -1 est crucial aussi);
	if (pt.x > (256/color_selector_size) )  pt.x =(256/color_selector_size)-1;
	if (pt.y > (256/color_selector_size) )  pt.y =(256/color_selector_size)-1;
	
	rgb_color cocol=PickColor(pt);
	char str[255];
	if (mode_fore == OFF )	shared->back_color=cocol;	
	else shared->fore_color = cocol;	
	
	//----------------
	sprintf(str,"%d",cocol.red);		red_box->SetText(str);
	sprintf(str,"%d",cocol.green);		green_box->SetText(str);
	sprintf(str,"%d",cocol.blue);		blue_box->SetText(str);
	sprintf(str,"%d",cocol.alpha);		alpha_box->SetText(str);

	util.toolWin->PostMessage(new BMessage(COL_SELECTED)); 
	//-------------------

	//sauvegarde des coordonnées
	if (mode_fore==OFF) shared->ori_front_square=pt;
	else shared->ori_back_square=pt;

//	UpdatePanel(col_ori);
	Draw(Bounds());
	
}


void ColView::ClickedInSpectrum(BPoint pt) //DANS LE CARRE
{

	if (pt.y <0) pt.y =0;
	if (pt.y >(256/color_selector_size)) pt.y =(256/color_selector_size);
	
	rgb_color col=SelectColor(pt);

	//sauvegarde des coordonnées
	if (mode_fore==OFF)	{ shared->ori_back_slide=pt;	ClickedInSquare(shared->ori_back_square);	}
	else				{ shared->ori_front_slide=pt;	ClickedInSquare(shared->ori_front_square);}

	util.mainWin->PostMessage(new BMessage(COL_SELECTED)); 			//envoie messge à la fenêtre mère
	UpdatePanel(col_ori);
	Draw(Bounds());
}


void ColView::MouseMoved(BPoint pt, uint32 transit, const BMessage *a_message)
{

if (transit==B_ENTERED_VIEW || B_INSIDE_VIEW) 
	{
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",6); //6 = simple_cross
		util.mainWin->PostMessage(mx);
	}
	
//pas la peine de restaurer le curseur c'est la Win de base qui le fait , au contraire ça fout le bordel

if(updating==false)
{
			if (pressed==true && pt!=old_pt)
			{	
				//si dans carré
				if ((pt.x < (256/color_selector_size)) && (pt.y < (256/color_selector_size)) ) 	
					{	
					
						BMessage msg(CLICK_SQUARE);
						msg.AddPoint("pos",pt);
						Window()->PostMessage(&msg);
					}
				else
				{
				//si dans zone slide spectrum
				if (pt.x > (264/color_selector_size))	
					{
						
						BMessage msg(CLICK_SPECTRUM);
						msg.AddPoint("pos",pt);
						Window()->PostMessage(&msg);
					}
				}
			}	
old_pt=pt;		
}			

}



ColWindow::ColWindow(uint8 back_m, BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_RESIZABLE 
										| B_WILL_ACCEPT_FIRST_CLICK	 | B_ASYNCHRONOUS_CONTROLS | B_AVOID_FOCUS)
{
shared=sh;

//------------------------

BRect rect;

//OFF = foreground
is_fore = back_m;

if (is_fore==ON) color_selector_size = ThePrefs.fore_selector_size;
   else color_selector_size = ThePrefs.back_selector_size;


	if (is_fore==ON) ThePrefs.fore_selector_open=true;
	else	ThePrefs.back_selector_open=true;
		
	

rect.Set(0,0,((256+32)/color_selector_size)-1,((256+28)/color_selector_size)-1);
if (color_selector_size ==2) rect.bottom+=64; //si grand on a les sliders en plus.

c_view= new ColView(rect,back_m,shared,Flags());
AddChild(c_view);

 //toujours la pic activée ...

}

void ColWindow::ResizeMe()
{
	Lock();	
		//ça peut être que 1 ou deux rien d'autre sinon ça risque de merder...
		//1 c'est le grand et 2 le petit (parce-que divisé par deux!)...
		if (color_selector_size==1) 
		{
		color_selector_size=2;

		
			if (is_fore==OFF)
			{
				shared->ori_back_slide.x   /= 2;   
				shared->ori_back_slide.y   /= 2;		
				shared->ori_back_square.x  /= 2;	
				shared->ori_back_square.y  /= 2;	
				//faut ABSOLUMENT FAIRE MOINS 1 en retrecissant sinon l'arrondi le pousse en dehors et CRASH
				shared->ori_back_square.x--;	
				shared->ori_back_square.y--;	
			}
		else
			{
				shared->ori_front_square.x /= 2;	
				shared->ori_front_square.y /= 2;
				shared->ori_front_slide.x  /= 2; 	
				shared->ori_front_slide.y  /= 2;
				//faut ABSOLUMENT FAIRE MOINS 1 en retrecissant sinon l'arrondi le pousse en dehors et CRASH
				shared->ori_front_square.x--;	
				shared->ori_front_square.y--;
			}
		
		}
		else //if color_selector_size != 1
		{
		color_selector_size=1;
		
				if (is_fore==OFF)
				{
					shared->ori_back_slide.x   *= 2;   	
					shared->ori_back_slide.y   *= 2;		
					shared->ori_back_square.x  *= 2;	
					shared->ori_back_square.y  *= 2;	
				}
				else
				{
					shared->ori_front_slide.x  *= 2; 	
					shared->ori_front_slide.y  *= 2;
					shared->ori_front_square.x *= 2;	
					shared->ori_front_square.y *= 2;
				}
		}
		
	

				
		
	if (c_view->RemoveSelf()) delete c_view;
	
	BRect rect;
	int16 height = ((256+25)/color_selector_size);
	if (color_selector_size==2) height-=13;  //pour pas qu'on voie les case de text ratatinées en petit

	ResizeTo( ((256+32)/color_selector_size), height);


	rect.Set(0,0,((256+32)/color_selector_size)-1,((256+28)/color_selector_size)-1);
	if (color_selector_size ==2) rect.bottom+=64; //si grand on a les sliders en plus.

	//la valeur a changé 
	if (is_fore==ON) ThePrefs.fore_selector_size =  color_selector_size;
 				else ThePrefs.back_selector_size =  color_selector_size;

	c_view= new ColView(rect,is_fore,shared,Flags());
	AddChild(c_view);
	Unlock();			


}


ColWindow::~ColWindow()
{
	if (is_fore==ON) 
	{
		ThePrefs.fore_selector_open=false;
		ThePrefs.fore_color_frame=Frame();
	}
	else
	{
		ThePrefs.back_selector_open=false;
		ThePrefs.back_color_frame=Frame();
	}
	
}



void ColWindow::MessageReceived(BMessage *msg)
{
BPoint pt;

	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		case RESIZE_CSEL:
		ResizeMe();
		break;
		
		case CLICK_SQUARE:
		msg->FindPoint("pos",&pt); 
		
		if (pt != last_picked)
			{
			be_app->Lock();
			Lock();
			c_view->ClickedInSquare(pt);
			Unlock();
			be_app->Unlock();
			}
		last_picked = pt;  

		break;

		case CLICK_SPECTRUM:
		msg->FindPoint("pos",&pt); 
		
		if (pt != last_picked)
			{
			be_app->Lock();
			Lock();
			c_view->ClickedInSpectrum(pt);
			Unlock();
			be_app->Unlock();
			}
		last_picked = pt;  
		break;
		
	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}



void ColWindow::Zoom(BPoint leftTop, float width, float height) //intercepte le zoom
{ 
	PostMessage(new BMessage(RESIZE_CSEL));
}



void ColWindow::FrameMoved(BPoint screenPoint)
{
	 //pour ramener au front la pic dessin
}
