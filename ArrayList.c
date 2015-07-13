#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ArrayList.h"

//create an array list of size 10 if the length specified is not greater than 10
ArrayList *createArrayList(int length)
{
    int i; ArrayList *list;
    list = NULL;
    list = malloc(sizeof(ArrayList));
//testing if the chosen length is greater than the default length and then creating an array
//and space in memory for elements to be added to the array
    if(length>DEFAULT_INIT_LEN)
    {
        list->array = malloc(sizeof(char*)*(length));
        if(list->array!=NULL)
        {
            printf("-> Created new ArrayList of size %d.\n", length);
            //nulling out the junk contents of the array list
           for(i=0;i<length;i++)
            {
                list->array[i]=NULL;
            }
            //setting capacity and setting size of current elements to 0 because it's a new, empty array list
            list->capacity=length;
            list->size=0;
        }
    }
    else
    {
        list->array = malloc(sizeof(char*)*(DEFAULT_INIT_LEN));
        if(list->array!=NULL)
        {
            printf("-> Created new ArrayList of size %d.\n", DEFAULT_INIT_LEN);
            for(i=0;i<DEFAULT_INIT_LEN;i++)
            {
                list->array[i]=NULL;
            }
            list->capacity=DEFAULT_INIT_LEN;
            list->size=0;
        }
    }
    //printf("Create Array pointers: %p %p\n", list, list->array);
//testing to see if malloc failed or not and returning the address of the list array
    if(list->array!=NULL&&list!=NULL&&list)
    {
        return list;
    }else return NULL;
}

//free and null the elements in the array, the array itself, and then free the list
ArrayList *destroyArrayList(ArrayList *list)
{
    int i;
    if(list)
    {
        if(list!=NULL&&list)
        {
           for(i=0;i<list->size;i++)
            {
                free(list->array[i]);
                list->array[i]=NULL;
            }
            free(list->array);
            //list->array=NULL;
            free(list);
            //list=NULL;
            return NULL;
        }else return NULL;
    }else return NULL;
}

//place the passed in word into the next available space of the array
char *put(ArrayList *list, char *str)
{
    char buffer[32], *word=NULL;
    if(list&&str)
    {
        //printf("------ArrayList before put---------\n");
        //printf("List Size: %d\n", list->size);
        //printArrayList(list);


        if(getSize(list)>=list->capacity)
        {
            expandArrayList(list, (list->capacity*2+1));
        }


        //buffer the word and then create a perfect amount of memory to fit the word in
        strcpy(buffer, str);
        word=malloc(sizeof(char*)*(strlen(buffer)));
        strcpy(word, buffer);


        //set the address of the array at array->size index to the word and then strcpy the word into it
        list->array[getSize(list)]=word;
        list->size=list->size+1;
        //printf("---------ArrayList after put----------\n");
        //printf("List Size: %d\n", list->size);
        //printArrayList(list);
        //printf("\n\n\n");
        if(word!=NULL&&list!=NULL&&str!=NULL&&list)
        {
            return word;
        }else return NULL;
    }else return NULL;
}

//returns the current size of the list
int getSize(ArrayList *list)
{
    if(!list)
    {
        return -1;
    }else return list->size;
}

//expanding the list to an adequate size if it runs out of room
ArrayList *expandArrayList(ArrayList *list, int length)
{
    int i;
    //checking if the requested expansion length is smaller or = to the current
    //or if the list being passed through is null
    if(length<=list->capacity||!list)
    {
        return NULL;
    }
    else
    {
        //creating a temporary list to replace the old array with
        ArrayList *newList; newList=NULL;
        newList=malloc(sizeof(ArrayList));
        newList->array=malloc(sizeof(char*)*length);
        for(i=0;i<length;i++)
        {
            newList->array[i]=NULL;
        }
        for(i=0;i<getSize(list);i++)
        {
            //putting old list data into new list, nulling out old list data, then freeing old list data memory
            newList->array[i]=list->array[i];
            list->array[i]=NULL;
        }
        //setting old list array pointer to point to the new list array
            free(list->array);
            list->array=newList->array;
            list->capacity=length;
            free(newList);
        if(list->array!=NULL)
        {
            printf("-> Expanded ArrayList to size %d.\n", list->capacity);
            return list;
        }else return NULL;
    }
}

