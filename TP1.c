#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 128

// Function to execute the command in the shell created
void execute_command(const char *command, ssize_t size_command) {
    pid_t pid = fork();
    // In the child process we attempts to execute the command if it is existing, else we have an error message
    if (pid == 0) {
        execlp(command, command, (char *)NULL);
        write(STDOUT_FILENO, "Erreur : Commande non trouvée.\n", 31);
        exit(EXIT_FAILURE);
    } 
    // In the father process : check how the child finished and update the prompt with exit or signal status
    else if (pid > 0) {
        int status;
        wait(&status);
        const char *prompt = "enseash %";
        char n[100];
        int message_status;
        if (WIFEXITED(status)) {
            message_status = snprintf(n,sizeof(n), "[exit:%d]", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            message_status = snprintf(n, sizeof(n), "[sign:%d]", WTERMSIG(status));
        }
        write(STDOUT_FILENO,n,message_status);
    } else {
        write(STDOUT_FILENO, "Error: Failed to fork.\n", 23);
    }
}
// function to exit the new shell
void exit_shell(const char *EXIT_MESSAGE) {
    write(STDOUT_FILENO, EXIT_MESSAGE, strlen(EXIT_MESSAGE));
    exit(EXIT_SUCCESS); 
}

int main(){
    const char *welcome_message = "Bienvenue dans le shell ENSEA. \nPour quitter, tapez 'exit'.\n";
	const char *prompt = "enseash %";
    const char *exit_message ="Bye bye... \n";
    ssize_t size_command;
    write(STDOUT_FILENO,welcome_message,strlen(welcome_message));
    //read  lit longueur il faut enlever \n : buffer sizeread-1 = 0 
    char buffer[BUFFER_SIZE];
    while (1) {
       
        write(STDOUT_FILENO, prompt, strlen(prompt));
        // read the command but remove the \n before
        int bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0) {
            buffer[strcspn(buffer, "\n")] = '\0';  

            if (strncmp(buffer, "exit", 4) == 0) {
                exit_shell(exit_message);
                break;
            }
        }
        execute_command(buffer, size_command);
    }
}