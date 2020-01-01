//COMPILE ALL .c files in gcc together, then run the executable
//Commands to Run:
//$ gcc *.c
//$ ./a.out


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include"pds.h"
#include"customer.h"

long rolling_counter; 
struct Game *store;
int number_of_games=2;

struct Game  //Basic Template of a Game, can be extended upon
{
    char game_name[30];
    float price;
};

void display_options()
{
    printf("Please choose one of the following options:\n");
    printf("Option [1] : Create Account\n");
    printf("Option [2] : View Account Details\n");
    printf("Option [3] : Modify Customer Name/Phone\n");
    printf("Option [4] : Deposit Money to Account\n");
    printf("Option [5] : Buy Game \n"); 
    printf("Option [6] : View Game Library\n");
    // printf("Option [7] : View Friends List\n");
    printf("Option [7] : Add friend\n");
    printf("Option [8] : View All Details\n");
    printf("Option [9] : Delete Account\n");
    printf("Option [0] : Exit\n");
    printf("Enter your Option here and press Enter: ");
}

void create_customer() 
{
    //User Provided Data
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    printf("\nEnter Customer Details Below \n");
    printf("Enter Customer Name: ");
    scanf("%s", c->cust_name);
    printf("Enter Customer Phone: ");
    scanf("%s", c->cust_phone);

    //Setting Date of Creation
    time_t t = time(NULL);
    struct tm tmo = *localtime(&t);
    strcpy(c->date_of_cr,"");
    char temp[20];
    strcpy(temp,"");
    sprintf(temp, "%d", tmo.tm_mday);
    strcat(c->date_of_cr,temp);
    strcat(c->date_of_cr,"/");
    sprintf(temp, "%d", tmo.tm_mon+1);
    strcat(c->date_of_cr,temp);
    strcat(c->date_of_cr,"/");
    sprintf(temp, "%d", tmo.tm_year+1900);
    strcat(c->date_of_cr,temp);
    printf("Date of Creation has been set to : %s\n", c->date_of_cr);
    printf("Balance has been set to: 0\n");
    c->balance = 0.0;
    c->last_transc = 0.0;
    printf("Enter Profile Name: ");
    scanf("%s", c->profile);
    getchar();

    //System Generated Data
    c->cust_id = rolling_counter++;
    printf("Customer ID Generated : %d",c->cust_id);
    char temp2[20];
    strcpy(temp2,"");
    sprintf(temp2, "%d", c->cust_id);
    strcat(temp2, c->date_of_cr);
    strcpy(c->acct_number,temp2);
    strcpy(c->owned_games_list,"");
    c->hours_played = 0.0;
    strcpy(c->friends_list, "");

    //Adding to Database
    add_customer(c);

    //Freeing Local Memory
    free(c);

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void display_customer()
{
    //Options for searching
    printf("Search Customer through Customer ID or Account_number ?\n");
    printf("Option [1] : Customer ID\n");
    printf("Option [2] : Profile Name\n");
    char inp[2];
    scanf("%s",inp);
    getchar();

    //Searching by Customer ID
    if(inp[0]=='1')
    {
        printf("Enter Customer ID: ");
        int cust_id;
        scanf("%d",&cust_id);
        getchar();
        struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
        if(search_customer(cust_id, c)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        }
        print_customer_account(c);
        free(c);
    }
    else if(inp[0]=='2')  //Searching by Profile Name
    {
        printf("Enter Profile Name: ");
        char profile[20];
        scanf("%s",profile);
        getchar();
        struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
        int temp;
        if(search_customer_by_profile_name(profile,c,&temp)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        }
        print_customer_account(c);
        free(c);
    }
    else
    {
        printf("Invalid Option\n");
    }

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void modify_customer_details()
{
    //Acquiring Details
    printf("Enter Customer ID: ");
    int cust_id;
    scanf("%d",&cust_id);
    getchar();
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    if(search_customer(cust_id, c)!=0)
    {
        printf("Customer Not Found\n");
        free(c);
        return;
    }
    printf("Enter Updated Customer Name: ");
    scanf("%s", c->cust_name);
    printf("Enter Updated Customer Phone: ");
    scanf("%s", c->cust_phone);
    printf("Enter Updated Profile Name: ");
    scanf("%s", c->profile);
    getchar();

    //Modifying in Database
    modify_rec_by_key(cust_id,c);

    free(c);
    
    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void deposit()
{
    //Acquiring Details
    printf("Enter Customer ID: ");
    int cust_id;
    scanf("%d",&cust_id);
    getchar();
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    if(search_customer(cust_id, c)!=0)
    {
        printf("Customer Not Found\n");
        free(c);
        return;
    }
    printf("Current Balance: %f\n", c->balance);
    printf("Do you wish to add money?\n");
    printf("Option [1] : YES\n");
    printf("Option [2] : NO\n");
    printf("Enter desired option : ");
    char inp[2];
    scanf("%s",inp);
    getchar();
    if(inp[0]=='1') //Adding money to balance
    {
        printf("Enter the amount of money to be added : ");
        float adding;
        scanf("%f", &adding);
        getchar();
        c->balance+=adding;
        c->last_transc = adding;
        modify_rec_by_key(cust_id,c);
    }
    //Freeing Local Memory
    free(c);

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void display_game_library()
{
    printf("Game Catalog:\n");
    for(int i=0;i<number_of_games;i++)
    {
        printf("Game %d :\n", i+1);
        printf("Name of Game : %s\n", store[i].game_name);
        printf("Price : %f\n", store[i].price);
    }
    printf("\n");
}

void buy_game()
{
    //Showing Game Library
    display_game_library();

    printf("Enter Game Number to buy : ");
    int buy;
    scanf("%d", &buy);
    if(buy>number_of_games||buy<=0) 
    {
        printf("Invalid Option \n");
        return;
    }

    printf("Enter Customer ID: ");
    int cust_id;
    scanf("%d",&cust_id);
    getchar();
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    if(search_customer(cust_id, c)!=0)
    {
        printf("Customer Not Found\n");
        free(c);
        return;
    }
    printf("Current Balance before purchase: %f\n", c->balance);
    
    //Balance Check
    if(c->balance<store[buy-1].price)
    {
        printf("Do not have enough balance to buy game \n\n");
        return;
    }

    //Reconfirming Purchase
    char option;
    printf("Do you wish to confirm purchase?\n");
    printf("Option [1] : YES\n");
    printf("Option [2] : NO\n");
    scanf("%c",&option);
    getchar();
    if(option!='1')
    {
        printf("Purchase has been aborted\n");
        free(c);
        return;
    }

    //Adding Game to Account
    c->balance-=store[buy-1].price;
    c->last_transc = -store[buy-1].price;
    strcat(c->owned_games_list,store[buy-1].game_name);
    strcat(c->owned_games_list," ");

    //Modifying Data in Database
    modify_rec_by_key(c->cust_id,c);

    printf("Current Balance after purchase: %f\n", c->balance);
    //Freeing Local Memory
    free(c);

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void add_friend()
{
    printf("Enter Your Customer ID : ");
    int cust_id;
    scanf("%d",&cust_id);
    getchar();
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    if(search_customer(cust_id, c)!=0)
    {
        printf("Customer Not Found\n");
        free(c);
        return;
    }

    printf("Choose Option to Select Customer to Friend\n");
    printf("Option [1] : Customer ID\n");
    printf("Option [2] : Profile Name\n");
    printf("Enter desired option : ");
    char inp[2];
    scanf("%s",inp);
    getchar();
    if(inp[0]=='1') //Search through Customer ID
    {
        printf("Enter Customer ID to Friend: ");
        int temp_id;
        scanf("%d", &temp_id);
        getchar();

        //Adding to friends list
        struct GameCustomer *d = calloc(1, sizeof(struct GameCustomer));
        if(search_customer(temp_id, d)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        } //Assuming such a friend exists as customer in Database
        
        strcat(c->friends_list,d->profile);
        strcat(d->friends_list,c->profile);
        printf("c %s %d\n", c->friends_list, c->cust_id);
        printf("d %s %d\n", d->friends_list, d->cust_id);

        //Changing in Database
        modify_rec_by_key(c->cust_id,c);
        modify_rec_by_key(d->cust_id,d);

        free(c);
        free(d);
    }
    else if(inp[0]=='2')
    {
        printf("Enter Profile Name to Friend: ");
        char profile[20];
        scanf("%s",profile);
        getchar();
        struct GameCustomer *d = calloc(1, sizeof(struct GameCustomer));
        int temp;
        if(search_customer_by_profile_name(profile,d,&temp)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        }
        
        strcat(c->friends_list,d->profile);
        strcat(d->friends_list,c->profile);

        //Changing in Database
        modify_rec_by_key(c->cust_id,c);
        modify_rec_by_key(d->cust_id,d);

        free(c);
        free(d);
    }
    else
    {
        printf("Invalid Option\n");
    }

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void view_all_details()
{
    //Options for searching
    printf("Search Customer through Customer ID or Account_number ?\n");
    printf("Option [1] : Customer ID\n");
    printf("Option [2] : Profile Name\n");
    char inp[3];
    scanf("%s",inp);
    getchar();

    //Searching by Customer ID
    if(inp[0]=='1')
    {
        printf("Enter Customer ID: ");
        int cust_id;
        scanf("%d",&cust_id);
        getchar();
        struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
        if(search_customer(cust_id, c)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        }
        print_customer(c);
        //Freeing Local Memory
        free(c);
    }
    else if(inp[0]=='2')  //Searching by Profile Name
    {
        printf("Enter Profile Name: ");
        char profile[20];
        scanf("%s",profile);
        getchar();
        struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
        int temp;
        if(search_customer_by_profile_name(profile,c,&temp)!=0)
        {
            printf("Customer Not Found\n");
            free(c);
            return;
        }
        print_customer(c);
        //Freeing Local Memory
        free(c);
    }
    else
    {
        printf("Invalid Option\n");
    }

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}


void delete_account()
{
    printf("Enter Customer ID: ");
    int cust_id;
    scanf("%d",&cust_id);
    getchar();
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    if(search_customer(cust_id, c)!=0)
    {
        printf("Customer Not Found\n");
        free(c);
        return;
    }

    delete_rec_by_ndx_key(c->cust_id);

    free(c);

    printf("\nSUCCESS");
    printf("\nPress Enter to Continue\n");
    getchar();
}

void closing()
{
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    //Adding to Database
    if(search_customer(0,c)==0)
    {
        c->balance=(float) rolling_counter;
        modify_rec_by_key(c->cust_id,c);
        free(c);
        return;
    }
    
    c->cust_id=0;
    c->balance = rolling_counter;
    c->last_transc = 0.0;
    
    strcpy(c->cust_name,"");
    strcpy(c->cust_phone, "");
    strcpy(c->date_of_cr,"22/9/2019");  
    strcpy(c->profile, "closing");
    char temp[20];
    strcpy(temp,"");
    sprintf(temp, "%d", c->cust_id);
    strcat(temp, c->date_of_cr);
    strcpy(c->acct_number,temp);
    strcpy(c->owned_games_list,"");
    c->hours_played = 0.0;
    strcpy(c->friends_list, "");

    c->balance=(float) rolling_counter;
    add_customer(c);
    free(c);
}

void opening()  //Both Closing and Opening could also be used to load games list form a different database
{
    struct GameCustomer *c = calloc(1, sizeof(struct GameCustomer));
    search_customer(0,c);
    rolling_counter = (int)c->balance;
    if(rolling_counter==0) rolling_counter++;
    free(c);
}

int main(int argc, char const *argv[])
{
    int status = pds_open("customer",sizeof(struct GameCustomer));
    if(status!=CUSTOMER_SUCCESS)
    {
        fprintf(stderr, "Failed to open database\n");
        exit(1);
    }

    int exit_state=0;

    //Game Store Creation - 2 games available
    store = calloc(2,sizeof(struct Game)); 
    strcpy(store[0].game_name,"Game 1");
    strcpy(store[1].game_name,"Game 2"); 
    store[0].price = 25.5;
    store[1].price = 60.0;

    printf("\nWelcome Customer!\n");
    printf("This is Ram Store, a platform to buy games and create friends!\n\n");

    opening();
    char input[2];
    while(!exit_state)
    {
        display_options();
        scanf("%s", input);
        while(input[0]<48||input[0]>57)
        {
            printf("\nInvalid Option, please try again: ");
            scanf("%s", input);
        }

        getchar();
        int option = input[0] - 48;
        printf("\n");
        switch (option)
        {
        case 1: 
            create_customer();
            break;

        case 2:
            display_customer();
            break;
        
        case 3:
            modify_customer_details();
            break;
        
        case 4:
            deposit();
            break;
        
        case 5:
            buy_game();
            break;

        case 6:
            display_game_library();
            printf("\nSUCCESS");
            printf("\nPress Enter to Continue\n");
            getchar();
            break;

        case 7:
            add_friend();
            break;
        
        case 8:
            view_all_details();
            break;

        case 9:
            delete_account();
            break;

        case 0:
            closing();
            pds_close();
            exit_state=1;
        default:
            break;
        }
        // system("clear");
    }
    
    return 0;
}
