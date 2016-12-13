#ifndef GIZMO_SERIAL_LIGHTS
#define GIZMO_SERIAL_LIGHTS
void init_serial_lights(int count);
void deinit_serial_lights();
void animate_serial_lights();
void control_serial_lights(String message, void *cbarg);
#endif
