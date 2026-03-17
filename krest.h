#ifndef KREST_H
#define KREST_H

// Definitions
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_STRING_CAPACITY 32
#define DEFAULT_VECTOR_CAPACITY 16
#define NULL_TERMINATOR_LEN 1

#define autofree_krstring __attribute__((cleanup(_kr_free_string_ptr)))
#define autofree_krvector __attribute__((cleanup(_kr_free_vector_ptr)))

// Array list type string object with _size, _capacity and _data fields
typedef struct {

    size_t _size;        // Metadata, _size of string
    size_t _capacity;    // Metadata, _size of allocated memory
    char* _data;         // Array, start of string

} KrString;

// Return type containing start and end indices for string objects
typedef struct {

    size_t _start;
    size_t _end;

} KrSlicePos;

// View object for KrString that doesnt own it just peeks into it
typedef struct {

    char* _data;
    size_t _size;

} KrStringView;

// Array list object but dynamic due to carrying type _size it can be set to anytype this way
typedef struct {

    size_t _size;
    size_t _capacity;
    size_t _type_size;
    void* _data;

} KrVector;

// Error codes enum
typedef enum {

    KR_SUCCESS,
    KR_ERR_MALLOC_FAIL,
    KR_ERR_REALLOC_FAIL,
    KR_ERR_NULL_INPUT,
    KR_ERR_INDEX_OOB,   // Out of Bound
    KR_ERR_SMALL_INPUT,
    KR_ERR_NOT_FOUND,


} KrError;

// Updates the _capacity by reallocating memory to the _data prop of the KrString object
bool _kr_update_string_capacity(KrString* str);

// Sets string _capacity to the minimum possible value
bool kr_minimize_string_capacity(KrString* str);

// Returns a new KrString object, user needs to free it with kr_free_string or declare it with autofree_krstring
KrString kr_new_string(const char src[]);

// Returns a new KrVector object, user needs to free it with kr_free_vector or declare it with autofree_krvector
KrVector kr_new_vector(size_t _type_size);

// Appends src at the end of dst
KrError kr_append_string(KrString* dst, const char src[]);

// Reverses the order of bytes of a given buffer, starting from 0 up to _size - 1
KrError kr_reverse_bytes(uint8_t* src, size_t _size);

// Appends src at the end of dst
KrError kr_append_to_vector(KrVector* dst, const void* src);

// Returns a void* user can assign with any type pointer since they defined the _size of each type at vector creation
void* kr_get_vector_element(const KrVector* src, size_t index);

// Inserts one char into string at specified index
KrError kr_insert_char(KrString* dst, char src, size_t index);

// Inserts src into string at specified index
KrError kr_insert_string(KrString* dst, const char src[], size_t index);

// Returns a new KrSting objects containing a slice of the inputted string, excludes last char and includes first char
KrStringView kr_slice_string(const KrString* src, size_t index, size_t size);

// Returns true if KrString->_data is an empty string
bool kr_is_empty_string(const KrString* str);

// Returns true if str is not an empty string and is all whitespaces
bool kr_is_string_all_whitespace(const KrString* str);

// Rreturns a KrSlicePos object containing start and end indices
KrSlicePos kr_find_substring(const KrString* str, const char src[]);

// Returns a KrVector object containing all occurrences of the substring in the form of KrSlicePos
KrVector kr_find_all_substrings(const KrString* str, const char src[]);

// Replaces src substring with 'with' substring
KrError kr_replace_first_substring(KrString* str, const char src[], const char with[]);

// Replaces all occurrences of src with 'with' substring
KrError kr_replace_all_substrings(KrString* str, const char src[], const char with[]);

// Replaces char at index with 'with'
KrError kr_replace_char(KrString* str, char with, size_t index);

// Changes strings _data prop
KrError kr_set_string_data(KrString* str, const char data[]);

// Splits the str1 in half stores first half in str1 second half in str2, char at index is the start of str2
KrError kr_split_string(KrString* str1, KrString* str2, size_t index);

// Note: Preallocate once with enough or more _size than you think you will need
// so KrString doesn't need to reallocate memory everytime it reaches its _capacity
KrError kr_preallocate_string(KrString* str, size_t capacity);

