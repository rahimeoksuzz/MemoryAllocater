#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

Block *split(Block *b, size_t size) {
	int temp_size = b->info.size;
	b->info.size = size;
	Block *bsplit = ((void*)b + b->info.size + sizeof(Block));
	bsplit->info.size = temp_size - size - sizeof(Block);
	return bsplit;
}

void *mymalloc(size_t size) {
	static int firstcall = 1;   /*executes only once*/ //Eğer ilk defa çağrılıyorsa
	size = size - (size % 16) + 16;  //size  16nın katı olacak şekilde yukarı yuvarlama yapılmalı
	if (firstcall) {
		firstcall = 0;
		heap_start = sbrk(sizeof(Block) * 2 + 1024); //sbrk kullanılarak 1024 byte heap alanı genişletilmeli
		if (heap_start == NULL) {
			perror(" HATA!!! sbrk(heap_start)");
			exit(1);
		}

		heap_start->info.size = 1024;
		heap_start->info.isfree = 1;
		heap_start->next = NULL;
		// free alan iki bloğa split edilerek, birisi free liste eklenmeli
		// diğerinin (size kadar alanın)  başlangıç adresi return edilmeli
		if (strategy == bestFit) {
			Block * free = free_list;
			while ((free->next != NULL) && (free->info.size < size)) {

				free = (void *)free + sizeof(Block) + free->info.size;
			}
			if (free == NULL) {
				fprintf(stderr, "no available space\n");
				return NULL;
			}
			else if (size < free->info.size) {
				split(free, size);
			}
			free->info.isfree = 1;
		}
		else if (strategy == worstFit) {
			Block * free = free_list;
			while ((free->next != NULL) && (free->info.size > size)) {

				free = (void *)free + sizeof(Block) + free->info.size;
			}
			if (free == NULL) {
				fprintf(stderr, "no available space\n");
				return NULL;
			}
			else if (size > free->info.size) {
				split(free, size);
			}
			free->info.isfree = 1;
		}
		else if (strategy == firstFit) {
			Block * free = free_list;
			while ((free->next != NULL) && ((free->info.isfree == 0) || (free->info.size < size))) {

				free = (void *)free + sizeof(Block) + free->info.size;
			}
			if (free == NULL) {
				fprintf(stderr, "no available space\n");
				return NULL;
			}
			else if (size < free->info.size) {
				split(free, size);
			}
			free->info.isfree = 1;
			if (free_list == NULL) {
				fprintf(stderr, "free_list is empty\n");
				return NULL;
			}
			else {
				Block * temp_free_list = free_list;
				while (temp_free_list->next != free) {
					temp_free_list = temp_free_list->next;
				}
				temp_free_list->next = free->next;
			}
		}
	}
}
//p ile verilen alan free edilerek free liste eklenmelidir ve coalescing yapılmalı
void *myfree(void *p) {
	Block *b = (Block*)(p - sizeof(Block));/*block header*/
	b->info.isfree = 0;
	Block *next = p + b->info.size; /*next block*/
	Block *prev = (void *)b - sizeof(Block); /*previous block*/
	if (!next->info.isfree) {
		b->info.size += next->info.size + sizeof(Block);
	}
	if (!prev->info.isfree) {
		prev->info.size += b->info.size + sizeof(Block);
		b = prev;
	}
	static void(*freep)(void *);
	freep(p);
	return (void *)b;

}
//basit test kodu yazılmalı,mymalloc ve myfree çağırılmalı,
int test(int fitStrategy) {
	/*char* ptr = mymalloc(1000);
	myfree(ptr);
	printf("Memory allocated and free'd\n");
	return -1;
	*/
	void *ptr[20];
	ptr[15] = mymalloc(10);
	if (ptr[15] == NULL)
	{
		fprintf(stderr, "HATA!!! malloc bu boyut ıcın basarısız");
		exit(1);
	}
	myfree(ptr[15]);
	ptr[0] = mymalloc(10);
	if (ptr[0] == NULL)
	{
		fprintf(stderr, "HATA!!! malloc bu boyut ıcın basarısız");
		exit(1);
	}
	myfree(ptr[0]);
	return 0;
}
int main() {}
