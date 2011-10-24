/*
** This table contains the machine-independent predefined routines
** for the OOT interpreter.
*/


/*
** Do not change this struct declaration without fully understanding
** how external routines are handled.
*/
struct LookupEntry {
    char *procName;
    void (*procRoutine)();
};

/*
** Predefined functions used in this module
*/
extern void Language_Execute_Graphics_Drawpic();
extern void Language_Execute_Graphics_Getch();
extern void Language_Execute_Graphics_Hasch();
extern void Language_Execute_Graphics_Sizepic();
extern void Language_Execute_Graphics_Takepic();

extern void Language_Execute_System_Fetcharg();
extern void Language_Execute_System_Flushstreams();
extern void Language_Execute_System_Getenv();
extern void Language_Execute_System_Getpid();
extern void Language_Execute_System_Nargs();
extern void Language_Execute_System_Setactive();
extern void Language_Execute_System_Simutime();
extern void Language_Execute_System_System();

extern void Limits_Getexp();
extern void Limits_Rreb();
extern void Limits_Setexp();

extern void Math_Arctan();	/* Math */
extern void Math_Arctand();
extern void Math_Cos();
extern void Math_Cosd();
extern void Math_Exp();
extern void Math_Ln();
extern void Math_Sign();
extern void Math_Sin();
extern void Math_Sind();
extern void Math_Sqrt();

extern void Math_Rand();            /* Rand */
extern void Math_Randint();
extern void Math_Randnext();
extern void Math_Randomize();
extern void Math_Randseed();

extern void Math_Ceil();	/* Typeconv */
extern void Math_Floor();
extern void Math_Intreal();
extern void Math_Natreal();
extern void Math_Round();

extern void String_Index();
extern void String_Length();
extern void String_Repeat();
extern void String_ReadLineFromCharArray();
extern void String_CharArrayCopy ();
extern void String_WriteLineToCharArray ();
extern void String_WriteToCharArray ();

extern void String_Erealstr();
extern void String_Frealstr();
extern void String_Realstr();
extern void String_Strreal();

extern void MIO_config_display();
extern void MIO_config_lang();
extern void MIO_config_machine();

extern void MIO_crypto_dessetkey();
extern void MIO_crypto_desencrypt();
extern void MIO_crypto_desdecrypt();
extern void MIO_crypto_des3set2key();
extern void MIO_crypto_des3set3key();
extern void MIO_crypto_des3decrypt();
extern void MIO_crypto_des3encrypt();

extern void MIO_dir_open();
extern void MIO_dir_close();
extern void MIO_dir_get();
extern void MIO_dir_getlong();
extern void MIO_dir_create();
extern void MIO_dir_delete();
extern void MIO_dir_change();
extern void MIO_dir_current();
extern void MIO_dir_exists ();

extern void MIO_dlg_alert();

extern void MIO_draw_arc();
extern void MIO_draw_box();
extern void MIO_draw_cls();
extern void MIO_draw_dot();
extern void MIO_draw_fill();
extern void MIO_draw_fillarc();
extern void MIO_draw_fillbox();
extern void MIO_draw_filloval();
extern void MIO_draw_fillpolygon();
extern void MIO_draw_line();
extern void MIO_draw_line_dashed();
extern void MIO_draw_line_thick();
extern void MIO_draw_oval();
extern void MIO_draw_polygon();
extern void MIO_draw_mapleleaf();
extern void MIO_draw_fillmapleleaf();
extern void MIO_draw_star();
extern void MIO_draw_fillstar();
extern void MIO_draw_text();

extern void MIO_error_halt();
extern void MIO_error_last();
extern void MIO_error_laststr();
extern void MIO_error_str();
extern void MIO_error_lastmsg();
extern void MIO_error_msg();
extern void MIO_error_set();
extern void MIO_error_setmsg();
extern void MIO_error_abort();
extern void MIO_error_abortmsg();

extern void MIO_file_status();
extern void MIO_file_copy();
extern void MIO_file_rename();
extern void MIO_file_delete();
extern void MIO_file_diskfree();
extern void MIO_file_exists();
extern void MIO_file_fullpath();
extern void MIO_file_parent();
extern void MIO_file_splitpath();
extern void MIO_file_createpath();

extern void MIO_font_new();
extern void MIO_font_free();
extern void MIO_font_width();
extern void MIO_font_sizes();
extern void MIO_font_startname();
extern void MIO_font_getname();
extern void MIO_font_getstyle();
extern void MIO_font_startsize();
extern void MIO_font_getsize();
extern void MIO_font_name();

