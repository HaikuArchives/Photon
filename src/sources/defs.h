#ifndef DEFS_H
#define DEFS_H

#define SET_CURSOR 'scur'
#define CURSOR_OF_ACTIVE_TOOL 6666 

#define IMAGE_CHOSEN	'imch'
#define PIC_CREATED 'pccr'

#define START_X_SIZE 640
#define START_Y_SIZE 480
#define INFO_H		 48

#define MAX_LAYER		 255 //ca fait déja beaucoup

#define SHOW_BRUSH_WIN 'shbw'
#define SHOW_PAPER_WIN 'shpw'
#define SHOW_LAYER_WIN 'shlw'
#define SHOW_INFO_WIN	'shiw'
#define SHOW_OPTION_WIN	'shop'
#define SHOW_TOOL_WIN	'shtw'
#define SHOW_NAVIGATION_WIN	'shnw'

#define CANCEL_FILTERING 'ccfl'
#define APPLY_FILTER     'apfl'


#define ON true
#define OFF false

#define MARGIN 64 //nb de pixels à l'éxtérieur de la pic

#define NAME_SIZE	8192
#define TEMP_BRUSH_VALUE 2000000000 //2 milliards... 
						 //histoire d'être sûr qu'on a jamais autant de brushs

//paint modes
#define NORMAL   	0  //ATTENTION CA DOIT IMPERATIVEMENT COMMENCER A zero
#define MULTIPLY	1
#define LIGHTEN 	2
#define DARKEN 		3
#define COLORIZE	4
#define DIFFERENCE	5
#define COMBINE		6

#define SHOW_PROGRESS_LIMIT 1000*1024 //on affiche le progresse qui si on touche plus de 1 mb

#define ZOOM_AMOUNT		0.25
#define MAX_UNDO 1024

//MESSAGES
#define DO_IT  'doit'
#define CANCEL 'canc'
#define SAVE_WITH_NAME 'svwn'

#define UPDATE_ME  'ptme'
#define DRAW_ME  'drme'

#define SHOW_PROGRESS_WIN  'swpw'
#define HIDE_PROGRESS_WIN  'hdpw'


#define PICKER_SCAN  'pksc'

#define TEST 		'test'
#define MENU_LOAD 	'load'
#define MENU_CLOSE 	'clos'
#define MENU_REVERT 'revr'
#define MENU_SAVE 	'save' //save as!!!
#define MENU_SAVE_DIRECT 	'svdr' //save direct


#define MENU_EDIT_CUT 'edct'
#define MENU_EDIT_COPY 'edcp'
#define MENU_EDIT_PASTE 'edps'


#define DISABLE_ALL 'dsal'
#define ENABLE_ALL  'ebal'

#define MENU_NEW 	'newi'

#define UPDATE_DISPLAY 'uddp'
#define ZOOM_IN 	  	'zmin'


#define TRANSLATOR_SELECTED 'trsl'
#define MASK_ENABLE_DISABLE 'mskd'
#define MASK_DELETE 'mdel'
#define MASK_DELETE_INIT 'mdli'

#define MOUSE_MOVEMENT  'mmvt'
#define MENU_FILE_QUIT 	'mfqt'
#define MENU_HELP_ABOUT 'mhab'
#define MENU_EDIT_UNDO  'edun'
#define MENU_EDIT_REDO  'edrd'
#define EDIT_PURGE_UNDO 'epru'

#define MENU_CURVES  	'curv'
#define MENU_INVERT  	'invr'
#define MENU_CROP  		'crop'
#define MENU_CANVAS		'canv'
#define MENU_RESIZE		'resz'

#define MENU_INVERT_MASK  	'invm'
#define PICK_BRUSH_SELECT 'pcbr'
#define USE_SEL_AS_PAPER 'uspp'

#define BRUSH_CHANGED     'brch' //pour toolwindow
#define PAPER_CHANGED	'ppch'

#define PAPER_NEXT   	'ppnx'
#define PAPER_PREVIOUS  'pppv'
#define BRUSH_NEXT   	'brnx'
#define BRUSH_PREVIOUS  'brpv'
#define BRUSH_PERSO_NEXT   	  'bpnx'
#define BRUSH_PERSO_PREVIOUS  'bppv'

