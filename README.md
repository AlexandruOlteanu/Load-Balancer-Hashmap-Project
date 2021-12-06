Copyright: Alexandru Olteanu (alexandruolteanu2001@gmail.com)
 
 
   I chose to use vectors of data structures for this topic
to store servers and hashmaps. Once the Load Balancer is initialized, add it
to a new server I allocated memory for a new server, I searched for the corresponding position
to store the server so that after that add the hashes of the labels of all
servers to be in ascending order and I moved all servers to the right starting
from the position found +1 to be able to store information on the new server !. Of course, once
with the addition of a new server, you can move products to its 3 replicas so
for each one, in addition, we remapped all the products from the successor's server
current to make sure that the products are always stored on the right server. At the time
when I find a product that needs to be moved, I move it to the current server and then remove it
on the successor server!
   If a server needs to be removed, we can easily demonstrate that all products are always the same
which it contains will be moved to the successor server. Thus, to each
in part I store it on the next server. To delete the server, we do
a move to his right until he reaches the last position after which we release
memory and decrement the number of servers.
   To store a product we go through each server in ascending order and if
its hash is greater than the product hash, we store it on this server.
At retrieve, we only look for the product by key in all servers.
    In the end, we had to take into account the release of the entire memory in order not to have
memory leaks.
    Thus, implementing these functions, we managed to create the desired Load Balancer
to store products efficiently on servers as evenly as possible.
