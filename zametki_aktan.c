#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 8192

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
    void set_console_color(WORD color){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }

    void reset_console_color(void){
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 7); //reset color
    }
    const char *const CLEAR = "cls";
	
#else
    void clearScreen(void) {
        printf("\033[2J\033[1;1H");

    }
    const char *const CLEAR = "clear";
    
    void set_console_color(unsigned int color) {
    	printf("\033[3%dm", color);
    }

    void reset_console_color(void) {
        printf("\033[0m");  //reset color
    }
#endif

typedef struct {
    const char *__filename;
    char TASKS[MAX_TASKS];
    char text[256];
    unsigned int index;
} TASK;

int _have_char(const char *namef) {
    FILE *ptr = fopen(namef, "r");
    if (!ptr) return 0;
    int ch = fgetc(ptr);
    fclose(ptr);
    return (ch != EOF);
}
int _All_Ind(const char* const filename){
	FILE *ptr = fopen(filename, "r");
	if(!ptr) return -1;

	char ch;
	int x = 0;
    while ((ch = fgetc(ptr)) != EOF) {
        if (ch == '\n') {
            ++x;
        }
    }
    fclose(ptr);
    return x;
}


void _find(const char *const filename, const char *const searchStr) {
    FILE *ptr = fopen(filename, "r");
    if (!ptr) {
        perror(filename);
        return;
    }

    char line[256];

    printf("Strings starting with '%s':\n", searchStr);
    while (fgets(line, sizeof(line), ptr) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, searchStr, strlen(searchStr)) == 0) {
            printf("%s\n", line);
        }
    }

    fclose(ptr);
}


void _new_task(const char *const msg, const char *const filen) {
    FILE *ptr = fopen(filen, "a+");
    if (ptr) {
        if (_have_char(filen)) fprintf(ptr, "\n%s", msg);
        else fprintf(ptr, "%s", msg);
        fclose(ptr);
    } else {
        perror(filen);
    }
}

char *_read_task(int indexRead, const char *filen) {
    FILE *ptr = fopen(filen, "r");
    if (!ptr) {
        perror("Error opening file");
        return NULL;
    }

    int ch;

    if (indexRead < 0) {
        int x = 0;
        set_console_color(2);
        printf("[%d]", x);
        reset_console_color();
        while ((ch = fgetc(ptr)) != EOF) {
            putchar(ch);
            if (ch == '\n') {
                ++x;
                set_console_color(2);
                printf("[%d]", x);
                reset_console_color();
            }
        }
        putchar('\n');
        fclose(ptr);
        return NULL;
    }

    char *msg = malloc(MAX_TASKS);
    if (!msg) {
        perror("Memory allocation failed");
        fclose(ptr);
        return NULL;
    }

    int current_line = 0;
    int pos = 0;

    while (current_line < indexRead) {
        ch = fgetc(ptr);
        if (ch == EOF) {
            fclose(ptr);
            free(msg);
            return NULL;
        }
        if (ch == '\n') {
            current_line++;
        }
    }

    while ((ch = fgetc(ptr)) != EOF && ch != '\n' && pos < MAX_TASKS - 1) {
        msg[pos++] = (char)ch;
    }
    msg[pos] = '\0';

    fclose(ptr);
    return msg;
}

int _ret_Code(void) {
    int code;
    if (scanf("%d", &code) != 1) {
        printf("Scanf error! Input '4' for help\n");
        while (getchar() != '\n');
        return -1;
    } else {
        while (getchar() != '\n');
        return code;
    }
}
const char *helpl = "1 - Add new task\n2 - Check all tasks\n3 - Check task with index\n4 - Help\n5 - Clear\n6 - Search engine\n0 - Exit";

void _shell(int inputCode, const char *const filename_) {
    TASK obj;
    switch (inputCode) {
        case 1:
            printf("Input new task >> ");
            fgets(obj.TASKS, MAX_TASKS, stdin);
            obj.TASKS[strcspn(obj.TASKS, "\n")] = '\0';
            _new_task(obj.TASKS, filename_);
            break;
        case 2:
            _read_task(-1, filename_);
            break;
        case 3:
            printf("Input index of your task >> ");
            if (scanf("%u", &obj.index) != 1) {
                printf("Invalid index!\n");
                while (getchar() != '\n');
                return;
            }
            while (getchar() != '\n');

            char *tasks = _read_task(obj.index, filename_);
            if (!tasks) return;

			set_console_color(2);
			printf("[%u]", obj.index);
			reset_console_color();
			puts(tasks);
            free(tasks);
            break;
        case 4:
            printf("%s\n", helpl);
            break;
        case 5:
            system(CLEAR);
            break;
    	case 6:
    		printf("Input your text(the beginning is also possible) >> ");

    		fgets(obj.text, 256, stdin);
    		obj.text[strcspn(obj.text, "\n")] = 0;
    		
    		_find(filename_, obj.text);
    		break;
        case 0:
            exit(0);
        default:
            break;
    }
}

int main(void) {
    TASK task;
    task.__filename = "zametka.txt";
    printf("Select action:\n%s\n", helpl);
    while (1) {
        printf(">> ");
        _shell(_ret_Code(), task.__filename);
    }
}