// Note: Preallocate once with enough or more _size than you think you will need
// so KrVector doesn't need to reallocate memory everytime it reaches its _capacity
KrError kr_preallocate_vector(KrVector* vector, size_t size_byte);

// Returns true if strings are equal
bool kr_is_equal_string(const KrString* str1, const KrString* str2);

// Returns a KrString objects pointing to the same memory as str
KrString kr_shallow_copy_string(const KrString* str);

// Returns a newly allocated KrString object with the same _data prop
KrString kr_deep_copy_string(const KrString* str);

// Removes the last element of the string
KrError kr_string_pop_back(KrString* str);

// Removes the first element of the string
KrError kr_string_pop(KrString* str);

// Removes whitespaces at the beginning and end of the string
KrError kr_trim_string(KrString* src);

// Free KrVector object from memory
KrError kr_free_vector(KrVector* vector);

// Free KrString object from memory
KrError kr_free_string(KrString* str);

// autofree macro dispatch functions
void _kr_free_string_ptr(KrString* s);
void _kr_free_vector_ptr(KrVector* v);

// Generic error handling macro dispatch functions for each type
static inline bool _kr_string_is_err(const KrString* s) { return s->_size == SIZE_MAX; }
static inline bool _kr_vector_is_err(const KrVector* v) { return v->_size == SIZE_MAX; }
static inline bool _kr_string_view_is_err(const KrStringView* sw) { return sw->_size == SIZE_MAX; }
static inline bool _kr_slice_pos_is_err(const KrSlicePos* sp) { return sp->_start == SIZE_MAX; }
static inline bool _kr_error_is_err(const KrError* e) { return *e != KR_SUCCESS; }

static inline KrError _kr_string_get_error(const KrString* s);
static inline KrError _kr_vector_get_error(const KrVector* v);
static inline KrError _kr_string_view_get_error(const KrStringView* sw);
static inline KrError _kr_slice_pos_get_error(const KrSlicePos* sp);
static inline KrError _kr_error_get_error(const KrError* e);

#define kr_is_error(obj) _Generic((obj),                                       \
    KrString:       _kr_string_is_err((const KrString*)&(obj)),                \
    KrVector:       _kr_vector_is_err((const KrVector*)&(obj)),                \
    KrStringView:   _kr_string_view_is_err((const KrStringView*)&(obj)),       \
    KrSlicePos:     _kr_slice_pos_is_err((const KrSlicePos*)&(obj)),           \
    KrError:        _kr_error_is_err((const KrError*)&(obj))                   \
)

static inline KrError _kr_string_get_error(const KrString* s) { return (KrError)(uintptr_t)s->_data; }
static inline KrError _kr_vector_get_error(const KrVector* v) { return (KrError)(uintptr_t)v->_data; }
static inline KrError _kr_string_view_get_error(const KrStringView* sw) { return (KrError)(uintptr_t)sw->_data; }
static inline KrError _kr_slice_pos_get_error(const KrSlicePos* sp) { return (KrError)(uintptr_t)sp->_end; }
static inline KrError _kr_error_get_error(const KrError* e) { return *e; }

#define kr_get_error(obj) _Generic((obj),                                       \
    KrString:       _kr_string_get_error((const KrString*)&(obj)),              \
    KrVector:       _kr_vector_get_error((const KrVector*)&(obj)),              \
    KrStringView:   _kr_string_view_get_error((const KrStringView*)&(obj)),     \
    KrSlicePos:     _kr_slice_pos_get_error((const KrSlicePos*)&(obj)),         \
    KrError:        _kr_error_get_error((const KrError*)&(obj))                 \
)

#endif //KREST_H

#ifdef KREST_IMPLEMENTATION

// Implementations
#include <stdio.h>
#include <string.h>

void _kr_free_string_ptr(KrString* s) { if(s->_data && s->_size != SIZE_MAX) kr_free_string(s); }
void _kr_free_vector_ptr(KrVector* v) { if (v->_data && v->_size != SIZE_MAX) kr_free_vector(v); }

__attribute__((constructor))
void _kr_auto_init() {

    // We are not doing embeded here
    static_assert(sizeof(size_t) <= 8 && "Krest only supports 64-bit systems!");

}

