#include <Path.h>
#include <File.h>
#include <FindDirectory.h>
#include <support/UTF8.h>
#include <stdio.h>

#include <Application.h>
#include <Window.h>
#include <View.h>

#include <Alert.h>
#include <Beep.h>
#include <Screen.h>
#include <Roster.h>
#include <Control.h>
#include <StatusBar.h>
#include <AppFileInfo.h>



#include "LanguageClass.h"

LanguageClass	Language;

LanguageClass::LanguageClass()
		: LList(100) {
}


void LanguageClass::InitLang()
{


	if(!load()){
//		beep();
		set("NOT_YET", "Not implemented yet!");
		set("YES", "Yes");
		set("NO", "No");
		set("CANCEL", "Cancel");
		set("VERSION_BY", "English version by");
		set("VERSION_AUTHOR", "Santiago Lema (santiago.lema@cryogen.ch)");

		set("GET_LATEST_VERSION", "Get latest version on BeWare, BeBits or at:");
		set("MEM_USAGE", "Memory in use:");
		set("NO_IMAGE_LOADED", "No image currently loaded.");
		set("MODIFIED_SAVE", "This picture has been modified. Closing it will lose changes. Would you like to save it before?");
		set("RELOAD_SAVED", "Do you really wish to reload last saved version and lose changes?");
		set("ALREADY_LOADED", "This picture is already loaded. Do you wish to open another copy?");
		set("COULD_NOT_LOAD", "For some reason, I could not load bitmap named :");
		set("NOT_RECOGNIZED","File is probably not an image... or the format is unknown");
		set("CREATE_NEW","New Image");
		set("DEST_NOT_B_RGB32", "This conversion filter tried to create something else than a 32 bits RGB image, which is all this software supports.");
		set("LOADING","Loading ");
		set("LOADING_THUMB","Loading Thumbnails...");
		set("PLEASE_WAIT","Please wait...");
		set("SHOW_THUMBNAIL","Show Thumbnails");
		set("CREATE_THUMBNAIL","Create Thumbnails");
		
		set("OPACITY", 	"Opacity");
		set("MASKING", 	"Quick Mask Mode (Q)");
		set("TOOLS", 	"Tools");
		
		set("FOREGROUND", 	"Foreground Color");
		set("BACKGROUND", 	"Background Color");
		
		set("WIDTH", 	"Width");
		set("HEIGHT", 	"Height");
		set("SAVE_THUMBNAIL", "Save Thumbnail");
		set("ONLY_IMG", "Only Images");
		
		set("RED", 	"Red");
   	    set("GREEN", 	"Green");
	    set("BLUE", 	"Blue");
	    set("ALPHA", 	"Alpha");
		
		//PAINT MODES
		set("PM_NORMAL", 	"Normal");
		set("PM_MULTIPLY", 	"Multiply");
		set("PM_LIGHTEN", 	"Lighten");
		set("PM_DARKEN", 	"Darken");
		set("PM_COLORIZE", 	"Colorize");
		set("PM_DIFFERENCE","Difference");
		set("PM_COMBINE", "Combine");
		
		
		
		//FILE
		set("FILE", 	"File");
		set("NEW", 	"New...");
		set("OPEN", "Open...");
		set("CLOSE", "Close");
		set("REVERT", "Revert to last saved");
		set("SAVE", "Save");
		set("SAVE_AS", "Save as...");
		set("PREFERENCES", "Preferences");
		set("QUIT", "Quit");
		
		//EDIT
		set("EDIT", 	"Edit");
		set("UNDO", "Undo");
		set("REDO", "Redo");
		set("CUT", "Cut");
		set("COPY", "Copy");
		set("PASTE", "Paste");
		set("PR_BRUSH", "Previous Brush");
		set("NX_BRUSH", "Next Brush");
		set("PR_P_BRUSH", "Previous Perso Brush");
		set("NX_P_BRUSH", "Next Perso Brush");
		set("PR_PAPER", "Previous Paper");
		set("NX_PAPER", "Next Paper");
		set("ZOOM_IN", "Zoom In");
		set("ZOOM_OUT", "Zoom Out");
		set("PURGE_UNDO", "Purge Undo");
		set("PICK_BRUSH", "Pick Brush");

		//IMAGE
		set("IMAGE", 	"Image");
		set("CURVES", 	"Curves...");
		set("INVERT", 	"Invert (Negative)");
		set("HSV", 	"Hue, Saturation, Value...");
		set("LIMIT_LEVELS", 	"Limit levels...");
		
		set("CROP", 	"Crop");
		set("CANVAS_SIZE", 	"Canvas Size...");
		set("RESIZE_IMAGE", "Resize Image...");
		set("WEIGHT", "Weight");
		
		set("ROTATION_SYMETRY","Rotation / Symetry...");
		set("ROTATE_180","Rotate 180°" );
		set("ROTATE_PLUS_90", "Rotate +90°");
		set("ROTATE_MINUS_90", "Rotate -90°");
		set("ROTATE_FREE", "Rotate freely...");
		set("HORIZONTAL_SYMETRY","Horizontal Symetry");
		set("VERTICAL_SYMETRY", "Vertical Symetry");
	
		//
		set("CONVERT_IMAGE", "Convert Image");
		set("MANIPULATE_IMAGE", "Manipulate Image");
		
		//MASK
		set("MASK", 	"Mask");
		set("SELECT_ALL", 	"Select All");
		set("DELETE_MASK", 	"Select None");
		set("INVERT_MASK", 	"Invert Mask");
		set("SELECT_RANGE",	"Select Range...");
	
		set("ENABLE_MASK", "Enable Mask");
		set("DISABLE_MASK", "Disable Mask");


		//WIN
		set("WINDOW", 	"Window");
		set("DISPLAY", 	"Display");
		set("BRUSHES", 	"Brushes");
		set("INFOS", 	"Infos");
		set("OPTIONS", 	"Options");
		set("NAVIGATION","Navigation");

		set("PERSO_BRUSHES", 	"Perso. Brushes");
		set("SETTINGS", 	"Settings");
		set("LOADING_BRUSHES", 	"Loading brushes...");
		set("PAPERS", 	"Papers");
		set("LOADING_PAPERS", 	"Loading papers...");
		set("TEMP_BRUSH", 	"Temp. Brush");
		set("TEMP_PAPER", 	"Temp. Paper");
		
		
		//HELP
		set("HELP", 	"Help");
		set("ABOUT", 	"About");
		set("MAKES_USE_OF", 	"This program makes use of:");


				
		//TOOLS
		set("PAINTBRUSH", "Paintbrush (B)");
		set("ERASER", "Eraser");
		set("BUCKET", "Bucket (K)");
		set("LINE", "Line (N)");
		set("F_POLY", "Filled Polygon");
		set("SPLINE", "Spline");
		set("F_SPLINE_POLY", "Filled Spline Polygon");
		set("RECTANGLE", "Rectangle (R)");
		set("F_RECTANGLE", "Filled Rectangle (R)");
		set("ELLIPSE", "Ellipse (E)");
		set("F_ELLIPSE", "Filled Ellipse (E)");
		set("TEXT", "Text (T)");
		set("ZOOM", "Zoom (Z)");
		set("PICKER", "Picker (I)");
		set("STAMP", "Stamp (S)");
		set("WAND", "(Select) Wand (W)");
		set("LASSO", "(Select) Lasso (L)");
		set("SEL_RECT", "(Select) Rectangle (M)");
		set("SEL_ELLIPSE", "(Select) Ellipse (Shift+M)");
		set("MOVE", "Move Tool (H)");
		set("PICK_BRUSH", "Pick Brush");
		set("USE_AS_PAPER","Use as Paper");
	
		//full screen buttons ('f' key)
		set("WINDOWED","Windowed mode");
		set("NO_TITLE","No title mode");
		set("FULL_SCREEN","Full screen mode");
	
	
		//preferences items
		set("MAIN","Main");
		set("UNITS","Units");
		set("RES_UNITS","Resolution Units");
		
		set("CENTIMETERS","centimeters");
		set("MILLIMETERS","milimeters");
		set("INCHES","inches");
		set("INCH","inch");
		set("CENTIMETER","centimeter");
		set("UNTITLED","Untitled image");
		set("FORMATS","Formats");
		
		set("LANGUAGE","Language");
		set("CHANGES_ON_NEXT_LAUNCH","Changes will take effect on next launch.");
		
		
		//layer win
		set("LAYER","Layer");
		set("LAYERS","Layers");
		set("LAYER_GUIDE","Guide Layer");
		set("VISIBLE","Visible");
		
		set("ADD_LAYER","Add new Layer");
		set("ADD_GUIDE_LAYER","Add Guide Layer");
		set("DELETE_LAYER","Delete this Layer");
 		set("DUPLICATE_LAYER","Duplicate this Layer");
		set("MERGE_LAYERS","Merge Layers");
		set("MERGE_VISIBLE_LAYERS","Merge Visible Layers");
		set("FLATTEN_IMAGE","Flatten Image");
		set("DISPLAY_OPTIONS","Display Options...");

		set("NEED_ONE_LAYER","You need at least one layer.");


		set("CROPPED", "cropped");
		set("FILTERED","filtered");

		set("SAVING","Saving");
		set("LOADING","Loading");
		
		set("COULD_NOT_SAVE_PREFS", "Preferences could not be saved!!!");
		set("COULD_NOT_LOAD_PREFS", "Preferences could not be loaded, resetting to defaults.");
		
		set("FILTERING","Applying filter...");
		set("HIDE_ALL","Hide all windows");

	}
	size = LList.CountItems();
}



