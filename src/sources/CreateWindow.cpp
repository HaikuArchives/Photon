#include "CreateWindow.h"
#include <Button.h>

#define COORD_CHANGED 'coch'

CreateWindow::CreateWindow(BRect frame, share *sh)
				: BWindow(frame, Language.get("CREATE_NEW"), B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
shared=sh;
//------------------------
ThePrefs.create_win_open = true;

crt_view = new CreateView(Bounds(),shared);
AddChild(crt_view);

old_unit=0;
old_res_type=0;

PostMessage(new BMessage(COORD_CHANGED)); 


//activate the first paper format
BMessage *x = new BMessage(ACTIVATE_FORMAT);
x->AddInt32("number",0);
PostMessage(x);		
}


CreateWindow::~CreateWindow()
{
	ThePrefs.create_win_open = false;
}


//--------------

void CreateWindow::MessageReceived(BMessage *msg)
{

char str[255];
float val,valb;
int32 number,new_unit,res_type;

	switch (msg->what)
	{
	
		case DO_IT:
		
		shared->CreateNewImage(crt_view->name_field->Text(),crt_view->pix_x-1,crt_view->pix_y-1); 	//revert mode =OFF
		shared->act_img->SetUnitsResType(crt_view->units->Menu()->IndexOf(crt_view->units->Menu()->FindMarked()),
										 atof(crt_view->res->Text()), 
										 crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked())
										 );
		
		Lock();
		Close();
		
		break;
		
		case CANCEL:
		Lock();
		Close();
		break;

		case COORD_CHANGED:
		UpdatePixSize();
		break;

		case UNITS_CHANGED:
		val  = atof(crt_view->x->Text());
		valb = atof(crt_view->y->Text());
		
		new_unit  = crt_view->units->Menu()->IndexOf(crt_view->units->Menu()->FindMarked());
		res_type  = crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked());
		
		//unit,int32 dest_unit, float resolution, float res_units)
		val = util.ConvertUnits(val, old_unit, new_unit,atof(crt_view->res->Text()),res_type);
		valb = util.ConvertUnits(valb, old_unit, new_unit,atof(crt_view->res->Text()),res_type);
		
		if (new_unit==UNIT_PIXELS)
		{
			//Les pixels peuvent pas avoir de virgule!
			sprintf(str,"%ld",int32(val));   crt_view->x->SetText(str);
			sprintf(str,"%ld",int32(valb));  crt_view->y->SetText(str);
		}
		else
		{
			sprintf(str,"%.2f",val);   crt_view->x->SetText(str);
			sprintf(str,"%.2f",valb);  crt_view->y->SetText(str);
		}
		
		
		UpdatePixSize();
		old_unit=crt_view->units->Menu()->IndexOf(crt_view->units->Menu()->FindMarked());

		break;

		case RES_CHANGED:
		UpdatePixSize();
		break;
		
		case RES_TYPE_CHANGED:
		
		val = atof(crt_view->res->Text());
		
		//only if it has really changed
		if (old_res_type!=crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked()))
			{
			if (crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked())==DPI) 
				 { val *= 2.54; }
			else { val /= 2.54; }
			}
			
		sprintf(str,"%.2f",val);
		crt_view->res->SetText(str);
		
		old_res_type=crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked());
		
		UpdatePixSize();
		break;

		case ACTIVATE_FORMAT:
		msg->FindInt32("number",&number);

		crt_view->res_type->Menu()->ItemAt(crt_view->the_formats[number]->what_res_type)->SetMarked(true);
		crt_view->res_type->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre

		crt_view->units->Menu()->ItemAt(crt_view->the_formats[number]->units)->SetMarked(true);
		crt_view->units->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre
		
		sprintf(str,"%.2f",crt_view->the_formats[number]->res);	crt_view->res->SetText(str);

		if (crt_view->the_formats[number]->units==UNIT_PIXELS)
		{
			//Les pixels peuvent pas avoir de virgule!
			sprintf(str,"%ld",int32(crt_view->the_formats[number]->x));   crt_view->x->SetText(str);
			sprintf(str,"%ld",int32(crt_view->the_formats[number]->y));  crt_view->y->SetText(str);
		}
		else
		{
			sprintf(str,"%.2f",crt_view->the_formats[number]->x);   crt_view->x->SetText(str);
			sprintf(str,"%.2f",crt_view->the_formats[number]->y);  crt_view->y->SetText(str);
		}
		
		//très important pour la prochaine conversion si on change d'unité
		old_unit = crt_view->the_formats[number]->units;
		old_res_type = crt_view->the_formats[number]->what_res_type;
	
		
		UpdatePixSize();
		break;
		
		
  	 }