// These functions are examples of Impossible Sentinel Values + Data Multiplexing
// SIZE_MAX = 18,446,744,073,709,551,615 in 64-bit systems which explains _kr_auto_init function above
// It's always a bigger value then there is memory addresses in the computer saying its astronomicaly large would be an
// understatement SIZE_MAX is the known universe of numbers inside computers we then put the error_code somewhere in
// the struct we are already going to return anyway which is the multiplexing partthat's why it's called a zero-cost
// abstraction but since its complicated I handle it inside the library and use _Generic macro to dispatch/paste
// according to the type passed as argument by the C Preprocessor at compile-time
inline KrSlicePos _kr_return_err_splice_pos(const KrError error_code) {

    return (KrSlicePos) {

        ._start = SIZE_MAX,
        ._end = (size_t)error_code

    };

}

inline KrString _kr_return_err_string(char* data_ptr, const KrError error_code) {

    if (data_ptr != (char*)SIZE_MAX) { free(data_ptr); }

    return (KrString) {

        ._size = SIZE_MAX,
        ._capacity = 0,
        ._data = (char*)error_code

    };

}

inline KrStringView _kr_return_err_string_view(const KrError error_code) {

    return (KrStringView) {

        ._size = SIZE_MAX,
        ._data = (char*)error_code

    };

}

inline KrVector _kr_return_err_vector(char* data_ptr, const KrError error_code) {

    if (data_ptr != (char*)SIZE_MAX) { free(data_ptr); }

    return (KrVector) {

        ._size = SIZE_MAX,
        ._capacity = 0,
        ._type_size = 1,    // To avoid division by zero errors or maybe I should let the user fail I am not sure
        ._data = (void*)error_code

    };

}

bool _kr_update_string_capacity(KrString* str) {

    // Error handling
    if (!str) { return false; }

    // Just in case
    if (str->_capacity == 0) { str->_capacity = DEFAULT_STRING_CAPACITY; }

    // _capacity update if necessary
    const size_t old_capacity = str->_capacity;
    size_t new_capacity = str->_capacity;
    while (str->_size + NULL_TERMINATOR_LEN >= new_capacity) { new_capacity *= 2; }

    // Ensure to only realloc if there is a change in _capacity
    if (old_capacity != new_capacity) {

        // realloc is smart enough to act like malloc when user inputs nullptr to the first argument
        // whole functions banks on it when it comes to empty strings or user inputting nullptr
        char* new_mem = realloc(str->_data, new_capacity);
        if (new_mem == nullptr) { return false; }

        // Else
        str->_capacity = new_capacity;
        str->_data = new_mem;

    }

    return true;

}

inline bool _kr_check_replace_helper(const KrString* str, const char src[], const char with[]) {

    if (!str || !src || !with || strlen(src) > str->_size) { return false; }

    return true;

}

bool kr_minimize_string_capacity(KrString* str) {

    const size_t used_capacity = str->_size + NULL_TERMINATOR_LEN;
    if (str->_capacity > used_capacity) {

        char* new_mem = realloc(str->_data, used_capacity);
        if (!new_mem) { return false; }

        str->_capacity = used_capacity;
        str->_data = new_mem;

    }

    return true;

}

KrString kr_new_string(const char src[]) {

    if (!src) { return _kr_return_err_string((char*)SIZE_MAX, KR_ERR_NULL_INPUT); }

    // Create new KrString object
    KrString s = {0};

    // Initialise
    s._size = strlen(src);

    if (s._size + NULL_TERMINATOR_LEN >= DEFAULT_STRING_CAPACITY) { s._capacity = s._size + DEFAULT_STRING_CAPACITY; }
    else { s._capacity = DEFAULT_STRING_CAPACITY; }

    char* new_data = malloc(s._capacity);
    if (!new_data) {
        free(new_data);
        return _kr_return_err_string((char*)SIZE_MAX, KR_ERR_MALLOC_FAIL);
    }
    // Else
    s._data = new_data;

    // _data transfer
    memcpy(s._data, src, s._size + NULL_TERMINATOR_LEN);

    return s;

}

KrVector kr_new_vector(size_t _type_size) {

    KrVector vector;
    vector._capacity = DEFAULT_VECTOR_CAPACITY;

    void* new_data = malloc(_type_size * vector._capacity);
    if (!new_data) {
        free(new_data);
        return _kr_return_err_vector((char*)SIZE_MAX, KR_ERR_MALLOC_FAIL);
    }
    // Else
    vector._data = new_data;

    vector._type_size = _type_size;
    vector._size = 0;

    return vector;

}

