/*
 * This file is part of gardOS 1.0 generic.
 *
 * Copyright (c) 2025 Gabriel Sîrbu
 *
 * This software is distributed under the terms of the GNU General Public License.
 * See LICENSE for more details.
 */

#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE (BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES)

unsigned int current_loc = 0;
char *vidptr = (char*)0xb8000;
unsigned int lines = 0;

// Function prototypes
void clear_screen(void);
void print(const char *str);
unsigned char inb(unsigned short port);
char scancode_to_ascii(unsigned char scancode);
void process_command(char *input_buffer);
int strcmp_custom(const char *s1, const char *s2);
int atoi(const char *str);
unsigned int get_random_number(void);
void number_guessing_game(void);

// Simple string comparison function
int strcmp_custom(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void clear_screen(void) {
    unsigned int i = 0;
    while (i < SCREENSIZE) {
        vidptr[i++] = ' ';
        vidptr[i++] = 0x07; // White text on black background
    }
    current_loc = 0;
    lines = 0;
}

void print(const char *str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            current_loc += COLUMNS_IN_LINE - (current_loc % COLUMNS_IN_LINE);
            lines++;
        } else {
            vidptr[current_loc * BYTES_FOR_EACH_ELEMENT] = str[i];
            vidptr[current_loc * BYTES_FOR_EACH_ELEMENT + 1] = 0x07;
            current_loc++;
        }
        if (current_loc >= SCREENSIZE / BYTES_FOR_EACH_ELEMENT) {
            clear_screen();
        }
        i++;
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

char scancode_to_ascii(unsigned char scancode) {
    static char ascii_map[] =
        "\x00\x1B" "1234567890-=\x08\t"
        "qwertyuiop[]\n\x00" "asdfghjkl;'`\x00" "\\"
        "zxcvbnm,./\x00" "*\x00 " "\x00";

    if (scancode < sizeof(ascii_map)) {
        return ascii_map[scancode];
    }
    return '\0';
}

int atoi(const char *str) {
    int result = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            break;
        }
        i++;
    }
    return result;
}

unsigned long lcg_state = 123456789;
unsigned int get_random_number(void) {
    lcg_state = (lcg_state * 1103515245 + 12345) & 0x7fffffff;
    return (unsigned int)lcg_state;
}

void number_guessing_game(void) {
    print("Number Guessing Game started!\n");
    print("Guess the number between 1 and 100:\n");

    unsigned int secret = (get_random_number() % 100) + 1;
    int guess = 0;
    int tries = 0;
    char guess_buffer[80];

    while (1) {
        print("Enter your guess: ");
        unsigned int input_index = 0;
        while (1) {
            unsigned char scancode = inb(0x60);
            if (scancode & 0x80)
                continue;
            if (scancode == 0x1C) {
                while (inb(0x60) != (0x1C | 0x80)) { }
                print("\n");
                guess_buffer[input_index] = '\0';
                break;
            } else if (scancode == 0x0E) {
                if (input_index > 0) {
                    input_index--;
                    current_loc--;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT] = ' ';
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT + 1] = 0x07;
                }
                while (inb(0x60) != (0x0E | 0x80)) { }
            } else {
                char c = scancode_to_ascii(scancode);
                if (c != '\0' && input_index < sizeof(guess_buffer) - 1) {
                    guess_buffer[input_index++] = c;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT] = c;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT + 1] = 0x07;
                    current_loc++;
                }
                while (inb(0x60) != (scancode | 0x80)) { }
            }
        }
        guess = atoi(guess_buffer);
        tries++;
        if (guess < (int)secret) {
            print("Too low!\n");
        } else if (guess > (int)secret) {
            print("Too high!\n");
        } else {
            print("Congratulations! You guessed correctly!\n");
            break;
        }
    }
}

void process_command(char *input_buffer) {
    if (strcmp_custom(input_buffer, "clear") == 0) {
        clear_screen();
    }
	else if (strcmp_custom(input_buffer, "help") == 0){
		print("about - about the OS\n");
		print("sysjoke - system joke\n");
		print("number-guessing-game - gaming on this OS\n");
		print("shutdown - shutdown the system\n");
	}
     else if (strcmp_custom(input_buffer, "about") == 0) {
        print("operating system:        gardOS\n");
        print("developer:              Gabriel Sirbu\n");
        print("OS version:             1.0 generic\n");
        print("architecture:           32 bits\n");
        print("defended by GPL\n");
    } else if (strcmp_custom(input_buffer, "sysjoke") == 0) {
        print("If America were discovered by Apple:\n");
        print("iAmerica\n");
    } else if (strcmp_custom(input_buffer, "number-guessing-game") == 0) {
        number_guessing_game();
    } else if (strcmp_custom(input_buffer, "shutdown") == 0) {
        asm volatile (
            "mov $0x5307, %%ax\n"
            "mov $0x0001, %%bx\n"
            "int $0x15\n"
            :
            :
            : "eax", "ebx"
        );
        asm volatile ("cli; hlt;");
    } else {
        print("Undefined command.\n");
    }
}

void kmain(void) {
    clear_screen();
    for (int i = 0; i < 80; i++) {
        print("=");
    }
    print("\n");
    
    char input_buffer[80];
    
    while (1) {
        print("]$ ");
        unsigned int input_index = 0;
        
        while (1) {
            unsigned char scancode = inb(0x60);

            if (scancode & 0x80) {
                continue;
            }

            if (scancode == 0x1C) {
                while (inb(0x60) != (0x1C | 0x80)) { }
                print("\n");
                input_buffer[input_index] = '\0';
                process_command(input_buffer);
                break;
            } 
            else if (scancode == 0x0E) {
                if (input_index > 0) {
                    input_index--;
                    current_loc--;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT] = ' ';
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT + 1] = 0x07;
                }
                while (inb(0x60) != (0x0E | 0x80)) { }
            } 
            else {
                char c = scancode_to_ascii(scancode);
                if (c != '\0' && input_index < sizeof(input_buffer) - 1) {
                    input_buffer[input_index++] = c;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT] = c;
                    vidptr[current_loc * BYTES_FOR_EACH_ELEMENT + 1] = 0x07;
                    current_loc++;
                }
                while (inb(0x60) != (scancode | 0x80)) { }
            }
        }
    }
}