//envoie messge à la fenêtre mère
//util.mainWin->PostMessage(new BMessage(Create_CHANGED)); 

}//fin message received


void CreateWindow::UpdatePixSize()
{

		//on doit obtenir des pixels à partir de xx,yy;
		float xx = atof(crt_view->x->Text());
		float yy = atof(crt_view->y->Text());
		
		//dots per inch (dpi)
		float divider =0;
		if (crt_view->res_type->Menu()->IndexOf(crt_view->res_type->Menu()->FindMarked())==DPI) divider=2.54;
		//dots per centimeterd
		else divider = 1;
		
		float inch_divider;
		if (divider ==1) inch_divider=2.54;
		else inch_divider=1;
		
		float res = atof(crt_view->res->Text());
		
		switch(crt_view->units->Menu()->IndexOf(crt_view->units->Menu()->FindMarked()))
				{	
				
					case UNIT_CM:
					xx *= (res/divider);
					yy *= (res/divider);
					break;
			
					case UNIT_MM:
					xx *= (res/(divider*10));
					yy *= (res/(divider*10));
					break;
					
					case UNIT_INCH:
					xx *= (res*inch_divider);
					yy *= (res*inch_divider);
					break;
					
					case UNIT_PIXELS:
					//que dalle
					break;
			
				}
			
		
		char s[255];	
		sprintf(s,"(%ld pixels)",int32(xx));	crt_view->pixel_x->SetText(s);
		sprintf(s,"(%ld pixels)",int32(yy));	crt_view->pixel_y->SetText(s);

		//ces valeurs ont utilisées lors de la création
		crt_view->pix_x = int32(xx);
		crt_view->pix_y = int32(yy);

		
		float sz= (xx *yy *4) ;
		
		char str[255];
		char st_base[255];
		sprintf(st_base, Language.get("WEIGHT"));
		strcat(st_base,": ");
		if (sz > 32768) 	{	sz /= 1024;	sprintf(str,"%G Kb",sz); strcat(st_base,str);}
		else { sprintf(str,"(%G bytes)",sz); strcat(st_base,str);}
		crt_view->taille->SetText(st_base);


}



PaperFormat::PaperFormat(char the_name[64], int32 the_res_type, float the_res, int32 the_units, float the_x,float the_y)
{

			sprintf(name,the_name);
			what_res_type = the_res_type;
			res = the_res;
			units = the_units;
			x = the_x;
			y = the_y;
}


