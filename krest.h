#ifndef KREST_H
#define KREST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// General constants
constexpr int8_t KR_POINTER_SIZE_B = sizeof(void*);

// Stack constants
constexpr int16_t DEFAULT_STACK_CAPACITY_S = 128;
constexpr int8_t STACK_HEADER_SIZE_B = 16;
constexpr int8_t STACK_HEADER_SIZE_S = 2;
constexpr int8_t DEFAULT_STACK_SIZE_S = 0;
constexpr int8_t GET_STACK_SIZE_INDEX = -1;
constexpr int8_t GET_STACK_CAPACITY_INDEX = -2;

// Queue constants
constexpr int16_t DEFAULT_QUEUE_CAPACITY_S = 128;
constexpr int8_t QUEUE_HEADER_SIZE_B = 32;
constexpr int8_t QUEUE_HEADER_SIZE_S = 4;
constexpr int8_t DEFAULT_QUEUE_SIZE_S = 0;
constexpr int8_t DEFAULT_QUEUE_HEAD_S = 0;
constexpr int8_t DEFAULT_QUEUE_TAIL_S = 0;
constexpr int8_t GET_QUEUE_TAIL_INDEX = -1;
constexpr int8_t GET_QUEUE_HEAD_INDEX = -2;
constexpr int8_t GET_QUEUE_SIZE_INDEX = -3;
constexpr int8_t GET_QUEUE_CAPACITY_INDEX = -4;

// Deque constants
constexpr int16_t KR_DEFAULT_DEQUE_BLOCK_SIZE_S = 128;
constexpr int8_t KR_DEQUE_HEADER_SIZE_S = 6;
constexpr int8_t KR_DEFAULT_DEQUE_MAP_CAP_S = 16;
constexpr int8_t KR_DEFAULT_DEQUE_MAP_SIZE_S = 0;
constexpr int8_t KR_DEFAULT_DEQUE_SIZE_S = 0;
constexpr int16_t KR_DEFAULT_DEQUE_HEAD_INDEX = 0;
constexpr int16_t KR_DEFAULT_DEQUE_TAIL_INDEX = 0;

// Error and error handling constants
constexpr int8_t MAX_ERROR_CODE_AMOUNT = 100;
constexpr int8_t MAX_ATTEMPT = 5;

typedef uint8_t KrByte;

// Errors are handled by subtracting the returned sentinel value from SIZE_MAX which gives us the enum value of the error
// So error handling logic is Impossible Sentinel Values + Data Multiplexing for KrByte* returning functions
// Others just return KrError, but it would be dumb to return NULL input and let you check for it again so user needs
// to do their own null checks since that will result in the same number of ifs if not less
typedef enum {

    KR_SUCCESS,
    KR_ERR_MALLOC_FAIL,
    KR_ERR_REALLOC_FAIL,
    KR_ERR_NULL_INPUT,
    KR_ERR_INDEX_OOB,     // Out of Bounds
    KR_ERR_SMALL_INPUT,
    KR_ERR_TYPE_MISMATCH,
    KR_ERR_INVALID_STRATEGY,
    KR_ERR_EMPTY,

} KrError;

typedef enum {

    KR_BOOL,
    KR_U8,
    KR_U16,
    KR_U32,
    KR_U64,
    KR_I8,
    KR_I16,
    KR_I32,
    KR_I64,
    KR_F32,
    KR_F64,
    KR_CHAR,
    KR_STRING,
    KR_VOID_PTR,
    KR_UNKNOWN,

} KrTypes;

typedef enum {

    KR_DO_LEFT = false,
    KR_DO_RIGHT = true,

} KrBlockDirections;

// Structs
typedef struct {

    size_t map_cap;
    size_t map_size;
    size_t dque_size;
    size_t head;
    size_t tail;

} KrDqueHeader;

#define GET_TYPE_AS_STRING(x) _Generic((x),    \
    bool:       "bool",                        \
    uint8_t:    "u8/byte",                     \
    uint16_t:   "u16",                         \
    uint32_t:   "u32",                         \
    uint64_t:   "u64",                         \
    int8_t:     "i8",                          \
    int16_t:    "i16",                         \
    int32_t:    "i32",                         \
    int64_t:    "i64",                         \
    float:      "f32",                         \
    double:     "f64",                         \
    char:       "char/byte",                   \
    char*:      "string",                      \
    void*:      "void*",                       \
    default:    "unknown"                      \
 )