KrError kr_append_string(KrString* dst, const char src[]) {

    if (!dst || !src) { return KR_ERR_NULL_INPUT; }

    // Calculate new _size and update dst's _capacity accordingly
    const size_t old_size = dst->_size;
    const size_t src_len = strlen(src);
    dst->_size = dst->_size + src_len;
    if (!_kr_update_string_capacity(dst)) {

        // By doing this I avoid zombie state if realloc fails we reset the state before return since this function
        // does mutation
        dst->_size = old_size;
        return KR_ERR_REALLOC_FAIL;

    }

    // Appending
    memcpy(&dst->_data[old_size], src, src_len + NULL_TERMINATOR_LEN);

    return KR_SUCCESS;

}

KrError kr_reverse_bytes(uint8_t* src, size_t size) {
    // Nothing to reverse
    if (size < 2) { return KR_ERR_SMALL_INPUT; }

    // Notice the type uint8_t* it's a pointer to a char/uint8_t I use it to peek at bytes, it's load-agnostic logic
    uint8_t* front = src;
    uint8_t* back = src + size - 1;
    uint8_t  temp = 0;

    while (front < back) {

        // The classic 3-step swap
        temp = *front;
        *front = *back;
        *back = temp;

        front++;
        back--;

    }

    return KR_SUCCESS;

}

KrError kr_append_to_vector(KrVector* dst, const void* src) {

    if (!dst || !src) { return KR_ERR_NULL_INPUT; }

    if (dst->_size + 1 >= dst->_capacity) {

        dst->_capacity *= 2;
        void* new_data = realloc(dst->_data, dst->_capacity * dst->_type_size);
        if (!new_data) { return KR_ERR_REALLOC_FAIL; }
        // Else
        dst->_data = new_data;

    }

    // Actual append
    memcpy(&dst->_data[dst->_size * dst->_type_size], src, dst->_type_size);
    dst->_size += 1;

    return KR_SUCCESS;

}

void* kr_get_vector_element(const KrVector* src, size_t index) {

    return &src->_data[index * src->_type_size];

}

KrError kr_insert_char(KrString* dst, char src, size_t index) {

    if (!dst) { return KR_ERR_NULL_INPUT; }
    if (index >= dst->_size) { return KR_ERR_INDEX_OOB; }

    // Calculate new _size and update _capacity
    const size_t old_size = dst->_size;
    dst->_size = dst->_size + sizeof(src);
    if (!_kr_update_string_capacity(dst)) { return KR_ERR_REALLOC_FAIL; }

    // 1 here means start moving into after the insert gap, do not move what will be inserted char
    memmove(&dst->_data[index] + 1, &dst->_data[index], old_size - index);

    // Insertion
    // +1 here is needed since we change the _size old index points to a different place than it used to
    // amount of increment to index is equal to how far away we moved the memory after index which is +1
    // as reader can see above in the first argument of the memmove
    dst->_data[index] = src;
    dst->_data[dst->_size] = '\0';

    return KR_SUCCESS;

}

KrError kr_insert_string(KrString* dst, const char src[], size_t index) {

    if (!dst || !src) { return KR_ERR_NULL_INPUT; }

    if (index >= dst->_size) { return KR_ERR_INDEX_OOB; }

    const size_t src_len = strlen(src);
    if (src_len == 0) { return KR_ERR_SMALL_INPUT; }

    // Calculate new _size and update _capacity
    const size_t old_size = dst->_size;
    dst->_size = dst->_size + src_len;
    if (!_kr_update_string_capacity(dst)) { return KR_ERR_REALLOC_FAIL; }

    // Create gap
    memmove(&dst->_data[index + src_len], &dst->_data[index], old_size - index);

    // Insertion
    memcpy(&dst->_data[index], src, src_len);
    dst->_data[dst->_size] = '\0';

    return KR_SUCCESS;

}

KrStringView kr_slice_string(const KrString* src, size_t index, size_t size) {

    if (!src) { return _kr_return_err_string_view(KR_ERR_NULL_INPUT); }

    return (KrStringView) { ._data = &src->_data[index], ._size = size };

}

