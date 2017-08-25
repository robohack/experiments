/*
 *  The basic idea of our algorithm is to compare 2 adjacent values and
 * determine if they are the same.  If they are not the same and we weren't
 * already looking previusly at adjacent pairs that were the same, then we
 * output the value at the current index.  The algorithm does everything
 * in-place and doesn't allocate any new memory.  It outputs the unique values
 * into the input array.
 */

#include <stdio.h>                                                                                        
#include <assert.h>                                                                                       

int remove_dups(int *arr, int n)                                                                          
{                                                                                                         
        int idx = 0, odx = -1;                                                                            
        bool dup = false;                                                                                 
        while (idx < n)                                                                                   
        {                                                                                                 
                if (arr[idx] != arr[idx+1])                                                               
                {                                                                                         
                        if (dup)                                                                          
                                dup = false;                                                              
                        else                                                                              
                        {                                                                                 
                                arr[++odx] = arr[idx];                                                    
                        }                                                                                 
                } else                                                                                    
                        dup = true;                                                                       

                idx++;                                                                                    
        }                                                                                                 

        return (odx == -1) ? -1 : ++odx;                                                                  
}                                                                                                         

int main(int argc, char *argv[])                                                                          
{                                                                                                         
        int a[] = {31,44,44,67,67,99,99,100,101};                                                         
        int k = remove_dups(a,9);                                                                         
        assert(k == 3);                                                                                   
        for (int i = 0;i<k;i++)                                                                           
                printf("%d ",a[i]);                                                                       

        printf("\n\n");                                                                                   
        int b[] = {-5,-3,-2,-2,-2,-2,1,3,5,5,18,18};                                                      
        k = remove_dups(b,12);                                                                            
        assert(k == 4);                                                                                   
        for (int i = 0;i<k;i++)                                                                           
                printf("%d ",b[i]);                                                                       

        printf("\n\n");                                                                                   
        int c[] = {1,2,3,4,5,6,7,8,9};                                                                    
        k = remove_dups(c,9);                                                                             
        assert(k == 9);                                                                                   
        for (int i = 0;i<k;i++)                                                                           
                printf("%d ",c[i]);                                                                       

        return 0;                                                                                         
}                                                                                                         
