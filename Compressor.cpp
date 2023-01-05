#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

using namespace std;

void WriteFromUnsignedChar(unsigned char, unsigned char &, int, FILE *);

int NotFolder(char *);
long int FileSize(char *);
void FolderCounter(string, long int *, long int &, long int &);

void WriteFileCounter(int, unsigned char &,int, FILE *);
void WriteFileSize(long int, unsigned char &, int, FILE *);
void WriteFileName(char *, string *, unsigned char &, int &, FILE *);
void WriteFile(FILE *, long int, string *, unsigned char &, int &, FILE *);
void WriteFolder(string, string *, unsigned char &, int &, FILE *);

//this structure will be used to create the translation tree
struct seed
{   
    seed * left, * right;
    long int number;
    unsigned char character;
    string bit;
};

bool SeedCompare(seed a, seed b)
{
    return (a.number < b.number);
}

//argc is the number of arguments being passed into your program from the command line 
//and argv is the array of arguments.
int main(int argc, char ** argv)
{
    long int number[256];   //Store how many times that all of the unique bytes is used on the files and file names and folder names.
    long int total_bits = 0;    //Store how many bits will be used to store the file and file names and folder names.
    int letter_counter = 0;     //Store how many unique bytes are used on the files and file names and folder names.
    //If argc is 1 then there is no argument passed to the program
    if(argc == 1)
    {
        cout << "Missing file! " << endl;
        return 0;
    }
    //Initialize the number array
    for(long int * i = number; i < number + 256; i++)
    {                       
        * i = 0;
    }
    
    string string_compressed;
    FILE * original_file_pointer, * compressed_file_pointer;

    //Check if the input is not correct.
    for(int i = 1; i < argc; i++)       
    {                    
        if(NotFolder(argv[i]))
        {
            original_file_pointer = fopen(argv[i], "rb");
            if(!original_file_pointer)
            {
                cout << argv[i] << "File does not exist. " << endl;
                return 0;
            }
            fclose(original_file_pointer);
        }
    }

    string_compressed = argv[1];            //Store the name of the compressed file.
    string_compressed += ".compressed";     //Create the compressed file's name, add the .compressed extension to the original file's name.

    //Count how many times that all of the unique bytes is used on the files and file names and folder names.
    //Store that into number[].

    unsigned char * x_pointer, x;                   //Temporary variables to take input from the file
    x_pointer = & x;
    long int total_size = 0, size;
    total_bits += 16 + 9 * (argc - 1);              //Calculate how many bits will be used to store the file and file names and folder names.   
    for(int current_file = 1; current_file < argc; current_file++)
    {

        //counting usage frequency of unique bytes on the file name (or folder name)
        for(char * c = argv[current_file]; * c != 0; c++)
        {        
            number[(unsigned char) (* c)]++;
        }

        if(NotFolder(argv[current_file]) == true)
        {
            total_size += size = FileSize(argv[current_file]);      //Calculate the total size of the files.
            total_bits += 64;                                       //Calculate how many bits will be used to store the file and file names and folder names.
            original_file_pointer = fopen(argv[current_file], "rb");        //Open the file to read.
            fread(x_pointer, 1, 1, original_file_pointer);                  //Read the first byte of the file.
            //counting usage frequency of unique bytes inside the file
            for(long int j = 0; j < size; j++)
            {     
                number[x]++;
                fread(x_pointer, 1, 1, original_file_pointer);
            }
            fclose(original_file_pointer);                                  //Close the file.
        }
        else
        {
            string temp = argv[current_file];                               //Store the name of the folder.
            FolderCounter(temp, number, total_size, total_bits);            //Count how many times that all of the unique bytes is used on the files and file names and folder names.
        }        
    }

    //Count how many letters.
	for(long int * i = number; i < number + 256; i++)
    {                 
        if(* i != 0)
        {
            letter_counter++;
        }
    }

    // creating the base of translation array(and then sorting them by ascending frequencies
    // this array of type 'seed' will not be used after calculating transformed versions of every unique byte
    // instead its info will be written in a new string array called string_array 
    seed array[letter_counter * 2 - 1];                     //Create the translation tree's base.
    seed * e = array;                                       //Create a pointer to the translation tree's base.

    //Initialize the translation tree's base.
    for(long int * i = number; i < number + 256; i++)
    {                         
        if(* i != 0)
        { 
            e->right = NULL;                //Initialize the right pointer of the current seed.
            e->left = NULL;                 //Initialize the left pointer of the current seed.
            e->number = * i;                //Initialize the number of the current seed to the number of the current unique byte.
            e->character = i - number;      //
            e++;                            //e is a pointer to the next element of the translation tree's base.
        }
    }
    sort(array, array + letter_counter, SeedCompare);
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


    compressed_file_pointer = fopen(& string_compressed[0], "wb");      //Open the compressed file to write.
    int current_bit_count = 0;
    unsigned char current_byte;

    fwrite(& letter_counter, 1, 1, compressed_file_pointer);
    total_bits += 8;

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


    // cout << "The size of the sum of ORIGINAL files is: " << total_size << " bytes" << endl;
    // cout << "The size of the COMPRESSED file will be: " << total_bits / 8 << " bytes" << endl;
    // cout << "Compressed file's size will be [%" << 100 * ((float)total_bits / 8 / total_size) << "] of the original file" << endl;
    // if(total_bits / 8 > total_size)
    // {
    //     cout << endl << "COMPRESSED FILE'S SIZE WILL BE HIGHER THAN THE SUM OF ORIGINALS" << endl << endl;
    // }
    // cout << "If you wish to abort this process write 0 and press enter" << endl
    //     << "If you want to continue write any other number and press enter" << endl;
    // int check;
    // cin >> check;
    // if(!check)
    // {
    //     cout << endl << "Process has been aborted" << endl;
    //     fclose(compressed_file_pointer);
    //     remove(&string_compressed[0]);
    //     return 0;
    // }



    // PROGRESS.MAX = (array + letter_counter * 2 - 2)->number;      //setting progress bar

    //-------------writes fourth---------------
    WriteFileCounter(argc - 1, current_byte, current_bit_count, compressed_file_pointer);
    //---------------------------------------

    for(int current_file = 1; current_file < argc; current_file++)
    {
        //if current is a file and not a folder
        if(NotFolder(argv[current_file]))
        {   
            original_file_pointer = fopen(argv[current_file], "rb");
            fseek(original_file_pointer, 0, SEEK_END);
            size = ftell(original_file_pointer);
            rewind(original_file_pointer);

            //-------------writes fifth--------------
            if(current_bit_count == 8)
            {
                fwrite(&current_byte, 1, 1, compressed_file_pointer);
                current_bit_count = 0;
            }
            current_byte <<= 1;
            current_byte |= 1;
            current_bit_count++;
            //---------------------------------------

            WriteFileSize(size,current_byte, current_bit_count, compressed_file_pointer);             //writes sixth
            WriteFileName(argv[current_file], string_array, current_byte, current_bit_count, compressed_file_pointer);   //writes seventh
            WriteFile(original_file_pointer, size, string_array, current_byte, current_bit_count, compressed_file_pointer);      //writes eighth
            fclose(original_file_pointer);
        }
        else    //if current is a folder instead
        {   

            //-------------writes fifth--------------
            if(current_bit_count == 8)
            {
                fwrite(&current_byte, 1, 1, compressed_file_pointer);
                current_bit_count=0;
            }
            current_byte <<= 1;
            current_bit_count++;
            //---------------------------------------

            WriteFileName(argv[current_file], string_array, current_byte, current_bit_count, compressed_file_pointer);   //writes seventh

            string folder_name = argv[current_file];
            WriteFolder(folder_name, string_array, current_byte, current_bit_count, compressed_file_pointer);
        }
    }





    if(current_bit_count == 8)
    {      // here we are writing the last byte of the file
        fwrite(&current_byte, 1, 1, compressed_file_pointer);
    }
    else
    {
        current_byte <<= 8 - current_bit_count;
        fwrite(&current_byte, 1, 1, compressed_file_pointer);
    }

    fclose(compressed_file_pointer);
    system("clear");
    cout << endl << "Created compressed file: " << string_compressed << endl;
    cout << "Compression is complete" << endl;
    
}