#define GET_TYPE_AS_ENUM(x) _Generic((x),      \
    bool:       KR_BOOL,                       \
    uint8_t:    KR_U8,                         \
    uint16_t:   KR_U16,                        \
    uint32_t:   KR_U32,                        \
    uint64_t:   KR_U64,                        \
    int8_t:     KR_I8,                         \
    int16_t:    KR_I16,                        \
    int32_t:    KR_I32,                        \
    int64_t:    KR_I64,                        \
    float:      KR_F32,                        \
    double:     KR_F64,                        \
    char:       KR_CHAR,                       \
    char*:      KR_STRING,                     \
    void*:      KR_VOID_PTR,                   \
    default:    KR_UNKNOWN                     \
)

// General macros
#define kr_is_err(ptr) (size_t)(ptr) >= SIZE_MAX - MAX_ERROR_CODE_AMOUNT
#define kr_get_err(ptr) (KrError)(SIZE_MAX - (size_t)(ptr))
#define kr_memrev(buf, size) _kr_memrev((KrByte*)(buf), (size_t)(size))

// Stack macros
#define kr_stk_new(type) (type*)_kr_stk_new(sizeof(type))
#define kr_stk_cap(stack) ((size_t*)stack)[GET_STACK_CAPACITY_INDEX]
#define kr_stk_size(stack) ((size_t*)stack)[GET_STACK_SIZE_INDEX]
#define kr_stk_clear(stack) kr_stk_size((stack)) = 0
#define kr_stk_push(stack, value) _kr_stk_push((KrByte**)(&stack), (KrByte*)(typeof(stack[0])[]){value}, sizeof(stack[0]))
#define kr_stk_pop(stack, dest) _kr_stk_pop((KrByte*)(stack), (KrByte*)(&dest), sizeof(stack[0]))
#define kr_stk_fit(stack) _kr_stk_fit((KrByte**)(&stack), sizeof(stack[0]))
#define kr_stk_res(stack, size) _kr_stk_res((KrByte**)(&stack), (size_t)(size), sizeof(stack[0]))
#define kr_stk_is_empty(stack) kr_stk_size((stack)) == 0
#define kr_stk_peek(stack, dest) _kr_stk_peek((KrByte*)(stack), (KrByte*)(&dest), sizeof(stack[0]))
#define kr_stk_free(stack) _kr_stk_free((KrByte**)(&stack))
#define kr_stk_avail(stack) kr_stk_cap((stack)) - kr_stk_size((stack))

// Queue macros
#define kr_que_new(type) (type*)_kr_que_new(sizeof(type))
#define kr_que_cap(queue) ((size_t*)queue)[GET_QUEUE_CAPACITY_INDEX]
#define kr_que_size(queue) ((size_t*)queue)[GET_QUEUE_SIZE_INDEX]
#define kr_que_head(queue) ((size_t*)queue)[GET_QUEUE_HEAD_INDEX]
#define kr_que_tail(queue) ((size_t*)queue)[GET_QUEUE_TAIL_INDEX]
#define kr_que_enq(queue, value) _kr_que_enq((KrByte**)(&queue), (KrByte*)(typeof(queue[0])[]){value}, sizeof(queue[0]))
#define kr_que_deq(queue, dest) _kr_que_deq((KrByte*)(queue), (KrByte*)(&dest), sizeof(queue[0]))
#define kr_que_clear(queue) _kr_que_clear((KrByte*)(queue))
#define kr_que_free(queue) _kr_que_free((KrByte**)(&queue))
#define kr_que_is_empty(queue) kr_que_size((queue)) < 1
#define kr_que_avail(queue) kr_que_cap((queue)) - kr_que_size((queue))
#define kr_que_peek(queue, dest) _kr_que_peek((KrByte*)(queue), (KrByte*)(&dest), sizeof(queue[0]))
#define kr_que_fit(queue) _kr_que_fit((KrByte**)(&queue), sizeof(queue[0]))
#define kr_que_res(queue, size) _kr_que_res((KrByte**)(&queue), (size_t)(size), sizeof(queue[0]))

