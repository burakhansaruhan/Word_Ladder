#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 30
#define MAX_STACK_SIZE 100
#define MAX_QUEUE_SIZE 10000

char **dictionary;

// Stack veri yapisi
typedef struct {
    char items[MAX_STACK_SIZE][MAX_WORD_LENGTH];
    int top;
} Stack;

// Queue veri yapisi
typedef struct {
    Stack *stacks;
    int front, rear;
} Queue;

// fonksiyon prototipleri
void initializeStack(Stack *s);
int isStackEmpty(Stack *s);
void push(Stack *s, char *word);
char* pop(Stack *s);
void initializeQueue(Queue *q);
int isQueueEmpty(Queue *q);
int isQueueFull(Queue *q);
void enqueue(Queue *q, Stack *s);
Stack dequeue(Queue *q);
int isEndWord(Stack *s, const char *endWord);
int isOneLetterDifferent(const char *word1, const char *word2);
int isWordUsedInAnyStack(Queue *q, const char *word);
void addToWordArray(char ***wordArray, int *arraySize, const char *word);
int readWordsFromFile(const char *filename, int targetLength, char ***wordArray, int *arraySize);
void processQueue(Queue *q, char *startWord, char *endWord, int dictionarySize, char **dictionary);

void initializeStack(Stack *s) {
    s->top = -1;
}

int isStackEmpty(Stack *s) {
    return s->top == -1;
}

void push(Stack *s, char *word) {
    if (s->top == MAX_STACK_SIZE - 1) {
        printf("Stack dolu. Push yapilamadi.\n");
        return;
    }
    strcpy(s->items[++(s->top)], word);
}

char* pop(Stack *s) {
    if (isStackEmpty(s)) {
        printf("Stack bos. Pop yapilamadi.\n");
        exit(1);
    }
    return s->items[(s->top)--];
}

void initializeQueue(Queue *q) {
    q->stacks = (Stack *)malloc(MAX_QUEUE_SIZE * sizeof(Stack));
    q->front = q->rear = -1;
}

int isQueueEmpty(Queue *q) {
    return (q->front == -1 && q->rear == -1);
}

int isQueueFull(Queue *q) {
    return (q->rear + 1) % MAX_QUEUE_SIZE == q->front;
}

void enqueue(Queue *q, Stack *s) {
    if (isQueueFull(q)) {
        printf("Queue dolu. Enqueue yapilamadi.\n");
        return;
    }
    if (isQueueEmpty(q))
        q->front = q->rear = 0;
    else
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->stacks[q->rear] = *s;
}

Stack dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue bos. Dequeue yapilamadi.\n");
        exit(1);
    }
    Stack item = q->stacks[q->front];
    if (q->front == q->rear)
        q->front = q->rear = -1;
    else
        q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    return item;
}

int isEndWord(Stack *s, const char *endWord) {		// hedef kelimenin bulunup bulunmadigimi kontrol eden fonksiyon
    return (strcmp(s->items[s->top], endWord) == 0);
}

int isOneLetterDifferent(const char *word1, const char *word2) {		// tek harf farkini kontrol eden fonksiyon
    int i;
    int differences = 0;
    int len1 = strlen(word1);

    for (i = 0; i < len1; i++) {
        if (word1[i] != word2[i]) {
            differences++;
            if (differences > 1)
                return 0;
        }
    }
    
    return (differences == 1);		// fark 1 ise true donecek
}

int isWordUsedInAnyStack(Queue *q, const char *word) {			// bir kelimenin herhangi bir stack içinde daha önce eklenip eklenmedigini kontrol eden fonksiyon
    int i, j;
    for (i = 0; i <= q->rear; i++) {
        Stack currentStack = q->stacks[i];
        for (j = 0; j <= currentStack.top; j++) {
            if (strcmp(currentStack.items[j], word) == 0)
                return 1; // kelime bulunduysa
        }
    }
    return 0; // kelime bulunmadisa
}