//below function is used for writing the uChar to compressed file
//It does not write it directly as one byte instead it mixes uChar and current byte, writes 8 bits of it and puts the rest to curent byte for later use
void WriteFromUnsignedChar(unsigned char uChar,unsigned char &current_byte, int current_bit_count, FILE * fp_write)
{
    current_byte <<= 8 - current_bit_count;
    current_byte |= (uChar >> current_bit_count);
    fwrite(& current_byte, 1, 1, fp_write);
    current_byte = uChar;   
}

//below function is writing number of files we re going to translate inside current folder to compressed file's 2 bytes. It is done like this to make sure that it can work on little, big or middle-endian systems
void WriteFileCounter(int file_count, unsigned char &current_byte, int current_bit_count, FILE * compressed_file_pointer)
{
    unsigned char temp = file_count%256;
    WriteFromUnsignedChar(temp, current_byte, current_bit_count, compressed_file_pointer);
    temp = file_count / 256;
    WriteFromUnsignedChar(temp, current_byte, current_bit_count, compressed_file_pointer);
}

//This function is writing byte count of current input file to compressed file using 8 bytes
//It is done like this to make sure that it can work on little, big or middle-endian systems
void WriteFileSize(long int size, unsigned char &current_byte, int current_bit_count, FILE * compressed_file_pointer)
{
    // PROGRESS.next(size);        //updating progress bar
    for(int i = 0; i < 8; i++)
    {
        WriteFromUnsignedChar(size % 256, current_byte, current_bit_count, compressed_file_pointer);
        size /= 256;
    }
}

