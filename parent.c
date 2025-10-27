#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    char filename[BUFFER_SIZE];
    int pipe_to_child[2];   // pipe для отправки данных в child
    int pipe_from_child[2]; // pipe для получения данных от child
    
    // Запрос имени файла у пользователя
    printf("Введите имя файла: ");
    if (fgets(filename, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        return 1;
    }
    // Удаляем символ новой строки
    filename[strcspn(filename, "\n")] = 0;
    
    // Создание pipe'ов
    if (pipe(pipe_to_child) == -1) {
        perror("pipe_to_child");
        return 1;
    }
    
    if (pipe(pipe_from_child) == -1) {
        perror("pipe_from_child");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        return 1;
    }
    
    // Создание дочернего процесса
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        // Дочерний процесс
        close(pipe_to_child[1]);    // закрываем запись в pipe_to_child
        close(pipe_from_child[0]);  // закрываем чтение из pipe_from_child
        
        // Перенаправляем stdin на чтение из pipe
        dup2(pipe_to_child[0], STDIN_FILENO);
        close(pipe_to_child[0]);
        
        // Перенаправляем stdout на запись в pipe
        dup2(pipe_from_child[1], STDOUT_FILENO);
        close(pipe_from_child[1]);
        
        // Запускаем дочернюю программу
        execve("./child", (char *[]){ "./child", NULL }, NULL);
        
        // Если execve вернулся, произошла ошибка
        perror("execve");
        exit(1);
    }
    
    // Родительский процесс
    close(pipe_to_child[0]);    // закрываем чтение из pipe_to_child
    close(pipe_from_child[1]);  // закрываем запись в pipe_from_child
    
    // Открываем файл
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);
        kill(pid, SIGTERM);
        wait(NULL);
        return 1;
    }
    
    printf("Родительский процесс: читаю файл '%s'\n", filename);
    
    char line[BUFFER_SIZE];
    int should_terminate = 0;
    
    // Читаем файл построчно
    while (fgets(line, BUFFER_SIZE, file) != NULL && !should_terminate) {
        int number;
        if (sscanf(line, "%d", &number) == 1) {
            printf("Родительский процесс: отправляю число %d дочернему процессу\n", number);
            
            // Отправляем число дочернему процессу
            if (write(pipe_to_child[1], &number, sizeof(int)) == -1) {
                perror("write");
                break;
            }
            
            // Читаем ответ от дочернего процесса
            int response;
            ssize_t bytes_read = read(pipe_from_child[0], &response, sizeof(int));
            
            if (bytes_read == sizeof(int)) {
                if (response == -1) {
                    // Дочерний процесс сигнализирует о завершении
                    printf("Родительский процесс: получен сигнал завершения от дочернего процесса\n");
                    should_terminate = 1;
                } else if (response > 0) {
                    // Дочерний процесс вывел составное число
                    printf("Родительский процесс: получено составное число %d от дочернего процесса\n", response);
                }
            } else if (bytes_read == 0) {
                // Дочерний процесс закрыл pipe
                printf("Родительский процесс: дочерний процесс завершился\n");
                break;
            }
        }
    }
    
    // Закрываем файл и pipe'ы
    fclose(file);
    close(pipe_to_child[1]);
    close(pipe_from_child[0]);
    
    // Ожидаем завершения дочернего процесса
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        printf("Родительский процесс: дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
    }
    
    printf("Родительский процесс: завершение работы\n");
    return 0;
}

