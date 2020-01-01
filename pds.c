#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "pds.h"
// #include"contact.h"
#include "bst.h"


struct PDS_RepoInfo repo_handle;

int bst_pre_order_write(struct BST_Node *node)
{
    if (node == NULL) 
          return PDS_SUCCESS;
    
    int result;
    struct PDS_NdxInfo *cont = node->data;
    if(cont->is_deleted==0)
        result =fwrite(node->data,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
    if(result==0||result==-1) 
        return PDS_NDX_SAVE_FAILED;
    bst_pre_order_write(node->left_child);
    bst_pre_order_write(node->right_child); 
}

int pds_load_ndx()
{
    //Opening Index file
    char ndxfilename[30];
    strcpy(ndxfilename,repo_handle.pds_name);
    strcat(ndxfilename,".ndx"); 
    repo_handle.pds_ndx_fp = fopen(ndxfilename,"rb+");
    if(repo_handle.pds_ndx_fp==NULL)
        repo_handle.pds_ndx_fp = fopen(ndxfilename,"wb+");
    fseek(repo_handle.pds_ndx_fp,0,SEEK_SET);

    //Building BST from the Index file
    struct PDS_NdxInfo *temp = calloc(sizeof(struct PDS_NdxInfo),1); //Hard limit for the number of nodes     
    int tempstatus = fread(temp,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
    if(tempstatus==EOF||tempstatus==0) 
    { 
        fclose(repo_handle.pds_ndx_fp); 
        return PDS_SUCCESS;
    }

    int bstatus = bst_add_node(&repo_handle.pds_bst,temp->key,temp);
    while (tempstatus!=EOF&&tempstatus!=0)
    {
        struct PDS_NdxInfo *temp2 = calloc(sizeof(struct PDS_NdxInfo),1);     
        tempstatus = fread(temp2, sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
        if(tempstatus==0) 
            break;  //When reading fails 
        int bstatus = bst_add_node(&repo_handle.pds_bst,temp2->key,temp2);
    }
    
    //Closing Index file
    fclose(repo_handle.pds_ndx_fp);

    // bst_print(repo_handle.pds_bst);
    return PDS_SUCCESS;
}

int pds_open(char *repo_name, int rec_size )
{
    //PDS_Creation
    // FILE *fp = fopen(strcat(repo_name,".dat"),"ab+");
    // if(fp == NULL) return PDS_FILE_ERROR;
    // fclose(fp);
    // return PDS_SUCCESS;

    //Error Handling - PDS_REPO_ALREADY_OPEN
    // if(repo_handle.pds_data_fp!=NULL) 
    //     return PDS_REPO_ALREADY_OPEN;
    
    //Opening data file     
    char datafilename[30];
    strcpy(datafilename,repo_name);
    strcat(datafilename,".dat"); 
    repo_handle.pds_data_fp = fopen(datafilename,"rb+");
    if(repo_handle.pds_data_fp==NULL) 
        repo_handle.pds_data_fp = fopen(datafilename,"wb+");
    fseek(repo_handle.pds_data_fp,0,SEEK_SET);
    
    //Updating repo_handle
    strcpy(repo_handle.pds_name,repo_name);
    repo_handle.repo_status=PDS_REPO_OPEN;
    //For Fixed Length Records
    repo_handle.rec_size = rec_size;
    // bst_destroy(repo_handle.pds_bst);
    repo_handle.pds_bst=NULL;

    // pds_load_ndx
    // Internal function used by pds_open to read index entries into BST
    return pds_load_ndx();
}


int put_rec_by_key( int key, void *rec )
{
    //Error Handling
    if(repo_handle.repo_status==PDS_REPO_CLOSED)
    {
        return PDS_REPO_NOT_OPEN;
    }

    //Seeking to EOF
    fseek(repo_handle.pds_data_fp,0,SEEK_END);

    //Creating Index entry with current location
    int location = ftell(repo_handle.pds_data_fp);
    struct PDS_NdxInfo *temp = calloc(sizeof(struct PDS_NdxInfo),1);
    temp->key = key;
    temp->offset = location;
    temp->is_deleted = 0;

    //Adding entry to BST
    int bststatus=bst_add_node(&repo_handle.pds_bst,key,temp);
    if(bststatus==1)
    {
        free(temp);
        return PDS_SUCCESS;  //Assuming that duplicate key addition is considered success and ignored
    }

    //Writing the key and the record
    if(fwrite(&temp->key,sizeof(int),1,repo_handle.pds_data_fp)==0)
        return PDS_ADD_FAILED;
    if(fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp)==0)
        return PDS_ADD_FAILED;  //Might need to do different error handling here

    return PDS_SUCCESS;   
}

int get_rec_by_ndx_key( int key, void *rec )
{
    //Error Handling
    if(repo_handle.repo_status==PDS_REPO_CLOSED)
    {
        return PDS_REPO_NOT_OPEN;
    }

    //Searching for record location using offset from BST
    struct BST_Node *location;
    location = bst_search(repo_handle.pds_bst,key);
    if(location==NULL) 
        return PDS_REC_NOT_FOUND; //PDS_REC_NOT_FOUND sends an error code 3, but the testcase.in expects a 1
    
    //Creating a container to read the index data
    struct PDS_NdxInfo *tempndx = calloc(sizeof(struct PDS_NdxInfo),1);
    tempndx = location->data;
    // tempndx->key = location->key;

    if(tempndx->is_deleted==1)
        return PDS_REC_NOT_FOUND;
    
    //Moving the file pointer to the desired location
    fseek(repo_handle.pds_data_fp,tempndx->offset,SEEK_SET);
    
    //Reading off the key to get to the record
    int rkey;
    fread(&rkey,sizeof(int),1,repo_handle.pds_data_fp);
    
    //Error Handling when keys don't match
        // if(rkey!=key) return PDS_REC_NOT_FOUND;

    //Reading the record
    fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
    
    return PDS_SUCCESS;
}

int pds_close()
{
    //Error Handling
    if(repo_handle.repo_status==PDS_REPO_CLOSED)
    {
        return PDS_SUCCESS;
    }

    //Opening Index file
    char datafilename[30];
    strcpy(datafilename,repo_handle.pds_name);
    strcat(datafilename,".ndx"); 
    repo_handle.pds_ndx_fp = fopen(datafilename,"wb");    

    //Writing BST to Index file
    struct BST_Node *roothandle = repo_handle.pds_bst;
    bst_pre_order_write(roothandle);

    //Updating repo_handle
    strcpy(repo_handle.pds_name,"");
    repo_handle.repo_status=PDS_REPO_CLOSED;
    repo_handle.rec_size=0;

    //Closing the Data file
    fclose(repo_handle.pds_data_fp);
    repo_handle.pds_data_fp=NULL;

    //Closing the Index file
    fclose(repo_handle.pds_ndx_fp);
    repo_handle.pds_ndx_fp=NULL;

    //Freeing the BST
    bst_destroy(repo_handle.pds_bst);
    return PDS_SUCCESS;
}

int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key), int *io_count)
{
    //Go to beginning of file
    fseek(repo_handle.pds_data_fp,0,SEEK_SET);
    
    //Performing a table scan
    int status=1;
    while(status)
    {
        int tempkey;
        status=fread(&tempkey,sizeof(int),1,repo_handle.pds_data_fp);
        status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
        *io_count = *io_count + 1; //Incrementing counter
        // if(deletionCheck(rec)==1)
        //     break;
        
        if((*matcher)(rec,key)==0)
        {
            struct BST_Node *location;
            location = bst_search(repo_handle.pds_bst,tempkey);
            if(location==NULL) 
                break; //PDS_REC_NOT_FOUND sends an error code 3, but the testcase.in expects a 1
    
            //Creating a container to read the index data
            struct PDS_NdxInfo *tempndx = calloc(sizeof(struct PDS_NdxInfo),1);
            tempndx = location->data;
            // tempndx->key = location->key;
            if(tempndx->is_deleted==1)
            {
                free(tempndx);
                break;
            }
            status=0;
        }
        else if((*matcher)(rec,key)>1)
        {
            return PDS_FILE_ERROR;
        }
    }
    if(status==1) 
        return PDS_REC_NOT_FOUND;
    return PDS_SUCCESS;
}