// This function writes bytes that are translated from current input file's name to the compressed file.
void WriteFileName(char *file_name, string * string_array, unsigned char & current_byte, int & current_bit_count, FILE * compressed_file_pointer)
{
    WriteFromUnsignedChar(strlen(file_name), current_byte, current_bit_count, compressed_file_pointer);
    char * str_pointer;
    for(char * c = file_name; * c; c++)
    {
        str_pointer = & string_array[(unsigned char)(* c)][0];
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
                    current_byte <<= 1; 
                    current_byte |= 1; 
                    current_bit_count++; 
                    break;
                case '0': 
                    current_byte <<= 1; 
                    current_bit_count++; 
                    break;
                default:
                    cout << "An error has occurred. " << endl << "Process aborted. ";
                exit(2);
            }
            str_pointer++;
        }
    }
}

// Below function translates and writes bytes from current input file to the compressed file.
void WriteFile(FILE * original_file_pointer, long int size, string * string_array, unsigned char &current_byte, int & current_bit_count, FILE * compressed_file_pointer)
{
    unsigned char * x_pointer, x;
    x_pointer = & x;
    char * str_pointer;
    fread(x_pointer, 1, 1, original_file_pointer);
    for(long int i = 0; i < size; i++)
    {
        str_pointer = & string_array[x][0];
        while(* str_pointer)
        {
            if(current_bit_count == 8)
            {
                fwrite(& current_byte, 1, 1, compressed_file_pointer);
                current_bit_count = 0;
            }
            switch(* str_pointer)
            {
                case '1':
                    current_byte <<= 1;
                    current_byte |= 1;
                    current_bit_count++;
                    break;
                case '0':
                    current_byte <<= 1;
                    current_bit_count++;
                    break;
                default:    
                    cout << "An error has occurred. " << endl << "Process aborted. ";
                exit(2);
            }
            str_pointer++;
        }
        fread(x_pointer, 1, 1, original_file_pointer);
    }
}

int NotFolder(char * path)
{
    DIR * temp = opendir(path);     //open directory
    //Is a Folder
    if(temp != NULL)
    {
        closedir(temp);
        return 0;
    }
    //Not a Folder
    return 1;
}

