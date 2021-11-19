/*********************
 *      INCLUDES
 *********************/
#include "lv_theme.h"
#include "theme.h"

#if LV_USE_THEME_TB

#include "../lvgui/scale.h"
#include "../lvgui/hal.h"
#include "../lvgui/lv_lib_freetype/lv_freetype.h"
#include <stdio.h>

#if LV_COLOR_DEPTH == 1
#error THEME_TB needs colours
#endif

/*********************
 *      DEFINES
 *********************/
#define DEF_RADIUS 0

#define COL_PADDING PIXEL_SCALE(32)
#define ROW_PADDING PIXEL_SCALE(32)

#define HOR_GRID PIXEL_SCALE(32)
#define VER_GRID PIXEL_SCALE(41/2) // 20.5

#define DEF_COLOR_TEXT lv_color_hex(0xFFFFFF)
#define DEF_COLOR_TEXT_SECONDARY lv_color_hex(0xDDDDDD)
#define DEF_SHADOW_COLOR   lv_color_hex(0x000000)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static lv_style_t def;

/*Static style definitions*/
static lv_style_t sb;

/*Saved input parameters*/
static uint16_t _hue;
static lv_font_t * _font;
static lv_font_t * _button_font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    static lv_style_t bg, panel, scr;

    lv_style_copy(&def, &lv_style_plain); /*Initialize the default style*/
    def.text.font   = _font;
    def.body.radius = DEF_RADIUS;
    def.text.color  = DEF_COLOR_TEXT;

    lv_style_copy(&bg, &def);
    bg.body.main_color = lv_color_hsv_to_rgb(_hue, 100, 45);
    bg.body.grad_color = bg.body.main_color;
    bg.body.radius     = 0;

    lv_style_copy(&scr, &bg);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    lv_style_copy(&panel, &def);
    panel.body.radius         = DEF_RADIUS;
    panel.body.main_color     = lv_color_hsv_to_rgb(_hue, 100, 45);
    panel.body.grad_color     = lv_color_hsv_to_rgb(_hue, 100, 45);
    panel.body.border.width   = PIXEL_SCALE(0);
    panel.body.border.color   = lv_color_hex3(0x000);
    panel.body.border.opa     = LV_OPA_COVER;
    panel.body.shadow.color   = DEF_SHADOW_COLOR;
    panel.body.shadow.type    = LV_SHADOW_BOTTOM;
    panel.body.shadow.width   = PIXEL_SCALE(0);
    panel.body.padding.left   = COL_PADDING;
    panel.body.padding.right  = COL_PADDING;
    panel.body.padding.top    = VER_GRID;
    panel.body.padding.bottom = VER_GRID;
    panel.body.padding.inner  = VER_GRID;
    panel.text.color          = DEF_COLOR_TEXT;
    panel.image.color         = DEF_COLOR_TEXT;

    lv_style_copy(&sb, &def);
    sb.body.radius         = PIXEL_SCALE(4);
    sb.body.main_color     = lv_color_hsv_to_rgb(_hue, 200, 100);
    sb.body.grad_color     = sb.body.main_color;
    sb.body.opa            = LV_OPA_60;

	// ???
    sb.body.padding.right  = COL_PADDING;
    sb.body.padding.bottom = ROW_PADDING;

    theme.style.bg    = &bg;
    theme.style.scr   = &scr;
    theme.style.panel = &panel;
}

static void cont_init(void)
{
#if LV_USE_CONT != 0
    theme.style.cont = theme.style.panel;
#endif
}

