#include <pebble.h>
#include "qr.h"

static Window *qr_window;
static BitmapLayer *qr_layer;
static GBitmap *qr_bitmap;

static void initialise_ui(void) {
  qr_window = window_create();
  qr_bitmap = gbitmap_create_with_resource(RESOURCE_ID_QR);
  qr_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(qr_layer, qr_bitmap);
  layer_add_child(window_get_root_layer(qr_window), bitmap_layer_get_layer(qr_layer));
}

static void destroy_ui(void) {
  gbitmap_destroy(qr_bitmap);
  bitmap_layer_destroy(qr_layer);
  window_destroy(qr_window);
}

void show_qr(void) {
  initialise_ui();
  window_stack_push(qr_window, true);
}

void hide_qr(void) {
  window_stack_remove(qr_window, true);
  destroy_ui();
}