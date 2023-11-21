#include<stdio.h>
#include<iostream>
#include<cmath>
#include <float.h>
#include <stdlib.h>
#include <time.h>
#include<sys/time.h>
#include<omp.h>
#include<bits/stdc++.h>
#include<conio.h>
using namespace std;
//global variables
int flag1=0,flag2=0; //initially flags are false for both options 
long t_parallel , t_sequential;
//global structure to read file
typedef struct
{
	char name[50];
	float lat;       //latitude
	float lon;     //longitude
	float alt;       //altitude
	float x;       //calculated x position
	float y;     //calculated y position
	float z;       //calculated z position
}graph;
//defining function to get time of day
long gettime()
{
	struct timeval start;
	gettimeofday(&start,NULL);
	return ((start.tv_sec*1000000)+start.tv_usec); // usec are milli-seconds and 10^6 is multiplied with sec to convert whole term into milli seconds
}
void parallel()
{
	graph **paths=new graph*[8];//assuming we have eight paths
	float sum[8]={0};
	int size[8];
	float min= FLT_MAX;
	int minindex=-1;
	FILE *fin; //file handle 
	fin = fopen("project1-1.csv","r");
	for(int i=0;i<8;i++)
	{
		fscanf(fin, "%d", &size[i]);
		paths[i]=new graph[size[i]];
		for(int k=0;k<size[i];k++)
		{
			fscanf(fin, " %50[^,], %f, %f, %f" , paths[i][k].name, &paths[i][k].lat, &paths[i][k].lon, &paths[i][k].alt );//reading values from file
		}		
	}
	//start time 
	long start = gettime();
	int j,i;
	omp_set_nested(1);
	printf("Local results of values x, y, and z for each thread: \n\n");
	#pragma omp parallel for private(j,i) schedule(dynamic,2)
	for(i=0;i<8;i++)
	{
		#pragma omp parallel for private(j) schedule(dynamic)
		for(j=0;j<size[i];j++)
		{
			long start1 = gettime();
			#pragma omp parallel sections num_threads(3)
			{
				#pragma omp section
					paths[i][j].x=paths[i][j].alt*cos(paths[i][j].lat)*sin(paths[i][j].lon);
				#pragma omp section
					paths[i][j].y=paths[i][j].alt*sin(paths[i][j].lat);
				#pragma omp section
					paths[i][j].z=paths[i][j].alt*cos(paths[i][j].lat)*cos(paths[i][j].lon);//calculating x,y and z values to find distance
			}
			long end1 = gettime();
			printf("thread ID: %d, Time: %f, node:%d x: %f, y: %f, z: %f\n",omp_get_thread_num(),end1-start1,j,paths[i][j].x,paths[i][j].y,paths[i][j].z);
		}                                                                                                     
	
	}printf("\nThe local sums for each thread are: \n\n");
	#pragma omp parallel for private(i,j) schedule(dynamic,2)
	for(i=0;i<8;i++)
	{
		long start1= gettime();
		#pragma parallel for private(j) schedule(dynamic)
		for(j=0;j<size[i]-1;j++)
		{
			sum[i]+=sqrt(pow((paths[i][j+1].x-paths[i][j].x),2)+pow((paths[i][j+1].y-paths[i][j].y),2)+pow((paths[i][j+1].z-paths[i][j].z),2));//calculating distance for each path
		}
		long end1 = gettime();
		printf("thread ID: %d, time: %f, local sum: %f\n",omp_get_thread_num(),end1-start1,sum[i]);
	}
	printf("\n");
	#pragma omp parallel for schedule(dynamic,2) private(i)
	for(i=0;i<8;i++)
	{
		long start1 = gettime();
		#pragma omp critical
		{
			if(sum[i]<min)
			{
				min=sum[i];
				minindex=i;
				printf("minimum distance up till now: %f\n",sum[i]);
			}
		}
		long end1 = gettime();
		printf("thread ID: %d, time taken: %f\n",omp_get_thread_num(),end1-start1);
	}
	printf("\nThe minimum path is: ");
	for(int i=0;i<size[minindex];i++)
		cout<<paths[minindex][i].name<<" ";//showing minimum path
	printf("\nwith distance: %f\n",min);//showing minimum distance that is related to path
	//storing end time
	long end = gettime();
	//time difference = time taken by program 
	long t = end - start ;
	//storing time in global var
	t_parallel = t;
    printf("Processing took %ld Milli-Seconds to execute \n", t);
}
void sequential()
{
	graph **paths=new graph*[8];//assuming we have eight paths
	float sum[8]={0};
	int size[8];
	FILE *fin; //file handle 
	fin = fopen("project1-1.csv","r");
	for(int i=0;i<8;i++)
	{
		fscanf(fin, "%d", &size[i]);
		paths[i]=new graph[size[i]];
		for(int k=0;k<size[i];k++)
		{
			fscanf(fin, " %50[^,], %f, %f, %f" , paths[i][k].name, &paths[i][k].lat, &paths[i][k].lon, &paths[i][k].alt );//reading values from file
		}	
	}
	//start time 
	long start = gettime();
	printf("Results of values x, y, and z for each path: \n\n");
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<size[i];j++)
		{
			paths[i][j].x=paths[i][j].alt*cos(paths[i][j].lat)*sin(paths[i][j].lon);
			paths[i][j].y=paths[i][j].alt*sin(paths[i][j].lat);
			paths[i][j].z=paths[i][j].alt*cos(paths[i][j].lat)*cos(paths[i][j].lon);//calculating x,y and z values to find distance
			printf("Path number %d node number %d: x=%f y=%f z=%f",i,j,paths[i][j].x,paths[i][j].y,paths[i][j].z);
			printf("\n");
		}
	}
	printf("\n");
	printf("The sums for each path are: \n\n");
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<size[i]-1;j++)
		{
			sum[i]+=sqrt(pow((paths[i][j+1].x-paths[i][j].x),2)+pow((paths[i][j+1].y-paths[i][j].y),2)+pow((paths[i][j+1].z-paths[i][j].z),2));//calculating distance for each path
			printf("Path %d: %f",i,sum[i]);
			printf("\n");
		}
	}
	printf("\n");
	float min= FLT_MAX;
	int minindex=-1;
	for(int i=0;i<8;i++)
	{
		if(sum[i]<min)
		{
			min=sum[i];
			minindex=i;
			printf("minimum distance up till now: %f",sum[i]);
			printf("\n");
		}
	}//find minimum distance, and path number
	printf("\n");
	printf("The minimum path is: ");
	for(int i=0;i<size[minindex];i++)
		cout<<paths[minindex][i].name<<" ";//showing minimum path
	printf("\n");
	printf("with distance: %f",min);//showing minimum distance that is related to path
	printf("\n");
    //end time
	long end = gettime();
	
	//time difference = time taken by program 
    long t = end - start ;
    //storing time in global var
    t_sequential = t;
	printf("Processing took %ld Milli-Seconds to execute", t);
	printf("\n");
}
void compare_time()
{
	printf("\n\n\n\n\n\n\n\n");
	printf("\t\t\t\t\tSequential Time = %ld\n",t_sequential);
		printf("\t\t\t\t\tParallel Time = %ld\n",t_parallel);
	if(t_sequential < t_parallel)
	{
		printf("\t\t\t\t\tSequential Code is efficient\n");
	}
	else{
		printf("\t\t\t\t\tParallel Code is effiecient\n");
	}
}
//menu function
void menu()
{
	system("cls");
	char choice;
	printf("\n\n\n\n\n\n\n\n");
	printf("\t\t\t\t\t1 - Parallel Execution \n");
	printf("\t\t\t\t\t2 - Serial Execution\n");
	//if(flag1==1 && flag2==1)   //This option will only work when User has run both serial And parallel codes
	//{
		printf("\t\t\t\t\t3 - Compare Time Results\n");
	//}
	printf("\t\t\t\t\te - Exit Program .\n");
	printf("\t\t\t\t\t\t\t");
	scanf("%c",&choice);
	if(choice == '1')   //serial code
	{
		system("cls");
		flag1=1;
		parallel();
	}
	else if(choice == '2')   //Parallel code
	{
		system("cls");
		flag2=1;
		sequential();
	}
	else if(choice == '3')  //compare time results
	{
		system("cls");
		compare_time();
	}
	else if(choice == 'e')
	{
		exit(1);
	}
	else
	{
		printf("\n\t\t\t\t\tWrong Choice :");
	}
	printf("\n\t\t\t\t\tPress key");
	getch();
	menu();
	
}
//Main 
int main()
{
	//project intro
	printf("\n\n\n\n\n\n\n\n\t\t\t\t\t");
	printf("Welcome To Shortest Path Finder\n");
	printf("\t\t\t\t\tLoading Menu...\n");	
	//add loading here
	printf("\n\t\t\t\t\tPress key");
	getch();
	//calling menu function
	menu();
}
