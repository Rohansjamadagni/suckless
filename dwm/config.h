/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 2;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;           /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 0;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
static const int topbar             = 1;     /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]	    = "#282828";
static const char col_gray2[]       = "#282828";
static const char col_gray3[] 	    = "#ebdbb2";
static const char col_gray4[] 	    = "#282828";
static const char col_cyan[] 	    = "#ebdbb2";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
//static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };
//static const char *tags[] = { "α", "β", "ɣ", "δ", "ε", "ζ", "η", "θ", "ι" };
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 *	WM_WINDOW_ROLE(STRING) = role
	 *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
	 */
	RULE(.class = "Gimp", .tags = 1 << 4)
	RULE(.class = "Ferdi", .tags = 1 << 8, .monitor = 1)
	RULE(.class = "Ferdi", .tags = 1 << 8, .monitor = 1)
	RULE(.class = "Signal", .tags = 1 << 8, .monitor = 1)
	RULE(.class = "Spotify", .tags = 1 << 7, .monitor = 1)
	RULE(.class = "discord", .tags = 1 << 7, .monitor = 1)
	RULE(.class = "Galculator", .isfloating = 1)
	// RULE(.class = "firefox", .monitor = 1)
	RULE(.class = "Chromium", .tags = 1<< 6, .monitor = 1)

};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define OTHERMODKEY Mod1Mask
#define TAGKEYS(KEY,TAG ) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, \

