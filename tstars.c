#include <stdio.h>
#include <stdlib.h>

int nums[3][4]= {
			{ 3, 6, 9,12},
			{15,25,30,35},
			{66,77,88,99}
		};

int a[10][10][10];

int main(void);

void funsub(int (*)[]);

void
funsub(int (*subarr)[])
{
	**subarr = 1;
}

void funsubsub(int []);

void
funsubsub(int arr[])
{
	*arr = 1;
}

int
main()
{
	int	i = 10;
	int	*ip;
	int	**ipp;
	int	***ippp;
	int	(*ipc)[];		/* pointer to an array of int */
	int	(*ipr)[][10];		/* pointer to array of array 10 of int */
	int	(*ipa)[][10][10];	/* pointer to array of array 10 of array 10 of int */

	ip = &i;			/* address of an int */
	ipp = &ip;			/* address of an address of an int */
	ippp = &ipp;			/* address of an address of an address of an int */
	
	***ippp = i;

	ip = &(a[1][2][3]);
	ip = &(a[1][2][0]);
	ip = &(a[1][0][0]);

	/*
	 * since: a[1][2][3]	==> *(*(*(a+1)+2)+3)
	 *
	 * we can say that:
	 *
	 * 	a[1][2]		==> *(*(*(a+1)+2)+0)
	 * 	a[1]		==> *(*(*(a+1)+0)+0)
	 */

	ipc = &(a[1][2]);
	ipr = &(a[1]);
	ipa = &(a);

	funsub(a[1]);
	funsubsub(a[1][2]);

	printf("%d\n", *(*(nums+1)+1)+1);

	exit(0);
}

/*

Declaration: T a[N];

Expression    Type    Converts to     Value
----------    ----    ------------    -----
         a    T [N]   T *             Address of the first element in a;
                                        identical to writing &a[0]
        &a    T (*)[N]                Address of the array; value is the same
                                        as above, but the type is different
  sizeof a    size_t                  Number of bytes contained in the array
                                        object (N * sizeof T)
        *a    T                       Value at a[0]
      a[i]    T                       Value at a[i]
     &a[i]    T *                     Address of a[i]

Declaration: T a[N][M];

Expression     Type        Converts to     Value
----------     ----        ------------    -----
          a    T [N][M]    T (*)[M]        Address of the first subarray (&a[0])
         &a    T (*)[N][M]                 Address of the array (same value as
                                             above, but different type)
   sizeof a    size_t                      Number of bytes contained in the
                                             array object (N * M * sizeof T)
         *a    T [M]      T *              Value of a[0], which is the address
                                             of the first element of the first subarray
                                             (same as &a[0][0])
       a[i]    T [M]      T *              Value of a[i], which is the address
                                             of the first element of the i'th subarray
      &a[i]    T (*)[M]                    Address of the i-th subarray; same value as
                                             above, but different type
sizeof a[i]    size_t                      Number of bytes contained in the i'th subarray
                                             object (M * sizeof T)
      *a[i]    T                           Value of the first element of the i'th
                                             subarray (a[i][0])
    a[i][j]    T                           Value at a[i][j]
   &a[i][j]    T *                         Address of a[i][j]

Declaration: T a[N][M][O];

Expression        Type             Converts to
----------        ----             -----------
         a        T [N][M][O]      T (*)[M][O]
        &a        T (*)[N][M][O]
        *a        T [M][O]         T (*)[O]
      a[i]        T [M][O]         T (*)[O]
     &a[i]        T (*)[M][O]
     *a[i]        T [O]            T *
   a[i][j]        T [O]            T *
  &a[i][j]        T (*)[O]
  *a[i][j]        T 
a[i][j][k]        T

 */