// Deque macros
#define kr_dque_new(type) (type*)_kr_dque_new(sizeof(type))
#define kr_dque_hdr(dque) (KrDqueHeader*)((KrByte*)(dque) - sizeof(KrDqueHeader))
#define kr_dque_pushb(dque, value) _kr_dque_pushb((KrByte**)(&dque), (KrByte*)(typeof(dque[0])[]){value}, sizeof(dque[0]))
#define kr_dque_popb(dque, dest) _kr_dque_popb((KrByte**)(&dque), (KrByte*)(&dest), sizeof(dque[0]))
#define kr_dque_free(dque) _kr_dque_free((KrByte**)(&dque))
#define kr_dque_at(dque, dest, index) _kr_dque_at((KrByte**)(&dque), (KrByte*)(&dest), (index), sizeof(dque[0]))
#define kr_dque_is_empty(dque) _kr_dque_is_empty((KrByte**)(&dque))

// General functions
KrError _kr_memrev(KrByte* buf, const size_t size);
void _kr_auto_init() __attribute__((constructor));

// Stack functions
KrByte* _kr_stk_new(const size_t type_size);
KrError _kr_stk_push(KrByte** stk, const KrByte* value, const size_t type_size);
KrError _kr_stk_pop(KrByte* stk, KrByte* dest, const size_t type_size);
KrError _kr_stk_fit(KrByte** stk, const size_t type_size);
KrError _kr_stk_res(KrByte** stk, const size_t new_cap, const size_t type_size);
KrError _kr_stk_peek(KrByte* stk, KrByte* dest, const size_t type_size);
KrError _kr_stk_free(KrByte** stk);

// Queue functions
KrByte* _kr_que_new(const size_t type_size);
KrError _kr_que_enq(KrByte** que, const KrByte* value, const size_t type_size);
KrError _kr_que_deq(KrByte* que, KrByte* dest, const size_t type_size);
KrError _kr_que_clear(KrByte* que);
KrError _kr_que_peek(KrByte* que, KrByte* dest, const size_t type_size);
KrError _kr_que_fit(KrByte** que, const size_t type_size);
KrError _kr_que_res(KrByte** que, const size_t new_cap, const size_t type_size);
KrError _kr_que_free(KrByte** que);

// Deque functions
KrByte* _kr_dque_new(const size_t type_size);
KrError _kr_dque_pushb(KrByte** dque, const KrByte* value, const size_t type_size);
KrError _kr_dque_popb(KrByte** dque, KrByte* dest, const size_t type_size);
KrError _kr_dque_at(KrByte** dque, KrByte* dest, const size_t index, const size_t type_size);
bool _kr_dque_is_empty(KrByte** dque);
KrError _kr_dque_free(KrByte** dque);

#endif

#ifdef KREST_IMPLEMENTATION

/*
 * **********************
 * # Internal Functions #
 * **********************
*/

void _kr_auto_init() {

    // We are not doing embedded here
    static_assert(sizeof(size_t) >= 8 && "Krest only supports 64-bit systems!");

}

void _kr_stk_malloc(KrByte** new_buf, const size_t type_size) {

    const size_t malloc_size_b = STACK_HEADER_SIZE_B + (DEFAULT_STACK_CAPACITY_S * type_size);
    for (size_t i = 0; i < MAX_ATTEMPT; i++) {

        *new_buf = malloc(malloc_size_b);
        // This check ensure the last 100 addresses are reserved for sentinel error codes since we employ data multiplexing here
        if (*new_buf && (size_t)*new_buf < (SIZE_MAX - MAX_ERROR_CODE_AMOUNT)) { break; }

    }

}