static void btn_init(void)
{
#if LV_USE_BTN != 0
    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&rel, &def);
    rel.text.font           = _button_font;
    rel.body.main_color     = lv_color_hsv_to_rgb(_hue, 100, 69);
    rel.body.grad_color     = rel.body.main_color;
    rel.body.radius         = PIXEL_SCALE(6);
    rel.body.padding.left   = COL_PADDING;
    rel.body.padding.right  = COL_PADDING;
    rel.body.padding.top    = PIXEL_SCALE(23);
    rel.body.padding.bottom = PIXEL_SCALE(23);
    rel.body.padding.inner  = VER_GRID;

    rel.body.border.width   = PIXEL_SCALE(5);
    rel.body.border.color   = lv_color_hsv_to_rgb(_hue, 100, 56);
    rel.body.border.opa     = LV_OPA_COVER;

    rel.body.shadow.color   = DEF_SHADOW_COLOR;
    rel.body.shadow.type    = LV_SHADOW_BOTTOM;
    rel.body.shadow.width   = PIXEL_SCALE(10);

    rel.text.color          = DEF_COLOR_TEXT;
    rel.image.color         = DEF_COLOR_TEXT;

    lv_style_copy(&pr, &rel);
    pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 56);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.shadow.width = PIXEL_SCALE(4);

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 56);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;
    tgl_rel.body.shadow.width = PIXEL_SCALE(4);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 56);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.shadow.width = PIXEL_SCALE(2);

    lv_style_copy(&ina, &rel);
    ina.body.main_color   = lv_color_hex3(0xccc);
    ina.body.grad_color   = ina.body.main_color;
    ina.body.shadow.width = 0;
    ina.text.color        = lv_color_hsv_to_rgb(_hue, 95, 5);
    ina.image.color       = lv_color_hsv_to_rgb(_hue, 95, 5);

    theme.style.btn.rel     = &rel;
    theme.style.btn.pr      = &pr;
    theme.style.btn.tgl_rel = &tgl_rel;
    theme.style.btn.tgl_pr  = &tgl_pr;
    theme.style.btn.ina     = &ina;
#endif
}

static void label_init(void)
{
#if LV_USE_LABEL != 0
    static lv_style_t prim, sec, hint;

    lv_style_copy(&prim, &def);
    prim.text.font  = _font;
    prim.text.color = DEF_COLOR_TEXT;

    lv_style_copy(&sec, &prim);
    sec.text.color = DEF_COLOR_TEXT_SECONDARY;

    lv_style_copy(&hint, &prim);
    hint.text.color = DEF_COLOR_TEXT_SECONDARY;

    theme.style.label.prim = &prim;
    theme.style.label.sec  = &sec;
    theme.style.label.hint = &hint;
#endif
}

static void img_init(void)
{
#if LV_USE_IMG != 0
    static lv_style_t img_light, img_dark;
    lv_style_copy(&img_light, &def);
    img_light.image.color   = lv_color_hsv_to_rgb(_hue, 15, 85);
    img_light.image.intense = LV_OPA_80;

    lv_style_copy(&img_dark, &def);
    img_light.image.color   = lv_color_hsv_to_rgb(_hue, 85, 65);
    img_light.image.intense = LV_OPA_80;

    theme.style.img.light = &def;
    theme.style.img.dark  = &def;
#endif
}

static void line_init(void)
{
#if LV_USE_LINE != 0

    theme.style.line.decor = &def;
#endif
}

static void bar_init(void)
{
#if LV_USE_BAR
    static lv_style_t bar_bg, bar_indic;

    lv_style_copy(&bar_bg, &def);
    bar_bg.body.main_color     = lv_color_hsv_to_rgb(_hue, 100, 45);
    bar_bg.body.grad_color     = bar_bg.body.main_color;
    bar_bg.body.radius         = PIXEL_SCALE(8);
    bar_bg.body.border.width   = PIXEL_SCALE(4);
    bar_bg.body.padding.left   = LV_DPI / 16;
    bar_bg.body.padding.right  = LV_DPI / 16;
    bar_bg.body.padding.top    = LV_DPI / 16;
    bar_bg.body.padding.bottom = LV_DPI / 16;
    bar_bg.body.border.color = lv_color_hsv_to_rgb(_hue, 77, 96);
    bar_bg.body.border.opa   = LV_OPA_COVER;

    lv_style_copy(&bar_indic, &bar_bg);
    bar_indic.body.main_color     = lv_color_hsv_to_rgb(_hue, 77, 96);
    bar_indic.body.grad_color     = bar_indic.body.main_color;
    bar_indic.body.border.width   = 0;
    bar_indic.body.padding.left   = 0;
    bar_indic.body.padding.right  = 0;
    bar_indic.body.padding.top    = 0;
    bar_indic.body.padding.bottom = 0;

    theme.style.bar.bg    = &bar_bg;
    theme.style.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if LV_USE_SLIDER != 0
    static lv_style_t knob;

    lv_style_copy(&knob, &def);
    knob.body.radius       = LV_RADIUS_CIRCLE;
    knob.body.border.width = 0;
    knob.body.main_color   = theme.style.bar.indic->body.main_color;
    knob.body.grad_color   = knob.body.main_color;

    theme.style.slider.bg    = theme.style.bar.bg;
    theme.style.slider.indic = theme.style.bar.indic;
    theme.style.slider.knob  = &knob;
#endif
}

