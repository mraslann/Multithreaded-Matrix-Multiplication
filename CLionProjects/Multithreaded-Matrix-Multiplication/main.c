#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
// identifying the global variables
int **matrixA;
int **matrixB;
int ** matrixResult;
int c1,r1,c2,r2;

//struct for each element
typedef struct {
    int i;
    int j;
}Data;

//functions used
void readInput(char* url);
void* getCell(void* args);
void* getRow(void* args);
void writeToOutput(const char*url,double time);

void main(void)
{
    readInput("testcases.txt");
    pthread_t threadsForRows[r1];
    pthread_t threadsForcells[r1][c2];
    int arg[c2][2];
    int arg2[r1];
    double time;
    clock_t start;
    clock_t end;
    int choice;
    while(1)
    {
        printf("\n**Please Enter 1 for Thread per Cell 2 for Thread per row, 3 for exit**\n>>");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
            {
                int i,j;
                start = clock();
                for( i=0;i<r1;i++)
                {
                    for( j=0;j<c2;j++)
                    {
                        Data data = {i,j};
                        arg[j][0] = i;
                        arg[j][1]=j;
                        pthread_create(&threadsForcells[i][j],NULL,getCell,&arg[j]);
                    }
                }
                for(i=0;i<r1;i++)
                    for(j=0;j<c2;j++)
                        pthread_join(threadsForcells[i][j],NULL);
                end = clock();
                time =(double)( end-start)/CLOCKS_PER_SEC;
                writeToOutput("output.txt",time);
                break;
            }
            case 2:
            {
                start = clock();
                for(int i=0;i<r1;i++)
                {
                    arg2[i]=i;
                    pthread_create(&threadsForRows[i],NULL,getRow,&arg2[i]);
                }
                for(int i=0;i<r1;i++)
                {
                    pthread_join(&threadsForRows[i],NULL);
                }
                end = clock();
                time =(double) (end-start)/CLOCKS_PER_SEC;;
                writeToOutput("output.txt",time);
                break;
            }
            case 3:
                return;
        };
    }
}
//reading the 2 matrices that will be multiplied from an external file
void readInput(char* url)
{
    FILE *input = fopen(url,"r");
    fscanf(input,"%d %d",&r1,&c1);
    int i,j;
//Dynamically initialize the 2 Matrices;
    matrixA = (int**)malloc(sizeof(int*)*r1);
    for( i=0;i<r1;i++)
        matrixA[i] = (int*)malloc(c1*sizeof(int));
    for(i=0;i<r1;i++)
    {
        for(j=0;j<c1;j++)
            fscanf(input,"%d",&matrixA[i][j]);
    }
    fscanf(input,"%d %d",&r2,&c2);
    matrixB = (int**)malloc(sizeof(int*)*r2);
    for(int i=0;i<r2;i++)
        matrixB[i] = (int*)malloc(c2*sizeof(int));
    for(i=0;i<r2;i++)
    {
        for(j=0;j<c2;j++)
            fscanf(input,"%d",&matrixB[i][j]);

    }
    printf("r1=%d , c1= %d \nr2= %d , c2= %d\n",r1,c1,r2,c2);
//For the Result Matrix
    matrixResult= (int**) malloc(sizeof(int*) * c1);
    for(int i=0;i<c1;i++)
        matrixResult[i] = (int*)malloc(sizeof(int)*r2);
    fclose(input);
}

//writing the output matrix to another external file
void writeToOutput(const char*url,double time)
{
    FILE *output = fopen(url,"a");
    for(int i=0;i<r1;i++)
    {
        for(int j=0;j<c2;j++)
        {
            fprintf(output,"%d ",matrixResult[i][j]);
        }
        fprintf(output,"\n");
    }
    fprintf(output, "Time Elasped: %lf seconds\n\n",time);
    fclose(output);
}

//function for help in multiplication by using a thread for each element
void* getCell(void* args)
{
    int *arguments = (int*)args;
    int r = (int)arguments[0];
    int c = (int)arguments[1];
    matrixResult[r][c]=0;
    for(int i=0;i<c1;i++)
    {
        matrixResult[r][c]+=matrixA[r][i] * matrixB[i][c];
    }
    pthread_exit(NULL);
}

//function for help in multiplication by using a thread for each row
void* getRow(void* args)
{
    int *arguments = (int*)args;
    int r= (int)arguments[0];
    for(int k =0;k<c2;k++)
    {
        matrixResult[r][k]=0;
        for(int i=0;i<c1;i++)
        {
            matrixResult[r][k]+=matrixA[r][i] * matrixB[i][k];
        }
    }
    pthread_exit(NULL);
}

/*We can observe from the results that creating a thread for each row is more efficient than creating one for each element, as the time needed to create a thread for each element is more, thus making it more time consuming than the actual sorting. Creating a thread for each row is a good middle ground, where the time needed to create the threads isn't as much, but still making it more efficient than if sorted without using any threads at all.*/
