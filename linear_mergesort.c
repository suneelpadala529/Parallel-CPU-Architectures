#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void merge(int arr[], int l, int m, int r) 
{ 
	int i, j, k; 
	int n1 = m - l + 1; 
	int n2 = r - m; 

	int L[n1], R[n2]; 

	for (i = 0; i < n1; i++) 
		L[i] = arr[l + i]; 
	for (j = 0; j < n2; j++) 
		R[j] = arr[m + 1+ j]; 

	i = 0;
	j = 0;  
	k = l; 
	while (i < n1 && j < n2) 
	{ 
		if (L[i] <= R[j]) 
		{ 
			arr[k] = L[i]; 
			i++; 
		} 
		else
		{ 
			arr[k] = R[j]; 
			j++; 
		} 
		k++; 
	} 
	while (i < n1) 
	{ 
		arr[k] = L[i]; 
		i++; 
		k++; 
	} 

	while (j < n2) 
	{ 
		arr[k] = R[j]; 
		j++; 
		k++; 
	} 
} 

void mergeSort(int arr[], int l, int r) 
{ 
	if (l < r) 
	{ 
		int m = l+(r-l)/2; 
		mergeSort(arr, l, m); 
		mergeSort(arr, m+1, r); 
		merge(arr, l, m, r); 
	} 
} 

void printArray(int A[], int size) 
{ 
	int i; 
	for (i=0; i < size; i++) 
		printf("%d ", A[i]); 
	printf("\n"); 
} 



