#include "stdafx.h"
#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int found=0;

int binarySearch(int arr[], int l, int r, int key,int process_id)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == key) {
            found=1;
           // printf("Found at %d by process %d\n",arr[mid],process_id);
            return mid;
        }
        if (arr[mid] > key)
            return binarySearch(arr, l, mid - 1, key,process_id);
        return binarySearch(arr, mid + 1, r, key,process_id);
    }
    return -1;
}


int main(int argc, char* argv[]){
	int process_id, no_of_processes, n=12,key, found_index;
	int start_index, end_index, size_of_data;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
   	MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);
	
	size_of_data=n/no_of_processes;
	start_index=0;
	key=97;
	end_index=size_of_data-1;

	if(process_id==0){
		int array[]={2,4,7,11,13,23,34,57,61,73,85,97};
		if(no_of_processes==1){
      			found_index = binarySearch(array,start_index,end_index,key,process_id);
		}
		else{
			int size1=n-(n/2);
			int size2=(n/2)-size_of_data;
			int start=n/2;

			MPI_Send(&start,1, MPI_INT, 1, 1, MPI_COMM_WORLD);
			MPI_Send(&size1,1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(array+n/2, size1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			
			if(no_of_processes>2){
				MPI_Send(&size_of_data, 1, MPI_INT, no_of_processes-1, 1, MPI_COMM_WORLD);
				MPI_Send(&size2, 1, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD);
				MPI_Send(array+size_of_data, size2, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD);
			}
			start_index=0;
			end_index =size_of_data -1;
			
			found_index = binarySearch(array,start_index,end_index,key,process_id);
			
			int x,y=-1;
			MPI_Recv(&x, 1, MPI_INT, 1, 10,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(no_of_processes>2)
				MPI_Recv(&y, 1, MPI_INT, no_of_processes-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(x>=0)
				found_index=x;
			if(y>=0)
				found_index=y;
		}
		if(found_index >0)
        {
            printf("key found at = %d", found_index);
        }
        else
        {
            printf("key not found");
        }
	}
	else if(process_id==no_of_processes/2){
		start_index=0;
		int start;
		MPI_Recv(&start, 1, MPI_INT, (no_of_processes/2 )-1, 1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int size;
		MPI_Recv(&size, 1, MPI_INT, (no_of_processes/2 )-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		end_index=start_index+size-1;

		int *array=(int *)malloc(sizeof(int)*size);
		MPI_Recv(array, size, MPI_INT, (no_of_processes/2 )-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		found_index = binarySearch(array,start_index,end_index,key,process_id);
		if(found_index>(-1))
			found_index=found_index+start;
		if(process_id==1)
			MPI_Send(&found_index, 1 , MPI_INT, 0, 10, MPI_COMM_WORLD);
		else
			MPI_Send(&found_index, 1 , MPI_INT, (no_of_processes/2 )-1, 0, MPI_COMM_WORLD);
	}
	else if(process_id == no_of_processes/2 +1){
		start_index=0;
		
		int start;
		MPI_Recv(&start, 1, MPI_INT,(((no_of_processes/2) +2)%no_of_processes), 1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int size;
		MPI_Recv(&size, 1, MPI_INT,(((no_of_processes/2) +2)%no_of_processes), 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		end_index=start_index+size-1;

		int *array=(int *)malloc(sizeof(int)*size);
		MPI_Recv(array, size, MPI_INT, (((no_of_processes/2) +2)%no_of_processes), 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		found_index = binarySearch(array,start_index,end_index,key,process_id);
		if(found_index>(-1))
			found_index=found_index+start;
		MPI_Send(&found_index, 1 , MPI_INT, (((no_of_processes/2) +2)%no_of_processes), 0, MPI_COMM_WORLD);
	}
	else if((process_id>0) &&  process_id< no_of_processes/2){
		start_index=0;
		end_index=start_index+size_of_data-1;
		int start;
		MPI_Recv(&start,1, MPI_INT, process_id-1, 1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int size;
		MPI_Recv(&size,1, MPI_INT, process_id-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int *array=(int *)malloc(sizeof(int)*size);
		MPI_Recv(array, size, MPI_INT, process_id-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int start1=size_of_data;
		MPI_Send(&start1,1 , MPI_INT, process_id+1, 1, MPI_COMM_WORLD);

		int size1=(size-size_of_data);
		MPI_Send(&size1,1 , MPI_INT, process_id+1, 0, MPI_COMM_WORLD);

		MPI_Send(array+size_of_data,size1 , MPI_INT, process_id+1, 0, MPI_COMM_WORLD);
		
		found_index = binarySearch(array,start_index,end_index,key,process_id);
		
		int x;
		MPI_Recv(&x,1, MPI_INT, process_id+1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(x>=0)
			found_index=x;
		if(found_index>(-1))
			found_index=found_index+start;
		if(process_id==1)
			MPI_Send(&found_index, 1 , MPI_INT, 0, 10, MPI_COMM_WORLD);
		else
			MPI_Send(&found_index, 1 , MPI_INT, process_id-1 , 0, MPI_COMM_WORLD);
	}
	else{
		start_index=0;
		end_index=start_index+size_of_data-1;
		int start;
		MPI_Recv(&start,1, MPI_INT, ((process_id+1)%(no_of_processes)) , 1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int size;
		MPI_Recv(&size,1, MPI_INT, ((process_id+1)%(no_of_processes)) , 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int *array=(int *)malloc(sizeof(int)*size);
		MPI_Recv(array, size, MPI_INT, ((process_id+1)%(no_of_processes)) , 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int size1=(size-size_of_data);
		MPI_Send(&size1,1 , MPI_INT, process_id-1, 0, MPI_COMM_WORLD);

		int start1=size_of_data;
		MPI_Send(&start1,1 , MPI_INT, process_id-1, 1, MPI_COMM_WORLD);

		MPI_Send(array+size_of_data,size1 , MPI_INT, process_id-1, 0, MPI_COMM_WORLD);
		
		found_index = binarySearch(array,start_index,end_index,key,process_id);
		int x;
		MPI_Recv(&x,1, MPI_INT, process_id-1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if(x>=0)
			found_index=x;
		if(found_index>=0)
			found_index=found_index+start;
		
		MPI_Send(&found_index, 1 , MPI_INT,((process_id+1)%(no_of_processes)) , 0, MPI_COMM_WORLD);
	}
		
	MPI_Finalize();
	return 0;
}
