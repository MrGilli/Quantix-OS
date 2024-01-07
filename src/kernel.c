#include "kernel.h"
#include "console.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "io_ports.h"
#include "framebuffer.h"
#include "multiboot.h"
#include "stdint-gcc.h"
#include "ctypes.h"
#include "qemu.h"
#include "romfont.h"


#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#define BRAND_QEMU  1
#define BRAND_VBOX  2

#define MAX_FILENAME_LENGTH 20
#define MAX_FILE_COUNT 100
#define MAX_FILE_CONTENT_LENGTH 10000

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    int size;
    char content[MAX_FILE_CONTENT_LENGTH];
} File;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    int file_count;
    File files[MAX_FILE_COUNT];
} Directory;

Directory root_directory;

void scan(const char *shell, bool init_all){
	char new_buffer [255];
    printf(shell);
    memset(new_buffer, 0, sizeof(new_buffer));
    getstr_bound(new_buffer, strlen(shell));
}


void custom_strcpy(char *dest, const char *src) {
    while (*src != '\0') {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0'; // Null-terminate the destination string
}

void initFileSystem() {
    custom_strcpy(root_directory.name, "root");
    root_directory.file_count = 0;
}

void createFile(char *name, char *content) {
    if (root_directory.file_count < MAX_FILE_COUNT) {
        File newFile;
        custom_strcpy(newFile.name, name);
        newFile.size = strlen(content);
        custom_strcpy(newFile.content, content);
        newFile.content[MAX_FILE_CONTENT_LENGTH - 1] = '\0';

        root_directory.files[root_directory.file_count++] = newFile;
        printf("File '%s' created successfully.\n", name);
    } else {
        printf("File system full. Cannot create more files.\n");
    }
}



void listFiles() {
    printf("Files in root directory:\n");
    for (int i = 0; i < root_directory.file_count; ++i) {
        printf("- FILE NAME: %s, FILE CONTENT: %s\n", root_directory.files[i].name, root_directory.files[i].content);
    }
}


void __cpuid(uint32 type, uint32 *eax, uint32 *ebx, uint32 *ecx, uint32 *edx) {
    asm volatile("cpuid"
                : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                : "0"(type)); // put the type into eax
}

int cpuid_info(int print) {
    uint32 brand[12];
    uint32 eax, ebx, ecx, edx;
    uint32 type;

    memset(brand, 0, sizeof(brand));
    __cpuid(0x80000002, (uint32 *)brand+0x0, (uint32 *)brand+0x1, (uint32 *)brand+0x2, (uint32 *)brand+0x3);
    __cpuid(0x80000003, (uint32 *)brand+0x4, (uint32 *)brand+0x5, (uint32 *)brand+0x6, (uint32 *)brand+0x7);
    __cpuid(0x80000004, (uint32 *)brand+0x8, (uint32 *)brand+0x9, (uint32 *)brand+0xa, (uint32 *)brand+0xb);

    if (print) {
        printf("Brand: %s\n", brand);
        for(type = 0; type < 4; type++) {
            __cpuid(type, &eax, &ebx, &ecx, &edx);
            printf("type:0x%x, eax:0x%x, ebx:0x%x, ecx:0x%x, edx:0x%x\n", type, eax, ebx, ecx, edx);
        }
    }

    if (strstr(brand, "QEMU") != NULL)
        return BRAND_QEMU;

    return BRAND_VBOX;
}


BOOL is_echo(char *b) {
    if((b[0]=='e')&&(b[1]=='c')&&(b[2]=='h')&&(b[3]=='o'))
        if(b[4]==' '||b[4]=='\0')
            return TRUE;
    return FALSE;
}

void shutdown() {
    int brand = cpuid_info(0);
    // QEMU
    if (brand == BRAND_QEMU)
        outports(0x604, 0x2000);
    else
        // VirtualBox
        outports(0x4004, 0x3400);
}

void new_kernel_instance(char *cmd_to_run){
	//INIT KERNEL CODE
	//RUN THE MKFILE CMD AS A KERNEL INSTANCE
	//RETURN OUTPUT TO MAIN FUNCTION
	printf("\nRunning Command/Program '%s' in Quantum instance...\n\n", cmd_to_run);
	if(cmd_to_run = "mkfile") {
		char name [255];
        const char *shell_file = "File Name> ";
        printf(shell_file);
        memset(name, 0, sizeof(name));
        getstr_bound(name, strlen(shell_file));
        
        char file_content [255];
        const char *shell_file_content = "File Content> ";
        printf(shell_file_content);
        memset(file_content, 0, sizeof(file_content));
        getstr_bound(file_content, strlen(shell_file_content));
        
        //CREATE THE FILE BASED ON THE FILE NAME AND IT'S CONTENT
        createFile(name, file_content);
	} else if(cmd_to_run = "clear"){
		printf("\nThe 'clear' command should not be run in a Quantum Instance.\n");
		printf("Just run 'clear' in the Terminal.\n");
	}else {
		printf("Command '%s' not found!!\n", cmd_to_run);
	}
}

void non_terminal(){
	//init the terminal but when enter is pressed dont add the terminal text.
}

void magix(){
	
	
	char name [255];
    const char *shell_file = "File Name> ";
    printf(shell_file);
    memset(name, 0, sizeof(name));
    getstr_bound(name, strlen(shell_file));
    printf("___________________________________ \n");
    printf("Welcome To The Magix Editor!!\n\n");
    printf_color(COLOR_GREEN, "EXIT(.out magix)\n");
    
    char file_content [255];
    const char *shell_file_content = "> ";
    
    while(1){
    	if(strcmp(file_content, ".out magix") == 0){
    		main_loop();
    	}
    	else {
    		printf(shell_file_content);
    		memset(file_content, 0, sizeof(file_content));
    		getstr_bound(file_content, strlen(shell_file_content));
    	}
    }
    
    //CREATE THE FILE BASED ON THE FILE NAME AND IT'S CONTENT
    createFile(name, file_content);
}




void login_and_run() {
    char buffer[255];
    const char *shell = "Password> ";
    gdt_init();
    idt_init();

    console_init(COLOR_WHITE, COLOR_BLUE);
    keyboard_init();

    printf("Login Needed To Continue...\n");
    printf("Default Password: 'root'.\n");

    while(1) {
        printf(shell);
        memset(buffer, 0, sizeof(buffer));
        getstr_bound(buffer, strlen(shell));

        if (strlen(buffer) == 0)
            continue;
        if(strcmp(buffer, "root") == 0) {
            main_loop();
            console_clear(COLOR_WHITE, COLOR_BLACK);
        }
        else if(strcmp(buffer, "shutdown") == 0) {
            shutdown();
        } 
        else {
            printf("invalid command: %s\n", buffer);
        }

    }
}

void main_loop() {
    char buffer[255];
    const char *shell = "[root@Quantix]$ ";
    gdt_init();
    idt_init();
    initFileSystem();

    console_init(COLOR_WHITE, COLOR_BLACK);
    keyboard_init();

    printf("starting terminal...\n");
    printf("INIT Quantix.....\n");
    printf("Kernel Ready (Type 'help' to see list of supported commands)\n");
    printf("Important Info: 'MAX FILES: 100', 'MAX FILE CONTENT: 10,000'\n\n");

    while(1) {
        printf_color(COLOR_RED, shell);
        printf("");
        memset(buffer, 0, sizeof(buffer));
        getstr_bound(buffer, strlen(shell));
        // Draw the box using framebuffer functions
       // Draw the box using framebuffer functions

        if (strlen(buffer) == 0)
            continue;
        if(strcmp(buffer, "cpuid") == 0) {
            cpuid_info(1);
        } 
        else if(strcmp(buffer, "help") == 0) {
            printf("Quantix Operating System & Tiny OS Terminal\n");
            printf("Commands:\n\n help\n cpuid\n clear\n mkfile(Create a basic file, like touch command)\n ls(List all files and their contents in DIR)\n whoami\n echo\n exec(Execute a file/program)\n qexec(Execute program in Quantum instance)\n shutdown\n\n");
        }
        else if(strcmp(buffer, "mkfile") == 0) {
        	char name [255];
        	const char *shell_file = "File Name> ";
        	printf(shell_file);
        	memset(name, 0, sizeof(name));
        	getstr_bound(name, strlen(shell_file));
        	
        	char file_content [255];
        	const char *shell_file_content = "File Content> ";
        	printf(shell_file_content);
        	memset(file_content, 0, sizeof(file_content));
        	getstr_bound(file_content, strlen(shell_file_content));
        	
        	//CREATE THE FILE BASED ON THE FILE NAME AND IT'S CONTENT
            createFile(name, file_content);
        }
        else if(strcmp(buffer, "rm") == 0){
        	scan("File to remove> ", TRUE);
        	printf("'rm' command isn't ready for use yet.\n");
        }
        else if(strcmp(buffer, "ls") == 0) {
            listFiles();
        }
        else if(strcmp(buffer, "qexec") == 0){
        	char program_name [255];
        	const char *prompt = "Program To Run In Quantum Instance> ";
        	printf(prompt);
        	memset(program_name, 0, sizeof(program_name));
        	getstr_bound(program_name, strlen(prompt));
        	new_kernel_instance(program_name);
        }
        else if(strcmp(buffer, "exec") == 0){
        	char program_name [255];
        	const char *prompt = "Program To Run In Kernel> ";
        	printf("Available Programs/Commands to execute:\n");
        	printf_color(COLOR_GREEN, " - Magix(CLI Text Editor)\n - calc(CLI Calculator)\n - mani(Quantix OS Manifesto)\n - guide(List of all sys cmds and what they do)\n\n");
        	
        	printf(prompt);
        	memset(program_name, 0, sizeof(program_name));
        	getstr_bound(program_name, strlen(prompt));
        	if(strcmp(program_name, "calc") == 0){
        		printf("Running basic calculator app...\n");
        	} else if(strcmp(program_name, "magix") == 0){
        		magix();
        	}else {
        		printf("ERROR: Command '%s' not found :(\n\n");
        	}
        }
        else if(strcmp(buffer, "whoami") == 0) {
            printf("Quantix Operating System User:\n");
            printf("User: ROOT; (Sudo Privileges: TRUE)\n");
            printf("CPUID INFO: \n\n");
            cpuid_info(1);
        }
        else if(strcmp(buffer, "clear") == 0) {
            console_clear(COLOR_WHITE, COLOR_BLACK);
        } 
        else if(is_echo(buffer)) {
            printf("%s\n", buffer + 5);
        } 
        else if(strcmp(buffer, "shutdown") == 0) {
            shutdown();
        } else {
            printf("Invalid command: %s\n", buffer);
        }
    }
}

void kmain(){
    //FUNCTION THAT RUNS THE OS
    login_and_run();
}