long int FileSize(char * path)
{
    long int size;                                                
    FILE * fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

// This function counts usage frequency of bytes inside a folder only give folder path as input
void FolderCounter(string path, long int * number, long int & total_size, long int & total_bits)
{
    FILE * original_file_pointer;
    path += '/';
    DIR * dir= opendir(& path[0]), * next_dir;
    string next_path;
    total_size += 4096;
    total_bits += 16; //for file_count
    struct dirent * current;
    while((current = readdir(dir)))
    {
        if(current->d_name[0] == '.')
        {
            if(current->d_name[1] == 0)
                continue;
            if(current->d_name[1] == '.' && current->d_name[2] == 0)
                continue;
        }
        total_bits += 9;

        //counting usage frequency of bytes on the file name (or folder name)
        for(char * c = current->d_name; * c; c++)
        {        
            number[(unsigned char)(* c)]++;
        }

        next_path = path + current->d_name;
        //
        if((next_dir = opendir(& next_path[0])))
        {
            closedir(next_dir);
            FolderCounter(next_path, number, total_size, total_bits);
        }
        else
        {
            long int size;
            unsigned char * x_pointer, x;
            x_pointer = & x;
            total_size += size = FileSize(& next_path[0]);
            total_bits += 64;

            //--------------------2------------------------
            original_file_pointer = fopen(& next_path[0], "rb");

            fread(x_pointer, 1, 1, original_file_pointer);
            //counting usage frequency of bytes inside the file
            for(long int j = 0; j < size; j++)
            {    
                number[x]++;
                fread(x_pointer, 1, 1, original_file_pointer);
            }
            fclose(original_file_pointer);
        }
    }
    closedir(dir);
}

void WriteFolder(string path, string * string_array, unsigned char & current_byte, int & current_bit_count, FILE * compressed_file_pointer)
{
    FILE * original_file_pointer;
    path += '/';
    DIR * dir = opendir(& path[0]), * next_dir;
    string next_path;
    struct dirent * current;
    int file_count = 0;
    long int size;
    while((current=readdir(dir)))
    {
        if(current->d_name[0] == '.')
        {
            if(current->d_name[1] == 0)
                continue;
            if(current->d_name[1] == '.' && current->d_name[2] == 0)
                continue;
        }
        file_count++;
    }
    rewinddir(dir);
    WriteFileCounter(file_count, current_byte, current_bit_count, compressed_file_pointer);  //writes fourth

    while((current = readdir(dir)))     //if current is a file
    {  
        if(current->d_name[0] == '.')
        {
            if(current->d_name[1] == 0)
                continue;
            if(current->d_name[1] == '.' && current->d_name[2] == 0)
                continue;
        }

        next_path = path + current->d_name;
        if(NotFolder(& next_path[0]))
        {

            original_file_pointer = fopen(& next_path[0], "rb");
            fseek(original_file_pointer, 0, SEEK_END);
            size = ftell(original_file_pointer);
            rewind(original_file_pointer);

            //-------------writes fifth--------------
            if(current_bit_count == 8)
            {
                fwrite(&current_byte, 1, 1, compressed_file_pointer);
                current_bit_count = 0;
            }
            current_byte <<= 1;
            current_byte |= 1;
            current_bit_count++;
            //---------------------------------------

            WriteFileSize(size, current_byte, current_bit_count, compressed_file_pointer);                     //writes sixth
            WriteFileName(current->d_name, string_array, current_byte, current_bit_count, compressed_file_pointer);                //writes seventh
            WriteFile(original_file_pointer, size, string_array, current_byte, current_bit_count, compressed_file_pointer);      //writes eighth
            fclose(original_file_pointer);
        }
        // if current is a folder
        else
        {   

            //-------------writes fifth--------------
            if(current_bit_count == 8)
            {
                fwrite(& current_byte, 1, 1, compressed_file_pointer);
                current_bit_count = 0;
            }
            current_byte <<= 1;
            current_bit_count++;
            //---------------------------------------

            WriteFileName(current->d_name, string_array, current_byte, current_bit_count, compressed_file_pointer);   //writes seventh

            WriteFolder(next_path, string_array, current_byte, current_bit_count, compressed_file_pointer);
        }
    }
    closedir(dir);
}