int deletionCheck(void *data)
{
    struct PDS_NdxInfo *cont = data;
    return cont->is_deleted;
}

int modify_rec_by_key( int key, void *rec )
{
    //Error Handling
    if(repo_handle.repo_status==PDS_REPO_CLOSED)
    {
        return PDS_REPO_NOT_OPEN;
    }

    //Searching for record location using offset from BST
    struct BST_Node *location;
    location = bst_search(repo_handle.pds_bst,key);
    if(location==NULL) 
        return PDS_REC_NOT_FOUND; //PDS_REC_NOT_FOUND sends an error code 3, but the testcase.in expects a 1
    
    if(deletionCheck(location->data))
        return PDS_MODIFY_FAILED;

    //Creating a container to read the index data
    struct PDS_NdxInfo *tempndx = location->data;
    // printf("%d %d %d\n", location->key, tempndx->offset, ftell(repo_handle.pds_data_fp));
    location->key = key;
    // tempndx->key = location->key;

    fseek(repo_handle.pds_data_fp,0,SEEK_END);
    // printf("%d %d %d\n", location->key, tempndx->offset, ftell(repo_handle.pds_data_fp));
    
    //Moving the file pointer to the desired location
    fseek(repo_handle.pds_data_fp,tempndx->offset,SEEK_SET);
    // printf("%d %d %ld\n", location->key, tempndx->offset, ftell(repo_handle.pds_data_fp));

    
    //Writing off the key to get to the record
    // int rkey;
    fwrite(&location->key,sizeof(int),1,repo_handle.pds_data_fp);
    
    //Error Handling when keys don't match
        // if(rkey!=key) return PDS_REC_NOT_FOUND;

    //Writing the record
    fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
    
    return PDS_SUCCESS;
}

int delete_rec_by_ndx_key(int key)
{
    //Error Handling
    if(repo_handle.repo_status==PDS_REPO_CLOSED)
    {
        return PDS_REPO_NOT_OPEN;
    }

    //Searching for record location using offset from BST
    struct BST_Node *location;
    location = bst_search(repo_handle.pds_bst,key);
    if(location==NULL) 
        return PDS_DELETE_FAILED; //PDS_REC_NOT_FOUND sends an error code 3, but the testcase.in expects a 1
    
    struct PDS_NdxInfo *tempndx = location->data;
    if(tempndx->is_deleted==1)
        return PDS_DELETE_FAILED;
    if(tempndx->is_deleted==0)
        tempndx->is_deleted=1;
    return PDS_SUCCESS;
}

//Printing BST contents
    // struct PDS_NdxInfo *temp2;
    // temp2 = repo_handle.pds_bst->data;
    // printf("%d lol", temp2->offset);
    // temp2 = repo_handle.pds_bst->right_child->data;
    // printf("%d %dlol", temp2->offset,temp2->key);
    // temp2 = repo_handle.pds_bst->right_child->right_child->data;
    // printf("%d %dlol", temp2->offset, temp2->key);