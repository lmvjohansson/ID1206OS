#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int num_threads = 0;
typedef struct node {
    int node_id; //a unique ID assigned to each node
    struct node *next;
} Node;

typedef struct thread_arg{ // Struct to allow passing unique id and which push or pop function to use
    int thread_id;
    int opt;
} ThreadArg;

Node *top; // top of stack
pthread_mutex_t stack_lock; //mutex for stack
int global_id = 0; // Global counter for unique node IDs
/*Option 1: Mutex Lock*/
void push_mutex() {
    Node *old_node;
    Node *new_node;
    new_node = malloc(sizeof(Node));

    pthread_mutex_lock(&stack_lock); // start critical section

    new_node->node_id = global_id; // assign unique ID
    global_id++;

    new_node->next = top; // link new node
    top = new_node; // update top

    pthread_mutex_unlock(&stack_lock); // end critical section
}
int pop_mutex() {
    Node *old_node;
    Node *new_node;

    pthread_mutex_lock(&stack_lock);  // start critical section

    old_node = top; // node to be popped
    new_node = old_node->next; // new top node
    int old_node_id = old_node->node_id; // save node id to return

    top = new_node; // update top
    free(old_node); // free old_node

    pthread_mutex_unlock(&stack_lock); // end critical section

    return old_node_id;
}
/*Option 2: Compare-and-Swap (CAS)*/
void push_cas() {
    Node *old_node;
    Node *new_node;
    new_node = malloc(sizeof(Node));
    //update top of the stack below
    //assign a unique ID to the new node
}
int pop_cas() {
    Node *old_node;
    Node *new_node;
    //update top of the stack below
    return old_node->node_id;
}
/* the thread function */
void *thread_func(void *arg){ // Changed parameter since pthreads requires a void *arg parameter to work
    ThreadArg *targ = (ThreadArg *)arg;
    int my_id = targ->thread_id;
    int opt = targ->opt;
    if( opt==0 ){
        push_mutex();push_mutex();pop_mutex();pop_mutex();push_mutex();
    }else{
        push_cas();push_cas();pop_cas();pop_cas();push_cas();
    }
    printf("Thread %d: exit\n", my_id);
    pthread_exit(0);
}
int main(int argc, char *argv[]) {
    num_threads = atoi(argv[1]);
    /* Option 1: Mutex */
    pthread_t *workers;
    workers = malloc(num_threads * sizeof(pthread_t));
    int thread_ids[num_threads]; // Array to assign unique thread ids
    pthread_mutex_init(&stack_lock, NULL); // Initialize mutex
    ThreadArg thread_args[num_threads]; // Struct to allow passing of thread id and which algorithm to use
    for (int i = 0; i < num_threads; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        thread_args[i].thread_id = i;
        thread_args[i].opt = 0;
        pthread_create(&workers[i], NULL, thread_func, &thread_args[i]);
    }
    for (int i = 0; i < num_threads; i++)
        pthread_join(workers[i], NULL);
    //Print out all remaining nodes in Stack
    printf("Mutex: Remaining nodes \n");
    int node_output;
    while (top != NULL) {
        node_output = pop_mutex();
        printf("Node: %d\n", node_output);
    }

    free(workers);
    pthread_mutex_destroy(&stack_lock);/*free up resources properly */

    // /* Option 2: CAS */
    // for (int i = 0; i < num_threads; i++) {
    //     pthread_attr_t attr;
    //     pthread_attr_init(&attr);
    //     pthread_create(...);
    // }
    // for (int i = 0; i < num_threads; i++)
    //     pthread_join(...);
    // //Print out all remaining nodes in Stack
    // printf("CAS: Remaining nodes \n");
    // /*free up resources properly */
}