extern void MIO_gui_alert();
extern void MIO_gui_choose();
/*
extern void MIO_gui_opendlg();
extern void MIO_gui_savedlg();
extern void MIO_gui_fileviewer();
extern void MIO_gui_request();
extern void MIO_gui_input();

extern void MIO_gui_commondlg_open();
extern void MIO_gui_commondlg_save();
extern void MIO_gui_responsedlg_alert();
extern void MIO_gui_responsedlg_request();
extern void MIO_gui_responsedlg_input();
*/

extern void MIO_hashmap_new ();
extern void MIO_hashmap_free ();
extern void MIO_hashmap_put ();
extern void MIO_hashmap_get ();
extern void MIO_hashmap_remove ();

extern void MIO_input_keydown ();

extern void MIO_lexer_begin();
extern void MIO_lexer_end();
extern void MIO_lexer_scan();

extern void MIO_mouse_where();
extern void MIO_mouse_buttonmoved();
extern void MIO_mouse_buttonwait();
extern void MIO_mouse_buttonchoose();
extern void MIO_mouse_hide();
extern void MIO_mouse_show();
extern void MIO_mouse_setposition();

extern void MIO_net_registeropen();
extern void MIO_net_registerclose();
extern void MIO_net_waitforconnect();
extern void MIO_net_getaddr();
extern void MIO_net_getlocaladdr();
extern void MIO_net_getlocalname();
extern void MIO_net_gethostname();
extern void MIO_net_gethostaddr();
extern void MIO_net_available();
extern void MIO_net_connectionstatus();
extern void MIO_net_deletecookie();
extern void MIO_net_getcookie();
extern void MIO_net_getcookies();
extern void MIO_net_getnumcookies();
extern void MIO_net_setcookie();

extern void MIO_obsolete ();
extern void MIO_obsolete_replace ();

extern void MIO_pc_inport ();
extern void MIO_pc_outport ();
extern void MIO_pc_parallelget ();
extern void MIO_pc_parallelput ();

extern void MIO_pic_new();
extern void MIO_pic_blend();
extern void MIO_pic_blur();
extern void MIO_pic_draw();
extern void MIO_pic_drawframes();
extern void MIO_pic_drawspecial();
extern void MIO_pic_free();
extern void MIO_pic_filenew();
extern void MIO_pic_filenewframes();
extern void MIO_pic_filesave();
extern void MIO_pic_frames();
extern void MIO_pic_screenload();
extern void MIO_pic_screensave();
extern void MIO_pic_settransclr();
extern void MIO_pic_getdimensions();
extern void MIO_pic_flip();
extern void MIO_pic_mirror();
extern void MIO_pic_rotate();
extern void MIO_pic_scale();


extern void MIO_rgb_getcolour();
extern void MIO_rgb_setcolour();
extern void MIO_rgb_addcolour();
extern void MIO_rgb_maxcolour();

extern void MIO_music_soundoff();
extern void MIO_music_soundon();
extern void MIO_music_play();
extern void MIO_music_playdone();
extern void MIO_music_settempo();
extern void MIO_music_playfile();
extern void MIO_music_playfiledone();
extern void MIO_music_playfilestop();
extern void MIO_music_preload();

extern void MIO_sdlgraph_newwin();
extern void MIO_sdlgraph_closewin();
extern void MIO_sdlgraph_cls();
extern void MIO_sdlgraph_update();
extern void MIO_sdlgraph_setlight();

extern void MIO_sdlgraph_pushmatrix();
extern void MIO_sdlgraph_popmatrix();
extern void MIO_sdlgraph_clearmatrix();
extern void MIO_sdlgraph_translate();
extern void MIO_sdlgraph_rotate();
extern void MIO_sdlgraph_scale();

extern void MIO_sdlgraph_drawline();
extern void MIO_sdlgraph_drawfilltriangle();
extern void MIO_sdlgraph_drawtriangle();

extern void MIO_sys_exec();
extern void MIO_sys_exit();
extern void MIO_sys_getcomputername();
extern void MIO_sys_getenv();
extern void MIO_sys_getpid();
extern void MIO_sys_getusername();

extern void MIO_text_cls();
extern void MIO_text_colour();
extern void MIO_text_colourback();
extern void MIO_text_locate();
extern void MIO_text_locatexy();
extern void MIO_text_maxcol();
extern void MIO_text_maxrow();
extern void MIO_text_whatcol();
extern void MIO_text_whatrow();
extern void MIO_text_whatcolour();
extern void MIO_text_whatcolourback();
extern void MIO_text_whatchar();