//printing the entire array list
void printArrayList(ArrayList *list)
{
    int i,temp;
    char* tempString;
    //if the list is not a null pointer and the first element of the array is used up, print the contents
    if(list!=NULL&&list&&list->array[0]!=NULL)
    {
        for(i=0;i<list->capacity;i++)
        {
            if(list->array[i]!=NULL)
            {
                char* tempString = list->array[i];
                temp = tempString[0];
                printf("%s\n", list->array[i]);
                printf("The ascii value is --> %d\n", temp);
            }
        }
    }
    else
    {
        printf("(empty list)\n");
    }
}

//returning the pointer to a word at a certain index if the list is not a null pointer and the index is valid
char *get(ArrayList *list, int index)
{
    if(list&&index>=0)
    {
        if(list->array[index]==NULL||index>list->capacity)
        {
            return NULL;
        }else return list->array[index];
    }else return NULL;
}

//removing an element at a certain index and replacing it with a copy of the passed in string
char *set(ArrayList *list, int index, char *str)
{
    if(list&&index>=0&&str)
    {
        char buffer[32], *word=NULL;
        if(get(list, index)!=NULL)
        {
            //using a buffer to give me the exact amount of memory i need for the word
            strcpy(buffer, str);
            word=malloc(sizeof(char*)*(strlen(buffer)+1));
            strcpy(word, buffer);
            list->array[index]=word;
        }else return NULL;
        if(word!=NULL&&str!=NULL&&list!=NULL&&index<list->capacity)
        {
            return word;
        }else return NULL;
    }else return NULL;
}

char *insertElement(ArrayList *list, int index, char *str)
{
    if(list&&index>=0&&str)
    {
        //declaring a buffer to temporarily hold my passed in string
        int i; char buffer[32], *word=NULL;
        //creating a temporary array and nulling out the array pointers
        ArrayList *temp=NULL;
        temp=malloc(sizeof(ArrayList));
        temp->array=malloc(sizeof(char*)*(getSize(list)+1));
        for(i=0;i<(getSize(list)+1);i++)
        {
            temp->array[i]=NULL;
        }
        //if the word at the index is not null and the index is smaller than the list's size
        if(get(list, index)!=NULL&&index<list->size)
        {
            //if the size of my list is greater than the capacity, expand it
            if(getSize(list)>=list->capacity)
            {
                expandArrayList(list, (list->capacity*2+1));
            }
            //fill the temp array with every word 1 space to the right of the old index
            for(i=index;i<list->size;i++)
            {
                temp->array[i+1]=list->array[i];
            }
            //place all the words in the temp array right back into the old array
            for(i=index;i<=list->size;i++)
            {
                list->array[i]=temp->array[i];
                temp->array[i]=NULL;
            }
            //replace the word at index with the new word
                strcpy(buffer, str);
                word=malloc(sizeof(char*)*(strlen(buffer)+1));
                strcpy(word, buffer);
                list->array[index]=word;
                //strcpy(list->array[index], word);
                list->size=list->size+1;
        }
        //if the index is greater than or = the current size, just simply put it in the array
        else if(index>=list->size)
        {
            put(list, str);
        }
        free(temp->array); free(temp);
        if(word!=NULL&&str!=NULL&&list!=NULL&&list)
        {
            return word;
        }else return NULL;
    }else return NULL;
}

//remove the element at the index by nulling it out and shift all the elements to the left
int removeElement(ArrayList *list, int index)
{
    if(list&&index>=0)
    {
        int i;
        if(index<list->size)
        {
            list->array[index]=NULL;
            for(i=index;i<getSize(list);i++)
            {
                list->array[i]=list->array[i+1];
            }
            list->size--;
        }
        else return 0;
    }else return 0;
}

//using a temporary array to transfer the items in my list to a list without excess capacity
ArrayList *trimArrayList(ArrayList *list)
{
    int i;
    if(list->size<list->capacity)
    {
        //creating yet another temporary array (i should've made a function for this)
            ArrayList *newList; newList=NULL;
            newList=malloc(sizeof(ArrayList));
            newList->array=malloc(sizeof(char*)*getSize(list));
            for(i=0;i<getSize(list);i++)
            {
                newList->array[i]=NULL;
            }
            for(i=0;i<getSize(list);i++)
            {
                //putting old list data into new list, nulling out old list data, then freeing old list data memory
                newList->array[i]=list->array[i];
                list->array[i]=NULL;
            }
            //setting old list array pointer to point to the new list array
            free(list->array);
            list->array=newList->array;
            list->capacity=getSize(list);
            free(newList);
            if(list->array!=NULL&&getSize(list)==list->capacity)
            {
                printf("-> Trimmed ArrayList to size %d.\n", list->capacity);
                return list;
            }else return NULL;
        }
}
