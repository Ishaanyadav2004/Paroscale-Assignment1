# Paroscale-Assignment1
I have successfully completed this Assignment using custom sharded hash Tables implemented as array of linked list to store unique number.Multiple pthreads concurently process different segments adding number to global hash table.Each shard or independent segment is protected by it owm mutex.

# Complie Command
gcc -o app Assignment_1.c -pthread

# Run Command
./app input.txt

# Output:Screenshot
![image alt](https://github.com/Ishaanyadav2004/Paroscale-Assignment1/blob/main/Screenshot%20from%202025-07-03%2017-34-46.png?raw=true)

![iamge alt](https://github.com/Ishaanyadav2004/Paroscale-Assignment1/blob/main/Screenshot%20from%202025-07-03%2017-33-34.png?raw=true)