bool kr_is_empty_string(const KrString* str) {

    if (!str || !str->_data) { return false; }

    if (str->_data[0] == '\0') { return true; }
    // Else if not empty
    return false;

}

bool kr_is_string_all_whitespace(const KrString* str) {

    if (!str) { return false; }
    if (kr_is_empty_string(str)) { return false; }

    for (size_t i = 0; i < str->_size; i++) {

        if (str->_data[i] != ' ') { return false; }

    }

    return true;

}

KrSlicePos kr_find_substring(const KrString* str, const char src[]) {

    KrSlicePos pos = { 0, 0, };

    if (!str || !src || kr_is_error(*str)) { return _kr_return_err_splice_pos(KR_ERR_NULL_INPUT); }

    if (strlen(src) > str->_size) { return _kr_return_err_splice_pos(KR_ERR_INDEX_OOB); }

    const size_t src_size = strlen(src);
    if (src_size == 0) { return _kr_return_err_splice_pos(KR_ERR_SMALL_INPUT); }

    // Number of iterations = main string length - substring length
    // Sliding window implementation
    bool found = false;
    for (size_t i = 0; i <= str->_size - src_size; i++) {

        if (memcmp(&str->_data[i], src, src_size) == 0) {

            // -1 is a convertion from string length to array index value
            pos._start = i;
            pos._end = src_size + i - 1;
            found = true;
            break;

        }

    }

    if (!found) { return _kr_return_err_splice_pos(KR_ERR_NOT_FOUND); }

    return pos;

}

KrError kr_replace_first_substring(KrString* str, const char src[], const char with[]) {

    if (!_kr_check_replace_helper(str, src, with)) { return KR_ERR_NULL_INPUT; }

    const KrSlicePos pos = kr_find_substring(str, src);
    if (kr_is_error(pos)) { return KR_ERR_NOT_FOUND; }

    const ssize_t diff = (ssize_t)strlen(with) - (ssize_t)strlen(src);
    if (diff >= 0) {

        str->_size += diff;
        if (!_kr_update_string_capacity(str)) { return KR_ERR_REALLOC_FAIL; }

    }

    // Calculating how much and what to move
    // +1 here is present because pos.end is exclusive
    char* tail = &str->_data[pos._end + 1];
    const size_t bytes_to_move = strlen(tail) + NULL_TERMINATOR_LEN;

    // We don't need to use memmove if there is nothing to move meaning the gap is where it needs to be
    if (diff != 0) memmove(tail + diff, tail, bytes_to_move);
    memcpy(&str->_data[pos._start], with, strlen(with));

    return KR_SUCCESS;

}

KrVector kr_find_all_substrings(const KrString* str, const char src[]) {

    if (!str || !src || kr_is_error(*str)) { return _kr_return_err_vector((char*)SIZE_MAX, KR_ERR_NULL_INPUT); }

    // Positions array
    KrVector positions = kr_new_vector(sizeof(KrSlicePos));

    const size_t src_len = strlen(src);
    if (src_len == 0) { return _kr_return_err_vector(positions._data, KR_ERR_SMALL_INPUT); }

    // Sliding window implementation and occurrence storing
    const size_t step_amount = str->_size - src_len;
    KrSlicePos pos = { 0, 0 };
    bool found = false;
    for (size_t i = 0; i <= step_amount; i++) {

        // Start from ith offset go as long as length of src and compare with src, 0 means no difference has been found
        if (memcmp(&str->_data[i], src, src_len) == 0) {

            // -1 means pos.end includes the char after the end for some reason I don't know why
            pos._start = i;
            pos._end = src_len + i - 1;

            KrError ret = kr_append_to_vector(&positions, &pos);
            if (kr_is_error(ret)) { printf("Cought append error!\n"); exit(0); }

            found = true;

        }

    }

    if (!found) { return _kr_return_err_vector(positions._data, KR_ERR_NOT_FOUND); }

    return positions;

}