static void sw_init(void)
{
#if LV_USE_SW != 0
    static lv_style_t sw_bg, sw_indic, sw_knob_off, sw_knob_on;
    lv_style_copy(&sw_bg, theme.style.slider.bg);
    sw_bg.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&sw_indic, theme.style.slider.bg);
    sw_indic.body.radius = LV_RADIUS_CIRCLE;

    lv_style_copy(&sw_knob_on, theme.style.slider.knob);
    sw_knob_on.body.border.width    = PIXEL_SCALE(2);
    sw_knob_on.body.shadow.width = PIXEL_SCALE(3);
    sw_knob_on.body.shadow.type  = LV_SHADOW_BOTTOM;
    sw_knob_on.body.shadow.color = DEF_SHADOW_COLOR;
    sw_knob_on.body.main_color   = lv_color_hsv_to_rgb(_hue, 77, 96);
    sw_knob_on.body.grad_color   = sw_knob_on.body.main_color;
    sw_knob_on.body.border.color = lv_color_hsv_to_rgb(_hue, 100, 56);
    sw_knob_on.body.border.opa   = LV_OPA_COVER;

    lv_style_copy(&sw_knob_off, &sw_knob_on);
    sw_knob_off.body.main_color   = lv_color_hsv_to_rgb(_hue, 100, 56);
    sw_knob_off.body.border.color = lv_color_hsv_to_rgb(_hue + 14, 58, 60);
    sw_knob_off.body.grad_color   = sw_knob_off.body.main_color;

    theme.style.sw.bg       = &sw_bg;
    theme.style.sw.indic    = &sw_indic;
    theme.style.sw.knob_off = &sw_knob_off;
    theme.style.sw.knob_on  = &sw_knob_on;
#endif
}

static void lmeter_init(void)
{
#if LV_USE_LMETER != 0
    static lv_style_t lmeter;
    lv_style_copy(&lmeter, &def);
    lmeter.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 90);
    lmeter.body.grad_color   = lmeter.body.main_color;
    lmeter.body.padding.left = LV_DPI / 10; /*Scale line length*/
    lmeter.line.color        = lv_color_hex3(0x999);
    lmeter.line.width        = 2;

    theme.style.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if LV_USE_GAUGE != 0

    static lv_style_t gauge;
    lv_style_copy(&gauge, &def);
    gauge.body.main_color    = lv_color_hsv_to_rgb(_hue, 10, 60);
    gauge.body.grad_color    = gauge.body.main_color;
    gauge.body.padding.left  = LV_DPI / 16; /*Scale line length*/
    gauge.body.padding.inner = LV_DPI / 8;
    gauge.body.border.color  = lv_color_hex3(0x999);
    gauge.text.color         = lv_color_hex3(0xf00);
    gauge.line.width         = 3;
    gauge.line.color         = lv_color_hsv_to_rgb(_hue, 95, 70);

    theme.style.gauge = &gauge;
#endif
}

static void arc_init(void)
{
#if LV_USE_ARC != 0

    static lv_style_t arc;
    lv_style_copy(&arc, &def);
    arc.line.width = 10;
    arc.line.color = lv_color_hsv_to_rgb(_hue, 90, 90);

    /*For prelaoder*/
    arc.body.border.width   = 10;
    arc.body.border.color   = lv_color_hsv_to_rgb(_hue, 30, 90);
    arc.body.padding.left   = 0;
    arc.body.padding.right  = 0;
    arc.body.padding.top    = 0;
    arc.body.padding.bottom = 0;

    theme.style.arc = &arc;
#endif
}