extern void MIO_time_date();
extern void MIO_time_datesec();
extern void MIO_time_delay();
extern void MIO_time_elapsed();
extern void MIO_time_elapsedcpu();
extern void MIO_time_partssec();
extern void MIO_time_sec();
extern void MIO_time_secdate();
extern void MIO_time_secparts();
extern void MIO_time_secstr();

extern void MIO_view_clipset();
extern void MIO_view_clipadd();
extern void MIO_view_clipaddoval();
extern void MIO_view_clipoff();
extern void MIO_view_set();
extern void MIO_view_maxx();
extern void MIO_view_maxy();
extern void MIO_view_whatdotcolour();
extern void MIO_view_scroll();
extern void MIO_view_settransclr();
extern void MIO_view_update ();
extern void MIO_view_updatearea ();

extern void MIO_win_registeropen();
extern void MIO_win_registerclose();
extern void MIO_win_select();
extern void MIO_win_getselect();
extern void MIO_win_setactive();
extern void MIO_win_getactive();
extern void MIO_win_hide();
extern void MIO_win_show();
extern void MIO_win_isvisible();
extern void MIO_win_set();
extern void MIO_win_setposition();
extern void MIO_win_getposition();
extern void MIO_win_settransclr();
extern void MIO_win_update ();
extern void MIO_win_updatearea ();

extern void MIO_joy_read();
extern void MIO_joy_caps();

extern void MIO_sprite_new();
extern void MIO_sprite_free();
extern void MIO_sprite_show();
extern void MIO_sprite_hide();
extern void MIO_sprite_setframerate();
extern void MIO_sprite_setposition();
extern void MIO_sprite_setheight();
extern void MIO_sprite_changepic();
extern void MIO_sprite_animate();

extern void MIO_time_secstr();

extern void MIO_view_scroll();

extern void MT_InterpretIfc();

extern void Star_finalize();
extern void Star_getToken();
extern void Star_initialize();

/*
** Entries must be sorted in increasing alphabetic order on "procName"!
*/

struct LookupEntry externLookupTable[] = {

    { "concur_simutime",	Language_Execute_System_Simutime },

    { "config_display",		MIO_config_display },
    { "config_lang",		MIO_config_lang },
    { "config_machine",		MIO_config_machine },

    { "crypto_des3decrypt",	MIO_crypto_des3decrypt },
    { "crypto_des3encrypt",	MIO_crypto_des3encrypt },
    { "crypto_des3set2key",	MIO_crypto_des3set2key },
    { "crypto_des3set3key",	MIO_crypto_des3set3key },
    { "crypto_desdecrypt",	MIO_crypto_desdecrypt },
    { "crypto_desencrypt",	MIO_crypto_desencrypt },
    { "crypto_dessetkey",	MIO_crypto_dessetkey },

    { "dir_change",		MIO_dir_change },
    { "dir_close",		MIO_dir_close },
    { "dir_create",		MIO_dir_create },
    { "dir_current",		MIO_dir_current },
    { "dir_delete",		MIO_dir_delete },
    { "dir_exists",		MIO_dir_exists },
    { "dir_get",  		MIO_dir_get },
    { "dir_getlong",		MIO_dir_getlong },
    { "dir_open",		MIO_dir_open },

    { "dlg_alert",		MIO_dlg_alert },

    { "draw_arc",		MIO_draw_arc },
    { "draw_box",		MIO_draw_box },
    { "draw_cls",		MIO_draw_cls },
    { "draw_dot",		MIO_draw_dot },
    { "draw_fill",		MIO_draw_fill },
    { "draw_fillarc",		MIO_draw_fillarc },
    { "draw_fillbox",		MIO_draw_fillbox },
    { "draw_fillmapleleaf",	MIO_draw_fillmapleleaf },
    { "draw_filloval",		MIO_draw_filloval },
    { "draw_fillpolygon",	MIO_draw_fillpolygon },
    { "draw_fillstar",		MIO_draw_fillstar },
    { "draw_line",		MIO_draw_line },
    { "draw_line_dashed",	MIO_draw_line_dashed },
    { "draw_line_thick",	MIO_draw_line_thick },
    { "draw_mapleleaf",		MIO_draw_mapleleaf },
    { "draw_oval",		MIO_draw_oval },
    { "draw_polygon",		MIO_draw_polygon },
    { "draw_star",		MIO_draw_star },
    { "draw_text",		MIO_draw_text },

