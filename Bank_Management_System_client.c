#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 8080

void gotoxy(int x, int y)
{
  printf("\033[%d;%df", y, x);
  fflush(stdout);
}

int get_client_socket() {
    /*
        This function returns the client socket which is needed to 
        communicate with the banking server.
        :Params: None
        :Returns: int descriptor containing the client socket
    */

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    return client_fd;
}
void admin();
void login(); 
void menu();
void create_account();

int m, r;


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


int main() {
    printf(" WELCOME TO TBC BANKING SYSTEM ");
    printf("\n Select the option from the menu below\n");
    int client_fd = get_client_socket();
    admin(client_fd);
    return 0;
}


void login(int client_fd) {
    char login_info[36], id[20], password[15];
    char create_acc;
    int user_exists;
    printf("Enter The Username : ");
    scanf(" %[^\n]", id);
    printf("Enter The Password : ");
    scanf(" %[^\n]", password);
    
    // send the username and password to the server by concatinating them
    strcpy(login_info, id);
    strcat(login_info, " ");
    strcat(login_info, password);
    
    send(client_fd, login_info, strlen(login_info), 0);
    printf("Sent the login info to the server\n");
    recv(client_fd, &user_exists, sizeof(user_exists), 0);
    
    if (user_exists == 0) {
        printf("User does not exist Try Again!!\n");
        login(client_fd);
    } else if (user_exists == -1) {
        printf("Server Error\n");
        close(client_fd);
    }else{
        printf("Login Successful\n");
        menu(client_fd);
    }
}

void create_account(int client_fd) {
    rec new_account;
    /*Take the details of complete record structure as input
    from the user*/
    int succ_flag;
    printf("Enter The Name : ");
    scanf(" %[^\n]", new_account.name);
    new_account.account = 0; // account number will be generated by the server
    printf("Enter The Phone Number : ");
    scanf(" %[^\n]", new_account.phone);
    printf("Enter The Address : ");
    scanf(" %[^\n]", new_account.address);
    printf("Enter The Email : ");
    scanf(" %[^\n]", new_account.email);
    printf("Enter The Citizenship : ");
    scanf(" %[^\n]", new_account.citiz);
    printf("Enter The Balance : ");
    scanf("%lf", &new_account.blnc);
    printf("Enter The ID : ");
    scanf(" %[^\n]", new_account.id);
    printf("Enter The Password : ");
    scanf(" %[^\n]", new_account.password);
    send(client_fd, &new_account, sizeof(new_account), 0);
    ssize_t val_read = recv(client_fd, &succ_flag, sizeof(succ_flag), 0);
    if (val_read < 0) {
        perror("Error reading the file");
        exit(EXIT_FAILURE);
    }
    if (succ_flag == 0) {
        //Assuming that the account creation failed because id already exists
        printf("Account creation failed\n");
        create_account(client_fd);
    }else if (succ_flag == -1)
    {
        printf("Server Error\n");
        close(client_fd);
    }else
    {
        printf("Account created successfully\n");
        menu(client_fd);
    }
}

void admin(int client_fd) {
    int choice;

    printf("1. Existing User\n");
    printf("2. New User\n");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        send(client_fd, &choice, sizeof(choice),0);
        login( client_fd);
        break;
    case 2:
        send(client_fd, &choice, sizeof(choice),0);
        create_account(client_fd);
        break;
    default:
        printf("Invalid choice\n");
        admin(client_fd);
    }
}

void check_balance(int m, int client_fd) {
    // send the account number to the server
    double balance;
    send(client_fd, &m, sizeof(m), 0);
    printf("Sent the amount to the server\n");
    recv(client_fd, &balance, sizeof(balance), 0);
    printf("The balance in the account is : %lf\n", balance);
    menu(client_fd);
}

void deposit(int m, int client_fd) {
    // send the account number and the amount to the server
    double amount;
    send(client_fd, &m, sizeof(m), 0);
    printf("Enter the amount to deposit : \n");
    scanf("%lf", &amount);
    send(client_fd, &amount, sizeof(amount), 0);
    printf("Sent the amount to the server\n");
    menu(client_fd);
}

void withdrawl(int m, int client_fd) {
    // send the account number and the amount to the server
    double amount;
    int res;
    send(client_fd, &m, sizeof(m), 0);
    printf("Enter the amount to withdraw : \n");
    scanf("%lf", &amount);
    send(client_fd, &amount, sizeof(amount), 0);
    printf("Sent the amount to the server\n");
    ssize_t val_read = recv(client_fd, &res, sizeof(r), 0);
    if (val_read < 0) {
        perror("Error reading the file");
        exit(EXIT_FAILURE);
    }
    if (res == 0) {
        printf("Insufficient balance, Enter a lower amount or deposit money\n");
    } 
    
    menu(client_fd);
}

void transaction(int m, int client_fd) {
    // send the money to a specified account number
    double amount;
    int account;
    int res;
    send(client_fd, &m, sizeof(m), 0);
    printf("Enter the account number to send money to : \n");
    scanf("%d", &account);
    send(client_fd, &account, sizeof(account), 0);
    printf("Enter the amount to send : \n");
    scanf("%lf", &amount);
    send(client_fd, &amount, sizeof(amount), 0);
    printf("Sent the amount to the server\n");
    ssize_t val_read = recv(client_fd, &res, sizeof(r), 0);
    if (val_read < 0) {
        perror("Error reading the file");
        exit(EXIT_FAILURE);
    }
    //res == 0: Insufficient balance
    //res == -1: Receiving Account does not exist
    if (res == 0) {
        printf("Insufficient balance, Enter a lower amount or deposit money\n");
    } else if (res == -1) {
        printf("Receiving Account does not exist\n");
    }
    menu(client_fd);
}

void menu(int client_fd) {
    int choice;
    printf("1. Check Balance\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transaction\n");
    printf("5. Exit\n");
    printf("Enter your choice : ");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
        printf("check balance\n");
        check_balance(choice, client_fd);
        break;
    case 2:
        printf("deposit\n");
        deposit(choice, client_fd);
        break;
    case 3:
        printf("withdraw\n");
        withdrawl(choice, client_fd);
        break;
    case 4:
        printf("Sending Money to Another account\n");
        transaction(choice, client_fd);
        break;
    case 5:
        printf("Exiting the system\n");
        send(client_fd, &choice, sizeof(choice), 0);
        close(client_fd);
        exit(0);
    default:
        printf("Invalid choice\n");
        menu(client_fd);
    }
}