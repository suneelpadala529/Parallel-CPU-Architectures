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
        if (arr[mid] == key){
            found=1;
           printf("Found %d by %d\n",arr[mid],process_id); 
            return mid;
        }
        if (arr[mid] > key) 
            return binarySearch(arr, l, mid - 1, key,process_id);
        return binarySearch(arr, mid + 1, r, key,process_id); 
    }
    return -1; 
}

void printArray(int A[], int size)
{
	int i;
	for (i=0; i < size; i++)
		printf("%d ", A[i]);
	printf("\n");
}

int check(int n){
	int flag=0;
	while(n>1){
		if(n%2==1){
			flag=1;
			break;
		}
		n=n/2;
	}
	return flag;
}
int log(int n){
	int count=0,num=n;
	if(n==1){
		return 1;
	}
	for(;n>1;){
		n=n/2;
		count++;
	}
	if(check(num)){
		count++;
	}
	return count;
}
int exp(int n){
	int num=1,i;
	for(i=1;i<=n;i++){
		num=num*2;
	}
	return num;
}

int main(int argc, char* argv[]){
	int process_id,no_of_processes, n =12,i,j,end,found_index=-1;
	int source,destination,my_d,temp;
	int start_index,end_index;
	int num=n;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);
	int size_of_data=n/no_of_processes;
	int key=2;
	if(no_of_processes==1 && process_id==0){
		int array[]={2,4,7,11,13,23,34,57,61,73,85,97};
		found_index=binarySearch(array,0,n-1,key,0);
		if(found_index>(-1)){
				printf("Found at index : %d",found_index);
			}
		else{
				printf("Key not found");
		}
	}
	else{
		if(process_id==0){
			end=n;
			num=n;
			int array[]={2,4,7,11,13,23,34,57,61,73,85,97};
			for(i=0;i<log(no_of_processes);i++){
				destination=process_id+exp(i);
				num=num/2;
				MPI_Send(&num,1,MPI_INT, destination,10,MPI_COMM_WORLD);
				MPI_Send(&end,1,MPI_INT, destination,0,MPI_COMM_WORLD);	
				MPI_Send(array+end-num, num, MPI_INT, destination,0, MPI_COMM_WORLD);
				
				end=end-num;
				num=end;
			}
			start_index=process_id*size_of_data;
			end_index=start_index+size_of_data-1;
			found_index=binarySearch(array,start_index,end_index , key,process_id);
			for(i=log(no_of_processes)-1;0<=i;i--){
				source=process_id+exp(i);
				int x;
				MPI_Recv(&x,1, MPI_INT,source,source,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(found_index<0)
					found_index=x;
			}
			if(found_index>(-1)){
				printf("Found at index : %d",found_index);
			}
			else{
				printf("Key not found");
			}
		}
		else if(process_id<(no_of_processes/2) && (process_id!=0)){
			my_d=1;
			temp=process_id;
			while(1<=temp/2){
				temp=temp/2;
				my_d++;
			}
			source=process_id-exp(my_d-1);
			int size,num;
			MPI_Recv(&size,1, MPI_INT,source,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			num=size;
			int end;
			MPI_Recv(&end,1, MPI_INT,source,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int *a=(int *)malloc(sizeof(int)*size);
			MPI_Recv(a,size, MPI_INT,source,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			end_index=end-1;
			for(j=my_d;j<log(no_of_processes);j++){
				destination=process_id+exp(j);
				size=size/2;
				MPI_Send(&size,1,MPI_INT, destination,10,MPI_COMM_WORLD);
				MPI_Send(&end,1,MPI_INT, destination,0,MPI_COMM_WORLD);	
				MPI_Send(a+end-size, size, MPI_INT, destination,0, MPI_COMM_WORLD);
				end_index=end_index-size;
				end=end-size;
				size=end;
				
			}
			start_index=0;
			found_index=binarySearch(a,start_index,end_index , key,process_id);
			for(j=log(no_of_processes)-1;my_d<=j;j--){
				source=process_id+exp(j);
				int x;
				MPI_Recv(&x,1, MPI_INT,source,source,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if(found_index<0)
					found_index=x;
			}
			destination=process_id-exp(my_d-1);
			if(found_index>=0){
				found_index=found_index+num;
			}
			MPI_Send(&found_index,1,MPI_INT, destination,process_id,MPI_COMM_WORLD);
		}
		else{
			my_d=1;
			temp=process_id;
			while(1<=temp/2){
				temp=temp/2;
				my_d++;
			}
			source=process_id-exp(my_d-1);
			int size;
			MPI_Recv(&size,1, MPI_INT,source,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int e;
			MPI_Recv(&e,1, MPI_INT,source,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int *a=(int *)malloc(sizeof(int)*size);
			MPI_Recv(a,size, MPI_INT,source,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			start_index=0;
			end_index=start_index+size_of_data-1;
			found_index=binarySearch(a,start_index,end_index , key,process_id);
			if(found_index>=0){
				found_index=found_index+size;
			}
			
			MPI_Send(&found_index,1,MPI_INT, source,process_id,MPI_COMM_WORLD);
			
		}
	}	
	MPI_Finalize();
	return 0;
}