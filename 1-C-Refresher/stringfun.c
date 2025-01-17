#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);

//add additional prototypes here
int  reverse_buff(char *, int, int);
int  print_words(char *, int, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    int at_start = 1;
    int input_length = 0;  
    int in_whitespace = 0;

    while (*user_str){
        if (input_length + 1 > len){
            // Error: user supplied string is too large
            return -1;
        }
        if (!isspace(*user_str)){
            *buff = *user_str;
            buff++;
            input_length++;
            in_whitespace = 0;
            at_start = 0;
        }
        else if (!in_whitespace & !at_start){
            *buff = ' ';
            buff++;
            input_length++;
            in_whitespace = 1;
        }
        user_str++;
    }

    //remove trailing whitespace
    if (*(buff - 1) == ' '){
        *(buff - 1) = '.';
    }

    for (int i = input_length; i < BUFFER_SZ; i++){
        *buff = '.';
        buff++;
    }

    return input_length;  
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    if (str_len > len) {
        return -1;
    }
    if (str_len == 0) {
        return 0;
    }

    int word_count = 0;
    int at_start = 1;

    while (*buff != '.'){
        if (at_start){
            word_count++;
            at_start = 0;
        }
        char c = *buff;
        if (c == ' '){
            at_start = 1;
        }
        buff++;
    }
    return word_count;
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int reverse_buff(char *buff, int len, int str_len){
    if (str_len > len) {
        return -1;
    }
    if (str_len == 0) {
        return 0;
    }
    char temp;
    for (int i = 0; i < str_len/2; i++){
        temp = *(buff + i);
        *(buff + i) = *(buff + str_len - 1 - i);
        *(buff + str_len - i) = temp;
    }
    return 1;
}

int print_words(char *buff, int len, int str_len){
    if (str_len > len) {
        return -1;
    }
    if (str_len == 0) {
        return 0;
    }

    int word_count = 0;
    int char_ctr = 0;
    int at_start = 1;

    while (*buff != '.'){
        if (at_start){
            word_count++;
            printf("%d. ", word_count);
            at_start = 0;
        }
        char c = *buff;
        if (c == ' '){
            printf(" (%d)\n", char_ctr);
            char_ctr = 0;
            at_start = 1;
        }
        else{
            printf("%c", c);
            char_ctr++;
            if (*(buff + 1) == '.'){
                printf(" (%d)\n", char_ctr);
            }
        }
        buff++;
    }
    return word_count;
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      The first conditional, argc < 2, ensures arg[0] & arg[1] exist
    //      before accessing arg[1] for the second conditional.

    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      This if statement ensures the number of arguments is correct.
    //      If it is not, it will invoke usage, whuch will inform the user
    //      of the proper format to use for this program.

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = malloc(BUFFER_SZ);
    if (!buff){
        printf("Error allocating memory, rc = %d\n", 99);
        exit(2);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            rc = reverse_buff(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error reversing buffer, rc = %d\n", rc);
                exit(2);
            }
            break;
        case 'w':
            rc = print_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error printing words, rc = %d\n", rc);
                exit(2);
            }
            printf("Number of words returned: %d\n", rc);
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE