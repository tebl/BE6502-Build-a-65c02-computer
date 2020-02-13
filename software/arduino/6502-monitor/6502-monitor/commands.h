extern bool colorize;

void print_welcome();
void print_version();

bool handle_command(String command, String name, void (*function)());
void select_command(String command);
void echo_command(String command);
void echo_unknown(String command);
