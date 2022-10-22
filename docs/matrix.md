**Group Number :** 16

**Student Name 1 :** Sk. Abukhoyer\
**Roll Number :** 2021201023\
**Course :** M.Tech CSIS\
**Email Id :** sk.a@students.iiit.ac.in

**Student Name 2 :** Seshadri Mazumder\
**Roll Number :** 2021801002\
**Course :** PhD CSE\
**Lab :** Center for Visual Information Technology(CVIT)\
**Email Id :** seshadri.mazumder@research.iiit.ac.in

**Course Name :** Data Systems\
**Course Id :** CS4.401\
**Instructor :** Dr. Kamal Karlapalem\
**Concerned TAs :** Kartik Garg & Ayush Goyal



# Part 1 - Writing Queries

Refer this link : [https://github.com/DSMonsoon22/phase-0-16/blob/main/data/queries.txt](https://github.com/DSMonsoon22/phase-0-16/blob/main/data/queries.txt)
# Part-2 : Extending SimpleRA to Include Matrices

### Background Description
The Objective of this second part of the project is to get familiarity with the boilerplate code structure.
In this part, we have to extend the boilerplate which currently only handles tables, to handle matrices as well and finding out the cross transpose of two matrices.
Square `n ∗ n,(n ≤ 10^5)` matrices are provided as csv files in the data folder. Since Matrices should be square
### Page Layout to Store Matrices
Below description shows how a matrix A can be split into block/page matrices, and how we can get the cross transpose of two matrices by some simple operations over the block/page matrices.

We take two N x N matrix A,B and then divide matrices into block/page matrices. Now each block matrix is written in a page as a file. We have implemented a matrix_page class, which is very similar to the ones given in the original page class. However, matrix pages have certain extra functions like transpose(), which returns the page in the transposed form. Each page has a index (which is present in the name of the page). This page index number is used to capture the relative position of a block in the block matrix. This information is required later for accessing the specefic block or page and also it would be used for the swapping of pages while calculating the cross transpose of two matrices. Hence, we create a pageName array inside the matrix class and store this information there. 

#### Analysis of the amount of data stored in a page (one block of the block matrix)
Suppose K be the maximum number of rows and columns of a matrix present in each martix block. So, K<sup>2</sup> will be the number of elements in each block matrix. Now, we can determine the value of K using the formula,

(K<sup>2</sup>) * size_of_integer in bytes = BLOCK_SIZE in bytes

We use the given BLOCK_SIZE = 8000 bytes in this equation. However, the boundary rows and columns can contain fewer elements than k<sup>2</sup>. Our code can handle those conditions without any issue.

The number of blocks would be created = ceill(N/K) x ceill(N/K)

### How Cross Transpose is Implemented
Since we have decided to divide the matrix into block matrices and write these blocks in pages, now we can easily cross transpose of matrices A and B according to the below steps:
 
1) Preveiously we said that after paging, all the block id or page index would be stored in a 2-D array, pageName. So,   
  
   
for each (i,j)th block in pageName 2-D array of matrix A  
 
      transpose (i,j)th block of A  
      
      transpose (i,j)th block of A  
      
      then swap the page between (i,j)th block of A and (j,i)th block of B  
      

2) If matrix A and B are same,swap each non diagonals block and swap it with its oppsite non diagonal block. and just transpose the diagonal blocks only, no need to swap. We read the corresponding pages which are present in the diagonal of the pageNames array using a cursor and buffer manager.
So in this approach, we only access each block only once. The number of block access equals the number of blocks/block matrices in the full matrix. 

We have implemented the following functionalities :
#### 1. LOAD MATRIX 
```bash
LOAD MATRIX <matrix name>
```
LOAD MATRIX command loads the contents of the csv files
and store it as blocks in the temp directory.

The main load matrix functionality file is located in : `\src\executors\load_matrix.cpp`

We are loading the matrix in specific blocks of sizes N x N which are stored in temp folder.


#### 2. PRINT MATRIX 
```bash
PRINT MATRIX <matrix name>
```
The main code of printing the matrix can be found in `\src\matrix.cpp`
This code is written to print the first 20 rows and the first 20 columns of the matrix. So we read the matrices by blocks and then read the corresponding vectors. We stop reading/accessing the blocks as soon as the specified criteria of 20 x 20 is met.

#### 3. CROSS TRANSPOSE 
```bash
CROSS_TRANSPOSE <matrix name1> <matrix name2>
```


#### 4. EXPORT MATRIX
```bash
EXPORT MATRIX <matrix name>
```

# Part 3 : Extending Part 2 to Sparse Matrices

A matrix is a two-dimensional data item with total values of m x n and is composed of m rows and n columns. A matrix is referred to as sparse if the majority of its elements have a value of 0.

#### Why use a sparse matrix rather than a basic matrix ?

Storage: Since there are fewer non-zero elements than zeroes, fewer elements can be stored in a given amount of memory.
Computing time: By logically creating a data structure that only traverses non-zero components, computing time can be reduced.

#### Question 3.1 How can we efficiently store sparse matrix ?

This question we will answer in two units :

1. UNIT - I : Page Layout Format
2. UNIT - II : Compression Technique

###### UNIT-1 : Page Layout Format
For Efficiently storing a sparse matrix first we will load and read the matrix block by block. We first decide a block size = N, i.e. block size = N x N. We then read the N x N blocks of data from the csv file in a column major fashion.

###### UNIT-2 : Compression Technique

No as we are reading the csv file block by block so first we create a global file and for each of the blocks we iterate over all the lines and for each line we write a tuple **<i, j, val>**, where, 
1. **i** describes the row number, 
2. **j** describes the column number, and 
3. **val** represents the value stored.

This is the way we perform compression of the matrix which have much lesser entries than the original matrix.

#### Question 3.2 Considering the page design how can we perform transpose of the matrix ?

For performing transpose of the sparse matrix we first read the file generated in previous question(3.1) row by row, and then for each  tuple in a row we rewrite a specific permutation of the row, and that reflects the transpose of the matrix.

The permutation is simple which involves the mutual interchange of the first and the second column in the row, i.e **<i, j, val>** is permuted to form **<j, i, val>**. This reflects the Transpose of the Sparse Matrix.

#### Extra Question : If we want to print the first nxn elements of the transposed sparse matrix ?

Since we want to access the first n rows and the n columns we need to first sort the files using external sorting and use the key pairwise, i.e **<i,j>**. Then we can iterate over the values of the tuples **<i, j>** and print them with intermediate 0s until **(i<=n && j<=n)**. While external sorting we will use paging technique as explained in the previous section.
