#include "set.h"
#include "node.h"

int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/

/*
 *  Default constructor :create an empty Set
 */
int Set::get_count_nodes() {
    return Set::Node::count_nodes;
}

/*
 *  Default constructor :create an empty Set
 */
Set::Set() : counter{0} {
    head = new Node(0, nullptr, nullptr);
    tail = new Node(0, nullptr, head);
    head->next = tail;
}

/*
 *  Conversion constructor: convert val into a singleton {val}
 */
Set::Set(int val) : Set{} {  // create an empty list
    insert_node(head, val);
}

/*
 * Constructor to create a Set from a sorted vector of unique ints
 * Create a Set with all ints in sorted vector list_of_values
 */
Set::Set(const std::vector<int>& list_of_values) : Set{} {  // create an empty list
    Node* n = head;
    
    for(int i = 0;i < std::ssize(list_of_values);i++)
    {
        insert_node(n, list_of_values[i]);
        n = n->next;
    }
}

/*
 * Copy constructor: create a new Set as a copy of Set S
 * \param S Set to copied
 * Function does not modify Set S in any way
 */
Set::Set(const Set& S) : Set{} {  // create an empty list
    Node* n_original = S.head;
    Node* n_copy = head;

    while(n_original->next != S.tail)
    {
        n_original = n_original->next;
        insert_node(n_copy, n_original->value);
        n_copy = n_copy->next;
    }
}

/*
 * Transform the Set into an empty set
 * Remove all nodes from the list, except the dummy nodes
 */
void Set::make_empty() {
    if(is_empty())
        return;

    Node* n = head->next;

    while(!is_empty())
    {
        n = n->next;
        remove_node(n->prev);
    }

    head->next = tail;
    tail->prev = head;
}

/*
 * Destructor: deallocate all memory (Nodes) allocated for the list
 */
Set::~Set() {
    make_empty();
    
    delete head;
    delete tail;
}

/*
 * Assignment operator: assign new contents to the *this Set, replacing its current content
 * \param S Set to be copied into Set *this
 * Call by valued is used
 */
Set& Set::operator=(Set S) {
    std::swap(head, S.head);
    std::swap(tail, S.tail);
    std::swap(counter, S.counter);
    
    return *this;
}

/*
 * Test whether val belongs to the Set
 * Return true if val belongs to the set, otherwise false
 * This function does not modify the Set in any way
 */
bool Set::is_member(int val) const {
    Node* n = head;

    while(n != tail)
    {
        n = n->next;

        if(n->value == val)
            return true;
    }
    
    return false;
}

/*
 * Test whether Set *this and S represent the same set
 * Return true, if *this has same elemnts as set S
 * Return false, otherwise
 */
bool Set::operator==(const Set& S) const {
    if(counter != S.counter)
        return false;
    
    Node* n_current = head;
    Node* n_compare = S.head;

    while(n_current != tail)
    {
        n_current = n_current->next;
        n_compare = n_compare->next;

        if(n_current->value != n_compare->value)
            return false;
    }
    
    return true;
}

/*
 * Three-way comparison operator: to test whether *this == S, *this < S, *this > S
 * Return std::partial_ordering::equivalent, if *this == S
 * Return std::partial_ordering::less, if *this < S
 * Return std::partial_ordering::greater, if *this > S
 * Return std::partial_ordering::unordered, otherwise
 */
std::partial_ordering Set::operator<=>(const Set& S) const {
    if(*this == S)
        return std::partial_ordering::equivalent;

    if(counter < S.counter)
    {
        Node* n_current = head->next;
        Node* n_compare = S.head;

        while(n_compare != S.tail)
        {
            n_compare = n_compare->next;

            if(n_current->value == n_compare->value)
                n_current = n_current->next;

            if(n_current == tail)
                return std::partial_ordering::less;
        }
    }
    else
    {
        Node* n_current = head;
        Node* n_compare = S.head->next;

        while(n_current != tail)
        {
            n_current = n_current->next;

            if(n_current->value == n_compare->value)
                n_compare = n_compare->next;

            if(n_compare == S.tail)
                return std::partial_ordering::greater;
        }
    }
    
    return std::partial_ordering::unordered; // remove this line
}

/*
 * Modify Set *this such that it becomes the union of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator+=(const Set& S) {
    Node* n_counter = head;
    Node* n_current = head->next;
    Node* n_other = S.head->next;

    while(n_current->next != nullptr && n_other->next != nullptr)
    {
        if(n_current->value < n_other->value)
        {
            insert_node(n_counter, n_current->value);
            n_counter = n_counter->next;
            n_current = n_current->next;
        }
        else if(n_current->value > n_other->value)
        {
            insert_node(n_counter, n_other->value);
            n_counter = n_counter->next;
            n_other = n_other->next;
        }
        else
        {
            insert_node(n_counter, n_current->value);
            n_counter = n_counter->next;
            n_current = n_current->next;
            n_other = n_other->next;
        }
    }

    while(n_current != tail)
    {
        insert_node(n_counter, n_current->value);
        n_counter = n_counter->next;
        n_current = n_current->next;
    }

    while(n_other != S.tail)
    {
        insert_node(n_counter, n_other->value);
        n_counter = n_counter->next;
        n_other = n_other->next;
    }

    n_counter = n_counter->next;
    while(n_counter != tail)
    {
        n_counter = n_counter->next;
        remove_node(n_counter->prev);
    }
    
    return *this;
}

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator*=(const Set& S) {
    Node* n_current = head->next;
    Node* n_other = S.head->next;

    while(n_current != tail && n_other != S.tail)
    {
        if(n_current->value < n_other->value)
        {
            n_current = n_current->next;
            remove_node(n_current->prev);
        }
        else if(n_current->value > n_other->value)
        {
            n_other = n_other->next;
        }
        else
        {
            n_current = n_current->next;
            n_other = n_other->next;
        }
    }

    while(n_current != tail)
    {
        n_current = n_current->next;
        remove_node(n_current->prev);
    }
    
    return *this;
}

/*
 * Modify Set *this such that it becomes the Set difference between Set *this and Set S
 * Set *this is modified and then returned
 */
Set& Set::operator-=(const Set& S) {
    Node* n_current = head->next;
    Node* n_other = S.head->next;

    while(n_current != tail && n_other != S.tail)
    {
        if(n_current->value == n_other->value)
        {
            n_current = n_current->next;
            n_other = n_other->next;
            remove_node(n_current->prev);
        }
        else if(n_current->value < n_other->value)
        {
            n_current = n_current->next;
        }
        else
        {
            n_other = n_other->next;
        }
    }
    
    return *this;
}


/* ******************************************** *
 * Private Member Functions -- Implementation   *
 * ******************************************** */

/*
 * Insert a new Node storing val after the Node pointed by p
 * \param p pointer to a Node
 * \param val value to be inserted  after position p
 */
void Set::insert_node(Node* p, int val) {
    Node* n = new Node(val, p->next, p);
    p->next = p->next->prev = n;
    counter++;
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) {
    p->next->prev = p->prev;
    p->prev->next = p->next;
    delete p;
    counter--;
}

/*
 * Write Set *this to stream os
 */
void Set::write_to_stream(std::ostream& os) const {
    if (is_empty()) {
        os << "Set is empty!";
    } else {
        Set::Node* ptr{head->next};

        os << "{ ";
        while (ptr != tail) {
            os << ptr->value << " ";
            ptr = ptr->next;
        }
        os << "}";
    }
}
