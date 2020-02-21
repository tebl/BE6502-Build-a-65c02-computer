#define DATA_DIRECTION_READ 1
#define DATA_DIRECTION_WRITE 0
#define MEMORY_BLOCK_SIZE 256

void dump_ram();
void dump_zp();
void dump_stack();
void dump_rom();
void dump_vectors();
byte peek(const unsigned int address);
byte poke(const unsigned address, byte value);
void set_control_on();
void set_control_off();
void test_ram();
void test_zp();
void test_stack();
void zero_zp();
void zero_stack();
void zero_ram();