void _kr_stk_realloc(KrByte** new_buf, KrByte** stk, const size_t type_size, const KrByte growth_factor) {

    KrByte* header = *stk - STACK_HEADER_SIZE_B;

    // If cap hits zero it breaks the growth factor math since zero swallows everything when multiplied
    if (kr_stk_cap(*stk) < 1) { kr_stk_cap(*stk) = 1; }

    const size_t realloc_size_b = STACK_HEADER_SIZE_B + ((kr_stk_cap(*stk) * type_size) * growth_factor);
    for (size_t i = 0; i < MAX_ATTEMPT; i++) {

        *new_buf = realloc((void*)header, realloc_size_b);
        if (*new_buf && (size_t)*new_buf < (SIZE_MAX - MAX_ERROR_CODE_AMOUNT)) { break; }

    }

    if (!*new_buf) { return; }

    // Rebinding
    *stk = *new_buf + STACK_HEADER_SIZE_B;
    kr_stk_cap(*stk) *= growth_factor;

}

void _kr_que_malloc(KrByte** new_buf, const size_t type_size) {

    const size_t malloc_size_b = QUEUE_HEADER_SIZE_B + (DEFAULT_QUEUE_CAPACITY_S * type_size);
    for (size_t i = 0; i < MAX_ATTEMPT; i++) {

        *new_buf = malloc(malloc_size_b);
        if (*new_buf && (size_t)*new_buf < (SIZE_MAX - MAX_ERROR_CODE_AMOUNT)) { break; }

    }

}

void _kr_que_grow(KrByte** new_buf, KrByte** que, const size_t type_size, const KrByte growth_factor) {

    // If cap is 0 it breaks the cap * type_size math
    if (kr_que_cap(*que) < 1) { kr_que_cap(*que) = 1; }

    // Allocate new_buf
    const size_t malloc_size_b = QUEUE_HEADER_SIZE_B + ((kr_que_cap(*que) * type_size) * growth_factor);
    for (size_t i = 0; i < MAX_ATTEMPT; i++) {

        *new_buf = malloc(malloc_size_b);
        if (*new_buf && (size_t)*new_buf < (SIZE_MAX - MAX_ERROR_CODE_AMOUNT)) { break; }

    }

    // Safety fuse - realloc fail instead of malloc fail for consistency with other objects fit functions
    // this function is essentially a manual realloc with buffer linearization
    if (!new_buf) { return; }

    // Copy header into new_buf
    KrByte* header = *que - QUEUE_HEADER_SIZE_B;
    memcpy(*new_buf, header, QUEUE_HEADER_SIZE_B);

    // Create new handle
    KrByte* new_handle = *new_buf + QUEUE_HEADER_SIZE_B;

    if (kr_que_tail(*que) < kr_que_head(*que)) {

        // 1. First memcpy from and including head to the end of the queue into new_buf queue section
        const size_t tail_from_head_b = (kr_que_cap(*que) - kr_que_head(*que)) * type_size;
        const KrByte* head_to_mem = *que + (kr_que_head(*que) * type_size);
        memcpy(new_handle, head_to_mem, tail_from_head_b);

        // 2. Than memcpy from the start of the *que into new_buf queue section up until and excluding tail
        KrByte* resumption = new_handle + tail_from_head_b;
        memcpy(resumption, *que, kr_que_tail(*que) * type_size);

        // 3. Set tail and head accordingly
        kr_que_tail(new_handle) = kr_que_size(*que);
        kr_que_head(new_handle) = DEFAULT_QUEUE_HEAD_S;

    }
    else if (kr_que_tail(*que) > kr_que_head(*que)) {

        // We need to assume the worst possible situtation for linear grow which is head and tail being
        // in the middle of the buffer or at the end of it in that case we cant just memcpy N empty slots
        // that would be massively inefficient

        // In order to do that I need to convert head index into a memory address
        const KrByte* head_to_mem = *que + (kr_que_head(*que) * type_size);

        // Now I need to memcpy from the address above using size
        memcpy(new_handle, head_to_mem, kr_que_size(*que) * type_size);

        // Update tail and head accordingly
        kr_que_tail(new_handle) = kr_que_size(*que);
        kr_que_head(new_handle) = DEFAULT_QUEUE_HEAD_S;

    }

    // Update cap accordingly
    kr_que_cap(new_handle) *= growth_factor;

    // Manually free *que since we don't use realloc but malloc
    free(header);

}

