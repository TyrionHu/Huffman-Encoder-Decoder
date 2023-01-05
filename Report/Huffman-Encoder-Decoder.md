# Lab03: Huffman 压缩实验报告

## 目录

[toc]

## 问题描述

在合适的情况下，利用 Huffman 编码对文件进行压缩可以减少其占用空间，同时在需要使用到文件的时 候也可以根据压缩文件中所提供的信息来将其还原为原文件。本次实验中，我们将实现一个基于 Huffman 编码的文件压缩/解压缩工具。

## 基本要求

基于 Huffman 编码实现一个压缩器和解压缩器(其中 Huffman 编码以字节作为统计和编码的基本符号单元)，使其可以对任意的文件进行压缩和解压缩操作。针对编译生成的程序，要求压缩和解压缩部分 可以分别独立运行。具体要求为:

- 每次运行程序时，用戶可以指定只压缩**/**只解压缩指定路径的文件。实现的时候不限制与用戶的交互方式，可供参考的方式包括但不限于
  - 根据命令行参数指定功能(压缩/解压缩)和输入/输出文件路径 ；
  - GUI 界面；
  - 运行程序后由用戶交互输入指定功能和路径；
- 【**CAUTION!**】不被允许的交互方式: 通过修改源代码指定功能和文件路径 压缩时不需要指定解压文件的目标路径，解压缩时不需要指定压缩前原文件的路径，压缩后的文件；
- 可以换到另一个位置再做解压缩。

## 实验原理

### 压缩

实现霍夫曼编码的方式主要是创建一个二叉树和其节点。这些树的节点可以存储在数组里，数组的大小为符号（symbols）数的大小n，而节点分别是终端节点（叶节点）与非终端节点（内部节点）。

一开始，所有的节点都是终端节点，节点内有三个字段：

1. 符号（Symbol）
2. 权重（Weight、Probabilities、Frequency）
3. 指向父节点的链接（Link to its parent node）

而非终端节点内有四个字段：

1. 权重（Weight、Probabilities、Frequency）
2. 指向两个子节点的 链接（Links to two child node）
3. 指向父节点的链接（Link to its parent node）

基本上，我们用'0'与'1'分别代表指向左子节点与右子节点，最后为完成的二叉树共有n个终端节点与n-1个非终端节点，去除了不必要的符号并产生最佳的编码长度。

过程中，每个终端节点都包含着一个权重（Weight、Probabilities、Frequency），两两终端节点结合会产生一个新节点，新节点的权重是由两个权重最小的终端节点权重之总和，并持续进行此过程直到只剩下一个节点为止。

实现霍夫曼树的方式有很多种，可以使用优先队列（Priority Queue）简单达成这个过程，给与权重较低的符号较高的优先级（Priority），算法如下：

1. 把n个终端节点加入优先队列，则n个节点都有一个优先权Pi，1 ≤ i ≤ n
2. 如果队列内的节点数>1，则：
   1. 从队列中移除两个最小的Pi节点，即连续做两次remove（min（Pi）, Priority_Queue)
   2. 产生一个新节点，此节点为（1）之移除节点之父节点，而此节点的权重值为（1）两节点之权重和
   3. 把（2）产生之节点加入优先队列中
3. 最后在优先队列里的点为树的根节点（root）

而此算法的时间复杂度（ Time Complexity）为O（n log n）；因为有n个终端节点，所以树总共有2n-1个节点，使用优先队列每个循环须O（log n）。

此外，有一个更快的方式使时间复杂度降至线性时间（Linear Time）O（n），就是使用两个队列（Queue）创件霍夫曼树。第一个队列用来存储n个符号（即n个终端节点）的权重，第二个队列用来存储两两权重的合（即非终端节点）。此法可保证第二个队列的前端（Front）权重永远都是最小值，且方法如下：

1. 把n个终端节点加入第一个队列（依照权重大小排列，最小在前端）
2. 如果队列内的节点数>1，则：
   1. 从队列前端移除两个最低权重的节点
   2. 将（1）中移除的两个节点权重相加合成一个新节点
   3. 加入第二个队列
3. 最后在第一个队列的节点为根节点

虽然使用此方法比使用优先队列的时间复杂度还低，但是注意此法的第1项，节点必须依照权重大小加入队列中，如果节点加入顺序不按大小，则需要经过排序，则至少花了O（n log n）的时间复杂度计算。

但是在不同的状况考量下，时间复杂度并非是最重要的，如果我们今天考虑英文字母的出现频率，变量n就是英文字母的26个字母，则使用哪一种算法时间复杂度都不会影响很大，因为n不是一笔庞大的数字。

### 解压缩

简单来说，霍夫曼码树的解压缩就是将得到的前置码（Prefix Huffman code）变换回符号，通常借由树的追踪（Traversal），将接收到的位串（Bits stream）一步一步还原。但是要追踪树之前，必须要先重建霍夫曼树；某些情况下，如果每个符号的权重可以被事先预测，那么霍夫曼树就可以预先重建，并且存储并重复使用，否则，发送端必须预先发送霍夫曼树的相关信息给接收端。

最简单的方式，就是预先统计各符号的权重并加入至压缩之位串，但是此法的运算量花费相当大，并不适合实际的应用。若是使用Canonical encoding，则可精准得知树重建的资料量只占*B*2^*B*比特（其中B为每个符号的比特数（bits））。如果简单将接收到的位串一个比特一个比特的重建，例如：'0'表示父节点，'1'表示终端节点，若每次读取到1时，下8个比特则会被解读是终端节点（假设资料为8-bit字母），则霍夫曼树则可被重建，以此方法，资料量的大小可能为2~320字节不等。虽然还有很多方法可以重建霍夫曼树，但因为压缩的资料串包含"trailing bits"，所以还原时一定要考虑何时停止，不要还原到错误的值，如在资料压缩时时加上每笔资料的长度等。

### 文件读写

事实上，本工程难度最大的并非是二叉树的编写，而是文件的读写。

由于实验要求中需要能够压缩文件夹，单纯使用fread等函数还不够，还需判断读写的文件是否为文件夹，应使用opendir等库函数。

#### 最后一字节

压缩文件的最后一字节不一定对齐到字节边界，因此可能有几个多余的 0，而这些多余 的 0 可能恰好构成一个 Huffman 编码。解码程序无法获知这个编码是否属于源文件的一部分。因此有的文件解压后末尾可能出现一个多余的字节。

- 在压缩文件头部写入源文件的总长度(字节数)。需要四个字节来存储这个信息(假定文件长度不超过 4GB)；
- 增加第 257 个字符(在一个字节的 0~255 之外)用于 EOF。对于较长的文件，会造成较大的损耗；
- 在压缩文件头写入源文件的总长度%256 的值，需要一个字节。由于最后一个字节存在或不存在会影响 $文件总长\%256$ 的值，因此可以根据这个值判断整个压缩文件的最后一字节末尾的 0 是否在源文件中存在。

## 概要设计

### Encoder 

```c
//this structure will be used to create the translation tree
struct seed
{   
    seed * left, * right;
    long int number;
    unsigned char character;
    string bit;
};
```

```c
//below function is used for writing the uChar to compressed file. It does not write it directly as one byte instead it mixes uChar and current byte, writes 8 bits of it and puts the rest to curent byte for later use
void WriteFromUnsignedChar(unsigned char uChar,unsigned char &current_byte, int current_bit_count, FILE * fp_write)
```

```c
//below function is writing number of files we re going to translate inside current folder to compressed file's 2 bytes. It is done like this to make sure that it can work on little, big or middle-endian systems
void WriteFileCounter(int file_count, unsigned char &current_byte, int current_bit_count, FILE * compressed_file_pointer)
```

```c
//This function is writing byte count of current input file to compressed file using 8 bytes. It is done like this to make sure that it can work on little, big or middle-endian systems
void WriteFileSize(long int size, unsigned char &current_byte, int current_bit_count, FILE * compressed_file_pointer)
```

```c
// This function writes bytes that are translated from current input file's name to the compressed file.
void WriteFileName(char *file_name, string * string_array, unsigned char & current_byte, int & current_bit_count, FILE * compressed_file_pointer)
```

```c
// Below function translates and writes bytes from current input file to the compressed file.
void WriteFile(FILE * original_file_pointer, long int size, string * string_array, unsigned char &current_byte, int & current_bit_count, FILE * compressed_file_pointer)
```

```c
// This function counts usage frequency of bytes inside a folder only give folder path as input
void FolderCounter(string path, long int * number, long int & total_size, long int & total_bits)
```

### Decoder

```c
// translate_folder function is used for creating files and folders inside given path by using information from the compressed file.
// whenever it creates another file it will recursively call itself with path of the newly created file and in this way translates the compressed file.
void translate_folder(string path, unsigned char & current_byte, int & current_bit_count, FILE * fp_compressed, translation * root)
```

```c
// burn_tree function is used for deallocating translation tree
void burn_tree(translation * node)
```

```c
// process_n_bits_TO_STRING function reads n successive bits from the compressed file and stores it in a leaf of the translation tree, after creating that leaf and sometimes after creating nodes that are binding that leaf to the tree.
void process_n_bits_TO_STRING(unsigned char & current_byte, int n, int & current_bit_count, FILE * fp_read, translation * node, unsigned char uChar)
```

```c
// process_8_bits_NUMBER reads 8 successive bits from compressed file (does not have to be in the same byte) and returns it in unsigned char form
unsigned char process_8_bits_NUMBER(unsigned char & current_byte, int current_bit_count, FILE * fp_read)
```

```c
//checks if next input is either a file or a folder
//returns 1 if it is a file
//returns 0 if it is a folder
bool this_is_a_file(unsigned char & current_byte, int & current_bit_count, FILE * fp_compressed)
```

```c
// checks if the file or folder exists
bool file_exists(char *name)
```

```c
// returns file's size
long int read_file_size(unsigned char & current_byte, int current_bit_count, FILE * fp_compressed)
```

```c
// Decodes current file's name and writes file name to new_file char array
void write_file_name(char *new_file, int file_name_length, unsigned char & current_byte, int & current_bit_count, translation * root, FILE * fp_compressed)
```

```c
// This function translates compressed file from info that is now stored in the translation tree then writes it to a newly created file
void translate_file(char * path, long int size, unsigned char & current_byte, int & current_bit_count, translation * root, FILE * fp_compressed)
```

## 部分代码实现

```c
char * str_pointer;
unsigned char len, current_character;
string string_array[256];
for(e = array; e < array + letter_counter; e++)
{
    string_array[e->character] = e->bit;     //we are putting the transformation string to string_array array to make the compression process more time efficient
    len = e->bit.length();
    current_character = e->character;
    WriteFromUnsignedChar(current_character, current_byte, current_bit_count, compressed_file_pointer);
    WriteFromUnsignedChar(len,current_byte, current_bit_count, compressed_file_pointer);
    total_bits += len + 16;
    // above lines will write the byte and the number of bits
    // we re going to need to represent this specific byte's transformed version
    // after here we are going to write the transformed version of the number bit by bit.

    str_pointer = & e->bit[0];
    while(* str_pointer)
    {
        if(current_bit_count == 8)
        {
            fwrite(&current_byte, 1, 1, compressed_file_pointer);
            current_bit_count = 0;
        }

        switch(* str_pointer)
        {
            case '1':
                {
                    current_byte <<= 1; 
                    current_byte |= 1; 
                    current_bit_count++; 
                    break;
                }
            case '0':
            {
                current_byte <<= 1; 
                current_bit_count++; 
                break;
            }
            default:
            {
                cout << "An error has occurred" << endl << "Compression process aborted" << endl;
            }
            fclose(compressed_file_pointer);
            remove(&string_compressed[0]);
            return 1;
        }
        str_pointer++;
    }

    total_bits += len * (e->number);
    cout << "Character: " << e->character << endl << " Weight: " << e->number << endl <<  " Transformation: " << e->bit << endl;
    cout << "-------------------------------" << endl;
}
if(total_bits % 8 != 0)
{
    total_bits = (total_bits / 8 + 1) * 8;        
    // from this point on total bits doesn't represent total bits
    // instead it represents 8 * number_of_bytes we are gonna use on our compressed file
}
// Above loop writes the translation script into compressed file and the string_array array
//----------------------------------------
```

```c
// min_1 and min_2 represents nodes that has minimum weights
// is_leaf is the pointer that traverses through leafs and
// not_leaf is the pointer that traverses through nodes that are not leafs
seed * min_1 = array;
seed * min_2 = array + 1;                               //Create a pointer to the second minimum weight node.   
seed * current = array + letter_counter;                //Create a pointer to the current node.
seed * not_leaf = array + letter_counter;               //Create a pointer to the current node that is not a leaf.
seed * is_leaf = array + 2;                             //Create a pointer to the current leaf.
//Create the translation tree.
for(int i = 0; i < letter_counter - 1; i++)
{                           
    current->number = min_1->number + min_2->number;
    current->left = min_1;
    current->right = min_2;
    min_1->bit = "1";
    min_2->bit = "0";     
    current++;
    //If the current leaf is the last leaf then the current node is the last node.
    if(is_leaf >= array + letter_counter)
    {
        min_1 = not_leaf;
        not_leaf++;
    }
    else
    {
        if(is_leaf->number < not_leaf->number)
        {
            min_1 = is_leaf;
            is_leaf++;
        }
        else
        {
            min_1 = not_leaf;
            not_leaf++;
        }
    }
    if(is_leaf >= array + letter_counter)
    {
        min_2 = not_leaf;
        not_leaf++;
    }
    else if(not_leaf >= current)
    {
        min_2 = is_leaf;
        is_leaf++;
    }
    else
    {
        if(is_leaf->number < not_leaf->number)
        {
            min_2 = is_leaf;
            is_leaf++;
        }
        else
        {
            min_2 = not_leaf;
            not_leaf++;
        }
    }
}
// At every cycle, 2 of the least weighted nodes will be chosen to
// create a new node that has weight equal to sum of their weights combined.
// After we are done with these nodes they will become childrens of created nodes
// and they will be passed so that they wont be used in this process again.
```

```c
//Create the translation tree's string array.
for(e = array + letter_counter * 2 - 2; e > array - 1; e--)
{
    if(e->left)
    {
        e->left->bit = e->bit + e->left->bit;
    }
    if(e->right)
    {
        e->right->bit = e->bit + e->right->bit;
    }

}
// In this block we are adding the bytes from root to leafs
// and after this is done every leaf will have a transformation string that corresponds to it
// Note: It is actually a very neat process. Using 4th and 5th code blocks, we are making sure that
// the most used character is using least number of bits.
// Specific number of bits we re going to use for that character is determined by weight distribution
```

## 调试分析

对于英文编码的文本文件，八字节的不同组合非常有限，可以用一棵相对较小的 Huffman 树表示，意味着能够取得理想的较高压缩率。

然而对于图片、音频、视频等文件，八字节并不存在规律，因此压缩效率是较差的，甚至由于压缩后的文件需要加上额外的信息，压缩率可能大于100%。