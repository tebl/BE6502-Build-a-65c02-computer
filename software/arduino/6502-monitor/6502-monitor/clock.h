void clock_initialize();
void do_reset();

void print_clock();
void do_clock_disable();

void do_auto_clock();
void set_clock_1Hz();
void set_clock_2Hz();
void set_clock_4Hz();
void set_clock_16Hz();
void set_clock_32Hz();
void set_clock_128Hz();
void set_clock_256Hz();
void do_toggle_speed();

void do_manual_clock();
void do_tick();

void int_attach();
void int_detach();
void set_monitor_on();
void set_monitor_off();