KrError _kr_dque_map_block_add(KrByte** map_handle, const size_t type_size, const bool side) {

    // Allocating new block
    KrByte* new_buf_block = calloc(KR_DEFAULT_DEQUE_BLOCK_SIZE_S, type_size);

    // If calloc fails abort
    if (new_buf_block == nullptr) { return KR_ERR_MALLOC_FAIL; }

    // Get header
    KrDqueHeader* header = kr_dque_hdr(*map_handle);

    // Grow map if needed
    if (header->map_size >= header->map_cap) {

        // Realloc
        const size_t map_realloc_size_b = sizeof(KrDqueHeader) + ((header->map_cap * KR_POINTER_SIZE_B) * 2);
        KrByte* new_buf_map = realloc((void*)header, map_realloc_size_b);

        // Abort if failed
        if (new_buf_map == nullptr) { return KR_ERR_REALLOC_FAIL; }

        // Rebind
        *map_handle = new_buf_map + sizeof(KrDqueHeader);
        header = kr_dque_hdr(*map_handle);

        // Update cap
        header->map_cap *= 2;

    }

    // False means left, true means right
    if (side == false) {

        // TODO: Implement add block left

    }
    else if (side == true) {

        // Map size is cardinal while index values are ordinal
        // because of this map size is pointing to then next available slot
        // KrByte* map_target = *map_handle + (header->map_size * KR_POINTER_SIZE_B);
        // memcpy(map_target, &new_buf_block, KR_POINTER_SIZE_B);

        KrByte** map_slots = (KrByte**)*map_handle;
        map_slots[header->map_size] = new_buf_block;

    }

    // Deque cap needs to increase by one block and map size needs to increase by one
    header->map_size += 1;

    return KR_SUCCESS;

}

KrError _kr_dque_map_block_remove(KrByte** map_handle, const size_t type_size, const bool side) {

    // Get header
    KrDqueHeader* header = kr_dque_hdr(*map_handle);

    // We should reserve at least 1 block at all times
    if (header->map_size < 2) { return KR_ERR_SMALL_INPUT; }

    // False means left, true means right
    if (side == false) {

        // TODO: Implement remove from left

    }
    else if (side == true) {

        // Get last block of map with map_size - 1
        KrByte** last_block_ptr = ((KrByte**)*map_handle) + (header->map_size - 1);

        // Free and nullify
        free((void*)*last_block_ptr);
        ((KrByte**)*map_handle)[header->map_size - 1] = nullptr;

        // Decrement map_size and dque_cap
        header->map_size -= 1;

    }

    return KR_SUCCESS;

}

KrByte* _kr_dque_idx2coor(KrByte** map_handle, const size_t index, const size_t type_size) {

    // Calculate coordinates
    const size_t map_idx    = index / KR_DEFAULT_DEQUE_BLOCK_SIZE_S;
    const size_t block_idx  = index % KR_DEFAULT_DEQUE_BLOCK_SIZE_S;

    // Follow them and find where tail points to
    KrByte* block_addr = *(((KrByte**)*map_handle) + map_idx);  // God have mercy...
    KrByte* item_addr = block_addr + (block_idx * type_size);

    // Return a pointer to it
    return item_addr;

}

/*
 * ********************
 * # General Funtions #
 * ********************
*/

KrError _kr_memrev(KrByte* buf, const size_t size) {

    // Error handling
    if (!buf) { return KR_ERR_NULL_INPUT; }
    if (size == 0) { return KR_ERR_EMPTY; }

    // Actual reverse
    KrByte* head = buf;
    KrByte* tail = &buf[size - 1];  // Value to index convertion
    KrByte temp = 0;
    while (head < tail) {

        // Store head in temp, put tail in head, put temp in tail
        temp = *head;
        *head = *tail;
        *tail = temp;

        // Increase head, decrease tail
        head += 1;
        tail -= 1;

    }

    return KR_SUCCESS;

}

bool _kr_dque_is_empty(KrByte** dque) {

    // Get header
    const KrDqueHeader* header = kr_dque_hdr(*dque);

    // Is empty
    return header->dque_size < 1;

}

/*
 * *******************
 * # Object Creation #
 * *******************
*/

KrByte* _kr_stk_new(const size_t type_size) {

    // Creation
    KrByte* new_buf = nullptr;
    _kr_stk_malloc(&new_buf, type_size);

    if (!new_buf) { return (KrByte*)(SIZE_MAX - KR_ERR_MALLOC_FAIL); }

    // Initialization
    ((size_t*)new_buf)[0] = DEFAULT_STACK_CAPACITY_S;
    ((size_t*)new_buf)[1] = DEFAULT_STACK_SIZE_S;

    return new_buf + STACK_HEADER_SIZE_B;

}