    { "error_abort",		MIO_error_abort },
    { "error_abortmsg",		MIO_error_abortmsg },
    { "error_halt",		MIO_error_halt },
    { "error_last",		MIO_error_last },
    { "error_lastmsg",		MIO_error_lastmsg },
    { "error_laststr",		MIO_error_laststr },
    { "error_msg",		MIO_error_msg },
    { "error_set",		MIO_error_set },
    { "error_setmsg",		MIO_error_setmsg },
    { "error_str",		MIO_error_str },

    { "file_copy",		MIO_file_copy },
    { "file_createpath",	MIO_file_createpath },
    { "file_delete",		MIO_file_delete },
    { "file_diskfree",		MIO_file_diskfree },
    { "file_exists",		MIO_file_exists },
    { "file_fullpath",		MIO_file_fullpath },
    { "file_parent",		MIO_file_parent },
    { "file_rename",		MIO_file_rename },
    { "file_splitpath",		MIO_file_splitpath },
    { "file_status",		MIO_file_status },

    { "font_free",		MIO_font_free },
    { "font_getname",		MIO_font_getname },
    { "font_getsize",		MIO_font_getsize },
    { "font_getstyle",		MIO_font_getstyle },
    { "font_name",		MIO_font_name },
    { "font_new",		MIO_font_new },
    { "font_sizes",		MIO_font_sizes },
    { "font_startname",		MIO_font_startname },
    { "font_startsize",		MIO_font_startsize },
    { "font_width",		MIO_font_width },

    { "gui_alert",		MIO_gui_alert },
    { "gui_choose",		MIO_gui_choose },
/*
    { "gui_alert",		MIO_gui_alert },
    { "gui_fileviewer",		MIO_gui_fileviewer },
    { "gui_input",		MIO_gui_input },
    { "gui_opendlg",		MIO_gui_opendlg },
    { "gui_request",		MIO_gui_request },
    { "gui_savedlg",		MIO_gui_savedlg },
*/

	{ "hashmap_free", MIO_hashmap_free },
	{ "hashmap_get", MIO_hashmap_get },
	{ "hashmap_new", MIO_hashmap_new },
	{ "hashmap_put", MIO_hashmap_put },
	{ "hashmap_remove", MIO_hashmap_remove },

    { "input_getch",		Language_Execute_Graphics_Getch },
    { "input_hasch",		Language_Execute_Graphics_Hasch },
    { "input_keydown",		MIO_input_keydown },

    { "interpret",		MT_InterpretIfc },

    { "joy_caps",		MIO_joy_caps },
    { "joy_read",		MIO_joy_read },

    { "lexer_begin",		MIO_lexer_begin },
    { "lexer_end",		MIO_lexer_end },
    { "lexer_scan",		MIO_lexer_scan },

    { "limits_getexp",		Limits_Getexp },
    { "limits_rreb",		Limits_Rreb },
    { "limits_setexp",		Limits_Setexp },

    { "math_arctan",		Math_Arctan },
    { "math_arctand",		Math_Arctand },
    { "math_cos",		Math_Cos },
    { "math_cosd",		Math_Cosd },
    { "math_exp",		Math_Exp },
    { "math_ln",		Math_Ln },
    { "math_sign",		Math_Sign },
    { "math_sin",		Math_Sin },
    { "math_sind",		Math_Sind },
    { "math_sqrt",		Math_Sqrt },

    { "mouse_buttonchoose",	MIO_mouse_buttonchoose },
    { "mouse_buttonmoved",	MIO_mouse_buttonmoved },
    { "mouse_buttonwait",	MIO_mouse_buttonwait },
    { "mouse_hide",		MIO_mouse_hide },
    { "mouse_setposition",	MIO_mouse_setposition },
    { "mouse_show",		MIO_mouse_show },
    { "mouse_where",		MIO_mouse_where },

    { "music_play",		MIO_music_play },
    { "music_playdone",		MIO_music_playdone },
    { "music_playfile",		MIO_music_playfile },
    { "music_playfiledone",	MIO_music_playfiledone },
    { "music_playfilestop",	MIO_music_playfilestop },
    { "music_preload",		MIO_music_preload },
    { "music_settempo",		MIO_music_settempo },
    { "music_sound",		MIO_music_soundon },
    { "music_soundoff",		MIO_music_soundoff },