static void preload_init(void)
{
#if LV_USE_PRELOAD != 0

    theme.style.preload = theme.style.arc;
#endif
}

static void calendar_init(void)
{
#if LV_USE_CALENDAR
    static lv_style_t ina_days;
    lv_style_copy(&ina_days, &def);
    ina_days.text.color = lv_color_hsv_to_rgb(_hue, 0, 70);

    static lv_style_t high_days;
    lv_style_copy(&high_days, &def);
    high_days.text.color = lv_color_hsv_to_rgb(_hue, 80, 90);

    static lv_style_t week_box;
    lv_style_copy(&week_box, &def);
    week_box.body.main_color     = lv_color_hsv_to_rgb(_hue, 40, 100);
    week_box.body.grad_color     = lv_color_hsv_to_rgb(_hue, 40, 100);
    week_box.body.padding.top    = LV_DPI / 20;
    week_box.body.padding.bottom = LV_DPI / 20;
    week_box.body.padding.left   = theme.style.panel->body.padding.left;
    week_box.body.padding.right  = theme.style.panel->body.padding.right;
    week_box.body.border.color   = theme.style.panel->body.border.color;
    week_box.body.border.width   = theme.style.panel->body.border.width;
    week_box.body.border.part    = LV_BORDER_LEFT | LV_BORDER_RIGHT;
    week_box.body.radius         = 0;

    static lv_style_t today_box;
    lv_style_copy(&today_box, &def);
    today_box.body.main_color     = LV_COLOR_WHITE;
    today_box.body.grad_color     = LV_COLOR_WHITE;
    today_box.body.padding.top    = LV_DPI / 20;
    today_box.body.padding.bottom = LV_DPI / 20;
    today_box.body.radius         = 0;

    theme.style.calendar.bg               = theme.style.panel;
    theme.style.calendar.header           = &lv_style_transp;
    theme.style.calendar.inactive_days    = &ina_days;
    theme.style.calendar.highlighted_days = &high_days;
    theme.style.calendar.week_box         = &week_box;
    theme.style.calendar.today_box        = &today_box;
#endif
}

static void cb_init(void)
{
#if LV_USE_CB != 0
    static lv_style_t rel, pr, tgl_rel, tgl_pr, ina;
    lv_style_copy(&rel, theme.style.panel);
    rel.body.shadow.type  = LV_SHADOW_BOTTOM;
    rel.body.shadow.width = 3;

    lv_style_copy(&pr, &rel);
    pr.body.main_color   = lv_color_hex3(0xccc);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.shadow.width = 0;

    lv_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 85);
    tgl_rel.body.grad_color   = tgl_rel.body.main_color;
    tgl_rel.body.shadow.type  = LV_SHADOW_FULL;
    tgl_rel.body.shadow.width = 0;

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 75, 65);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.shadow.width = 0;

    lv_style_copy(&ina, theme.style.btn.ina);

    theme.style.cb.bg          = &lv_style_transp;
    theme.style.cb.box.rel     = &rel;
    theme.style.cb.box.pr      = &pr;
    theme.style.cb.box.tgl_rel = &tgl_rel;
    theme.style.cb.box.tgl_pr  = &tgl_pr;
    theme.style.cb.box.ina     = &ina;
#endif
}

