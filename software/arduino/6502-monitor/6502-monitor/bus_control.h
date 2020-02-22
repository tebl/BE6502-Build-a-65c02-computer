#define DATA_DIRECTION_READ 1
#define DATA_DIRECTION_WRITE 0
#define MEMORY_BLOCK_SIZE 256
#define MEMORY_RECORD_LENGTH 16

void dump_memory(const unsigned long start_address, const unsigned long end_address);
void dump_ram();
void dump_ram_1k();
void dump_ram_2k();
void dump_ram_4k();
void dump_ram_8k();
void dump_ram_16k();
void dump_zp();
void dump_stack();
void dump_rom();
void dump_rom_1k();
void dump_rom_2k();
void dump_rom_4k();
void dump_rom_8k();
void dump_rom_16k();
void dump_rom_32k();
void dump_vectors();

void dump_intel(const unsigned long start_address, const unsigned long end_address);
void dump_intel_ram();
void dump_intel_ram_1k();
void dump_intel_ram_2k();
void dump_intel_ram_4k();
void dump_intel_ram_8k();
void dump_intel_ram_16k();
void dump_intel_rom();
void dump_intel_rom_1k();
void dump_intel_rom_2k();
void dump_intel_rom_4k();
void dump_intel_rom_8k();
void dump_intel_rom_16k();
void dump_intel_rom_32k();
void dump_intel_stack();
void dump_intel_zp();
bool read_intel(String c);

void dump_paper(const unsigned long start_address, const unsigned long end_address);
void dump_paper_ram();
void dump_paper_ram_1k();
void dump_paper_ram_2k();
void dump_paper_ram_4k();
void dump_paper_ram_8k();
void dump_paper_ram_16k();
void dump_paper_rom();
void dump_paper_rom_1k();
void dump_paper_rom_2k();
void dump_paper_rom_4k();
void dump_paper_rom_8k();
void dump_paper_rom_16k();
void dump_paper_rom_32k();
void dump_paper_stack();
void dump_paper_zp();

byte peek(const unsigned long address);
byte poke(const unsigned long address, byte value);
void set_control_on();
void set_control_off();
void test_ram();
void test_zp();
void test_stack();
void zero_zp();
void zero_stack();
void zero_ram();