	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <time.h>
	#define MAX_USERS 100
	#define MAX_CARD_ID 20
	#define MAX_PIN 4
	#define LOG_FILE "transaction.log"
	#define MAX_FAILED_LOGIN_ATTEMPTS 3
	
	// ATM Parameters
	float t = 1000.0;
	float k = 500.0;
	float m = 300.0;
	float n = 100.0;
	int chk = 3;
	
	// User structure
	struct User {
	    char name[100];
	    int id;
	    char card_id[MAX_CARD_ID];
	    char pin[MAX_PIN];
	    float balance;
	    int failedLoginAttempts;
	    int cardBlocked;
	    time_t lastWithdrawalDate;
	    float dailyWithdrawalTotal;
	};
	
	void adminModule(struct User users[], int *numUsers);
	void userModule(struct User users[], int numUsers);
	void createUser(struct User users[], int *numUsers);
	void saveUserData(struct User users[], int numUsers);
	void loadUserData(struct User users[], int *numUsers);
	int authenticateUser(struct User users[], int numUsers, char *card_id, char *pin);
	void displayMenu(struct User *user);
	void withdraw(struct User *user);
	void checkBalance(struct User *user);
	void deposit(struct User *user);
	void logTransaction(struct User *user, char *transactionType, float amount);
	void atmModule(struct User users[], int numUsers);
	void displayTransactionLog();
	void initializeATMParameters();
	
	int main() {
	    int choice;
	    int numUsers = 0;
	    struct User users[MAX_USERS];
	
	    loadUserData(users, &numUsers);
	
	    while (1) {
	        printf("+---------------------------------------+\n");
	        printf("|           WELCOME TO ATM              |\n");
	        printf("+---------------------------------------+\n");
	        printf("|  1. BANK                              |\n");
	        printf("|  2. CUSTOMER                          |\n");
	        printf("|  3. ATM                               |\n");
	        printf("|  4. EXIT                              |\n");
	        printf("+---------------------------------------+\n");
	
	        scanf("%d", &choice);
	
	        switch (choice) {
	            case 1:
	                adminModule(users, &numUsers);
	                break;
	            case 2:
	                userModule(users, numUsers);
	                break;
	            case 3:
	                atmModule(users, numUsers);
	                break;
	            case 4:
	                saveUserData(users, numUsers);
	                exit(0);
	            default:
	                printf("Invalid choice! Please try again.\n");
	        }
	    }
	
	    return 0;
	}
	
	void adminModule(struct User users[], int *numUsers) {
	    char adminUsername[] = "canara";
	    char adminPassword[] = "canara@123";
	    char username[100];
	    char password[100];
	
	    printf("Enter Bank username: ");
	    scanf("%s", username);
	
	    printf("Enter Bank password: ");
	    scanf("%s", password);
	
	    if (strcmp(username, adminUsername) == 0 && strcmp(password, adminPassword) == 0) {
	        printf("Bank login successful.\n");
	        createUser(users, numUsers);
	    } else {
	        printf("Bank login failed. Incorrect username or password.\n");
	    }
	}
	
	void userModule(struct User users[], int numUsers) {
	    char card_id[MAX_CARD_ID];
	    char pin[MAX_PIN];
	    printf("Insert your card:\n");
	    printf("Enter Card ID: ");
	    scanf("%s", card_id);
	    printf("Enter PIN: ");
	    scanf("%s", pin);
	
	    int userId = authenticateUser(users, numUsers, card_id, pin);
	
	    if (userId != -1) {
	        struct User *currentUser = &users[userId];
	        if (currentUser->cardBlocked) {
	            printf("Card blocked! Too many failed login attempts.\n");
	        } else {
	            printf("Welcome, %s!\n", currentUser->name);
	            displayMenu(currentUser);
	        }
	    } else {
	    	chk--;
	    	if(chk > -1)
	    	{
	    		printf("Authentication failed. Invalid card ID or PIN. Remaining attempts %d\n",chk);
			}
			else
			{
				printf("Error!! Reinsert the card or Try again!!");
			}
	        
	    }
	}
	
	void createUser(struct User users[], int *numUsers) {
	    if (*numUsers < MAX_USERS) {
	        struct User newUser;
	        printf("ADD NEW USER:\n");
	        printf("Enter User Name: ");
	        scanf("%s", newUser.name);
	        newUser.id = *numUsers + 1;
	        printf("Enter Card ID: ");
	        scanf("%s", newUser.card_id);
	        printf("Enter PIN: ");
	        scanf("%s", newUser.pin);
	        newUser.balance = 0.0;
	        newUser.failedLoginAttempts = 0;
	        newUser.cardBlocked = 0;
	        newUser.lastWithdrawalDate = 0;
	        newUser.dailyWithdrawalTotal = 0;
	        users[*numUsers] = newUser;
	        (*numUsers)++;
	        printf("User account created successfully.\n");
	    } else {
	        printf("Maximum number of users reached.\n");
	    }
	}
	
	void saveUserData(struct User users[], int numUsers) {
	    FILE *file = fopen("users.txt", "w");
	    if (file == NULL) {
	        perror("Unable to open file");
	        exit(1);
	    }
	
	    for (int i = 0; i < numUsers; i++) {
	        fprintf(file, "Name: %s\n", users[i].name);
	        fprintf(file, "ID: %d\n", users[i].id);
	        fprintf(file, "Card ID: %s\n", users[i].card_id);
	        fprintf(file, "PIN: %s\n", users[i].pin);
	        fprintf(file, "Balance: %.2f\n", users[i].balance);
	        fprintf(file, "Failed Login Attempts: %d\n", users[i].failedLoginAttempts);
	        fprintf(file, "Card Blocked: %d\n", users[i].cardBlocked);
	        fprintf(file, "Last Withdrawal Date: %ld\n", users[i].lastWithdrawalDate);
	        fprintf(file, "Daily Withdrawal Total: %.2f\n", users[i].dailyWithdrawalTotal);
	    }
	
	    fclose(file);
	}
	
	void loadUserData(struct User users[], int *numUsers) {
	    FILE *file = fopen("users.txt", "r");
	    if (file != NULL) {
	        int i = 0;
	        char buffer[1000];
	
	        while (fgets(buffer, sizeof(buffer), file) != NULL) {
	            sscanf(buffer, "Name: %[^\n]", users[i].name);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "ID: %d", &users[i].id);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Card ID: %s", users[i].card_id);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "PIN: %s", users[i].pin);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Balance: %f", &users[i].balance);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Failed Login Attempts: %d", &users[i].failedLoginAttempts);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Card Blocked: %d", &users[i].cardBlocked);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Last Withdrawal Date: %ld", &users[i].lastWithdrawalDate);
	            fgets(buffer, sizeof(buffer), file);
	            sscanf(buffer, "Daily Withdrawal Total: %f", &users[i].dailyWithdrawalTotal);
	
	            i++;
	        }
	
	        *numUsers = i;
	        fclose(file);
	    }
	}
	
	int authenticateUser(struct User users[], int numUsers, char *card_id, char *pin) {
	    for (int i = 0; i < numUsers; i++) {
	        if (strcmp(users[i].card_id, card_id) == 0) {
	            if (strcmp(users[i].pin, pin) == 0 && !users[i].cardBlocked) {
	                users[i].failedLoginAttempts = 0;
	                return i;
	            } else {
	                users[i].failedLoginAttempts++;
	                if (users[i].failedLoginAttempts >= MAX_FAILED_LOGIN_ATTEMPTS) {
	                    users[i].cardBlocked = 1;
	                    printf("Card blocked! Too many failed login attempts.\n");  // Message indicating the card is blocked.
	                    return -1;  // Return immediately after the card is blocked.
	                }
	                return -1;
	            }
	        }
	    }
	    return -1;
	}
	
	
	void displayMenu(struct User *user) {
	    int choice;
	
	    while (1) {
	        printf("------------------\n");
	        printf("Select an option:\n");
	        printf("1. Withdraw\n");
	        printf("2. Check Balance\n");
	        printf("3. Deposit\n");
	        printf("4. Logout\n");
	        printf("------------------\n");
	
	        scanf("%d", &choice);
	
	        switch (choice) {
	            case 1:
	                withdraw(user);
	                break;
	            case 2:
	                checkBalance(user);
	                break;
	            case 3:
	                deposit(user);
	                break;
	            case 4:
	                printf("Logging out. Goodbye, %s!\n", user->name);
	                return;
	            default:
	                printf("Invalid choice! Please try again.\n");
	        }
	    }
	}
	
	void withdraw(struct User *user) {
	    float amount;
	    time_t now;
	    time(&now);
	
	    if (difftime(now, user->lastWithdrawalDate) >= 86400) {
	        user->dailyWithdrawalTotal = 0;
	        user->lastWithdrawalDate = now;
	    }
	
	    printf("Enter the amount to withdraw: ");
	    scanf("%f", &amount);
	
	    if (amount > 0 && amount <= user->balance) {
	        if (amount <= m) {
	            if (user->dailyWithdrawalTotal + amount <= k) {
	                if (amount <= t) {
	                    user->balance -= amount;
	                    t -= amount;
	                    user->dailyWithdrawalTotal += amount;
	                    printf("Withdrawal of %.2f successful.\n", amount);
	                    logTransaction(user, "Withdrawal", amount);
	                } else {
	                    printf("Withdrawal failed. Insufficient funds in the ATM.\n");
	                }
	            } else {
	                printf("Withdrawal failed. Exceeds maximum daily withdrawal limit.\n");
	            }
	        } else {
	            printf("Withdrawal failed. Exceeds maximum per-transaction limit.\n");
	        }
	    } else {
	        printf("Withdrawal failed. Insufficient balance or invalid amount.\n");
	    }
	}
	
	void checkBalance(struct User *user) {
	    printf("Account Balance: %.2f\n", user->balance);
	    logTransaction(user, "Balance Check", 0.0);
	}
	
	void deposit(struct User *user) {
	    float amount;
	
	    printf("Enter the amount to deposit: ");
	    scanf("%f", &amount);
	
	    if (amount > 0) {
	        user->balance += amount;
	        t += amount;
	        printf("Deposit of %.2f successful.\n", amount);
	        logTransaction(user, "Deposit", amount);
	    } else {
	        printf("Invalid deposit amount.\n");
	    }
	}
	
	void logTransaction(struct User *user, char *transactionType, float amount) {
	    FILE *logFile = fopen(LOG_FILE, "a");
	    if (logFile != NULL) {
	        time_t now;
	        struct tm *timestamp;
	        char datetime[20];
	        time(&now);
	        timestamp = localtime(&now);
	        strftime(datetime, sizeof(datetime), "%Y-%m-d %H:%M:%S", timestamp);
	        fprintf(logFile, "Time: %s, ID: %d, User: %s, Transaction Type: %s, Amount: %.2f\n", datetime, user->id, user->name, transactionType, amount);
	        fclose(logFile);
	    }
	}
	
	void atmModule(struct User users[], int numUsers) {
	    int choice;
	
	    while (1) {
	        printf("+---------------------------------------+\n");
	        printf("|             ATM MODULE                |\n");
	        printf("+---------------------------------------+\n");
	        printf("|  1. Check ATM Balance                |\n");
	        printf("|  2. Display Transaction Log          |\n");
	        printf("|  3. Initialize ATM Parameters        |\n");
	        printf("|  4. Exit                             |\n");
	        printf("+---------------------------------------+\n");
	
	        scanf("%d", &choice);
	
	        switch (choice) {
	            case 1:
	                printf("Remaining ATM Balance: %.2f\n", t);
	                break;
	            case 2:
	                displayTransactionLog();
	                break;
	            case 3:
	                initializeATMParameters();
	                break;
	            case 4:
	                return;
	            default:
	                printf("Invalid choice! Please try again.\n");
	        }
	    }
	}
	
	void initializeATMParameters() {
	    FILE *paramFile = fopen("atm_parameters.txt", "w");
	    if (paramFile == NULL) {
	        perror("Unable to open parameter file");
	        exit(1);
	    }
	
	    printf("Enter ATM Parameters:\n");
	    printf("Initial ATM Balance (t): ");
	    scanf("%f", &t);
	    fprintf(paramFile, "Initial ATM Balance (t): %.2f\n", t);
	
	    printf("Maximum Daily Withdrawal Limit (k): ");
	    scanf("%f", &k);
	    fprintf(paramFile, "Maximum Daily Withdrawal Limit (k): %.2f\n", k);
	
	    printf("Maximum Per-Transaction Limit (m): ");
	    scanf("%f", &m);
	    fprintf(paramFile, "Maximum Per-Transaction Limit (m): %.2f\n", m);
	
	    printf("Initial ATM Deposit Limit (n): ");
	    scanf("%f", &n);
	    fprintf(paramFile, "Initial ATM Deposit Limit (n): %.2f\n", n);
	
	    fclose(paramFile);
	}
	
	void displayTransactionLog() {
	    FILE *logFile = fopen(LOG_FILE, "r");
	    if (logFile != NULL) {
	        char buffer[256];
	        while (fgets(buffer, sizeof(buffer), logFile) != NULL) {
	            printf("%s", buffer);
	        }
	        fclose(logFile);
	    } else {
	        printf("Transaction log not found or empty.\n");
	    }
	}