KrByte* _kr_que_new(const size_t type_size) {

    // Creation
    KrByte* new_buf = nullptr;
    _kr_que_malloc(&new_buf, type_size);

    if (!new_buf) { return (KrByte*)(SIZE_MAX - KR_ERR_MALLOC_FAIL); }

    // Initialization
    ((size_t*)new_buf)[0] = DEFAULT_QUEUE_CAPACITY_S;
    ((size_t*)new_buf)[1] = DEFAULT_QUEUE_SIZE_S;
    ((size_t*)new_buf)[2] = DEFAULT_QUEUE_HEAD_S;
    ((size_t*)new_buf)[3] = DEFAULT_QUEUE_TAIL_S;

    return new_buf + QUEUE_HEADER_SIZE_B;

}

KrByte* _kr_dque_new(const size_t type_size) {

    // We are going to implement a block-map deque the overall logic of it is simple we have a map that map has pointers
    // to block we use / and % operators to calculate our coordinates get a pointer in desired block and operate on that

    // Let's start creating a header object and mapping it
    KrDqueHeader map_header = {0};  // ZII
    map_header.map_cap = KR_DEFAULT_DEQUE_MAP_CAP_S;
    map_header.map_size = KR_DEFAULT_DEQUE_MAP_SIZE_S;
    map_header.dque_size = KR_DEFAULT_DEQUE_SIZE_S;
    map_header.head = KR_DEFAULT_DEQUE_HEAD_INDEX;
    map_header.tail = KR_DEFAULT_DEQUE_TAIL_INDEX;

    // Now we need to allocate map buffer itself
    const size_t map_buf_malloc_size_b = sizeof(map_header) + (KR_DEFAULT_DEQUE_MAP_CAP_S * KR_POINTER_SIZE_B);
    KrByte* new_buf_map = calloc(map_buf_malloc_size_b, 1);

    // If malloc fails abort
    if (new_buf_map == nullptr) { return (KrByte*)(SIZE_MAX - KR_ERR_MALLOC_FAIL); }

    // Put header into buffer
    memcpy(new_buf_map, &map_header, sizeof(map_header));

    // Map handle creation
    KrByte* map_handle = new_buf_map + sizeof(map_header);

    // Also let's not forget to add one block for the road
    _kr_dque_map_block_add(&map_handle, type_size, KR_DO_RIGHT);

    return map_handle;

}

/*
 * ****************
 * # Reading Data #
 * ****************
*/

KrError _kr_stk_peek(KrByte* stk, KrByte* dest, const size_t type_size) {

    // Error handling
    if (kr_stk_is_empty(stk)) { return KR_ERR_EMPTY; }

    // Actual peek
    const KrByte* peek = &stk[(kr_stk_size(stk) - 1) * type_size];
    memcpy(dest, peek, type_size);

    return KR_SUCCESS;

}

KrError _kr_que_peek(KrByte* que, KrByte* dest, const size_t type_size) {

    // Error handling
    if (kr_que_is_empty(que)) { return KR_ERR_EMPTY; }

    // Actual peek
    const KrByte* peek = &que[kr_que_head(que) * type_size];
    memcpy(dest, peek, type_size);

    return KR_SUCCESS;

}

KrError _kr_dque_at(KrByte** dque, KrByte* dest, const size_t index, const size_t type_size) {

    // We are going to create another virtual number line from head to tail for this implementation
    // since pushf uses head to push to front we are conceptually consistant and valid

    // Get header
    KrDqueHeader* header = kr_dque_hdr(*dque);

    // Convert supplied index to virtual index our conceptual contract can understand
    // so basically virtual to virtual conversion, crazy right
    const size_t virtual_index = header->head + index;

    // Find the item using conceptually consistent virtual index and load it into destination buffer
    const KrByte* item = _kr_dque_idx2coor(dque, index, type_size);
    memcpy(dest, item, type_size);

    return KR_SUCCESS;

}