int main(int argc, char* argv[]) 
{
    int process_id, no_of_processes, total_data_size = 96, chunck_size;
    // int array[] = { 2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28};


    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);

    chunck_size = total_data_size/no_of_processes;


    if (process_id == 0)
    {
        int array[] = {83, 86, 77, 15, 93, 35, 86, 92, 49, 21, 62, 27, 90, 59, 63, 26, 40, 26, 72, 36, 11, 68, 67, 29, 82, 30, 62, 23, 67, 35, 29, 2, 22, 58, 69, 67, 93, 56, 11, 42, 29, 73, 21, 19, 84, 37, 98, 24, 15, 70, 13, 26, 91, 80, 56, 73, 62, 70, 96, 81, 5, 25, 84, 27, 36, 5, 46, 29, 13, 57, 24, 95, 82, 45, 14, 67, 34, 64, 43, 50, 87, 8, 76, 78, 88, 84, 3, 51, 54, 99, 32, 60, 76, 68, 39, 12};

        // int array[] = { 2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28};
        printf("=======before sorting===============\n");
        printArray(array,total_data_size);
        if(no_of_processes==1)
        {
            mergeSort(array,0,total_data_size-1);
            printf("=======final sorted array ===============\n");
            printArray(array,total_data_size);
        }
        else if(no_of_processes>total_data_size)
        {
            mergeSort(array,0,total_data_size-1);
            printf("=======final sorted array by master process===============\n");
            printArray(array,total_data_size);
            exit(1);
        }
        else
        {

            int send_to_one,send_to_last, data_in_main;
            
            send_to_one = (no_of_processes/2 -process_id)*chunck_size;
            send_to_last = (no_of_processes/2 -process_id-1)*chunck_size;
            
            data_in_main = total_data_size -(send_to_one+send_to_last);

            int array_1[send_to_one],array_n[send_to_last];
            for(int i = data_in_main; i < (data_in_main + send_to_one); i++)
            {
                array_1[i-data_in_main] = array[i];
                
            }
            if(no_of_processes>2)
            {
                for (int i = (data_in_main + send_to_one); i < total_data_size ; i++)
                {
                    array_n[i-(data_in_main + send_to_one)] = array[i];
                } 

            }
            MPI_Send(&send_to_one, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(array_1, send_to_one, MPI_INT, 1, 0, MPI_COMM_WORLD);

            if(no_of_processes>2)
            {
                MPI_Send(&send_to_last, 1, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD);
                MPI_Send(array_n, send_to_last, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD);
            }
            //perform mergeSort to remaining data
            mergeSort(array,0, data_in_main-1);
            //reduction part
            int come_in_size;
            MPI_Recv(&come_in_size,1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

            int recv_array[come_in_size];
            MPI_Recv(&recv_array,come_in_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

            int new_size;
            new_size = come_in_size + data_in_main;
            int merged_array[new_size];

            for (int i = 0; i < new_size; i++)
            {
                if (i<data_in_main)
                    merged_array[i] = array[i];
                else
                    merged_array[i] = recv_array[i-data_in_main];
                
            }
            //merge 0 and 1
            merge(merged_array,0,data_in_main-1,new_size);
            if(no_of_processes>2)
            {
                //receive from last process
                int come_in_last;
                MPI_Recv(&come_in_last,1, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD, &status);

                int last_recv_array[come_in_last];
                MPI_Recv(&last_recv_array,come_in_last, MPI_INT, no_of_processes-1, 0, MPI_COMM_WORLD, &status);

                int total_sorted_data_size;
                total_sorted_data_size = new_size + come_in_last;

                int sorted_array[total_sorted_data_size];
                for (int i = 0; i < total_sorted_data_size; i++)
                {
                    if(i<new_size)
                        sorted_array[i] = merged_array[i];
                    else
                        sorted_array[i] = last_recv_array[i-new_size];
                }
                //merging all the data
                merge(sorted_array,0,new_size-1,total_sorted_data_size-1);

                printf("=======final sorted array ===============\n");
                printArray(sorted_array,total_sorted_data_size);
            }
            else
            {
                printf("=======final sorted array ===============\n");
                printArray(merged_array,new_size);
            }
         
        }
    }
    else if(process_id == no_of_processes/2)
    {
        //receive from process/2-1
        int received_size;
        MPI_Recv(&received_size,1, MPI_INT, (no_of_processes/2 -1), 0, MPI_COMM_WORLD, &status);

        int a[received_size];
        MPI_Recv(&a,received_size, MPI_INT, (no_of_processes/2 -1), 0, MPI_COMM_WORLD, &status);
        //perform mergeSort to remaining data
        mergeSort(a,0,received_size-1);
        //reduction
        //send to process/2-1
        MPI_Send(&received_size,1, MPI_INT, (no_of_processes/2 -1), 0, MPI_COMM_WORLD);
        MPI_Send(&a,received_size, MPI_INT, (no_of_processes/2 -1), 0, MPI_COMM_WORLD);
    }
    else if(process_id == no_of_processes/2 +1)
    {
        //receive from process/2+1
        int received_size;
        MPI_Recv(&received_size,1, MPI_INT, (no_of_processes/2 + 2)%no_of_processes, 0, MPI_COMM_WORLD, &status);

        int a[received_size];
        MPI_Recv(&a,received_size, MPI_INT, (no_of_processes/2 + 2)%no_of_processes, 0, MPI_COMM_WORLD, &status);
        //perform mergeSort to remaining data
        mergeSort(a,0,received_size-1);
        //reduction
        //send to process/2+1
        MPI_Send(&received_size,1, MPI_INT, (no_of_processes/2 + 2)%no_of_processes, 0, MPI_COMM_WORLD);
        MPI_Send(&a,received_size, MPI_INT, (no_of_processes/2 + 2)%no_of_processes, 0, MPI_COMM_WORLD);


    }
    else if(process_id>0 && process_id < no_of_processes/2)
    {
        //recive from process-1
        int received_size;
        MPI_Recv(&received_size,1, MPI_INT, process_id-1, 0, MPI_COMM_WORLD, &status);

        int a[received_size];
        MPI_Recv(&a,received_size, MPI_INT, process_id-1, 0, MPI_COMM_WORLD, &status);

        int send_to_next, left_data;
        send_to_next = (no_of_processes/2 - process_id)*chunck_size;
        left_data = received_size-send_to_next;

        int send_array[send_to_next];
        for (int i = left_data; i < received_size; i++)
        {
            send_array[i-left_data] = a[i];
        }

        MPI_Send(&send_to_next, 1, MPI_INT, process_id+1, 0, MPI_COMM_WORLD);
        MPI_Send(&send_array, send_to_next, MPI_INT, process_id+1, 0, MPI_COMM_WORLD);
        //perform mergeSort to remaining data
        mergeSort(a,0,left_data-1);
        //reduction
        // receive from process+1
        int come_in_size;
        MPI_Recv(&come_in_size,1, MPI_INT, process_id+1, 0, MPI_COMM_WORLD, &status);

        int recv_array[come_in_size];
        MPI_Recv(&recv_array,come_in_size, MPI_INT, process_id+1, 0, MPI_COMM_WORLD, &status);

        int new_size;
        new_size = come_in_size + left_data;
        int merged_array[new_size];

        for (int i = 0; i < new_size; i++)
        {
            if (i<left_data)
                merged_array[i] = a[i];
            else
                merged_array[i] = recv_array[i-left_data];
            
        }
        //mergeing the data
        merge(merged_array,0,left_data-1,new_size-1);
        MPI_Send(&new_size, 1, MPI_INT, process_id-1, 0, MPI_COMM_WORLD);
        MPI_Send(&merged_array, new_size, MPI_INT, process_id-1, 0, MPI_COMM_WORLD);
    }
    else
    {
        //receive data from process+1
        int received_size;
        MPI_Recv(&received_size,1,MPI_INT, (process_id+1)%no_of_processes, 0, MPI_COMM_WORLD, &status);
        
        int a[received_size];
        MPI_Recv(&a,received_size, MPI_INT, (process_id+1)%no_of_processes, 0, MPI_COMM_WORLD, &status);

        int send_to_next,left_data;
        send_to_next = (process_id - no_of_processes/2-1)*chunck_size;
        left_data = received_size-send_to_next;

        int send_array[send_to_next];
        for (int i = left_data; i < received_size; i++)
        {
            send_array[i-left_data] = a[i];
        }
        MPI_Send(&send_to_next, 1, MPI_INT, process_id-1, 0, MPI_COMM_WORLD);
        MPI_Send(&send_array, send_to_next, MPI_INT, process_id-1, 0, MPI_COMM_WORLD);
        //perform mergeSort to remaining data
        mergeSort(a,0,left_data-1);
        //reduction 
        //receive from process-1
        int come_in_size;
        MPI_Recv(&come_in_size,1, MPI_INT, process_id-1, 0, MPI_COMM_WORLD, &status);

        int recv_array[come_in_size];
        MPI_Recv(&recv_array,come_in_size, MPI_INT, process_id-1, 0, MPI_COMM_WORLD, &status);

        int new_size;
        new_size = come_in_size + left_data;
        int merged_array[new_size];

        for (int i = 0; i < new_size; i++)
        {
            if (i<left_data)
                merged_array[i] = a[i];
            else
                merged_array[i] = recv_array[i-left_data];
            
        }
        //merging the data
        merge(merged_array,0,left_data-1,new_size-1);
        MPI_Send(&new_size, 1, MPI_INT, (process_id+1)%no_of_processes, 0, MPI_COMM_WORLD);
        MPI_Send(&merged_array, new_size, MPI_INT, (process_id+1)%no_of_processes, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();
	return 0;
}