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
			#pragma omp parallel sections num_threads(3)
			{
				#pragma omp section
					paths[i][j].x=paths[i][j].alt*cos(paths[i][j].lat)*sin(paths[i][j].lon);
				#pragma omp section
					paths[i][j].y=paths[i][j].alt*sin(paths[i][j].lat);
				#pragma omp section
					paths[i][j].z=paths[i][j].alt*cos(paths[i][j].lat)*cos(paths[i][j].lon);//calculating x,y and z values to find distance
			}
			printf("thread ID: %d, node:%d x: %f, y: %f, z: %f\n",i,j,paths[i][j].x,paths[i][j].y,paths[i][j].z);
		}                                                                                                     
	
	}printf("\nThe local sums for each thread are: \n\n");
	#pragma omp parallel for private(i,j) schedule(dynamic,2)
	for(i=0;i<8;i++)
	{
		#pragma parallel for private(j) schedule(dynamic)
		for(j=0;j<size[i]-1;j++)
		{
			sum[i]+=sqrt(pow((paths[i][j+1].x-paths[i][j].x),2)+pow((paths[i][j+1].y-paths[i][j].y),2)+pow((paths[i][j+1].z-paths[i][j].z),2));//calculating distance for each path
		}
		printf("thread ID: %d, local sum: %f\n",i,sum[i]);
	}
	printf("\n");
	#pragma omp parallel for schedule(dynamic,2) private(i)
	for(i=0;i<8;i++)
	{
		#pragma omp critical
		{
			if(sum[omp_get_thread_num()]<min)
			{
				min=sum[omp_get_thread_num()];
				minindex=omp_get_thread_num();
				printf("minimum distance up till now: %f\n",sum[omp_get_thread_num()]);
			}
		}
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
