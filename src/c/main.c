#include <pebble.h>

#define KEY_TITLE 0
#define KEY_EPISODE 1
#define KEY_HOSTS 2

// Store incoming information
//Splash Vars
static Window *s_splash_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

//All of these var names suck, I'll "fix them later" (no I won't)
//Hub Vars
Window *my_window;
TextLayer *text_layer;
TextLayer *s_episode_layer;
TextLayer *s_hosts_textlayer;
int onHosts = 0;

//AppGlance Vars
static char s_message[32];

//Thing for timer to do once it runs out? Idk it works though
void test(void * data){
  APP_LOG(APP_LOG_LEVEL_INFO, "Timer being executed....");
  window_stack_push(my_window, true);
  window_stack_remove(s_splash_window, false);
  APP_LOG(APP_LOG_LEVEL_INFO, "Hub being pushed....");
}

void hub_select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(onHosts == 0){
    onHosts = 1;
    
    layer_remove_child_layers(window_get_root_layer(my_window));
    // Add Text to Window
    layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_hosts_textlayer));
  }else{
    onHosts = 0;
    
    layer_remove_child_layers(window_get_root_layer(my_window));
    // Add Text to Window
    layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_episode_layer));
    layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  }
}

void window_config_provider(Window *window) {
// window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
 window_single_click_subscribe(BUTTON_ID_SELECT, hub_select_single_click_handler);
// window_single_click_subscribe(BUTTON_ID_DOWN, hub_down_single_click_handler);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  static char episode_buffer[32];
  static char episode_layer_buffer[32];
  
  static char hosts_buffer[88];
  static char hosts_layer_buffer[88];

  // Read tuples for data
  Tuple *conditions_tuple = dict_find(iterator, KEY_TITLE);
  Tuple *episode_tuple = dict_find(iterator, KEY_EPISODE);
  Tuple *hosts_tuple = dict_find(iterator, KEY_HOSTS);

  // If all data is available, use it
  if(conditions_tuple && episode_tuple) {
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(episode_buffer, sizeof(episode_buffer), "%s", episode_tuple->value->cstring);
    snprintf(hosts_buffer, sizeof(hosts_buffer), "%s", hosts_tuple->value->cstring);

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", conditions_buffer);
    text_layer_set_text(text_layer, weather_layer_buffer);
    snprintf(episode_layer_buffer, sizeof(episode_layer_buffer), "%s", episode_buffer);
    text_layer_set_text(s_episode_layer, episode_layer_buffer);
    snprintf(s_message, 32, "%s - %s", conditions_buffer, episode_buffer);
    snprintf(hosts_layer_buffer, sizeof(hosts_layer_buffer), "%s", hosts_buffer);
    text_layer_set_text(s_hosts_textlayer, hosts_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

//This is my first shot at trying this appglance stuff dear Jesus forgive me
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit,
                                                                  void *context) {
  // This should never happen, but developers should always ensure
  // they’re not adding more app glance slices than the limit
  if (limit < 1) return;

  // Cast the context object to a string
  const char *message = (char *)context;

  // Create the AppGlanceSlice
  // When layout.icon is not set, the app's default icon is used
  const AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      //Haven't added icon yet so no need to make a default one
      .icon = APP_GLANCE_SLICE_DEFAULT_ICON,
      .subtitle_template_string = message
    },
    // TODO: Change to APP_GLANCE_SLICE_NO_EXPIRATION in SDK 4-dp2
    //Did it for you baby ;)
    .expiration_time = time(NULL)+3600
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);
  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}

static void splash_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_splash_window, GColorBlack);
  
  s_background_layer = bitmap_layer_create(bounds);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(PBL_IF_COLOR_ELSE(RESOURCE_ID_IMAGE_SLEEPY_PNG, RESOURCE_ID_IMAGE_SLEEPYBW_PNG));
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);
  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
}


static void main_window_load(Window *window) {
  window_set_background_color(my_window, GColorBlack);
  
  text_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(20, 60, 144, 30),GRect(0, 50, 144, 30)));
  text_layer_set_text_color(text_layer, PBL_IF_COLOR_ELSE(GColorPurple, GColorWhite));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text(text_layer, "Loading...");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  // Add Text to Window
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(text_layer));
  
  s_episode_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(20, 110, 144, 30),GRect(0, 100, 144, 30)));
  text_layer_set_text_color(s_episode_layer, PBL_IF_COLOR_ELSE(GColorPurple, GColorWhite));
  text_layer_set_background_color(s_episode_layer, GColorClear);
  text_layer_set_text(s_episode_layer, "Loading...");
  text_layer_set_text_alignment(s_episode_layer, GTextAlignmentCenter);
  text_layer_set_font(s_episode_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  // Add Text to Window
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_episode_layer));
  
  s_hosts_textlayer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(20, 50, 144, 100),GRect(0, 40, 144, 100))) ;
  text_layer_set_text_color(s_hosts_textlayer, PBL_IF_COLOR_ELSE(GColorPurple, GColorWhite));
  text_layer_set_background_color(s_hosts_textlayer, GColorClear);
  text_layer_set_overflow_mode(s_hosts_textlayer, GTextOverflowModeWordWrap);
  text_layer_set_text(text_layer, "Loading...");
  text_layer_set_text_alignment(s_hosts_textlayer, GTextAlignmentCenter);
  text_layer_set_font(s_hosts_textlayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
  text_layer_destroy(s_episode_layer);
  text_layer_destroy(s_hosts_textlayer);
}

static void splash_window_unload(Window *window) {
  // Destroy TextLayer
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_bitmap);
}

void handle_init(void) {
  
  // Create main Window element and assign to pointer
  s_splash_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_splash_window, (WindowHandlers) {
    .load = splash_window_load,
    .unload = splash_window_unload
  });
  
  window_stack_push(s_splash_window, true);
  APP_LOG(APP_LOG_LEVEL_INFO, "Splash pushed....");
  
 // Create main Window element and assign to pointer
  my_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_click_config_provider(my_window, (ClickConfigProvider) window_config_provider);
  
   // Register callbacks
app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);

   // Open AppMessage
const int inbox_size = 128;
const int outbox_size = 128;
app_message_open(inbox_size, outbox_size);
  
    //Hi Ho timer
app_timer_register(1000, test, NULL);
}

void handle_deinit(void) {
  window_destroy(my_window);
  app_glance_reload(prv_update_app_glance, s_message);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
