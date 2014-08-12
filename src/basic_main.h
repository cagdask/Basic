typedef struct {int x; int y; int z;}Vector3D;

void de_init();
void init();
void window_load(Window *w);
static void timer_callback(void *data);
void calculateBuffer();
int updateBuffer(AccelData value);
int analyzeBuffer();
void printBuffer();