KrError kr_replace_all_substrings(KrString* str, const char src[], const char with[]) {

    if (!_kr_check_replace_helper(str, src, with)) { return KR_ERR_NULL_INPUT; }

    // Positions array
    autofree_krvector const KrVector positions = kr_find_all_substrings(str, src);
    if (kr_is_error(positions)) { return KR_ERR_NOT_FOUND; }

    if (positions._size == SIZE_MAX) { return KR_ERR_NOT_FOUND; }

    // Length of src
    const size_t src_len = strlen(src);

    // Construction of the new string buffer
    const ssize_t diff_per_occurrence = (ssize_t)strlen(with) - (ssize_t)src_len;
    const ssize_t cum_diff_of_changes = diff_per_occurrence * (ssize_t)positions._size;
    const size_t final_size = str->_size + cum_diff_of_changes;

    char* new_buffer = malloc(final_size  + NULL_TERMINATOR_LEN * sizeof(char));
    if (!new_buffer) {
        free(new_buffer);
        return KR_ERR_MALLOC_FAIL;
    }
    new_buffer[final_size] = '\0';

    // Filling new_buffer with double-pointer technique
    size_t nb_cursor = 0;
    size_t str_cursor = 0;
    size_t gap_len = 0;
    const size_t with_len = strlen(with);
    for (size_t i = 0; i < positions._size; i++) {

        const KrSlicePos* pos = kr_get_vector_element(&positions, i);

        gap_len = pos->_start - str_cursor;
        if (gap_len > 0) {

            memcpy(&new_buffer[nb_cursor], &str->_data[str_cursor], gap_len);
            nb_cursor += gap_len;

        }

        memcpy(&new_buffer[nb_cursor], with, with_len);
        nb_cursor += with_len;
        str_cursor = pos->_end + 1;  // +1 means start from the next char dont include pos->end

    }

    // This is needed because we iterate over positions found that leaves the tail from the last pos in the old buffer
    const size_t tail_len = str->_size - str_cursor;
    if (tail_len > 0) {

        memcpy(&new_buffer[nb_cursor], &str->_data[str_cursor], tail_len);
        nb_cursor += tail_len;

    }

    // Binding new_buffer to str
    str->_size = final_size;
    str->_capacity = final_size + DEFAULT_STRING_CAPACITY + NULL_TERMINATOR_LEN;
    free(str->_data);
    str->_data = new_buffer;

    return KR_SUCCESS;

}

KrError kr_replace_char(KrString* str, char with, size_t index) {

    if (!str) { return KR_ERR_NULL_INPUT; }

    if (index >= str->_size) { return KR_ERR_INDEX_OOB; }

    str->_data[index] = with;

    return KR_SUCCESS;

}

KrError kr_set_string_data(KrString* str, const char data[]) {

    if (!str || !data) { return KR_ERR_NULL_INPUT; }

    const size_t old_len = str->_size;
    str->_size = strlen(data);
    if (!_kr_update_string_capacity(str)) {
        str->_size = old_len;
        return KR_ERR_REALLOC_FAIL;
    }

    memcpy(str->_data, data, str->_size + NULL_TERMINATOR_LEN);

    return KR_SUCCESS;

}

KrError kr_split_string(KrString* str1, KrString* str2, size_t index) {

    if (!str1 || !str2) { return KR_ERR_NULL_INPUT; }
    if (index >= str1->_size) { return KR_ERR_INDEX_OOB; }

    // Filling str2
    const size_t right_hand_size = str1->_size - index;
    const size_t old_len = str2->_size;
    str2->_size = right_hand_size;
    if (!_kr_update_string_capacity(str2)) {
        str2->_size = old_len;
        return KR_ERR_REALLOC_FAIL;
    }

    memcpy(str2->_data, &str1->_data[index], right_hand_size);
    str2->_size = right_hand_size;
    str2->_data[right_hand_size] = '\0';

    // Clipping str1
    const size_t left_hand_size = str1->_size - right_hand_size;
    str1->_size = left_hand_size;
    str1->_data[left_hand_size] = '\0';

    return KR_SUCCESS;

}

KrError kr_preallocate_string(KrString* str, size_t capacity) {

    if (!str) { return KR_ERR_NULL_INPUT; }

    // No need to reallocate
    if (capacity <= str->_capacity) { return KR_ERR_SMALL_INPUT; }

    // Resize
    const size_t old_capacity = capacity;
    char* new_mem = realloc(str->_data, capacity);
    if (new_mem == nullptr) {
        str->_capacity = old_capacity;
        return KR_ERR_REALLOC_FAIL;
    }

    // Else
    str->_capacity = capacity;
    str->_data = new_mem;

    return KR_SUCCESS;

}

