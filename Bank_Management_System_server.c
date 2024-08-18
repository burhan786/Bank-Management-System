// Server side C program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080

typedef struct record {
    char name[25];
    int account;
    char phone[15];
    char address[25];
    char email[35];
    char citiz[20];
    double blnc;
    char id[20]; 
    char password[15];
} rec;

FILE* get_login_file(char *mode){
    //This function opens the login_info.txt file in the specified mode
    char login_file[256] = "/home/burhanuddin/Desktop/C Programming/Database/login_info.txt";
    FILE* fd = fopen(login_file, mode);
    if (fd == NULL){
        perror("Error opening the file");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int get_acc_num(){
    //This function returns the number of existing accounts in the login_info.txt file
    FILE* fd = get_login_file("r");
    char usernames[20];
    int acc_num = 0;
    while(fgets(usernames, 20, fd)){
        acc_num++;
    }
    printf("Number of accounts are: %d\n", acc_num);
    fclose(fd);
    return acc_num;
}

void write_username(char* username){
    //This function writes the username to the login_info.txt file
    FILE* fd = get_login_file("a+");
    fprintf(fd, "%s\n", username);
    fclose(fd);
}

int check_username_exists(char* username){
    //This function checks if the username exists in the login_info.txt file
    FILE* fd = get_login_file("a+");
    char usernames[20];
    while(fgets(usernames, 20, fd)){
        for (int i=0;usernames[i]!='\0';i++){
            if (usernames[i]=='\n')
            {
                usernames[i] = '\0';
            }
            
        }
        if (strcmp(usernames,username)==0){
            fclose(fd);
            return 1;
        }
    }
    fclose(fd);
    return 0;
}

void write_record(FILE* fd, rec* buffer_rec){
    //This function writes the record to the file
    fprintf(fd, "Name: %s\n", buffer_rec->name);
    fprintf(fd, "Account Number: %d\n", buffer_rec->account);
    fprintf(fd, "Phone Number: %s\n", buffer_rec->phone);
    fprintf(fd, "Address: %s\n", buffer_rec->address);
    fprintf(fd, "Email: %s\n", buffer_rec->email);
    fprintf(fd, "Citizenship: %s\n", buffer_rec->citiz);
    fprintf(fd, "Balance: %lf\n", buffer_rec->blnc);
    fclose(fd);
}

void close_connection_due_to_error(int client_socket){
    close(client_socket);
    exit(EXIT_FAILURE);
}


int login(int client_socket, char** main_username){
    char buffer[1024];
    ssize_t valread;
    valread = recv(client_socket, &buffer, 1024, 0);
    if (valread < 0){
        perror("Error reading the file");
        close_connection_due_to_error(client_socket);
    }
    buffer[valread] = '\0';
    //Split the buffer into username and password
    char* username = strtok(buffer, " ");
    char* password = strtok(NULL, " ");
    printf("Username is: %s\n", username);
    printf("Password is: %s\n", password);
    char usernames[100];
    int username_exists = check_username_exists(username);
    if (username_exists == 0){
        int res = 0;
        send(client_socket, &res, sizeof(int), 0);
        return 0;
    }
    char password_file[256] = "/home/burhanuddin/Desktop/C Programming/Database/";
    strcat(password_file, username);
    strcat(password_file, "/password.txt");
    printf("Password file is: %s\n", password_file);
    FILE* fd = fopen(password_file, "r");
    if (fd == NULL){
        perror("Error opening the Password file");
        close_connection_due_to_error(client_socket);
    }
    char pass[15];
    fgets(pass, 15, fd);
    for (int i=0;pass[i]!='\0';i++){
        if (pass[i]=='\n')
        {
            pass[i] = '\0';
        }
    }
    if (strcmp(pass, password) == 0){
        int res = 1;
        send(client_socket, &res, sizeof(int), 0);
        fclose(fd);
        *main_username = (char *)malloc(sizeof(char)*20);
        strcpy(*main_username, username);
        return 1;
    }
    fclose(fd);
    return 0;
}

int create_account(int client_socket, char** main_username){
    rec* buffer_rec;
    buffer_rec = (rec *)malloc(sizeof(rec));
    ssize_t valread;
    printf("waiting for the data to come\n");
    valread = recv(client_socket, buffer_rec, sizeof(rec), 0);
    if (valread < 0){
        perror("Error reading the file");
        close_connection_due_to_error(client_socket);
    }
    int user_exists = check_username_exists(buffer_rec->id);
    if (user_exists == 1){
        int res = 0;
        send(client_socket, &res, sizeof(int), 0);
        return 0;
    }
    int acc_num = get_acc_num()+1;
    write_username(buffer_rec->id);

    //create a subdirectory with the directory name as the username
    char filename[256];
    strcpy(filename, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(filename, buffer_rec->id);
    strcat(filename, "/");
    printf("User created in the database as: %s\n", filename);
    if (mkdir(filename, 0777) == -1){
        perror("Error creating the directory");
        close_connection_due_to_error(client_socket);
    }
    strcat(filename, "password");
    strcat(filename, ".txt");
    filename[strlen(filename)] = '\0';
    printf("Password is saved in: %s\n", filename);
    FILE* fd1 = fopen(filename, "w");
    if (fd1 == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    fprintf(fd1, "%s\n", buffer_rec->password);
    fclose(fd1);
    // Write the account details to the file
    char filename1[500];
    strcpy(filename1, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(filename1, buffer_rec->id);
    strcat(filename1, "/account_details.txt");
    filename1[strlen(filename1)] = '\0';
    printf("Account Details are stored in: %s\n", filename1);
    FILE* fd2 = fopen(filename1, "w");
    if (fd2 == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    buffer_rec->account = acc_num;
    write_record(fd2, buffer_rec);
    int res = 1;
    send(client_socket, &res, sizeof(int), 0);
    *main_username = (char *)malloc(sizeof(char)*20);
    strcpy(*main_username, buffer_rec->id);

    char mapping_file[256];
    strcpy(mapping_file, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(mapping_file, "mapping.txt");
    FILE* fd3 = fopen(mapping_file, "a+");
    if (fd3 == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    fprintf(fd3, "%d:%s\n", acc_num, buffer_rec->id);
    fclose(fd3);
    free(buffer_rec);
    return 1;
}

rec* parseFileToStructure(const char *filename, int client_socket) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        close_connection_due_to_error(client_socket);
    }

    char line[100];
    rec *acc = malloc(sizeof(rec));
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name: %[^\n]", acc->name);
        } else if (strncmp(line, "Account Number:", 15) == 0) {
            sscanf(line, "Account Number: %d", &acc->account);
        } else if (strncmp(line, "Phone Number:", 13) == 0) {
            sscanf(line, "Phone Number: %[^\n]", acc->phone);
        } else if (strncmp(line, "Address:", 8) == 0) {
            sscanf(line, "Address: %[^\n]", acc->address);
        } else if (strncmp(line, "Email:", 6) == 0) {
            sscanf(line, "Email: %[^\n]", acc->email);
        } else if (strncmp(line, "Citizenship:", 12) == 0) {
            sscanf(line, "Citizenship: %[^\n]", acc->citiz);
        } else if (strncmp(line, "Balance:", 8) == 0) {
            sscanf(line, "Balance: %lf", &acc->blnc);
        }
    }
    // free(acc);
    fclose(file);
    return acc;
}

double check_balance(char *username, int client_socket){
    //This function checks the balance of the user
    char filename[256];
    strcpy(filename, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(filename, username);
    strcat(filename, "/account_details.txt");
    printf("Checking the file %s for balance\n", filename);
    filename[strlen(filename)] = '\0';
    rec * acc_details = parseFileToStructure(filename, client_socket);
    double balance = acc_details->blnc;
    free(acc_details);
    return balance;
}

int deposit_money(double deposit, char *username, int client_socket){
    //This function deposits the amount to the user's account
    char filename[500];
    strcpy(filename, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(filename, username);
    strcat(filename, "/account_details.txt");
    filename[strlen(filename)] = '\0';
    printf("Opening file %s for depositing the money\n", filename);
    rec * acc_details = parseFileToStructure(filename, client_socket);
    acc_details->blnc += deposit;
    FILE* fd = fopen(filename, "w");
    if (fd == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    write_record(fd, acc_details);
    free(acc_details);
    printf("Money deposited successfully\n");
    return 1;
}

int withdraw_money(double amt, char *username, int client_socket){
    //This function withdraws the amount from the user's account
    char filename[500];
     int res;
    strcpy(filename, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(filename, username);
    strcat(filename, "/account_details.txt");
    filename[strlen(filename)] = '\0';
    rec * acc_details = parseFileToStructure(filename, client_socket);
    if (acc_details->blnc < amt){
        printf("Insufficient balance\n");
        res = 0;
        send(client_socket, &res, sizeof(int), 0);
        return 0;
    }
    acc_details->blnc -= amt;
    FILE* fd = fopen(filename, "w");
    if (fd == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    write_record(fd, acc_details);
    printf("Money withdrawn successfully\n");
    res = 1;
    send(client_socket, &res, sizeof(int), 0);
    free(acc_details);
    return 1;
}

char* fetch_username(int ac_n, int client_socket){
    //This function fetches the username from the account number
    char mapping_file[256];
    int flg = 0;
    strcpy(mapping_file, "/home/burhanuddin/Desktop/C Programming/Database/");
    strcat(mapping_file, "mapping.txt");
    FILE* fd = fopen(mapping_file, "r");
    if (fd == NULL){
        perror("Error opening the file");
        close_connection_due_to_error(client_socket);
    }
    char line[100];
    char* trans_username = (char *)malloc(sizeof(char)*20);
    while (fgets(line, sizeof(line), fd)){
        int acc_num;
        char user[20];
        sscanf(line, "%d:%s", &acc_num, user);
        if (acc_num == ac_n){
            printf("Received username is: %s\n", user);
            strcpy(trans_username, user);
            flg = 1;
            break;
        }
    }
    fclose(fd);
    if (flg == 0){
        return NULL;
    }
    return trans_username;
}

void perform_transaction(char* sendin_us, char* recv_usr, double amount, int client_socket){
    printf("Receiving User is: %s\n", recv_usr);
    printf("Sending User is: %s\n", sendin_us);
    if (recv_usr == NULL){
        int res = -1;
        send(client_socket, &res, sizeof(int), 0);
        return;
    }
    int flg = withdraw_money(amount, sendin_us, client_socket);
    if (flg == 0){
        return;
    }
    flg = deposit_money(amount, recv_usr, client_socket);
}

void operations(int op_val, int client_socket, char* username){ 
    if (op_val == 1){
        //Check balance
        printf("Checking the balance\n");
        double balance = check_balance(username, client_socket);
        send(client_socket, &balance, sizeof(double), 0);
    } 
    if (op_val == 2){
        //Deposit
        double depos;
        printf("Depositing the money\n");
        ssize_t valread = recv(client_socket, &depos, sizeof(double), 0);
        if (valread < 0)
        {
            perror("Error reading the file");
            close_connection_due_to_error(client_socket);
        }
        
        printf("Value to be deposited: %lf\n", depos);
        deposit_money(depos, username, client_socket);
    }
    if (op_val == 3){
        //Withdraw
        printf("Withdrawing the money\n");
        double withdraw;
        ssize_t valread = recv(client_socket, &withdraw, sizeof(double), 0);
        if (valread < 0)
        {
            perror("Error reading the file");
            close_connection_due_to_error(client_socket);

        }
        printf("Value to be withdrawn: %lf\n", withdraw);
        withdraw_money(withdraw, username, client_socket);
    }
    if (op_val == 4){
        printf("Mentioned amount has to be sent to the specified account number\n");
        int acc_num;
        double amount;
        ssize_t valread = recv(client_socket, &acc_num, sizeof(int), 0);
        printf("Account number to send money to: %d\n", acc_num);
        char * trans_usr_nm = fetch_username(acc_num, client_socket);
        if (trans_usr_nm == NULL){
            int res = -1;
            send(client_socket, &res, sizeof(int), 0);
            return;
        }
        ssize_t valread1 = recv(client_socket, &amount, sizeof(double), 0);
        printf("Amount to be sent: %lf\n", amount);
        perform_transaction(username, trans_usr_nm, amount, client_socket);
        free(trans_usr_nm);
    }
}
int main(int argc, char const* argv[])
{
    int server_fd, client_socket;
    char buffer[1024] = {0};
    int opt_val;
    rec* buffer_rec;
    buffer_rec = (rec *)malloc(sizeof(rec));
    ssize_t valread;
    struct sockaddr_in server_address;
    struct sockaddr_in connection_address;
    int opt = 1;
    socklen_t addrlen = sizeof(server_address);
    
    char* hello = "Hello from server";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);
 
    // Binding the socket to the specified address in the above
    // Three lines.
    if (bind(server_fd, (struct sockaddr*)&server_address,
             sizeof(server_address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((client_socket
         = accept(server_fd, (struct sockaddr*)&connection_address,
                  &addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
        }

    valread = recv(client_socket, &opt_val,
                sizeof(int), 0); 
    printf("Option received is:%d\n", opt_val);
    char* username=NULL;
    // username = NULL
    if ( opt_val == 1){
        int succ_flag = 0;
        while (succ_flag != 1)
        {
            /* code */
            succ_flag = login(client_socket, &username);
        }

    }if (opt_val == 2){
        printf("The option selected is to create a new account\n");
        int succ_flag = 0;
        while (succ_flag != 1)
        {
            /* code */
            succ_flag = create_account(client_socket, &username);
        }
    }
    
    while (1)
    {
        /* code */
        printf("User is authenticated waiting for the operation to be performed\n");
        printf("Username performing actions is: %s\n", username);
        int opval;
        valread = recv(client_socket, &opval, sizeof(int), 0);
        if (valread < 0){
            perror("Error reading the file");
            free(username);
            close_connection_due_to_error(client_socket);
        }
        if (opval == 5){
            printf("User logged out\n");
            free(username);
            break;
        }
        operations(opval, client_socket, username);
        printf(("-----------------------------------------\n"));

    }
 
    // closing the connected socket
    close(client_socket);
    // closing the listening socket
    close(server_fd);
    return 0;
}