char *LanguageClass::get(char *pLookUp){
	int	i;
	
	for(i=0; i<size; ++i)
		if(strcasecmp(((lang_struct*)LList.ItemAt(i))->name, pLookUp) == 0)
			return ((lang_struct*)LList.ItemAt(i))->entry;
	
	return strdup(pLookUp);
}

bool LanguageClass::load(){
	BPath	lLoadPath;
	char	*lBuffer,
			*lLine,
			*lFound,
			*lBufferPtr,
			lItem[80],
			lValue[1024];
	off_t	lSize;
	ssize_t	lBytesRead;
	BFile	*lFile;
	bool	lReturnValue;
	
	lFile = 0;
	lReturnValue = false;
	lSize = 0;
				
	//find_directory(B_USER_SETTINGS_DIRECTORY, &lLoadPath);
	
//----------------------------	
  app_info info;
  be_app->GetAppInfo(&info);  
  BEntry tmp_entry(&info.ref);
  BEntry file_entry;
  tmp_entry.GetParent(&file_entry); //we don't want the execs' name
  file_entry.GetPath(&lLoadPath); 
//----------------------------	

  	lLoadPath.Append("languages");
    lLoadPath.Append(ThePrefs.lang_name);
   /*
    printf("\nPath is: ");
    printf(lLoadPath.Path()); 
    printf("\n\n\n\n");
    printf(ThePrefs.lang_name);
    printf("\n\n\n\n");
    */
    
        
	
	lFile = new BFile(lLoadPath.Path(), B_READ_ONLY);
	if(lFile->InitCheck() == B_NO_ERROR){
		lReturnValue = true;
		lFile->GetSize(&lSize);
		lBuffer = new char[lSize+1];
		lBytesRead = lFile->Read(lBuffer, lSize);
		lBuffer[lBytesRead] = '\0';
		lBufferPtr = lBuffer;
		lFound = strstr(lBufferPtr, "\n");
		while(lFound != 0){
			*lFound = '\0';
			lLine = strdup(lBufferPtr);
			lItem[0] = '\0';
			lValue[0] = '\0';
			if(lLine[0] != ';'){
				sscanf(lLine, "%s = %s", &lItem, &lValue);
				if(strlen(lItem) > 0){
					char	*lK1, 
							*lK2;
					int		lI;
							
					lK1 = strstr(lLine, "{");
					lK2 = strstr(lLine, "}");
					for(lI=0; lI<lK2-lK1-1; ++lI){
						if(*(lK1+lI+1) == '\\' && *(lK1+lI+2) == 'n'){
							lValue[lI] = ' ';
							++lI;
							lValue[lI] = '\n';
						} else if(*(lK1+lI+1) == '\\' && *(lK1+lI+2) == 't'){
							lValue[lI] = ' ';
							++lI;
							lValue[lI] = '\t';
						} else if(*(lK1+lI+1) == '\\' && *(lK1+lI+2) == 'e'){
							lValue[lI] = (char) B_UTF8_ELLIPSIS;
							++lI;
							lValue[lI] = ' ';
						} else 
							lValue[lI] = *(lK1+lI+1);
					} 
					lValue[lI] = '\0';
					set(lItem, lValue);
				}
			}
			lBufferPtr = lFound + 1;
			lFound = strstr(lBufferPtr, "\n");
		}
		delete [] lBuffer;
	}
	return lReturnValue;
};

void LanguageClass::set(char *pName, char *pEntry, bool pCheck){
	if(pCheck){
	} else {
		LList.AddItem(new lang_struct(pName, pEntry));
	}
}