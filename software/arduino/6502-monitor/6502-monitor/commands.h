void print_welcome();
void print_version();
void set_monitor_off();
void set_monitor_on();

bool handle_command(String command, String name, void (*function)());
void select_command(String command);
void echo_command(String command);
void echo_unknown(String command);
