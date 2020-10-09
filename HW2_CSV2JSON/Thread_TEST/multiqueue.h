#include <queue>

using std::queue;

typedef unsigned int uint;

template < class T > class MultiQueue {
  public:
    queue< T > *queues;

    /**
     * set numbers and new queues
     * */
    MultiQueue ( uint queues ) {
        this->size = queues;
        this->queues = new queue< T >[this->size];
    }

    /**
     * push to certain queue based on current queue elements
     * */
    void push ( T value ) {
        // count elements
        uint elements = 0;
        for ( int i = 0; i < this->size; ++i )
            elements += this->queues[i].size ( );

        // elements + 1
        ++elements;
        // push to which queue
        uint target = elements % this->size;
        // push to target queue
        this->queues[target].push ( value );
    }


    uint size;
  private:
};