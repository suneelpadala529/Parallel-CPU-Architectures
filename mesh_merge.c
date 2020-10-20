#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);
void printArray(int A[], int size); 

int main(int argc, char* argv[]) 
{
    int process_id, no_of_processes, total = 96, chunk;
    // int array[] = {83, 86, 77, 15, 93, 35, 86, 92, 49, 21, 62, 27, 90, 59, 63, 26, 40, 26, 72, 36, 11, 68, 67, 29, 82, 30, 62, 23, 67, 35, 29, 2, 22, 58, 69, 67, 93, 56, 11, 42, 29, 73, 21, 19, 84, 37, 98, 24, 15, 70, 13, 26, 91, 80, 56, 73, 62, 70, 96, 81, 5, 25, 84, 27, 36, 5, 46, 29, 13, 57, 24, 95, 82, 45, 14, 67, 34, 64, 43, 50, 87, 8, 76, 78, 88, 84, 3, 51, 54, 99, 32, 60, 76, 68, 39, 12};
    // int array[] = { 2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,3, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28};
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_processes);
    
    chunk = total/no_of_processes;

    int i, j, root_p, row_data;
    root_p = (int)sqrt(no_of_processes);

    i = process_id/root_p;
    j = process_id%root_p;

    row_data = root_p*chunk; //top row data
    int check;
    check = (int)sqrt(no_of_processes);
    int array[] = {83, 86, 77, 15, 93, 35, 86, 92, 49, 21, 62, 27, 90, 59, 63, 26, 40, 26, 72, 36, 11, 68, 67, 29, 82, 30, 62, 23, 67, 35, 29, 2, 22, 58, 69, 67, 93, 56, 11, 42, 29, 73, 21, 19, 84, 37, 98, 24, 15, 70, 13, 26, 91, 80, 56, 73, 62, 70, 96, 81, 5, 25, 84, 27, 36, 5, 46, 29, 13, 57, 24, 95, 82, 45, 14, 67, 34, 64, 43, 50, 87, 8, 76, 78, 88, 84, 3, 51, 54, 99, 32, 60, 76, 68, 39, 12};
    // int array[] = { 2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,3, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28,2, 4 , 1, 3, 10, 6, 9, 8, 7, 15, 13, 18, 21,14,29,24,27,15,19,25,33,37,35,28};
    if(no_of_processes==1)
    {
        printf("=================before sorting===============\n");
        printArray(array,total);
        mergeSort(array,0,total-1);
        printf("=================final sorted array ===============\n");
        printArray(array,total);
    }
    
    
    else if(check*check != no_of_processes)
    {
    	printf("\n Invaid processors\n" );
    }
    else
    {
        /*************************** column phase *******************************/
        if(i==0)
        {
            
            if(j==0)
            {
                printf("=================before sorting===============\n");
                printArray(array,total);
            }

            int data_remaining;
            data_remaining = total - (total -row_data);
            int send_below, send_below_last, below_index, last_index;
            send_below = data_remaining/2;
            below_index = send_below;
            int offset;
            if(j == 0)
                offset= 0;
            if(j == 1)
                offset = total/2;
            if(j == root_p/2)
                offset = total/2 + row_data;
            if(j == root_p-1)
                offset = row_data;

            // send data to below to the process no
            MPI_Send(&send_below, 1, MPI_INT,root_p*1+j,0, MPI_COMM_WORLD);
            MPI_Send(array+below_index+offset,send_below, MPI_INT, root_p*1+j,0, MPI_COMM_WORLD);

            send_below_last = send_below - chunk;
            last_index = chunk;
            //send data to each column in a row 0
            MPI_Send(&send_below_last, 1, MPI_INT,root_p*(root_p-1)+j,0, MPI_COMM_WORLD);
            MPI_Send(array+last_index+offset,send_below_last, MPI_INT, root_p*(root_p-1)+j,0, MPI_COMM_WORLD);

            int remaining = data_remaining-(send_below_last+send_below);
            int left[remaining-1];  
	        for(int i=0;i<remaining;i++) 
            	left[i] = array[offset+i];
            mergeSort(left,0, remaining-1);
            //Reduction
            //receive sorted data from below processes 
            int received_below; 
            MPI_Recv(&received_below,1,MPI_INT, root_p*1+j, 0, MPI_COMM_WORLD, &status);
            int below[received_below];
            MPI_Recv(&below,received_below, MPI_INT, root_p*1+j, 0, MPI_COMM_WORLD, &status);

            int new_data = received_below+remaining;
            int new_1[new_data];
            for (int i = 0; i < new_data; i++)
            {
                if(i<remaining)
                    new_1[i] = left[i];
                else
                    new_1[i] = below[i-remaining];
                
            }
            merge(new_1, 0, remaining-1, new_data-1);
            //receive sorted data from last roww process
            int received_last; 
            MPI_Recv(&received_last,1,MPI_INT, root_p*(root_p-1)+j, 0, MPI_COMM_WORLD, &status);
            int last[received_last-1];
            MPI_Recv(&last,received_last, MPI_INT, root_p*(root_p-1)+j, 0, MPI_COMM_WORLD, &status);

            int final_size = new_data + received_last;
            int final_arr[final_size-1];
            for (int i = 0; i < final_size; i++)
            {
                if(i<new_data)
                    final_arr[i] = new_1[i];
                else
                    final_arr[i] = last[i-new_data];
                
            }
            merge(final_arr, 0, new_data-1, final_size-1);
	    	printf("\n============================================== \n");
	    	printf(" process_id = %d ::", process_id);
            printArray(final_arr, final_size);
            /* ************************row phase ********************************* */
            if(j==0)
            {
            	
            	int rlast;
	            MPI_Recv(&rlast,1,MPI_INT, root_p-1, 0, MPI_COMM_WORLD, &status);
	            int llast[rlast];
	            MPI_Recv(&llast,rlast, MPI_INT, root_p-1, 0, MPI_COMM_WORLD, &status);
	            int sorted_arr[total], nnew = final_size+rlast;
	            for (int i = 0; i < nnew; ++i)
	            {
	            	if (i<final_size)
	  					sorted_arr[i] = final_arr[i];
	  				else
	  					sorted_arr[i] = llast[i-final_size];
	            }
	            merge(sorted_arr, 0, final_size-1, nnew-1);
	            printf("\n============================================== \n");
		    	printf(" last and zero mered::");
	            printArray(sorted_arr, nnew);
                //receiving sorted data from process no 1    
            	int rone;
	            MPI_Recv(&rone,1,MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
	            int oone[rone];
	            MPI_Recv(&oone,rone, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

	            for (int i = nnew; i < total; ++i)
	            {
	  				sorted_arr[i] = oone[i-nnew];
	            }
	            merge(sorted_arr, 0, nnew-1, total-1);
	            printf("\n===================final sorted array=========================== \n");
	            printArray(sorted_arr, total);
	            
            }
		    if(j==root_p/2)
		    {
                //send sorted data of total column root_p/2 to process[i,j-1]
				MPI_Send(&final_size, 1,MPI_INT, j-1,0, MPI_COMM_WORLD);
	            MPI_Send(final_arr,final_size, MPI_INT, j-1,0, MPI_COMM_WORLD);
		    }
		    if(j== root_p-1)
		    {
                //send sorted data of total column root_p-1 to 0 
				MPI_Send(&final_size, 1,MPI_INT, 0,0, MPI_COMM_WORLD);
	            MPI_Send(final_arr,final_size, MPI_INT, 0,0, MPI_COMM_WORLD);
		    }
		    else if(j>0 && j< root_p/2)
            {
                //receive data from process[0,j-1]
                //send data to process[0, j+1]
				int red_size,received_size; 
                MPI_Recv(&red_size,1,MPI_INT, j+1, 0, MPI_COMM_WORLD, &status);          
                int rcv[red_size];
                MPI_Recv(rcv,red_size, MPI_INT, j+1, 0, MPI_COMM_WORLD, &status);

                received_size = red_size+final_size;
				int mer[received_size];
				for(int i = 0;i<received_size;i++)
				{
				    if(i<final_size)
					mer[i] = final_arr[i];
				    else
					mer[i] = rcv[i-final_size];
				}
				merge(mer,0,final_size-1, received_size-1);
				printf("\n============================================== \n");
		    	printf(" 1 nd 2 merged::");
	            printArray(mer, received_size);

				MPI_Send(&received_size, 1, MPI_INT, j-1,0, MPI_COMM_WORLD);
                MPI_Send(mer,received_size, MPI_INT, j-1,0, MPI_COMM_WORLD);
            }
            else if(j>(root_p/2+1) && j<=root_p-1)
            {

                int received_size; 
                MPI_Recv(&received_size,1,MPI_INT, (j+1)%root_p, 0, MPI_COMM_WORLD, &status);
                
                int a[received_size];
                MPI_Recv(&a,received_size, MPI_INT, (j+1)%root_p, 0, MPI_COMM_WORLD, &status);

                int send_next_left,index;
                send_next_left = received_size-row_data;
                index = received_size - send_next_left;

                MPI_Send(&send_next_left, 1, MPI_INT,j-1,0, MPI_COMM_WORLD);
                MPI_Send(a+index,send_next_left, MPI_INT, j-1,0, MPI_COMM_WORLD);
                mergeSort(a,0,index);

                //Reduction
                int rev; 
                MPI_Recv(&rev,1,MPI_INT,j-1,0, MPI_COMM_WORLD, &status);
                
                int r_1[rev-1];
                MPI_Recv(&r_1,rev-1, MPI_INT,j-1, 0, MPI_COMM_WORLD, &status);
                int new;
                int n_arr[new-1];
                for(int i =0;i<new;i++)
                {
                    if(i<index)
                        n_arr[i] = a[i];
                    else
                        n_arr[i] = r_1[i-index];
                }

                merge(n_arr,0,index-1,new-1);
                MPI_Send(&new, 1, MPI_INT,(j+1)%root_p,0, MPI_COMM_WORLD);
                MPI_Send(n_arr,new, MPI_INT, (j+1)%root_p,0, MPI_COMM_WORLD);

            }

        }
        else if(i==root_p/2)
        {
            //receive data from process[rootp/2-1, j]
            int received_size;
            MPI_Recv(&received_size,1,MPI_INT, root_p*(root_p/2-1)+j, 0, MPI_COMM_WORLD, &status);
            int a[received_size];
            MPI_Recv(&a,received_size, MPI_INT, root_p*(root_p/2-1)+j, 0, MPI_COMM_WORLD, &status);
            mergeSort(a,0,received_size-1);
            //reduction
            //send data to process[rootp/2-1, j]
            MPI_Send(&received_size, 1, MPI_INT, root_p*(root_p/2-1)+j, 0, MPI_COMM_WORLD);
            MPI_Send(a, received_size, MPI_INT, root_p*(root_p/2-1)+j, 0, MPI_COMM_WORLD);
        }
        else if(i==root_p/2+1)
        {
            //eceive data from process[rootp/2+2 mod rootp, j]
            int received_size;
            MPI_Recv(&received_size,1,MPI_INT, root_p*((root_p/2+2)%root_p)+j, 0, MPI_COMM_WORLD, &status);
            int a[received_size];
            MPI_Recv(&a,received_size, MPI_INT, root_p*((root_p/2+2)%root_p)+j, 0, MPI_COMM_WORLD, &status);

            mergeSort(a,0,received_size-1);
            //reduction
             //send data to process[rootp/2+2 mod rootp, j]
            MPI_Send(&received_size, 1, MPI_INT, root_p*((root_p/2+2)%root_p)+j, 0, MPI_COMM_WORLD);
            MPI_Send(a, received_size, MPI_INT, root_p*((root_p/2+2)%root_p)+j, 0, MPI_COMM_WORLD);

        }
        else if(i>0 && i<root_p/2)
        {
            //receive data from process[i-1,j]
            int received_size;
            MPI_Recv(&received_size,1,MPI_INT, root_p*(i-1)+j, 0, MPI_COMM_WORLD, &status);
            int a[received_size];
            MPI_Recv(&a,received_size, MPI_INT, root_p*(i-1)+j, 0, MPI_COMM_WORLD, &status);

            int next_below, index;
            next_below = received_size-chunk;
            index = received_size - next_below;
            //send data to process[i+1, j]
            MPI_Send(&next_below, 1, MPI_INT, root_p*(i+1)+j,0, MPI_COMM_WORLD);
            MPI_Send(a+index,next_below, MPI_INT, root_p*(i+1)+j,0, MPI_COMM_WORLD);
            mergeSort(a,0, (received_size-next_below)-1);
            //reduction
            //receive data from process[i+1, j]
            int rcvd;
            MPI_Recv(&rcvd, 1, MPI_INT, root_p*(i+1)+j,0, MPI_COMM_WORLD, &status);
            int r_array[rcvd];
            MPI_Recv(r_array,rcvd, MPI_INT, root_p*(i+1)+j,0, MPI_COMM_WORLD, &status);
            int new_size = (received_size-next_below) + rcvd;
            int merge_a[new_size];
            for (int i = 0; i < new_size; i++)
            {
                if(i<(received_size-next_below))
                    merge_a[i] = a[i];
                else
                    merge_a[i] = r_array[i-(received_size-next_below)];
            }
            merge(merge_a,0,(received_size-next_below)-1,new_size-1);
            //send data to process[i-1,j]
            MPI_Send(&new_size, 1, MPI_INT, root_p*(i-1)+j,0, MPI_COMM_WORLD);
            MPI_Send(merge_a,new_size, MPI_INT, root_p*(i-1)+j,0, MPI_COMM_WORLD);
            
        }
        else if(i>(root_p/2+1) && i<=root_p-1)
        {
            //recv i+1 mod rootp,j

             //recv i+1 mod rootp,j
            int received_size;
            MPI_Recv(&received_size,1,MPI_INT, root_p*((i+1)%root_p)+j, 0, MPI_COMM_WORLD, &status);
            int a[received_size];
            MPI_Recv(&a,received_size, MPI_INT, root_p*(i+1)+j, 0, MPI_COMM_WORLD, &status);
            int next_above, index;
            next_above = received_size -chunk;
            index = received_size - (received_size - chunk);
            //send i-1,j
            MPI_Send(&next_above, 1, MPI_INT, root_p*(i-1)+j,0, MPI_COMM_WORLD);
            MPI_Send(a+index,next_above, MPI_INT, root_p*(i-1)+j,0, MPI_COMM_WORLD);

            mergeSort(a,0, received_size);
            //Reduction
            //receive sorted data from process[i-1,j]
            int rev; 
            MPI_Recv(&rev,1,MPI_INT,root_p*(i-1)+j,0, MPI_COMM_WORLD, &status);
            int r_1[rev];
            MPI_Recv(&r_1,rev, MPI_INT,root_p*(i-1)+j, 0, MPI_COMM_WORLD, &status);
            int new;
            int n_arr[new-1];
            for(int i =0;i<new;i++)
	        {
	            if(i<index)
			        n_arr[i] = a[i];
		        else
			        n_arr[i] = r_1[i-index];
		    }
            merge(n_arr,0,index-1,new-1);
            //send sorted data to process[i+1.j]
            MPI_Send(&new, 1, MPI_INT,root_p*((i+1)%root_p)+j,0, MPI_COMM_WORLD);
            MPI_Send(n_arr,new, MPI_INT, root_p*((i+1)%root_p)+j,0, MPI_COMM_WORLD);
        }
    }
    
    MPI_Finalize();
	return 0;
}
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