/*
 * ***************
 * # Adding Data #
 * ***************
*/

KrError _kr_stk_push(KrByte** stk, const KrByte* value, const size_t type_size) {

    // Realloc if needed
    if (kr_stk_size(*stk) + 1 > kr_stk_cap(*stk)) {

        KrByte* new_buf = nullptr;
        _kr_stk_realloc(&new_buf, stk, type_size, 2);

        if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

    }

    // Actual push
    KrByte* dest = *stk + (kr_stk_size(*stk) * type_size);
    memcpy(dest, value, type_size);
    kr_stk_size(*stk) += 1;

    return KR_SUCCESS;

}

KrError _kr_que_enq(KrByte** que, const KrByte* value, const size_t type_size) {

    // Resize if needed
    if (kr_que_size(*que) + 1 >= kr_que_cap(*que)){

        KrByte* new_buf = nullptr;
        _kr_que_grow(&new_buf, que, type_size, 2);

        if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

        // Rebind
        *que = new_buf + QUEUE_HEADER_SIZE_B;

    }

    // Actual enqueue
    memcpy((*que) + (kr_que_tail(*que) * type_size), value, type_size);
    kr_que_size(*que) += 1;
    kr_que_tail(*que) = (kr_que_tail(*que) + 1) % kr_que_cap(*que);

    return KR_SUCCESS;

}

KrError _kr_dque_pushb(KrByte** dque, const KrByte* value, const size_t type_size) {

    // Get header
    KrDqueHeader* header = kr_dque_hdr(*dque);

    // Check if we are going to need another block
    if (header->tail + 1 > header->map_size * KR_DEFAULT_DEQUE_BLOCK_SIZE_S) {

        _kr_dque_map_block_add(dque, type_size, KR_DO_RIGHT);
        header = kr_dque_hdr(*dque);

    }

    // Actual pushb
    KrByte* slot_addr = _kr_dque_idx2coor(dque, header->tail, type_size);
    memcpy(slot_addr, value, type_size);
    header->tail += 1;
    header->dque_size += 1;

    return KR_SUCCESS;

}

/*
 * *****************
 * # Removing Data #
 * *****************
*/

KrError _kr_stk_pop(KrByte* stk, KrByte* dest, const size_t type_size) {

    // If there is nothing to pop
    if (kr_stk_is_empty(stk)) { return KR_ERR_EMPTY; }

    // Actual pop
    kr_stk_size(stk) -= 1;
    const KrByte* src = &stk[ kr_stk_size(stk) * type_size ];
    memcpy(dest, src, type_size);

    return KR_SUCCESS;

}

KrError _kr_que_deq(KrByte* que, KrByte* dest, const size_t type_size) {

    // Error handling
    if (kr_que_size(que) < 1) { return KR_ERR_EMPTY; }

    // Actual deq
    const KrByte* deq = &que[kr_que_head(que) * type_size];
    kr_que_head(que) = (kr_que_head(que) + 1) % kr_que_cap(que);
    kr_que_size(que) -= 1;
    memcpy(dest, deq, type_size);

    return KR_SUCCESS;

}

KrError _kr_dque_popb(KrByte** dque, KrByte* dest, const size_t type_size) {

    // Get header
    KrDqueHeader* header = kr_dque_hdr(*dque);

    // If deque is empty abort
    if (header->dque_size < 1 || header->tail < 1) { return KR_ERR_EMPTY; }

    // Let's make the tail point to the last element instead of next slot
    header->tail -= 1;

    // Remove block if last block is empty and we are at below 80% of the current block
    const bool last_block_empty = header->tail < (header->map_size * KR_DEFAULT_DEQUE_BLOCK_SIZE_S) - KR_DEFAULT_DEQUE_BLOCK_SIZE_S;
    const bool hysteresis = ((header->tail - 1) % KR_DEFAULT_DEQUE_BLOCK_SIZE_S) < (KR_DEFAULT_DEQUE_BLOCK_SIZE_S * 0.80);
    if (last_block_empty && hysteresis) { _kr_dque_map_block_remove(dque, type_size, KR_DO_RIGHT); }

    // Actual popb
    const KrByte* popb_item = _kr_dque_idx2coor(dque, header->tail, type_size);
    memcpy(dest, popb_item, type_size);
    header->dque_size -= 1;

    return KR_SUCCESS;

}