KrError kr_preallocate_vector(KrVector* vector, size_t size_byte) {

    if (!vector) { return KR_ERR_NULL_INPUT; }

    // No need to reallocate
    if (size_byte <= vector->_capacity) { return KR_ERR_SMALL_INPUT; }

    const size_t old_capacity = vector->_capacity;
    vector->_capacity = size_byte;
    void* new_data = realloc(vector->_data, vector->_capacity * vector->_type_size);
    if (!new_data) {
        vector->_capacity = old_capacity;
        return KR_ERR_REALLOC_FAIL;
    }
    // Else
    vector->_data = new_data;

    return KR_SUCCESS;

}

bool kr_is_equal_string(const KrString* str1, const KrString* str2) {

    if (!str1 || !str2) { return false; }

    if (str1->_size != str2->_size) { return false; }
    if (memcmp(str1->_data, str2->_data, str1->_size) != 0) { return false; }

    return true;

}

KrString kr_shallow_copy_string(const KrString* str) {

    if (!str) { return _kr_return_err_string((char*)SIZE_MAX, KR_ERR_NULL_INPUT); }

    const KrString s = { ._capacity = str->_capacity, ._data = str->_data, ._size = str->_size };
    return s;

}

KrString kr_deep_copy_string(const KrString* str) {

    if (!str) { return _kr_return_err_string((char*)SIZE_MAX, KR_ERR_NULL_INPUT); }

    KrString s = kr_new_string(str->_data);
    s._capacity = str->_capacity;
    s._size = str->_size;

    return s;

}

KrError kr_string_pop_back(KrString* str) {

    if (!str) { return KR_ERR_NULL_INPUT; }

    if (str->_size == 0) { return KR_ERR_SMALL_INPUT; }

    // -1 here is a convertion from size_t into an index value, last element to be precise
    str->_data[str->_size - 1] = '\0';
    str->_size -= 1;

    return KR_SUCCESS;

}

KrError kr_string_pop(KrString* str) {

    if (!str) { return KR_ERR_NULL_INPUT; }

    if (str->_size == 0) { return KR_ERR_SMALL_INPUT; }

    str->_size -= 1;
    memmove(str->_data, &str->_data[1], str->_size);
    str->_data[str->_size] = '\0';

    return KR_SUCCESS;

}

KrError kr_trim_string(KrString* src) {

    if (!src) { return KR_ERR_NULL_INPUT; }

    // Edge-case: If all whitespaces
    if (kr_is_string_all_whitespace(src)) {

        src->_size = 0;
        src->_data[0] = '\0';
        return KR_SUCCESS;

   }

    // Trim front
    for (size_t i = 0; i < src->_size; i++) {

        if (src->_data[i] != ' ') {

            src->_size -= i;
            memmove(src->_data, &src->_data[i], src->_size);
            src->_data[src->_size] = '\0';
            break;

        }

    }

    // If the string is all whitespaces and it is sucessfully trimmed
    if (src->_size == 0) return KR_SUCCESS;

    // Trim back
    // +1 and -1 here are conversions from nth element to index array values and back
    // accessing _data requires starting from 0 where _size starts from 1
    // This is often called cardinal (how many items) and ordinal (which one, index number)
    if (src->_data[src->_size - 1] != ' ') return KR_SUCCESS;
    for (size_t i = src->_size - 1; i > 0; i--) {

        if (src->_data[i] != ' ') {

            src->_data[i + NULL_TERMINATOR_LEN] = '\0';
            src->_size = i + 1;
            break;

        }

    }

    return KR_SUCCESS;

}

KrError kr_free_vector(KrVector* vector) {

    if (!vector || !vector->_data) { return KR_ERR_NULL_INPUT; }

    free(vector->_data);
    vector->_data = nullptr;

    return KR_SUCCESS;

}

KrError kr_free_string(KrString* str) {

    if (!str || !str->_data) { return KR_ERR_NULL_INPUT; }

    free(str->_data);
    str->_data = nullptr;

    return KR_SUCCESS;

}

#endif // KREST_IMPLEMENTATION