    { "net_avail",		MIO_net_available },
    { "net_connectionstatus",	MIO_net_connectionstatus },
    { "net_deletecookie",	MIO_net_deletecookie },
    { "net_getaddr",		MIO_net_getaddr },
    { "net_getcookie",		MIO_net_getcookie },
    { "net_getcookies",		MIO_net_getcookies },
    { "net_gethostaddr",	MIO_net_gethostaddr },
    { "net_gethostname",	MIO_net_gethostname },
    { "net_getlocaladdr",	MIO_net_getlocaladdr },
    { "net_getlocalname",	MIO_net_getlocalname },
    { "net_getnumcookies",	MIO_net_getnumcookies },
    { "net_registerclose",	MIO_net_registerclose },
    { "net_registeropen",	MIO_net_registeropen },
    { "net_setcookie",		MIO_net_setcookie },
    { "net_waitforconnect",	MIO_net_waitforconnect },

    { "obsolete", 		MIO_obsolete },
    { "obsolete_replace",	MIO_obsolete_replace },

    { "old_drawpic",		Language_Execute_Graphics_Drawpic },
    { "old_setactive",		Language_Execute_System_Setactive },
    { "old_sizepic",		Language_Execute_Graphics_Sizepic },
    { "old_takepic",		Language_Execute_Graphics_Takepic },

    { "pc_inport",		MIO_pc_inport },
    { "pc_outport",		MIO_pc_outport },
    { "pc_parallelget",		MIO_pc_parallelget },
    { "pc_parallelput",		MIO_pc_parallelput },
    
    { "pic_blend",		MIO_pic_blend },
    { "pic_blur",		MIO_pic_blur },
    { "pic_draw",		MIO_pic_draw },
    { "pic_drawframes",		MIO_pic_drawframes },
    { "pic_drawspecial",	MIO_pic_drawspecial },
    { "pic_filenew",		MIO_pic_filenew },
    { "pic_filenewframes",	MIO_pic_filenewframes },
    { "pic_filesave",		MIO_pic_filesave },
    { "pic_flip",		MIO_pic_flip },
    { "pic_frames",		MIO_pic_frames },
    { "pic_free",		MIO_pic_free },
    { "pic_getdimensions",	MIO_pic_getdimensions },
    { "pic_mirror",		MIO_pic_mirror },
    { "pic_new",		MIO_pic_new },
    { "pic_rotate",		MIO_pic_rotate },
    { "pic_scale",		MIO_pic_scale },
    { "pic_settransclr",	MIO_pic_settransclr },

    { "rand_int", 		Math_Randint },
    { "rand_next", 		Math_Randnext },
    { "rand_randomize", 	Math_Randomize },
    { "rand_real", 		Math_Rand },
    { "rand_seed", 		Math_Randseed },

    { "rgb_addcolour",		MIO_rgb_addcolour },
    { "rgb_getcolour",		MIO_rgb_getcolour },
    { "rgb_maxcolour",		MIO_rgb_maxcolour },
    { "rgb_setcolour",		MIO_rgb_setcolour },

	{ "sdlgraph_clearmatrix", MIO_sdlgraph_clearmatrix },
	{ "sdlgraph_closewin",	MIO_sdlgraph_closewin },
	{ "sdlgraph_cls",	MIO_sdlgraph_cls },
	{ "sdlgraph_drawfilltriangle",	MIO_sdlgraph_drawfilltriangle },
	{ "sdlgraph_drawline",	MIO_sdlgraph_drawline },
	{ "sdlgraph_drawtriangle",	MIO_sdlgraph_drawtriangle },
	{ "sdlgraph_newwin",	MIO_sdlgraph_newwin },
	{ "sdlgraph_popmatrix", MIO_sdlgraph_popmatrix },
	{ "sdlgraph_pushmatrix", MIO_sdlgraph_pushmatrix },
	{ "sdlgraph_rotate", MIO_sdlgraph_rotate },
	{ "sdlgraph_scale", MIO_sdlgraph_scale },
	{ "sdlgraph_setlight", MIO_sdlgraph_setlight },
	{ "sdlgraph_translate", MIO_sdlgraph_translate },
	{ "sdlgraph_update",	MIO_sdlgraph_update },

    { "sprite_animate",		MIO_sprite_animate },
    { "sprite_changepic",	MIO_sprite_changepic },
    { "sprite_free",		MIO_sprite_free },
    { "sprite_hide",		MIO_sprite_hide },
    { "sprite_new",		MIO_sprite_new },
    { "sprite_setframerate",	MIO_sprite_setframerate },
    { "sprite_setheight",	MIO_sprite_setheight },
    { "sprite_setposition",	MIO_sprite_setposition },
    { "sprite_show",		MIO_sprite_show },

