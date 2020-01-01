#ifndef CUSTOMER_H
#define CUSTOMER_H

#define CUSTOMER_SUCCESS 0
#define CUSTOMER_FAILURE 1

// This is a record of a Customer on a video game purchasing digital platform

//Note: A GameCustomer is tied to one account and can have only one outstanding loan
struct GameCustomer{
    int cust_id;
    char cust_name[30];
    char cust_phone[15];
    char date_of_cr[10];
    // char acct_type[20];
    char acct_number[20];
    float balance;
    float last_transc;
    // char loan_type[20];
    // int loan_id;
    // float debt;
    char owned_games_list[50];
    float hours_played;
    char profile[20];
    char friends_list[50];
};

extern struct PDS_RepoInfo *repoHandle;

int add_customer(struct GameCustomer *c);

void print_customer(struct GameCustomer *c);

void print_customer_account(struct GameCustomer *C);

void print_customer_games_list(struct GameCustomer *c);

void print_customer_friends_list(struct GameCustomer *c);

int search_customer(int cust_id, struct GameCustomer *c);

int search_customer_by_profile_name(char *profile_name, struct GameCustomer *c, int *io_count);

int store_customer(char *customer_data_file);

int match_profile_name(void *rec, void *key);

#endif