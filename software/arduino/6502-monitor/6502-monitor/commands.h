void do_auto_clock();
void do_manual_clock();
void do_tick();
void do_toggle_speed();
void do_reset();
void on_clock();
void print_welcome();
void print_version();
void set_monitor_off();
void set_monitor_on();

bool handle_command(String command, String name, void (*function)());
void select_command(String command);
void echo_command(String command);
void echo_unknown(String command);
