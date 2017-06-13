#include "LayerWindow.h"
#include <PopUpMenu.h>
AllLayersView::AllLayersView(BRect r, share *sh) : 
	BView(r, "all_layer_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
shared = sh;
//------------------------

SetViewColor(216,216,216);

}


PaintModeView::PaintModeView(const char *name, BRect r, share *sh, BView* the_p) : 
	BView(r, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{

shared = sh;

//------------------------
SetViewColor(216,216,216);

the_parent = the_p;

BRect rect(0,0,15,15);
/*
char file_name[NAME_SIZE];
char temp_name[NAME_SIZE];

sprintf(file_name,util.dossier_app);
strcat(file_name,"/data/");


sprintf(temp_name,file_name);	strcat(temp_name,"layer_empty.png");
icon_empty = util.load_bmp(temp_name);

sprintf(temp_name,file_name);	strcat(temp_name,"layer_paint.png");
icon_paint = util.load_bmp(temp_name);

sprintf(temp_name,file_name);	strcat(temp_name,"layer_mask_paint.png");
icon_mask = util.load_bmp(temp_name);
*/

icon_empty = BTranslationUtils::GetBitmap('RAWT',"layer_empty.png");
icon_paint = BTranslationUtils::GetBitmap('RAWT',"layer_paint.png");
icon_mask  = BTranslationUtils::GetBitmap('RAWT',"layer_mask_paint.png");

//sécurité
if (icon_empty == NULL) icon_empty= new BBitmap(rect, B_RGB32);
if (icon_paint == NULL) icon_paint = new BBitmap(rect, B_RGB32);
if (icon_mask  == NULL) icon_mask = new BBitmap(rect, B_RGB32);

active_pic=0;

}


TriangleMenuView::~TriangleMenuView()
{
delete on;
delete off;
//pas delete img! c'es juste un pointeur sur on ou off
}

PaintModeView::~PaintModeView()
{
delete icon_paint;
delete icon_mask;
delete icon_empty;

}

TriangleMenuView::TriangleMenuView(BRect r,share *sh) : 
	BView(r, "", B_FOLLOW_TOP | B_FOLLOW_RIGHT, B_WILL_DRAW)
{

shared = sh;

//------------------------
SetViewColor(216,216,216);

off = BTranslationUtils::GetBitmap('RAWT',"triangle_menu.png");
on = BTranslationUtils::GetBitmap('RAWT',"triangle_menu_p.png");

img = off;


pm = new  BPopUpMenu("", false, false, B_ITEMS_IN_COLUMN) ;

pm->AddItem(new BMenuItem(Language.get("ADD_LAYER"), new BMessage(ADD_LAYER) ));
pm->AddItem(new BMenuItem(Language.get("ADD_GUIDE_LAYER"), new BMessage(ADD_GUIDE_LAYER) ));
pm->AddItem(new BMenuItem(Language.get("DELETE_LAYER"), new BMessage(DELETE_LAYER) ));
pm->AddItem(new BMenuItem(Language.get("DUPLICATE_LAYER"), new BMessage(DUPLICATE_LAYER) ));
pm->AddSeparatorItem();
pm->AddItem(new BMenuItem(Language.get("MERGE_LAYERS"), new BMessage(MERGE_LAYERS) ));
pm->AddItem(new BMenuItem(Language.get("MERGE_VISIBLE_LAYERS"), new BMessage(MERGE_VISIBLE_LAYERS) ));
pm->AddItem(new BMenuItem(Language.get("FLATTEN_IMAGE"), new BMessage(FLATTEN_IMAGE) ));
pm->AddSeparatorItem();
pm->AddItem(new BMenuItem(Language.get("DISPLAY_OPTIONS"), new BMessage(DISPLAY_OPTIONS) ));

//pm->SetTargetForItems(this->Window());
pm->SetTargetForItems(util.layerWin); //ben ça ça marche et pas celle d'en dessus
										 //alors va savoir...

}


void TriangleMenuView::Draw(BRect update_rect)
{
	DrawBitmap(img,BPoint(0,0));
}

void TriangleMenuView::MouseDown(BPoint point)
{
	img = on;
	Draw(Bounds());
	pm->Go(ConvertToScreen(point),true,false,false);
	img = off;
	Draw(Bounds());
	
}


LayerOptionsView::LayerOptionsView(BRect r, share *sh) : 
	BView(r, "lay_options_view", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{

shared = sh;

//------------------------
SetViewColor(216,216,216);

//------------------------
BRect rect;

rect.Set(4,12,4+100,12+12);

BMenuItem *it;
BMenu *mode_menu = new BMenu("Drawing Mode");
mode_menu->AddItem(it = new BMenuItem(Language.get("PM_NORMAL"), new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_MULTIPLY"),   new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_LIGHTEN"),    new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_DARKEN"),     new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_COLORIZE"),   new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_DIFFERENCE"), new BMessage(MODE_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("PM_COMBINE"),    new BMessage(MODE_CHANGED)));
it->SetMarked(true);

mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
draw_mode = new BMenuField(rect,"",NULL,mode_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre

AddChild(draw_mode);

//------------------------


rect = Bounds();
rect.left =108;
rect.right = 108+150;
rect.top = 4;


opacity = new BSlider(rect,NULL,"100%",new BMessage(TRANSP_CHANGED),
							0,100,B_TRIANGLE_THUMB,B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);


rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
opacity->SetBarColor(a_col);
opacity->UseFillColor(true,&a_colb);
opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
opacity->SetHashMarkCount(10+1);
opacity->SetValue(100);

AddChild(opacity);

//---------------------------------------------

rect.right = Bounds().right;
rect.left = rect.right-(16-1);
rect.top = 0;
rect.bottom = 16-1;

rect.OffsetBy(-2,2); //pas toucher le bord


t_menu_view = new TriangleMenuView(rect, shared);							
AddChild(t_menu_view);

}



void PaintModeView::Draw(BRect update_rect)
{
	switch(active_pic)
	{
		case ICON_EMPTY:	DrawBitmap(icon_empty);  break;
		case ICON_PAINT:	DrawBitmap(icon_paint);  break;
		case ICON_MASK:		DrawBitmap(icon_mask);   break;
	}
		
}		



void PaintModeView::MouseDown(BPoint point)
{
 the_parent->MouseDown(point);

}


OneLayerView::OneLayerView(const char *name, BRect r,  Layer *ly, share *sh) : 
	BView(r, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
shared = sh;
//------------------------
SetViewColor(216,216,216);

//resize image to shared->perso_layer_sizexshared->perso_layer_size!

the_layer = ly; 


BRect rect;
rect.Set(4,4,4+16,4+16);

//faut passer le pointeur vers le layer car le layer reste toujours le même
//alors que l'id peut changer si on delete des calques

BMessage *x = new BMessage(MAKE_VISIBLE);			
x->AddPointer("layer",the_layer);
		
			CreateButton("layer_empty.png","layer_visible.png");
			visible_button = new BPictureButton(rect,"",off, on, x, B_TWO_STATE_BUTTON);
			AddChild(visible_button);
			
			
			rect.OffsetBy(16+2,0);
			p_mod = new PaintModeView("",rect,shared,this);
			AddChild(p_mod);

bb_help.SetHelp(visible_button,Language.get("VISIBLE"));
bb_help.SetHelp(p_mod,Language.get("VISIBLE"));

float ratio_lh;			
if (ly->layer_type == LAYER_TYPE_BITMAP) ratio_lh = ly->img->Bounds().Width() / ly->img->Bounds().Height();
else ratio_lh =1;

if (ratio_lh > 1) //plus large que haut...
	  mini_view_rect.Set(0,0,(LAYER_VIEW_HEIGHT-8)*1.333,((LAYER_VIEW_HEIGHT-8))/ratio_lh);
else  mini_view_rect.Set(0,0,(LAYER_VIEW_HEIGHT-8)*ratio_lh,LAYER_VIEW_HEIGHT-8);

//Centrage
if (mini_view_rect.Height() < LAYER_VIEW_HEIGHT-8) 
		mini_view_rect.OffsetBy(0,( (LAYER_VIEW_HEIGHT-8)-mini_view_rect.Height())/2);
if (mini_view_rect.Width() < (LAYER_VIEW_HEIGHT-8)*1.333) 
		mini_view_rect.OffsetBy( (((LAYER_VIEW_HEIGHT-8)*1.333)-mini_view_rect.Width() )/2,0);


mini_view_rect.OffsetBy(4+16+4+16+4, 4);
mini_alpha_rect = mini_view_rect;
mini_alpha_rect.OffsetBy(8+((LAYER_VIEW_HEIGHT)*1.3),0);


rect.Set(80,4,Bounds().right-4,20);
the_nom = new LayerNameView("",the_layer->name,rect,this);
AddChild(the_nom);

char str[200];
if (ly->layer_type == LAYER_TYPE_BITMAP) sprintf(str,"%ld x %ld pixels", int32(the_layer->img->Bounds().Width()+1), int32 ( the_layer->img->Bounds().Height()+1));
else sprintf(str,ly->name);
bb_help.SetHelp(this, str);


}


void OneLayerView::Draw(BRect update_rect)
{

if (ThePrefs.no_pictures_left==OFF) //désactivé aussi pendant chargement et création de pic et layer
	{
	
			switch(the_layer->layer_type)
			{	
				case LAYER_TYPE_BITMAP:		
				DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	break;		
				case LAYER_TYPE_TEXT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 
				break;		
				case LAYER_TYPE_EFFECT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				case LAYER_TYPE_GUIDE:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				default: beep(); beep(); printf("Layer has invalid type!!!!"); break;				
			}
			
			



		SetHighColor(0,0,200);
		SetPenSize(1);


	if (the_layer->active==true)
	{ 	
		StrokeRect(Bounds(), B_SOLID_HIGH);
		
		if (the_layer->alpha_activated==false)
		{
			//DrawBitmap(alpha_pic,alpha_pic->Bounds(),mini_alpha_rect);
			StrokeRect(mini_alpha_rect, B_SOLID_HIGH);
			p_mod->active_pic =1; //pinceau (dessin sur le calque)
			
		}
		else 
		{
			StrokeRect(mini_view_rect, B_SOLID_HIGH);
			p_mod->active_pic =2; //masque (Rond dans carré)
			
		}
	}
	
	else 	p_mod->active_pic =0; //vide (masque pas actif
}
	
}		



void OneLayerView::MouseDown(BPoint point)
{

	BMessage msg_x(ACTIVATE_LAYER);
	msg_x.AddInt32("num",the_layer->id);
	util.mainWin->PostMessage(&msg_x);

}


LayerNameView::LayerNameView(const char *name, const char *layer_name, BRect r, BView* the_p) : 
	BStringView(r, name, layer_name, B_FOLLOW_ALL, B_WILL_DRAW )
{
	the_parent = the_p;
}

void LayerNameView::MouseDown(BPoint point)
{
 
 the_parent->MouseDown(point);
 	
}


LayerWindow::LayerWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | 
							B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{

shared=sh;

//------------------------
shared->display_menu->ItemAt(2)->SetMarked(true);
ThePrefs.layer_selector_open=true;
util.layerWin=this;

BRect a_rect  = Bounds();
a_rect.bottom=LAYER_TOP_HEIGHT;

options_view = new LayerOptionsView(a_rect,shared);
AddChild(options_view);


BRect rect  = Bounds();

rect.right -= B_V_SCROLL_BAR_WIDTH;
rect.top += LAYER_TOP_HEIGHT+1;

pr_br_view = new AllLayersView(rect, shared);

perso_scroll_view = new BScrollView(Language.get("LAYERS"), pr_br_view, 
										    B_FOLLOW_ALL, B_WILL_DRAW, FALSE, TRUE);


AddChild(perso_scroll_view);

//AddChild(perso_scroll_view);

SetSizeLimits(ThePrefs.layer_frame.Width(),400 , LAYER_VIEW_HEIGHT+LAYER_TOP_HEIGHT+1, 3000);	
 //toujours la pic activée ...


FrameResized(Bounds().Width(),Bounds().Height()); //pour resize scroll barr
PostMessage(INIT_PERSO);

/*
if (shared->act_img!=NULL)
{
	BMessage tmp(DRAW_LAYERS);
	tmp.AddInt32("active",shared->act_img->active_layer);	
	PostMessage(&tmp);
}
*/
PostMessage(new BMessage(TRANSP_CHANGED));

 //pour ramener au front la pic dessin

}


LayerWindow::~LayerWindow()
{
	ThePrefs.layer_selector_open = false;
	ThePrefs.layer_frame = Frame(); //in order to use it again later
	shared->display_menu->ItemAt(2)->SetMarked(false);
}

void AllLayersView::AddLayers()
{

Window()->FrameResized(Window()->Bounds().Width(),Window()->Bounds().Height()); //histoire qu'il réactive les menus

if (shared->act_img!=NULL)
{

Window()->Lock();


//On efface les anciens Layers
while (ChildAt(0) !=NULL) ChildAt(0)->RemoveSelf();

//----------------------------------------------------------------------------------

	BRect rect;
	rect.Set(0,0,Bounds().Width()-1,LAYER_VIEW_HEIGHT);

	char str[255];

    int32 i = shared->act_img->layer_amount;
	while (i!= -1) //ordre décroissant pour que le backgroud se trouve en bas... logique comme photoshop 
	{ 
	
		sprintf(str,"lyview_%ld",i);
		tab_layer_views[i]= new OneLayerView(str, rect, shared->act_img->the_layers[i], shared);
	
		rect.OffsetBy(0,LAYER_VIEW_HEIGHT); //horizontale
	
		AddChild(tab_layer_views[i]);
	i--;
	} 
		
Window()->Unlock();

Window()->FrameResized(Window()->Bounds().Width(),Window()->Bounds().Height()); //pour la barre
}//fin if (shared->act_img!=NULL)

}


void LayerWindow::FrameResized(float x, float y)
{
	if (ThePrefs.no_pictures_left ==OFF)
	{
	if (shared->act_img!=NULL)
	{
	
	is_disabled = false;
 	options_view->opacity->SetEnabled(true);
	options_view->draw_mode->SetEnabled(true);
	
	int32 am = shared->act_img->layer_amount;

	if (pr_br_view!=NULL)
		{
			int32 val =  (int32) ( ((am+1)*LAYER_VIEW_HEIGHT)+ -(y-LAYER_TOP_HEIGHT)); // div par 3 et arrondi vers le haut
			perso_scroll_view->ScrollBar(B_VERTICAL)->SetRange(0,val);
		}
	}
	else
	{
	
		is_disabled = true;
		options_view->opacity->SetEnabled(false);
		options_view->draw_mode->SetEnabled(false);
	
	}
	}
	
	
}

void LayerWindow::MessageReceived(BMessage *msg)
{

if (is_disabled==false && ThePrefs.no_pictures_left==OFF)
{
	
		
int32 act;
int32 i;
char str[255];
char str2[255];
BMessage *msg_a;
int32 lay_amount;
BBitmap* img;
BRect rect;
Layer *lyp;
void *ptr;
BAlert *alert;
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;

		case DRAW_LAYERS:
		
		//clear old squares and draw the new one.
		i=0;
		lay_amount = shared->act_img->layer_amount;
		 while(i != lay_amount+1)
		 {   
			if (pr_br_view->tab_layer_views[i]!=NULL)
			{
				pr_br_view->tab_layer_views[i]->the_layer->active=false;
				pr_br_view->tab_layer_views[i]->Invalidate();
				pr_br_view->tab_layer_views[i]->Draw(pr_br_view->tab_layer_views[i]->Bounds());
	
				pr_br_view->tab_layer_views[i]->p_mod->active_pic = ICON_EMPTY;
				pr_br_view->tab_layer_views[i]->p_mod->Invalidate();
				pr_br_view->tab_layer_views[i]->p_mod->Draw(pr_br_view->tab_layer_views[i]->p_mod->Bounds());
		
				if (pr_br_view->tab_layer_views[i]->the_layer->is_visible ==true)
					pr_br_view->tab_layer_views[i]->visible_button->SetValue(B_CONTROL_ON);
				else 	pr_br_view->tab_layer_views[i]->visible_button->SetValue(B_CONTROL_OFF);
			}
			i++;
		} 
		
		msg->FindInt32("active",&act);	
		
		pr_br_view->tab_layer_views[act]->Invalidate();
		pr_br_view->tab_layer_views[act]->Draw(pr_br_view->tab_layer_views[act]->Bounds());
		pr_br_view->tab_layer_views[act]->the_layer->active=true;
		
		pr_br_view->tab_layer_views[act]->p_mod->active_pic = ICON_PAINT;
		pr_br_view->tab_layer_views[act]->p_mod->Invalidate();
		pr_br_view->tab_layer_views[act]->p_mod->Draw(pr_br_view->tab_layer_views[act]->p_mod->Bounds());
		
		shared->act_lay = pr_br_view->tab_layer_views[act]->the_layer;
		
	
			
		options_view->draw_mode->Menu()->ItemAt(shared->act_lay->draw_mode)->SetMarked(true); 
		options_view->opacity->SetValue(shared->act_lay->opacity);
		/*
			//first layer must be in normal mode at 100% opacity
			if (shared->act_lay->id ==0)
			{
				options_view->draw_mode->SetEnabled(false);
				options_view->opacity->SetEnabled(false);
			}
			*/
		PostMessage(TRANSP_CHANGED);
		break;
		
		case UPDATE_ACTIVE_LAYER:
		msg->FindInt32("active",&act);	
		pr_br_view->tab_layer_views[act]->Draw(pr_br_view->tab_layer_views[act]->Bounds());
		break;
		
		case INIT_PERSO:
		if (msg->FindInt32("active",&act)!=B_OK) act =0;	

		pr_br_view->AddLayers();
		msg_a = new BMessage(DRAW_LAYERS);
		msg_a->AddInt32("active",act); //select le premier
		if (ThePrefs.layer_selector_open==true) util.layerWin->PostMessage(msg_a);
		
		break;
		
		case MAKE_VISIBLE:
	//	with MetroWerks on PPC you could just do :  msg->FindPointer("layer",&lyp);	
	// but with GNU you need void pointer and then a cast to Layer*
		msg->FindPointer("layer",&ptr);	
		lyp = (Layer*) ptr; //cast as Layer*

		if (lyp->is_visible==true) lyp->is_visible=false;
		else lyp->is_visible=true;
		
		switch(lyp->layer_type)
			{	
				case LAYER_TYPE_BITMAP:		
				msg_a = new BMessage(UPDATE_ME);
				msg_a->AddRect("zone",shared->act_img->the_layers[0]->img->Bounds());
				util.mainWin->PostMessage(msg_a);
				break;
				case LAYER_TYPE_TEXT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 
				break;		
				case LAYER_TYPE_EFFECT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				case LAYER_TYPE_GUIDE:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				default: beep(); beep(); printf("Layer has invalid type!!!!"); break;				
			}
			
		
		break;
		
		case TRANSP_CHANGED:
		if (shared->act_img!=NULL)
		{
		sprintf(str,Language.get("OPACITY"));
		sprintf(str2," %ld%%",int32(options_view->opacity->Value()));
		strcat(str,str2);
		options_view->opacity->SetLabel(str);
		shared->act_lay->opacity = options_view->opacity->Value();

		switch(shared->act_lay->layer_type)
			{	
				case LAYER_TYPE_BITMAP:		
				msg_a = new BMessage(UPDATE_ME);
				msg_a->AddRect("zone",shared->act_img->the_layers[0]->img->Bounds());
				util.mainWin->PostMessage(msg_a);
				break;
				case LAYER_TYPE_TEXT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 
				break;		
				case LAYER_TYPE_EFFECT:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				case LAYER_TYPE_GUIDE:	
				//DrawBitmap(the_layer->img,the_layer->img->Bounds(),mini_view_rect); 	
				break;		
				default: beep(); beep(); printf("Layer has invalid type!!!!"); break;				
			}
					
		
		}
		break;
		
		case ADD_LAYER:
		shared->act_img->CreateNewLayer(NULL);
		//attention layer_amount a changé!
		shared->act_lay = shared->act_img->the_layers[shared->act_img->active_layer];
		shared->initLayer();
		pr_br_view->AddLayers();
		
		msg_a = new BMessage(DRAW_LAYERS);
		msg_a->AddInt32("active", shared->act_img->active_layer);	
		PostMessage(msg_a);
		break;
		
		case ADD_GUIDE_LAYER:
		shared->act_img->CreateNewGuideLayer(NULL);
		//attention layer_amount a changé!
		shared->act_lay = shared->act_img->the_layers[shared->act_img->active_layer];
		pr_br_view->AddLayers();
		
		msg_a = new BMessage(DRAW_LAYERS);
		msg_a->AddInt32("active", shared->act_img->active_layer);	
		PostMessage(msg_a);
		break;
		
		case DELETE_LAYER: 
		if (shared->act_img->layer_amount <= 0)
		{
			alert = new BAlert("",Language.get("NEED_ONE_LAYER"),"Oops"); 
    		alert->Go();
    	}
		else
		{
			ThePrefs.no_pictures_left=OFF; //stop drawing
			act = shared->act_img->active_layer;
			shared->act_img->DeleteLayer(act);
			//attention layer_amount a changé!
			shared->act_lay = shared->act_img->the_layers[shared->act_img->active_layer];
			shared->initLayer();
			ThePrefs.no_pictures_left=ON; //restart drawing
			pr_br_view->AddLayers();	

			msg_a = new BMessage(DRAW_LAYERS);
			msg_a->AddInt32("active",shared->act_img->active_layer);	
			PostMessage(msg_a);
	
			msg_a = new BMessage(ACTIVATE_LAYER);
			msg_a->AddInt32("num",shared->act_img->active_layer);
			util.mainWin->PostMessage(msg_a);
		}
		break;
		
		case DUPLICATE_LAYER:
		switch(shared->act_lay->layer_type)
			{	
				case LAYER_TYPE_BITMAP:		
				shared->act_img->NewLayerFromBmp(shared->act_lay->img);
				//attention layer_amount a changé!
				shared->act_lay = shared->act_img->the_layers[shared->act_img->active_layer];
				shared->initLayer();
				break;
				case LAYER_TYPE_TEXT:	
				util.NotImplemented();
				break;		
				case LAYER_TYPE_EFFECT:	
				util.NotImplemented();
				break;		
				case LAYER_TYPE_GUIDE:	
				util.NotImplemented();
				break;		
				default: beep(); beep(); printf("Layer has invalid type!!!!"); break;				
			}
		
		pr_br_view->AddLayers();

		msg_a = new BMessage(DRAW_LAYERS);
		msg_a->AddInt32("active",shared->act_img->active_layer);	
		PostMessage(msg_a);
		break;
		
		case MERGE_LAYERS: 	
		util.NotImplemented();
		break;
		
		case MERGE_VISIBLE_LAYERS: 
		util.NotImplemented();
		break;
		
		case FLATTEN_IMAGE: 		
		util.NotImplemented();
		break;
		
		case DISPLAY_OPTIONS:	
		util.NotImplemented();
		break;

		case MODE_CHANGED: 		
		shared->act_lay->draw_mode = options_view->draw_mode->Menu()->IndexOf(options_view->draw_mode->Menu()->FindMarked());

		msg_a = new BMessage(UPDATE_ME);  
		msg_a->AddRect("zone",shared->act_img->the_layers[0]->img->Bounds());
		util.mainWin->PostMessage(msg_a);
		break;

	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }//fin switch

}//fin if shared->act_img!=NULL
   	 	
}


void LayerWindow::FrameMoved(BPoint screenPoint)
{
	 //pour ramener au front la pic dessin
}


void OneLayerView::CreateButton(char nm[255],char nm2[255])
{
BBitmap *downBitmap, *upBitmap;
  	
			BRect rect;
			rect.Set(0,0, 16-1, 16-1);

			//fill bitmap
			downBitmap = BTranslationUtils::GetBitmap('RAWT',nm);
			upBitmap = BTranslationUtils::GetBitmap('RAWT',nm2);

			
			//sécurité
			if (upBitmap==NULL) upBitmap = new BBitmap(rect, B_RGB32);
			if (downBitmap==NULL) downBitmap = new BBitmap(rect, B_RGB32);
		

			//Faut un window pour le faire
			BWindow* x= new BWindow(rect,"", B_FLOATING_WINDOW, Flags(), B_CURRENT_WORKSPACE);
			
			//tempview for creating the picture
			BView *tempView = new BView( rect, "temp", B_FOLLOW_NONE, B_WILL_DRAW );
			
			x->AddChild(tempView);
			//create on picture
   			tempView->BeginPicture(new BPicture); 
   			tempView->DrawBitmap(upBitmap);
   			on = tempView->EndPicture();
   			//create off picture
   			tempView->BeginPicture(new BPicture); 
   			tempView->DrawBitmap(downBitmap);
   			off = tempView->EndPicture();
   			//get rid of tempview
   			x->RemoveChild(tempView);
   			delete tempView;
			delete x;
}