#include <pebble.h>
#include <string.h>
#include <stdlib.h>
#define STR_LEN 6

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_hexcolor_layer;

static BitmapLayer *lightest_layer;
static BitmapLayer *lighter_layer;
static BitmapLayer *color_layer;
static BitmapLayer *darker_layer;
static BitmapLayer *darkest_layer;


unsigned long hex2int(char *a, unsigned int len)
{
    unsigned int i;
    unsigned long val = 0;

    for(i=0;i<len;i++)
       if(a[i] <= 57)
        val += (a[i]-48)*(1<<(4*(len-1-i)));
       else
        val += (a[i]-55)*(1<<(4*(len-1-i)));

    return val;
}

char* concat(char *s1, char *s2, char *s3) //ALLOWS CONCATONATION
{
    char *result = malloc(strlen(s1)+strlen(s2)+strlen(s3)+1);
    strcpy(result, s1);
    strcat(result, s2);
    strcat(result, s3);
    return result;
}

static void update_background(){
  char str[STR_LEN + 1] = {0};
  const char *hex_digits = "0123456789ABCDEF";
  int i;

  for( i = 0 ; i < STR_LEN; i++ ) {
     str[i] = hex_digits[ ( rand() % 16 ) ];
  }
  
  char* s_hexcolor = "#000000";
  s_hexcolor = concat("#", str, "");
  APP_LOG(APP_LOG_LEVEL_DEBUG, str);
  APP_LOG(APP_LOG_LEVEL_DEBUG, s_hexcolor);
  text_layer_set_text(s_hexcolor_layer, s_hexcolor);
  

  //text_layer_set_text(s_hexcolor_layer, str);
  
  int bg_color = hex2int(str, 6);
  
//   int bg_color1 = (((bg_color & 0xfefefe) >> 1));
//   int bg_color2 = (((bg_color & 0xfefefe) << 1));
  
//   GColor gColorLightest = GColorFromHEX(bg_color1);
//   GColor gColorLighter = GColorFromHEX(bg_color1);
//   GColor gColorNormal = GColorFromHEX(bg_color);
//   GColor gColorDarker = GColorFromHEX(bg_color2);
//   GColor gColorDarkest = GColorFromHEX(bg_color2);
  
//   bitmap_layer_set_background_color(lightest_layer, (gColorLightest));
//   bitmap_layer_set_background_color(lighter_layer, (gColorLighter));
//     bitmap_layer_set_background_color(darker_layer, (gColorDarker));
//   bitmap_layer_set_background_color(darkest_layer, (gColorDarkest));



    
  window_set_background_color(s_main_window, GColorFromHEX(bg_color));
  text_layer_set_text_color(s_time_layer, gcolor_legible_over(GColorFromHEX(bg_color)));
  text_layer_set_text_color(s_hexcolor_layer, gcolor_legible_over(GColorFromHEX(bg_color)));


}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%l:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_background();
}

static void main_window_load(Window *window) {
    // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      //starting x, starting y, width x, width y
      GRect(0, PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 50));

  //Create the TextLayer for Text
  s_hexcolor_layer = text_layer_create(
      //starting x, starting y, width x, width y
      GRect(0, PBL_IF_ROUND_ELSE(88, 80), bounds.size.w, 50));
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "0:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_hexcolor_layer, GColorClear);
  text_layer_set_text_color(s_hexcolor_layer, GColorBlack);
  text_layer_set_text(s_hexcolor_layer, "#000000");
  text_layer_set_font(s_hexcolor_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28) );
  text_layer_set_text_alignment(s_hexcolor_layer, GTextAlignmentCenter);

//   double x = 0;
//   lightest_layer = bitmap_layer_create(
//       //starting x, starting y, width x, width y
//       GRect(x, 108, (bounds.size.w/4), 60));
  
//   x+= bounds.size.w/4;
//   lighter_layer = bitmap_layer_create(
//       //starting x, starting y, width x, width y
//       GRect(x, 108, (bounds.size.w/4), 60));
  
//   x+= bounds.size.w/4;
//   darker_layer = bitmap_layer_create(
//       //starting x, starting y, width x, width y
//       GRect(x, 108, (bounds.size.w/4), 60));
  
//   x+= bounds.size.w/4;
//   darkest_layer = bitmap_layer_create(
//       //starting x, starting y, width x, width y
//       GRect(x, 108, (bounds.size.w/4), 60));
  
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_hexcolor_layer));

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
//   layer_add_child(window_layer, bitmap_layer_get_layer(lightest_layer));
//   layer_add_child(window_layer, bitmap_layer_get_layer(lighter_layer));
//   layer_add_child(window_layer, bitmap_layer_get_layer(darker_layer));
//   layer_add_child(window_layer, bitmap_layer_get_layer(darkest_layer));
  
//   bitmap_layer_set_background_color(lightest_layer, GColorBlack);
//   bitmap_layer_set_background_color(lighter_layer, GColorBlack);
//   bitmap_layer_set_background_color(darker_layer, GColorBlack);
//   bitmap_layer_set_background_color(darkest_layer, GColorBlack);
  
 

}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_hexcolor_layer);
  bitmap_layer_destroy(lightest_layer);
  bitmap_layer_destroy(lighter_layer);
  bitmap_layer_destroy(color_layer);
  bitmap_layer_destroy(darker_layer);
  bitmap_layer_destroy(darkest_layer);

}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}