void addToWordArray(char ***wordArray, int *arraySize, const char *word) {	//   kelimeleri dinamik diziye ekleme fonksiyonu
    if (*wordArray == NULL) {
        *wordArray = (char **)malloc(sizeof(char *));
    } else {
        *wordArray = (char **)realloc(*wordArray, (*arraySize + 1) * sizeof(char *));
    }

    if (*wordArray == NULL) {
        printf("Bellek tahsisi basarisiz!");
        exit(1);
    }

    (*wordArray)[*arraySize] = strdup(word);	// kelimeye bellek ayirma
    (*arraySize)++;
}

int readWordsFromFile(const char *filename, int targetLength, char ***wordArray, int *arraySize) {		// dosyadan kelimeleri okuyup diziye ekleyen fonksiyon
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dosya acilamadi!");
        exit(1);
    }

    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%s", word) != EOF) {
        if (strlen(word) == targetLength) {
            addToWordArray(wordArray, arraySize, word);
        }
    }

    fclose(file);
    
    return (*arraySize);
}

void processQueue(Queue *q, char *startWord, char *endWord, int dictionarySize, char **dictionary) {	// que islemlerini yapan fonksiyon
    int i, j;

    Stack startStack;
    initializeStack(&startStack);	// baslangic kelimesiyle stack olusturna
    push(&startStack, startWord);	// bu kelimeyi push
    enqueue(q, &startStack);		// que ye ekleme

    while (!isQueueEmpty(q)) {		// que bos olmadigi surece devam 
        Stack currentStack = dequeue(q);

        if (isEndWord(&currentStack, endWord)) {		// en ustteki kelimeyle hedef kontrolu
            printf("\nEn kisa kelime merdiveni bulundu:\n\n");
            
            while (!isStackEmpty(&currentStack)) {	// stack bosalana kadar kelimeleri yazdirma
                printf("%s -> ", pop(&currentStack));
            }
            return;
        }

        char *currentWord = currentStack.items[currentStack.top];

        for (i = 0; i < dictionarySize; i++) {		// sozluk icinde gezme
    
            if (isOneLetterDifferent(currentWord, dictionary[i])) {		// bir harf farkli ise
                if (!isWordUsedInAnyStack(q, dictionary[i])) {	
                    Stack newStack;									// yeni kelimeyle yeni stack olusturma
                    initializeStack(&newStack);	

                    for (j = 0; j <= currentStack.top; j++) {
                        push(&newStack, currentStack.items[j]);
                    }
                    push(&newStack, dictionary[i]);		// sozlukteki yeni kelimeyi push
                    enqueue(q, &newStack);				// yeni stacki queue'ya ekleyin
                }
            }
        }
    }

    printf("Hedef kelimeye ulasilamadi.\n");
}

int main() {
	int i;
    int targetLength,startLength;
    char startWord[MAX_WORD_LENGTH], endWord[MAX_WORD_LENGTH];
    char **dictionary = NULL;
    const char *filename = "dictionary.txt";
    int arraySize = 0;
    printf("Kaynak kelimeyi girin: ");
    scanf("%s", endWord);
    printf("Hedef kelimeyi girin: ");
    scanf("%s", startWord);
	
    targetLength = strlen(endWord);
    startLength = strlen(startWord);
    
    if(targetLength==startLength){
    	
    int dictionarySize = readWordsFromFile(filename, targetLength, &dictionary, &arraySize);
	
	 int foundEndWord = 0;
        for (i = 0; i < dictionarySize; i++) {
            if (strcmp(dictionary[i], startWord) == 0) {
                foundEndWord = 1;
                break;
            }
        }

        if (!foundEndWord) {
            printf("Hedef kelime sözlükte bulunamadý!\n");
            return 1;
        }
        
    int foundStartWord = 0;
        for (i = 0; i < dictionarySize; i++) {
            if (strcmp(dictionary[i], endWord) == 0) {
                foundStartWord = 1;
                break;
            }
        }

        if (!foundStartWord) {
            printf("Baslangic kelime sözlükte bulunamadi!\n");
            return 1;
        }   
        
	dictionarySize--;
	
	printf("\n%d harfli kelime sayisi: %d\n",targetLength,dictionarySize);
	
    Queue q;
    initializeQueue(&q);

    processQueue(&q, startWord, endWord, dictionarySize,dictionary);

    for ( i = 0; i < dictionarySize; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
}
else {
	printf("\nKelimelerin uzunluklari farkli!!!");
}
    return 0;
}