static void btnm_init(void)
{
#if LV_USE_BTNM
    static lv_style_t bg, rel, pr, tgl_rel, tgl_pr, ina;

    lv_style_copy(&bg, theme.style.panel);
    bg.body.padding.left   = 0;
    bg.body.padding.right  = 0;
    bg.body.padding.top    = 0;
    bg.body.padding.bottom = 0;
    bg.body.padding.inner  = 0;
    bg.text.color          = lv_color_hex3(0x555);

    lv_style_copy(&rel, theme.style.panel);
    rel.body.border.part  = LV_BORDER_FULL | LV_BORDER_INTERNAL;
    rel.body.border.width = 1;
    rel.body.border.color = lv_color_hex3(0xbbb);
    rel.body.opa          = LV_OPA_TRANSP;
    rel.body.shadow.width = 0;

    lv_style_copy(&pr, &rel);
    pr.glass             = 0;
    pr.body.main_color   = lv_color_hex3(0xddd);
    pr.body.grad_color   = pr.body.main_color;
    pr.body.border.width = 0;
    pr.body.opa          = LV_OPA_COVER;

    lv_style_copy(&tgl_rel, &pr);
    tgl_rel.body.main_color = lv_color_hsv_to_rgb(_hue, 90, 70);
    tgl_rel.body.grad_color = tgl_rel.body.main_color;
    tgl_rel.text.color      = lv_color_hsv_to_rgb(_hue, 5, 95);

    lv_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = lv_color_hsv_to_rgb(_hue, 95, 65);
    tgl_pr.body.grad_color   = tgl_pr.body.main_color;
    tgl_pr.body.border.width = 0;

    lv_style_copy(&ina, &pr);
    ina.body.main_color = lv_color_hex3(0xccc);
    ina.body.grad_color = ina.body.main_color;

    theme.style.btnm.bg          = &bg;
    theme.style.btnm.btn.rel     = &rel;
    theme.style.btnm.btn.pr      = &pr;
    theme.style.btnm.btn.tgl_rel = &tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &tgl_pr;
    theme.style.btnm.btn.ina     = &def;
#endif
}

static void kb_init(void)
{
#if LV_USE_KB

    static lv_style_t rel;
    lv_style_copy(&rel, &lv_style_transp);
    rel.text.font = _font;

    theme.style.kb.bg          = theme.style.btnm.bg;
    theme.style.kb.btn.rel     = &rel;
    theme.style.kb.btn.pr      = theme.style.btnm.btn.pr;
    theme.style.kb.btn.tgl_rel = theme.style.btnm.btn.tgl_rel;
    theme.style.kb.btn.tgl_pr  = theme.style.btnm.btn.tgl_pr;
    theme.style.kb.btn.ina     = theme.style.btnm.btn.ina;
#endif
}

static void page_init(void)
{
#if LV_USE_PAGE

    theme.style.page.bg   = theme.style.panel;
    theme.style.page.scrl = &lv_style_transp;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if LV_USE_TA
    static lv_style_t oneline;

    lv_style_copy(&oneline, &def);
    oneline.body.opa          = LV_OPA_TRANSP;
    oneline.body.radius       = 0;
    oneline.body.border.part  = LV_BORDER_BOTTOM;
    oneline.body.border.width = 3;
    oneline.body.border.color = lv_color_hex3(0x333);
    oneline.body.border.opa   = LV_OPA_COVER;
    oneline.text.color        = DEF_COLOR_TEXT;

    theme.style.ta.area    = theme.style.panel;
    theme.style.ta.oneline = &oneline;
    theme.style.ta.cursor  = NULL; /*Let library to calculate the cursor's style*/
    theme.style.ta.sb      = &sb;
#endif
}

static void spinbox_init(void)
{
#if LV_USE_SPINBOX
    theme.style.spinbox.bg     = theme.style.panel;
    theme.style.spinbox.cursor = theme.style.ta.cursor;
    theme.style.spinbox.sb     = theme.style.ta.sb;
#endif
}

static void ddlist_init(void)
{
#if LV_USE_DDLIST != 0
    static lv_style_t bg, sel;
    lv_style_copy(&bg, theme.style.panel);
    bg.body.padding.left   = LV_DPI / 6;
    bg.body.padding.right  = LV_DPI / 6;
    bg.body.padding.top    = LV_DPI / 6;
    bg.body.padding.bottom = LV_DPI / 6;
    bg.text.line_space     = LV_DPI / 8;

    lv_style_copy(&sel, &bg);
    sel.body.main_color   = lv_color_hsv_to_rgb(_hue, 90, 70);
    sel.body.grad_color   = sel.body.main_color;
    sel.body.border.width = 0;
    sel.body.shadow.width = 0;
    sel.text.color        = lv_color_hsv_to_rgb(_hue, 5, 95);

    theme.style.ddlist.bg  = &bg;
    theme.style.ddlist.sel = &sel;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if LV_USE_ROLLER != 0
    static lv_style_t roller_bg, roller_sel;

    lv_style_copy(&roller_bg, &lv_style_transp);
    roller_bg.body.padding.left   = LV_DPI / 6;
    roller_bg.body.padding.right  = LV_DPI / 6;
    roller_bg.body.padding.top    = LV_DPI / 6;
    roller_bg.body.padding.bottom = LV_DPI / 6;
    roller_bg.text.line_space     = LV_DPI / 8;
    roller_bg.text.font           = _font;
    roller_bg.glass               = 0;

    lv_style_copy(&roller_sel, &roller_bg);
    roller_sel.text.color = lv_color_hsv_to_rgb(_hue, 90, 70);

    theme.style.roller.bg  = &roller_bg;
    theme.style.roller.sel = &roller_sel;
#endif
}

