#include <pebble.h>
#include <stdio.h>
#include <string.h>
#include "basic_main.h"
#define BUF_SIZE 5

Window *window; 
TextLayer *g_text_layer;
static AppTimer *timer;

Vector3D data[BUF_SIZE];
Vector3D stables[BUF_SIZE];
int iBuf = BUF_SIZE -1, iStable =  BUF_SIZE -1;

int prevMovement = 0;

bool begEndDiffOpFlag = false, devOpFlag = true, meanOpField = false;

AccelData accel; 


Vector3D cDiff = {0,0,0};
Vector3D cMean = {0,0,0};
Vector3D cSTD = {0,0,0};

static char* buf = "XXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\nXXXXX,YYYYY,ZZZZZ\n" ;
char tempBuf[] = "XXXXX,YYYYY,ZZZZZ!\nXXXXX,YYYYY,ZZZZZ!\n";


void printBuffer(Vector3D* a)
{
  int i = 0;
  
  snprintf(buf, sizeof(tempBuf), "%d,%d,%d!\n", a[i].x,a[i].y,a[i].z); 
  for(i=1;i<BUF_SIZE;i++){
    snprintf(tempBuf, sizeof(tempBuf), "%d,%d,%d!\n", a[i].x,a[i].y,a[i].z); 
    buf = strcat(buf,tempBuf);
  }
  text_layer_set_text(g_text_layer, buf);                      
}



int analyzeBuffer(){
  int value = cSTD.x + cSTD.y + cSTD.z  ;
  if(value < 25){

   text_layer_set_background_color(g_text_layer,GColorClear);
   text_layer_set_text_color(g_text_layer, GColorBlack);
   if(prevMovement ==1){
     if(abs(stables[iStable].x - cMean.x) + abs(stables[iStable].y - cMean.y) + abs(stables[iStable].z - cMean.z) > 250)
       iStable = (iStable + 1) % BUF_SIZE;
     stables[iStable].x = cMean.x;
     stables[iStable].y = cMean.y;
     stables[iStable].z = cMean.z;
     prevMovement =0;
     printBuffer(stables);
   }
     
  }
  else{
 
    prevMovement=1;
    text_layer_set_background_color(g_text_layer,GColorBlack);
    text_layer_set_text_color(g_text_layer, GColorClear);
  }
//    snprintf(tempBuf, sizeof(tempBuf),"%d,%d,%d\n%d,%d,%d",cMean.x,cMean.y,cMean.z,cSTD.x,cSTD.y,cSTD.z);
//    text_layer_set_text(g_text_layer, tempBuf);
    
  
  return 0;
  
}
  

int updateBuffer(AccelData value){
  iBuf = (iBuf + 1) % BUF_SIZE; //where to write
  

  data[iBuf].x = value.x;
  data[iBuf].y = value.y;
  data[iBuf].z = value.z;
  if(iBuf == BUF_SIZE -1)
    calculateBuffer();
  

  return 0; 
}

void calculateBuffer(){
  int i;
    // FIRST MEAN
  if(devOpFlag || meanOpField) //means we need to calculate mean anyway
  {
    cMean.x = 0; cMean.y = 0; cMean.z = 0 ;
    for(i=0;i<BUF_SIZE;i++)
    {
      cMean.x += data[i].x/BUF_SIZE;
      cMean.y += data[i].y/BUF_SIZE;
      cMean.z += data[i].z/BUF_SIZE;
    }
  }  
  
  
  if(devOpFlag)
  {
    cSTD.x = 0; cSTD.y = 0; cSTD.z = 0 ;
    for(i=0;i<BUF_SIZE;i++)
    {
      cSTD.x += abs(cMean.x - data[i].x)/BUF_SIZE;
      cSTD.y += abs(cMean.y - data[i].y)/BUF_SIZE;
      cSTD.z += abs(cMean.z - data[i].z)/BUF_SIZE;
    }
  }
  
  if(begEndDiffOpFlag){
    cDiff.x = data[iBuf].x - data[(iBuf +1)%BUF_SIZE].x;
    cDiff.y = data[iBuf].y - data[(iBuf +1)%BUF_SIZE].y;
    cDiff.z = data[iBuf].z - data[(iBuf +1)%BUF_SIZE].z;
  }
  analyzeBuffer();
  
}
                      
                      
                      
static void timer_callback(void *data) {
 
 accel_service_peek(&accel);
 updateBuffer(accel);
 timer = app_timer_register(50, timer_callback, NULL);
}



void window_load(Window *w)
{
  g_text_layer = text_layer_create(GRect(0,0,144,168));
  text_layer_set_background_color(g_text_layer,GColorClear);
  text_layer_set_text_color(g_text_layer, GColorBlack);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer));
  text_layer_set_text(g_text_layer, "Çagdas Karatas\n  2014");
  text_layer_set_text_alignment(g_text_layer, GTextAlignmentCenter);
//  text_layer_set_font(g_text_layer,fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
//  printBuffer();
  timer = app_timer_register(2000, timer_callback, NULL);
}
void window_unload(Window *w)
{
  text_layer_destroy(g_text_layer);
}

void init()
{
  int i;
   for(i = 0;i<BUF_SIZE;i++)
   {
     data[i].x = 0;data[i].y = 0;data[i].z = 0;
     stables[i].x = 0;stables[i].y = 0;stables[i].z = 0;
   }
  accel =(AccelData) { .x = 0, .y = 0, .z = 0 };
  
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers){
      .load = window_load,
      .unload = window_unload
    });
  window_stack_push(window, true);
  accel_data_service_subscribe(0, NULL);
  
 
}

void de_init()
{
    window_destroy(window);
}  
  
int main(void)
{
  init();
  app_event_loop();
  de_init();
}

