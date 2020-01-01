#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"pds.h"
#include"customer.h"

int add_customer(struct GameCustomer *c)
{
    int status;

    status = put_rec_by_key(c->cust_id, c);

    if(status!=PDS_SUCCESS)
    {
        fprintf(stderr, "\nUnable to add Customer to the database");
        return CUSTOMER_FAILURE;
    }

    return status;
}

void print_customer(struct GameCustomer *c)
{
    printf("\nCustomer Details:\n");
    printf("Customer ID: %d\n", c->cust_id);
    printf("Customer Name: %s\n", c->cust_name);
    printf("Customer Phone: %s\n", c->cust_phone);
    printf("Account Number: %s\n", c->acct_number);
    // printf("Account Type: %s\n", c->acct_type);
    printf("Date of Creation: %s\n", c->date_of_cr);
    printf("Balance: %f\n", c->balance);
    printf("Last Transaction: %f\n", c->last_transc);
    // printf("Loan ID: %d\n", c->loan_id);
    // printf("Load Type: %s\n", c->loan_type);
    // printf("Debt Remaining: %f\n", c->debt);
    printf("Games List: %s\n", c->owned_games_list);
    printf("Hours Played: %f\n", c->hours_played);
    printf("Profile: %s\n", c->profile);
    printf("Friends List: %s\n", c->friends_list);
}

void print_customer_account(struct GameCustomer *c)
{
    printf("\nCustomer Details:\n");
    printf("Customer ID: %d\n", c->cust_id);
    printf("Customer Name: %s\n", c->cust_name);
    printf("Customer Phone: %s\n", c->cust_phone);
    printf("Account Number: %s\n", c->acct_number);
    printf("Date of Creation: %s\n", c->date_of_cr);
    printf("Balance: %f\n", c->balance);
    printf("Last Transaction: %f\n", c->last_transc);
    printf("Profile Name: %s\n", c->profile);
}

void print_customer_games_list(struct GameCustomer *c)
{
    printf("Games List: %s\n", c->owned_games_list);
}

void print_customer_friends_list(struct GameCustomer *c)
{
    printf("Friends List: %s\n", c->friends_list);
}

int search_customer(int cust_id, struct GameCustomer *c)
{
    return get_rec_by_ndx_key(cust_id, c);
}


int search_customer_by_profile_name(char *profile_name, struct GameCustomer *c, int *io_count)
{
    return get_rec_by_non_ndx_key(profile_name, c, &match_profile_name, io_count);
}

int store_customer(char *customer_data_file)
{
    FILE *cfptr;
    char contact_line[1000], token;
    struct GameCustomer c, dummy;

    cfptr = (FILE *)fopen(customer_data_file, "r");
    while(fgets(contact_line, sizeof(contact_line)-1,cfptr))
    {
        sscanf(contact_line, "%d%s%s%s%s%f%f%s%f%s%s", 
        &(c.cust_id), 
        c.cust_name, 
        c.cust_phone, 
        c.date_of_cr, 
        // c.acct_type, 
        c.acct_number, 
        &(c.balance), 
        &(c.last_transc), 
        // c.loan_type, 
        // &(c.loan_id), 
        // &(c.debt));
        c.owned_games_list,
        &(c.hours_played),
        c.profile,
        c.friends_list);

        add_customer(&c);
    }
}

int match_profile_name(void *rec, void *key)
{
    struct GameCustomer *c = rec;
    char *profile = key;
    if(c==NULL||profile==NULL)
        return 2;
    if(strcmp(c->profile,profile)==0)
    {
        return CUSTOMER_SUCCESS;
    }
    else 
    {
        return CUSTOMER_FAILURE;
    }
}