static void tileview_init(void)
{
#if LV_USE_TILEVIEW != 0
    theme.style.tileview.bg   = &lv_style_transp_tight;
    theme.style.tileview.scrl = &lv_style_transp_tight;
    theme.style.tileview.sb   = theme.style.page.sb;
#endif
}

static void table_init(void)
{
#if LV_USE_TABLE != 0
    static lv_style_t cell;
    lv_style_copy(&cell, theme.style.panel);
    cell.body.radius         = 0;
    cell.body.border.width   = 1;
    cell.body.padding.left   = LV_DPI / 12;
    cell.body.padding.right  = LV_DPI / 12;
    cell.body.padding.top    = LV_DPI / 12;
    cell.body.padding.bottom = LV_DPI / 12;

    theme.style.table.bg   = &lv_style_transp_tight;
    theme.style.table.cell = &cell;
#endif
}

// Focus
static void style_mod(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
    uint16_t hue2 = (_hue + 60) % 360;

    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = lv_color_hex(0xFFAB0D);

    // If not empty or has border then emphasis the border
    if (style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) {
		style->body.border.width = PIXEL_SCALE(12);
	}
	else {
		// FIXME
		style->body.main_color   = lv_color_mix(style->body.main_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
		style->body.grad_color   = lv_color_mix(style->body.grad_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
		style->body.shadow.color = DEF_SHADOW_COLOR;
	}
}

static void style_mod_edit(lv_group_t * group, lv_style_t * style)
{
    (void)group; /*Unused*/
    uint16_t hue2 = (_hue + 300) % 360;

    /*Make the style to be a little bit orange*/
    style->body.border.opa   = LV_OPA_COVER;
    style->body.border.color = LV_COLOR_GREEN;

    /*If not empty or has border then emphasis the border*/
    if (style->body.opa != LV_OPA_TRANSP || style->body.border.width != 0) {
		style->body.border.width = PIXEL_SCALE(12);
	}
	else {
		// FIXME
		style->body.main_color   = lv_color_mix(style->body.main_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
		style->body.grad_color   = lv_color_mix(style->body.grad_color, lv_color_hsv_to_rgb(hue2, 90, 70), LV_OPA_70);
		style->body.shadow.color = DEF_SHADOW_COLOR;
	}
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the tb theme
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
lv_theme_t * lv_theme_tb_init(int hue, lv_font_t * font, lv_font_t * button_font)
{
    if(font == NULL) font = LV_FONT_DEFAULT;
    if(button_font == NULL) button_font = LV_FONT_DEFAULT;

    _hue  = hue;
    _font = font;
    _button_font = button_font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    lv_style_t ** style_p = (lv_style_t **)&theme.style;
    for(i = 0; i < LV_THEME_STYLE_COUNT; i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
    cont_init();
    btn_init();
    label_init();
    img_init();
    line_init();
    bar_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    arc_init();
    preload_init();
    calendar_init();
    cb_init();
    btnm_init();
    kb_init();
    page_init();
    ta_init();
    spinbox_init();
    ddlist_init();
    roller_init();
    tileview_init();
    table_init();

    theme.group.style_mod_xcb      = style_mod;
    theme.group.style_mod_edit_xcb = style_mod_edit;

    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
lv_theme_t * lv_theme_get_tb(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