CreateView::CreateView(BRect rec, share *sh):
	BView(rec, "create view", B_FOLLOW_ALL, B_WILL_DRAW)
{
shared=sh;
SetViewColor(216,216,216);

//--------------------------------------------

BRect rect;

rect.Set (4,4,Bounds().Width()-4,4+16);
taille = new BStringView(rect,NULL,"",B_FOLLOW_ALL,B_WILL_DRAW);
taille->SetAlignment(B_ALIGN_CENTER);
AddChild(taille);

char str[255]; 
char str2[255];
sprintf(str,Language.get("UNTITLED"));
sprintf(str2," %d",shared->image_amount+1); 
strcat(str,str2);

rect.OffsetBy(0,24);
name_field = new BTextControl(rect,NULL,NULL,str,new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);
AddChild(name_field);


///-----------------------------------------------------------------------------

int32 q =0;
the_formats[q] = new PaperFormat("320 x 240 pixels",DPI,72,UNIT_PIXELS,320,240);	q++;
the_formats[q] = new PaperFormat("640 x 480 pixels",DPI,72,UNIT_PIXELS,640,480);	q++;
the_formats[q] = new PaperFormat("800 x 600 pixels",DPI,72,UNIT_PIXELS,800,600);	q++;
the_formats[q] = new PaperFormat("1024 x 768 pixels",DPI,72,UNIT_PIXELS,1024,768);	q++;
the_formats[q] = new PaperFormat("A6 (105 x 148 mm)",DPI,300,UNIT_MM,105,148);		q++;
the_formats[q] = new PaperFormat("A5 (148 x 210 mm)",DPI,300,UNIT_MM,210,148);		q++;
the_formats[q] = new PaperFormat("A4 (210 x 297 mm)",DPI,300,UNIT_MM,210,297);		q++;
the_formats[q] = new PaperFormat("A3 (297 x 420 mm)",DPI,300,UNIT_MM,297,420);				
format_amount=q;

///-----------------------------------------------------------------------------


rect.OffsetBy(0,32);
rect.right=240;
BMenu *format_menu = new BMenu("");
BMessage *messie;
BMenuItem *ait;

//le premier pour pouvoir le sélecter
messie = new BMessage(ACTIVATE_FORMAT); messie->AddInt32("number",0);
format_menu->AddItem(ait = new BMenuItem(the_formats[0]->name,   messie) );//pour activer le premier après

int32 g=1; //on a déjà fait le 0

while (g != format_amount+1)
{
	messie = new BMessage(ACTIVATE_FORMAT); messie->AddInt32("number",g);

	format_menu->AddItem(new BMenuItem(the_formats[g]->name,   messie) );
	g++;
}
ait->SetMarked(true);

format_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
format_field = new BMenuField(rect,"",Language.get("FORMATS"),format_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
format_field->SetDivider(80);
format_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(format_field);

//----------------------------------------------------




rect.OffsetBy(0,50);
//rect.top = 64+4;
//rect.bottom= 64+4+16;
rect.left =4;
rect.right =4+80;
x = new BTextControl(rect,NULL,NULL,"14.8",new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

rect.OffsetBy(0,24);
y = new BTextControl(rect,NULL,NULL,"10.5",new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

rect.OffsetBy(0,24);
res = new BTextControl(rect,NULL,NULL,"300",new BMessage(RES_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

AddChild(x);
AddChild(y);
AddChild(res);


//---------------------------

BMenuItem *it;
rect.OffsetBy(4+80+16-1,-24*2);
rect.right+=60;
BMenu *unit_menu = new BMenu("");
unit_menu->AddItem(it = new BMenuItem(Language.get("CENTIMETERS"),   new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem(Language.get("MILLIMETERS"),    new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem(Language.get("INCHES"), new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem("pixels", new BMessage(UNITS_CHANGED))); //pixels cannot be translated in any language...

it->SetMarked(true);

unit_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
units = new BMenuField(rect,"",NULL,unit_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
unit_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(units);


//DPI Dots/cm

rect.OffsetBy(0,24+24);
BMenuItem *xit;
BMenu *res_menu = new BMenu("");

char str_a[256];
char str_b[256];
sprintf(str_a, "pixels/"); strcat(str_a,Language.get("INCH")); 		 strcat(str_a," (dpi)"); 
sprintf(str_b, "pixels/"); strcat(str_b,Language.get("CENTIMETER")); strcat(str_b," (dpcm)");

res_menu->AddItem(xit = new BMenuItem(str_a, new BMessage(RES_TYPE_CHANGED)));
res_menu->AddItem(new BMenuItem(str_b, new BMessage(RES_TYPE_CHANGED)));

xit->SetMarked(true);

res_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
res_type = new BMenuField(rect,"",NULL,res_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
res_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(res_type);


rect.OffsetBy(140,-24-24);
pixel_x = new BStringView(rect,"xpix","xxx",B_FOLLOW_ALL,B_WILL_DRAW);
rect.OffsetBy(0,24);
pixel_y = new BStringView(rect,"ypix","yyy",B_FOLLOW_ALL,B_WILL_DRAW);

AddChild(pixel_x);
AddChild(pixel_y);

//---------- BUTTONS

rect.bottom= Bounds().bottom-24;
rect.top = Bounds().bottom-32;
rect.left =8;
rect.right =8+80;
BButton *cancel = new BButton(rect,"",Language.get("CANCEL"),new BMessage(CANCEL),B_FOLLOW_ALL,B_WILL_DRAW | B_NAVIGABLE);
AddChild(cancel);

rect.right = Bounds().Width()-8;
rect.left = Bounds().Width()-8-80;
BButton *do_it = new BButton(rect,"",Language.get("YES"),new BMessage(DO_IT),B_FOLLOW_ALL,B_WILL_DRAW | B_NAVIGABLE);
AddChild(do_it);
do_it->MakeDefault(true);
}



void CreateView::MouseMoved(BPoint point, uint32 transit, const BMessage *le_message)
{
	if (transit==B_ENTERED_VIEW || transit== B_INSIDE_VIEW) 
	{
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",7); //7 = B_HAND_CURSOR
		util.mainWin->PostMessage(mx);
	}
	

}