/*
 * *********************
 * # Memory Management #
 * *********************
*/

KrError _kr_stk_fit(KrByte** stk, const size_t type_size) {

    // Shrink the allocated memory to fit the used memory
    kr_stk_cap(*stk) = kr_stk_size(*stk);
    KrByte* new_buf = nullptr;
    _kr_stk_realloc(&new_buf, stk, type_size, 1);

    if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

    return KR_SUCCESS;

}

KrError _kr_stk_res(KrByte** stk, const size_t new_cap, const size_t type_size) {

    KrByte* new_buf = nullptr;
    KrByte* handle = *stk - STACK_HEADER_SIZE_B;
    const size_t new_size_b = STACK_HEADER_SIZE_B + (new_cap * type_size);

    // No need to shrink we have kr_stk_fit for that so it's a no-op
    if (new_cap <= kr_stk_cap(*stk)) { return KR_SUCCESS; }

    for (size_t i = 0; i < MAX_ATTEMPT; i++) {

        new_buf = realloc((void*)handle, new_size_b);
        if (new_buf && (size_t)new_buf < (SIZE_MAX - MAX_ERROR_CODE_AMOUNT)) { break; }

    }

    if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

    *stk = new_buf + STACK_HEADER_SIZE_B;
    kr_stk_cap(*stk) = new_cap;

    return KR_SUCCESS;

}

KrError _kr_que_fit(KrByte** que, const size_t type_size) {

    // Shrink the allocated memory to fit the used memory
    // Cap needs to be size + 1 because we want to reserve the last slot for the tail in a linear configuration
    // otherwise it may break unit tests and it is a pain in the ass to debug I can never get what values should be right...
    kr_que_cap(*que) = kr_que_size(*que) + 1;
    KrByte* new_buf = nullptr;
    _kr_que_grow(&new_buf, que, type_size, 1);  // Linearize buffer to aid fragmentation

    // Rebind
    *que = new_buf + QUEUE_HEADER_SIZE_B;

    if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

    return KR_SUCCESS;

}

KrError _kr_que_res(KrByte** que, const size_t new_cap, const size_t type_size) {

    // No need to shrink we have kr_stk_fit for that so it's a no-op
    if (new_cap <= kr_stk_cap(*que)) { return KR_SUCCESS; }

    // Set cap to new_cap
    kr_que_cap(*que) = new_cap;

    // Linearize and memcpy buffer into new_buf
    KrByte* new_buf = nullptr;
    _kr_que_grow(&new_buf, que, type_size, 1);

    if (!new_buf) { return KR_ERR_REALLOC_FAIL; }

    // Rebind
    *que = new_buf + QUEUE_HEADER_SIZE_B;

    return KR_SUCCESS;

}

/*
 * ********************
 * # Resetting Object #
 * ********************
*/

KrError _kr_que_clear(KrByte* que) {

    kr_que_head(que) = DEFAULT_QUEUE_HEAD_S;
    kr_que_tail(que) = DEFAULT_QUEUE_TAIL_S;
    kr_que_size(que) = DEFAULT_QUEUE_SIZE_S;

    return KR_SUCCESS;

}

/*
 * ******************
 * # Freeing Memory #
 * ******************
*/

KrError _kr_stk_free(KrByte** stk) {

    if (!stk || !*stk) { return KR_ERR_NULL_INPUT; }

    KrByte* handle = *stk - STACK_HEADER_SIZE_B;

    free((void*)handle);
    *stk = nullptr;

    return KR_SUCCESS;

}

KrError _kr_que_free(KrByte** que) {

    if (!que || !*que) { return KR_ERR_NULL_INPUT; }

    KrByte* header = *que - QUEUE_HEADER_SIZE_B;

    free((void*)header);
    *que = nullptr;

    return KR_SUCCESS;

}

KrError _kr_dque_free(KrByte** dque) {

    if (dque == nullptr || *dque == nullptr) { return KR_ERR_NULL_INPUT; }

    KrDqueHeader* header = kr_dque_hdr(*dque);

    free((void*)header);
    *dque = nullptr;

    return KR_SUCCESS;

}

#endif
