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
	return 0;
}
