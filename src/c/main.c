#include <pebble.h>
#include <string.h>
#include <stdlib.h>
#define STR_LEN 6

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_hexcolor_layer;


static char s_buffer[8]; //write current hours and minutes into buffer

unsigned long hex2int(char *a, unsigned int len) //converts hexadecimal number to integer
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

char* concat(char *s1, char *s2, char *s3) //function to concatonate strings
{
    char *result = malloc(strlen(s1)+strlen(s2)+strlen(s3)+1);
    strcpy(result, s1);
    strcat(result, s2);
    strcat(result, s3);
    return result;
}

static void update_background(){ //updates the background of the watch and sets the color
  char str[STR_LEN + 1] = {0};
  const char *hex_digits = "0123456789ABCDEF";
  int i;

  for( i = 0 ; i < STR_LEN; i++ ) { 
     str[i] = hex_digits[ ( rand() % 16 ) ];
  }
  
  char* s_hexcolor = "#000000";
  s_hexcolor = concat("#", str, "");

  text_layer_set_text(s_hexcolor_layer, s_hexcolor); //set text to random color
  
  int bg_color = hex2int(str, 6); //get the background color

  window_set_background_color(s_main_window, GColorFromHEX(bg_color)); //set the background color
  text_layer_set_text_color(s_time_layer, gcolor_legible_over(GColorFromHEX(bg_color))); //set the color of text to be legible
  text_layer_set_text_color(s_hexcolor_layer, gcolor_legible_over(GColorFromHEX(bg_color))); //set the color of text to be legible
}

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp); //get a time structure setup

  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%l:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer); //display the time on the textlayer
}

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}

static void move_text_layers_offscreen(TextLayer *layer, int direction){
  GRect bounds = layer_get_frame(text_layer_get_layer(layer));

  GRect start = GRect(0, bounds.origin.y, 144, bounds.size.w); //starting x, starting y, width x, width y
  GRect finish = GRect((direction * 144), bounds.origin.y, 144, bounds.size.w); //starting x, starting y, width x, width y

  animate_layer(text_layer_get_layer(layer), &start, &finish, 300, 500);
}

static void move_text_layers_onscreen(TextLayer *layer, int direction){
  GRect bounds = layer_get_frame(text_layer_get_layer(layer));

  GRect start = GRect((direction * -144), bounds.origin.y, 144, bounds.size.w); //starting x, starting y, width x, width y
  GRect finish = GRect(0, bounds.origin.y, 144, bounds.size.w); //starting x, starting y, width x, width y

  animate_layer(text_layer_get_layer(layer), &start, &finish, 300, 500);
}

static void tick_handler_seconds(struct tm *tick_time, TimeUnits units_changed) {
    int seconds = tick_time->tm_sec;
 
    if(seconds == 59){
        move_text_layers_offscreen(s_time_layer, 1);
        move_text_layers_offscreen(s_hexcolor_layer, 1);
        //slide right CURRENT color layer

    }else if(seconds == 0){
        //change properties behind the scenees
        update_time();
        update_background();              
        
        //Slide onscreen from the left
        move_text_layers_onscreen(s_time_layer, 1);
        move_text_layers_onscreen(s_hexcolor_layer, 1);
    }else{
        text_layer_set_text(s_time_layer, s_buffer);
    }
}



static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window); //get window information
  GRect bounds = layer_get_bounds(window_layer);

  s_time_layer = text_layer_create( //create the TextLayer with specific bounds
      GRect(0, PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 50)); //starting x, starting y, width x, width y

  
  s_hexcolor_layer = text_layer_create( //create the TextLayer for Text
      GRect(0, PBL_IF_ROUND_ELSE(88, 80), bounds.size.w, 50)); //starting x, starting y, width x, width y
 
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_hexcolor_layer, GColorClear);
  text_layer_set_text_color(s_hexcolor_layer, GColorBlack);
  text_layer_set_text(s_hexcolor_layer, "#000000");
  text_layer_set_font(s_hexcolor_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28) );
  text_layer_set_text_alignment(s_hexcolor_layer, GTextAlignmentCenter);

  
  layer_add_child(window_layer, text_layer_get_layer(s_hexcolor_layer)); //add it as a child layer to the Window's root layer

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_hexcolor_layer);
}


static void init() {
  s_main_window = window_create();   //create main Window element and assign to pointer

  window_set_window_handlers(s_main_window, (WindowHandlers) { //set handlers to manage the elements inside the Window
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true); //show the Window on the watch, with animated=true

  update_time(); //make sure the time is displayed from the start
  update_background();

  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler_seconds);
}

static void deinit() {
  window_destroy(s_main_window); //destroy Window
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}