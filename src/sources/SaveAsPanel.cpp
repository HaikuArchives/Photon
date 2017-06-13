#include <stdio.h>

#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <TranslationKit.h>

#include "SaveAsPanel.h"

SaveAsPanel::SaveAsPanel( BLooper *looper, share *sh  )
{

shared = sh ;


	if( looper )
	{
		looper->Lock();
			looper->AddHandler( this );
		looper->Unlock();
	}
	
	target = new BMessenger( NULL, looper );
	
	BMessenger messenger( this );
	panel = new BFilePanel( B_SAVE_PANEL, &messenger );

	panel->Window()->Lock();
	panel->Window()->SetTitle(Language.get("SAVE_AS"));
	panel->SetButtonLabel(B_DEFAULT_BUTTON,Language.get("SAVE"));
	panel->SetButtonLabel(B_CANCEL_BUTTON,Language.get("CANCEL"));
		 
		BRect menuRect;
		BView *background = panel->Window()->ChildAt(0);
		
		menuRect.left = background->Bounds().right - 210;
		menuRect.top = background->Bounds().top +  background->FindView("MenuBar")->Bounds().Height() + 5;
		menuRect.bottom =  menuRect.top + 20;
		menuRect.right =  menuRect.left + 200;
		
		menuField = new BMenuField( menuRect, "FileTypes", "Format:", BuildFormatsMenu(), B_FOLLOW_RIGHT | B_FOLLOW_TOP );
		menuField->SetAlignment( B_ALIGN_RIGHT );
		background->AddChild( menuField );
	panel->Window()->Unlock();

	saveTypeWanted=0;
	toBeSaved = NULL;


}

SaveAsPanel::~SaveAsPanel()
{
	delete target;
	delete panel;
}
	
void SaveAsPanel::SaveAs( BBitmap *bitmap)
{	

	toBeSaved = bitmap;

	//-------------------------
		
	ThePrefs.no_pictures_left=ON; //pour stop display
	the_thumbnail = new DThumbnail(toBeSaved , true); // the thumbnail will be dithered 
	ThePrefs.no_pictures_left=OFF; //pour réactiver affich...

float height=0;
//DISABLED FOR POWERPC (PPC) since thumbnail doesn't work?
height = the_thumbnail->Bitmap()->Bounds().Height()+4;

if (height < 74) height=74; //pour afficher les infos textes.. 
							//si la pic est plus petite que la taille du thumbnail
							//la thumbnaillib le met à sa taille ori

  		panel->Window()->Lock();
  		BView *background = panel->Window()->ChildAt(0); //trouve view backgd c'est la premiere (0)

		background->ResizeBy(0,height*-1); //un peu de place en haut... pour la Preview
		background->MoveBy(0,height);
		
		BRect limit = background->Bounds();
		limit.bottom=height;
		fond_thumb = new ThumbView(limit,shared);
		panel->SetRefFilter(new ImgRefFilter(shared));

  		panel->Window()->AddChild(fond_thumb);
  		panel->Window()->Unlock();
	//-------------------------
	ThePrefs.save_panel_open=true;
	panel->Show();
	
}

void SaveAsPanel::CancelSave()
{
	ThePrefs.save_panel_open=false;
	panel->Hide();
	toBeSaved = NULL;
}

void SaveAsPanel::SetTarget( BMessenger *messenger )
{
	target = new BMessenger( *messenger );
}

BFilePanel *SaveAsPanel::Panel()
{
	return panel;
}

void SaveAsPanel::SetDefaultFormat( int32 type )
{
	panel->Window()->Lock();
		
		BMenu *popUp = ((BMenuField *) panel->Window()->ChildAt(0)->FindView("FileTypes"))->Menu();
		
		int i=0;
		BMenuItem *item;
		while( (item = popUp->ItemAt(i++)) != NULL )
			if( item->Message()->FindInt32("type_code") == type )
			{
				saveTypeWanted = type;
				item->SetMarked( true );
				break;
			}

	panel->Window()->Unlock();
}

#include "PBitmapModif.h"

