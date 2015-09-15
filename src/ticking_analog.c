#include <pebble.h>
#include "ticking_analog.h"
#include "pebble.h"
#include "effect_layer.h"  
#include "qr.h"  

static Window *window;
static Layer *s_simple_bg_layer;
static GPath *s_minute_arrow;
static EffectLayer *effect_layer;
static bool b_qr_displayed = false;

// when watch is shaken or tapped
static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
  
    if (b_qr_displayed) {
      hide_qr();
      b_qr_displayed = false;
    } else {
      show_qr();
      b_qr_displayed = true;
    }
}


static void  bg_update_proc(Layer *layer, GContext *ctx) {
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int half_width = bounds.size.w / 2;
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, half_width-10);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, half_width-20);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, half_width-24);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, half_width-32);

  /* Hour hand */
  int32_t angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  graphics_draw_arc(ctx, center, half_width-9, 11, angle-3000,  angle+3000, GColorWhite);
  /* Hour hand */
  
  /* minute hand */
  angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  graphics_draw_arc(ctx, center, half_width-23, 11, angle-6000,  angle-700, GColorWhite);
  
  gpath_rotate_to(s_minute_arrow, angle + angle_90);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, s_minute_arrow);
  /* minute hand */
    
}



static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_simple_bg_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);
  
  effect_layer = effect_layer_create(GRect(0,12,144,144));
  effect_layer_add_effect(effect_layer, effect_rotate_90_degrees, (void*)false);
  layer_add_child(window_layer, effect_layer_get_layer(effect_layer));;

}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  effect_layer_destroy(effect_layer);
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_background_color(window, GColorWhite);
  window_stack_push(window, true);
  
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  gpath_move_to(s_minute_arrow, center);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
  accel_tap_service_subscribe(&accel_tap_handler);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
  gpath_destroy(s_minute_arrow);
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}