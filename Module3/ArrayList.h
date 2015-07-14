#ifndef ARRAYLIST_H_INCLUDED
#define ARRAYLIST_H_INCLUDED

// Default capacity for new ArrayLists
#define DEFAULT_INIT_LEN 10

typedef struct ArrayList
{
	// We will store an array of strings (i.e., an array of char arrays)
	char **array;

	// Size of list (i.e., number of elements that have been added to the array)
	int size;

	// Length of the array (i.e., the array's current maximum capacity)
	int capacity;

} ArrayList;


// Functional Prototypes

ArrayList *createArrayList(int length);

ArrayList *destroyArrayList(ArrayList *list);

ArrayList *expandArrayList(ArrayList *list, int length);

ArrayList *trimArrayList(ArrayList *list);

char *put(ArrayList *list, char *str);

char *get(ArrayList *list, int index);

char *set(ArrayList *list, int index, char *str);

char *insertElement(ArrayList *list, int index, char *str);

int removeElement(ArrayList *list, int index);

int getSize(ArrayList *list);

void printArrayList(ArrayList *list);

#endif // ARRAYLIST_H_INCLUDED