void SaveAsPanel::MessageReceived(BMessage *message)
{

BNode node;
entry_ref ref;
BDirectory dir;
BFile file;
BEntry entry;
BPath the_path; 

char str[512];

	switch( message->what )
	{
	case B_SAVE_REQUESTED:
		
			message->FindRef( "directory", &ref );
			dir.SetTo( &ref );
			
			file.SetTo( &dir, message->FindString("name"), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE );
			if( file.InitCheck() == B_OK )
			{
	//			BBitmapStream inStream( toBeSaved );
				
				ProgressiveBitmapStream inStream( toBeSaved  ); // init with contents of bitmap 
      					
				sprintf(str, Language.get("SAVING"));
				strcat(str," ");
				strcat(str,message->FindString("name"));
				inStream.DisplayProgressBar(str);
      
				if( BTranslatorRoster::Default()->Translate( &inStream, NULL, NULL, &file, saveTypeWanted ) == B_OK )
				{
					message->what = M_BITMAP_SAVED;
					target->SendMessage( message );
						
		
					entry.SetTo(&ref,true);
					entry.GetPath(&the_path);
		
					sprintf(shared->act_img->full_path,the_path.Path());
					strcat(shared->act_img->full_path,"/");
					strcat(shared->act_img->full_path,message->FindString("name"));
	

					if (ThePrefs.save_with_thumb==B_CONTROL_ON && the_thumbnail->Bitmap())    // will be NULL if the thumbnail couldn't be created 
						{ 
	 					  	node.SetTo(shared->act_img->full_path); 
    						the_thumbnail->WriteThumbnailAttribute(&node); 
    						the_thumbnail->WriteIconAttribute(&node); 
    						the_thumbnail->WriteMiniIconAttribute(&node); 
    						the_thumbnail->WriteResolutionAttributes(&node); 
						} 
						
					//si on choisi un titre (nouveau fichier faut updater)
					util.mainWin->PostMessage(new BMessage(UPDATE_TITLE)); 
						
						
				}
			//	inStream.DetachBitmap( &toBeSaved );
			}

		break;

	case B_CANCEL:
	break;
		
	case M_SAVEASPANEL_TYPE:
	//beep();beep();beep();
		saveTypeWanted = message->FindInt32("type_code");
		break;

	default:
		BHandler::MessageReceived(message);
	}
}

// Builds a PopUp menu with all the possible file types
//
// KNOWN PROBLEM: If more than one translator can produce a given file type there will be a duplicate entry
 

 
BPopUpMenu *SaveAsPanel::BuildFormatsMenu()
{
	BMessage *message;
	bool first=true;
	
	BPopUpMenu *popUp = new BPopUpMenu( "" );
	BMenuItem *item;
	
	//	Find the translator to use for us
	translator_id * all_translators = NULL;
	int32 count = 0;
	status_t err = BTranslatorRoster::Default()->GetAllTranslators(&all_translators, &count);
	if (err >= B_OK)
	{
		err = B_ERROR;
		//	look through all available translators
		for (int ix=0; ix<count; ix++)
		{
			const translation_format *in_formats;
			int32 fmt_count;
			if( B_OK <= BTranslatorRoster::Default()->GetInputFormats(all_translators[ix], &in_formats, &fmt_count) )
			{
				//	look for translators that accept BBitmaps as input
				for (int iy=0; iy<fmt_count; iy++)
				{
					if (in_formats[iy].type == B_TRANSLATOR_BITMAP)
					{
						const translation_format *out_formats;
						if( B_OK <= BTranslatorRoster::Default()->GetOutputFormats(all_translators[ix],	&out_formats, &fmt_count))
						{
							//	look through the output formats
							for (int iy=0; iy<fmt_count; iy++)
							{
								//	and take the first output format that isn't BBitmap
								if( out_formats[iy].type != B_TRANSLATOR_BITMAP)
								{
									message = new BMessage( M_SAVEASPANEL_TYPE );
									message->AddInt32( "type_code", out_formats[iy].type );
									message->AddString( "name", out_formats[iy].name );
									item = new BMenuItem( out_formats[iy].name, message);
									item->SetTarget(this);
			
									if( first == true )
									{
										item->SetMarked( true );
										saveTypeWanted = out_formats[iy].type;
										first = false;
									}
									
									popUp->AddItem( item );
								}
							}
						}
					}
				}
			}
		}
	}
	delete[] all_translators;

	return popUp;
}