#define TAGKEYSOTHER(KEY,TAG ) \
	{ MODKEY,                       KEY,      viewother,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,          {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tagnextmon,          {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
  "/home/rohanj/.config/rofi/bin/launcher_colorful"
};
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "kitty", "-t", scratchpadname, "-g", "120x34", NULL };

static const char *termcmd[]  = { "kitty", NULL };
static const char *brwsrcmd1[]  = { "firefox", NULL };
static const char *filemgr[]  = { "thunar", NULL };

// static const char *upvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "+5%", NULL};//  , "kill", "-44", "$(", "pidof", "dwmblocks", ")", NULL};
// static const char *downvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "-5%", NULL};//, "kill", "-44", "$(", "pidof", "dwmblocks", ")", NULL};
// static const char *mutevol[] = {"/usr/bin/pactl", "set-sink-mute", "0", "toggle", NULL};//, "kill", "-44", "$(", "pidof", "dwmblocks", ")", NULL};

static const char *upvol[]   = { "/home/rohanj/.config/scripts/dwm/volup.sh", NULL };
static const char *downvol[] = { "/home/rohanj/.config/scripts/dwm/voldown.sh", NULL };
static const char *mutevol[] = { "/home/rohanj/.config/scripts/dwm/volmut.sh", NULL };

static const char *screenoff[] = {"/home/rohanj/.config/scripts/screen_off.sh", NULL};
static const char *audio_change[] = {"/home/rohanj/.config/scripts/audio_change.sh", NULL};
static const char *isildur[] = {"/home/rohanj/.config/scripts/isildur", NULL};
static const char *mac[] = {"/home/rohanj/.config/scripts/mac", NULL};
static const char *pwrmenu[] = {"/home/rohanj/.config/rofi/bin/menu_powermenu", NULL};
static const char *ss[] = {"/usr/bin/flameshot", "gui", NULL};
static const char *emoji[] = {"/home/rohanj/.config/rofi/bin/launcher_colorful_emoji", NULL};
static const char *win[] = {"/home/rohanj/.config/rofi/bin/launcher_colorful_win", NULL};
static const char *calc[] = {"galculator", NULL};

static const Key keys[] = {
	/* modifier                     key            function                argument */
	{ MODKEY,                       XK_p,          spawn,                  {.v = dmenucmd } },
	{ MODKEY,                       XK_w,          spawn,                  {.v = brwsrcmd1 } },
	{ MODKEY,                       XK_e,          spawn,                  {.v = filemgr } },
	{ MODKEY,                       XK_Return,     spawn,                  {.v = termcmd } },
	{ MODKEY,                       XK_F2,         spawn,                  {.v = audio_change } },
	{ MODKEY,                       XK_F3,         spawn,                  {.v = screenoff } },
	{ MODKEY,                       XK_x,          spawn,                  {.v = pwrmenu } },
	{ MODKEY,                       XK_equal,      spawn,                  {.v = calc } },
	{ MODKEY|ControlMask,           XK_m,          spawn,                  {.v = mac } },
	{ MODKEY|ControlMask,           XK_i,          spawn,                  {.v = isildur } },
 	{ MODKEY|ShiftMask,             XK_s,          spawn,                  {.v = ss } },
 	{ MODKEY|ShiftMask,             XK_e,          spawn,                  {.v = emoji } },
	{ OTHERMODKEY,                  XK_Tab,        spawn,                  {.v = win } },
	{ MODKEY,                       XK_b,          togglebar,              {0} },
	{ MODKEY,                       XK_j,          focusstack,             {.i = +1 } },
	{ MODKEY,                       XK_k,          focusstack,             {.i = -1 } },
	{ MODKEY,                       XK_Tab,          focusstack,             {.i = +1 } },
  { MODKEY|ShiftMask,             XK_j,          rotatestack,            {.i = +1 } },
 	{ MODKEY|ShiftMask,             XK_k,          rotatestack,            {.i = -1 } },
	{ MODKEY,                       XK_Up,         incnmaster,             {.i = +1 } },
	{ MODKEY,                       XK_Down,       incnmaster,             {.i = -1 } },
	{ MODKEY,                       XK_h,          setmfact,               {.f = -0.05} },
	{ MODKEY,                       XK_l,          setmfact,               {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return,     zoom,                   {0} },
	{ MODKEY,                       XK_q,          killclient,             {0} },
	{ MODKEY|ShiftMask,             XK_q,          quit,                   {0} },
  { MODKEY|ShiftMask,             XK_r,          quit,                   {1} },
	{ MODKEY,                       XK_t,          setlayout,              {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,          setlayout,              {.v = &layouts[5]} },
	{ MODKEY,                       XK_m,          setlayout,              {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,      setlayout,              {0} },
	{ MODKEY|ShiftMask,             XK_t,          togglefloating,         {0} },
	{ MODKEY,                       XK_0,          view,                   {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,          tag,                    {.ui = ~0 } },
	{ MODKEY,                       XK_a,          focusmon,               {.i = -1 } },
	{ MODKEY,                       XK_d,          focusmon,               {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_a,          tagmon,                 {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_d,          tagmon,                 {.i = +1 } },
  { 0,                            XF86XK_AudioLowerVolume, spawn,        {.v = downvol } },
	{ 0,                            XF86XK_AudioMute, spawn,               {.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn,        {.v = upvol   } },
	TAGKEYS(                        XK_1,                                  0)
	TAGKEYS(                        XK_2,                                  1)
	TAGKEYS(                        XK_3,                                  2)
	TAGKEYS(                        XK_4,                                  3)
	TAGKEYS(                        XK_5,                                  4)
	TAGKEYS(                        XK_6,                                  5)
	TAGKEYS(                        XK_7,                                  6)
	TAGKEYS(                        XK_8,                                  7)
	TAGKEYS(                        XK_9,                                  8)
	TAGKEYSOTHER(                   XK_KP_End,                             0)
	TAGKEYSOTHER(                   XK_KP_Down,                            1)
	TAGKEYSOTHER(                   XK_KP_Page_Down,                       2)
	TAGKEYSOTHER(                   XK_KP_Left,                            3)
	TAGKEYSOTHER(                   XK_KP_Begin,                           4)
	TAGKEYSOTHER(                   XK_KP_Right,                           5)
	TAGKEYSOTHER(                   XK_KP_Home,                            6)
	TAGKEYSOTHER(                   XK_KP_Up,                              7)
	TAGKEYSOTHER(                   XK_KP_Page_Up,                         8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