//BRushwin
#define DRAW_PAPERS	'drpp'
#define DRAW_BRUSHES 'drbr'
#define DRAW_PERSO_BRUSHES 'dpbr'
#define INIT_PERSO 'inpr'
#define BRUSH_TITLE   'btle'
#define PAPER_TITLE   'ptle'

#define ACTIVATE_PERSO_BRUSH 'acpb'
#define ACTIVATE_BRUSH 		 'acbr'
#define ACTIVATE_PAPER 'acpp'
#define ACTIVATE_THUMB_IMG 'acth'
#define ACTIVATE_LAYER 'acly'
#define DRAW_LAYERS 'dwly'
#define UPDATE_ACTIVE_LAYER 'ualy'


#define ZOOM_IN 	  	'zmin'
#define ZOOM_OUT  		'zmot'
#define ZOOM_CHANGED	'zmch'

#define MODE_CHANGED		'mdch'


#define TOOL_CHANGED	'tchd'
#define COL_CHANGED	'cohd'
#define COL_SELECTED	'csel'
#define TRANSP_CHANGED  'trch'

#define FORE_CLICKED	'fgck'
#define BACK_CLICKED	'bkck'
#define TEXTURE_CLICKED	'txck'
#define BRUSH_CLICKED	'brck'
#define UPDATE_TITLE    'uptt'
#define UPDATE_WIN_MENU 'upwm'

#define SAVE_THUMB_CHANGED    'stch'
#define ONLY_IMG_CHANGED    'oich'


//BUTTON MESSAGE
#define BUTTON_01_MSG   'bt01'
#define BUTTON_02_MSG   'bt02'
#define BUTTON_03_MSG   'bt03'
#define BUTTON_04_MSG   'bt04'
#define BUTTON_05_MSG   'bt05'
#define BUTTON_06_MSG   'bt06'
#define BUTTON_07_MSG   'bt07'
#define SET_MASK_BUTTON_OFF 'smbo'

#define BUTTON_08_MSG   'bt08'
#define BUTTON_09_MSG   'bt09'

#define BUTTON_B_01_MSG   'bb01'
#define BUTTON_B_02_MSG   'bb02'
#define BUTTON_B_03_MSG   'bb03'
#define BUTTON_B_04_MSG   'bb04'
#define BUTTON_B_05_MSG   'bb05'
#define BUTTON_B_06_MSG   'bb06'
#define BUTTON_B_07_MSG   'bb07'
#define BUTTON_B_08_MSG   'bb08'
#define BUTTON_B_09_MSG   'bb09'

#define RESIZE_CSEL        'rscs'
#define CLICK_SQUARE        'clsq'
#define CLICK_SPECTRUM        'clsp'

//TOOLS

#define PAINTBRUSH			100
#define	ERASER				1
#define	BUCKET				2
#define	LINE				3
#define	F_POLY				4
#define	SPLINE				5
#define	F_SPLINE_POLY		6
#define	RECTANGLE			7
#define	F_RECTANGLE			8
#define	ELLIPSE				9
#define	F_ELLIPSE			10
#define	TEXT				11
#define	ZOOM				12

#define	PICKER				13
#define	STAMP				14
#define	WAND				15
#define LASSO				16
#define	SEL_RECT			17
#define	SEL_ELLIPSE			18
#define	MOVE				19
#define PICK_BRUSH			20
#define MAX_TOOL			20

//FROM IMAGE_MANIP EXAMPLE
#define IMAGE_MANIP		'mnip'
#define IMAGE_CONV		'conv'

//MASK c'est pas un tool c'est une option


#define FORE_COLOR 100
#define BACK_COLOR 200
#define MASK_FORE_COLOR 50
#define MASK_BACK_COLOR 75

#define UNIT_CM 0
#define UNIT_MM 1
#define UNIT_INCH 2
#define UNIT_PIXELS 3

#define DPI 0
#define DPCM 1

#define MM_PER_INCH 25.4

#define PREFS_DIR "/boot/home/config/settings/Photon"



#endif