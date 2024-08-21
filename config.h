/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 10;       /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12", "Fira Code Nerd Font Mono:size=15"};
static const char dmenufont[]       = "monospace:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_normal_fg[]   = "#919090";
static const char col_selected_fg[] = "#000000";
static const char col_selected_bg[] = "#faee02";
static const char col_tag_bg[]      = "#000000";
static const char col_occ_fg[]	    = "#ffffff";
static const char col_pinned_fg[]   = "#ffffff";
static const char col_cyan[]        = "#005577";
static const char col_red[]         = "#EE0000";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	/*[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },*/
	[SchemeNorm] = { col_occ_fg, col_tag_bg, col_gray2 },
	/*[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },*/
	/*[SchemeSel]  = { col_gray4, col_cyan,  col_red},*/
	[SchemeSel]  = { col_occ_fg, col_tag_bg,  col_red},
};

static const char *tagsel[][2] = {
   /*   fg            bg    */
  { col_normal_fg,    col_tag_bg}, /* norm */
  { col_selected_fg,  col_selected_bg}, /* sel */
  { col_occ_fg,       col_tag_bg}, /* occ but not sel */
  { col_pinned_fg,    col_tag_bg}, /* has pinned tag */
};
/* tagging */
/*static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };*/
// Icons from: https://www.nerdfonts.com/cheat-sheet
static const char *tags[] = { "", "", "", "", "", "󱀁", "", "", "󰄙" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance      title        tags mask     isfloating   monitor */
	{ "Terminator", NULL,         NULL,         0,           1,           -1 },
	{ "Brave-browser", NULL,         "Bitwarden",         0,           1,           -1 },
	{ "AlacrittyScratchpad", NULL,         NULL,         0,           1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.78; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* helper functions */
static void viewnext(const Arg *arg);
static void viewprev(const Arg *arg);

void
viewnext(const Arg *arg) {
    int seltag = selmon->tagset[selmon->seltags]; // get the currently selected tag
    int i;

    for (i = 0; i < LENGTH(tags); i++) {
        if (seltag & (1 << i)) {
            // If the current tag is the last tag, wrap around to the first tag
            if (i == LENGTH(tags) - 1) {
                view(&((Arg) { .ui = 1 }));
            } else {
                // Select the next tag
                view(&((Arg) { .ui = 1 << (i + 1) }));
            }
            return;
        }
    }
}

void
viewprev(const Arg *arg) {
    int seltag = selmon->tagset[selmon->seltags]; // get the currently selected tag
    int i;

    for (i = 0; i < LENGTH(tags); i++) {
        if (seltag & (1 << i)) {
            // If the current tag is the first tag, wrap around to the last tag
            if (i == 0) {
                view(&((Arg) { .ui = 1 << (LENGTH(tags) - 1) }));
            } else {
                // Select the previous tag
                view(&((Arg) { .ui = 1 << (i - 1) }));
            }
            return;
        }
    }
}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *scratchcmd[]  = { "scratch", NULL };
static const char *wallpaperchangecmd[]  = { "set_random_wallpaper.sh", NULL };
static const char *maintagcmd[]  = { "main_tag.sh", NULL };
static const char *browsercmd[]  = { "browser_start.sh", NULL };
/*static const char *launchercmd[] = { "rofi", "-show", "drun", NULL };*/
static const char *launchercmd[] = { "rofi_launch.sh", NULL };


Autostarttag autostarttaglist[] = {
	{.cmd = browsercmd, .tags = 1 << 1 },
	{.cmd = maintagcmd, .tags = 1 << 0 },
	{.cmd = wallpaperchangecmd, .tags = 1 << 0 },
	{.cmd = NULL, .tags = 0 },
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	/* running scripts and programs */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,	                XK_t,	   spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,	        XK_t,	   spawn,          {.v = scratchcmd} },
	{ MODKEY,                       XK_r,      spawn,          {.v = launchercmd} }, // spawn rofi for launching other programs
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = wallpaperchangecmd} }, // change wallpaper
	{ MODKEY,			XK_b,      spawn,          {.v = browsercmd} }, // spawn browser
	/* navigation and focus */
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,			XK_i,      viewnext,       {0} },
	{ MODKEY,			XK_u,      viewprev,       {0} },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } }, // increase the number of windows in the master area 
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } }, // Decrease the number of windows in the master area 
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	/*{ MODKEY,                       XK_Tab,    view,           {0} },*/
	{ MODKEY,                       XK_Escape,    view,           {0} },
	{ MODKEY,	                XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
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