    { "stream_flushall",	Language_Execute_System_Flushstreams },

    { "string_chararraycopy",   String_CharArrayCopy },
    { "string_index",		String_Index },
    { "string_length",		String_Length },
    { "string_readlinefromchararray", String_ReadLineFromCharArray },
    { "string_repeat",		String_Repeat },
    { "string_writelinetochararray", String_WriteLineToCharArray },
    { "string_writetochararray", String_WriteToCharArray },

    { "sys_exec",		MIO_sys_exec },
    { "sys_exit",		MIO_sys_exit },
    { "sys_fetcharg",		Language_Execute_System_Fetcharg },
    { "sys_getcomputername",	MIO_sys_getcomputername },
    { "sys_getenv",		MIO_sys_getenv },
    { "sys_getpid",		MIO_sys_getpid },
    { "sys_getusername",	MIO_sys_getusername },
    { "sys_nargs",		Language_Execute_System_Nargs },

    { "text_cls",		MIO_text_cls },
    { "text_colour",		MIO_text_colour },
    { "text_colourback",	MIO_text_colourback },
    { "text_locate",		MIO_text_locate },
    { "text_locatexy",		MIO_text_locatexy },
    { "text_maxcol",		MIO_text_maxcol },
    { "text_maxrow",		MIO_text_maxrow },
    { "text_whatchar",		MIO_text_whatchar },
    { "text_whatcol",		MIO_text_whatcol },
    { "text_whatcolour",	MIO_text_whatcolour },
    { "text_whatcolourback",	MIO_text_whatcolourback },
    { "text_whatrow",		MIO_text_whatrow },

    { "time_date",		MIO_time_date },
    { "time_datesec",		MIO_time_datesec },
    { "time_delay",		MIO_time_delay },
    { "time_elapsed",		MIO_time_elapsed },
    { "time_elapsedcpu",	MIO_time_elapsedcpu },
    { "time_partssec",		MIO_time_partssec },
    { "time_sec",		MIO_time_sec },
    { "time_secdate",		MIO_time_secdate },
    { "time_secparts",		MIO_time_secparts },
    { "time_secstr",		MIO_time_secstr },

    { "typeconv_ceil",		Math_Ceil },
    { "typeconv_erealstr",	String_Erealstr },
    { "typeconv_floor",		Math_Floor },
    { "typeconv_frealstr",	String_Frealstr },
    { "typeconv_intreal",	Math_Intreal },
    { "typeconv_natreal",	Math_Natreal },
    { "typeconv_realstr",	String_Realstr },
    { "typeconv_round",		Math_Round },
    { "typeconv_strreal",	String_Strreal },

    { "view_clipadd",		MIO_view_clipadd },
    { "view_clipaddoval",	MIO_view_clipaddoval },
    { "view_clipoff",		MIO_view_clipoff },
    { "view_clipset",		MIO_view_clipset },
    { "view_maxx",		MIO_view_maxx },
    { "view_maxy",		MIO_view_maxy },
    { "view_scroll",		MIO_view_scroll },
    { "view_set",		MIO_view_set },
    { "view_settransclr",	MIO_view_settransclr },
    { "view_update",		MIO_view_update },
    { "view_updatearea",	MIO_view_updatearea },
    { "view_whatdotcolour",	MIO_view_whatdotcolour },

    { "win_getactive",		MIO_win_getactive },
    { "win_getposition",	MIO_win_getposition },
    { "win_getselect",		MIO_win_getselect },
    { "win_hide",		MIO_win_hide },
    { "win_isvisible",		MIO_win_isvisible },
    { "win_registerclose",	MIO_win_registerclose },
    { "win_registeropen",	MIO_win_registeropen },
    { "win_select",		MIO_win_select },
    { "win_set",		MIO_win_set },
    { "win_setactive",		MIO_win_setactive },
    { "win_setposition",	MIO_win_setposition },
    { "win_settransclr",	MIO_win_settransclr },
    { "win_show",		MIO_win_show },
    { "win_update",		MIO_win_update },
    { "win_updatearea",		MIO_win_updatearea },

    { "z_finalize",		Star_finalize },
    { "z_getToken",		Star_getToken },
    { "z_initialize",		Star_initialize },

};

long externLookupCount =
	sizeof(externLookupTable) / sizeof(struct LookupEntry);

