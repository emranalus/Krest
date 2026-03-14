#ifndef KREST_H
#define KREST_H

// Definitions
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_STRING_CAPACITY 32
#define DEFAULT_VECTOR_CAPACITY 16
#define NULL_TERMINATOR_LEN 1

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
    bool _success;

} KrSlicePos;

// Array list object but dynamic due to carrying type _size it can be set to anytype this way
typedef struct {

    size_t _size;
    size_t _capacity;
    size_t _type_size;
    void* _data;

} KrVector;

// View object for KrString that doesnt own it just peeks into it
typedef struct {

    char* _data;
    size_t _size;

} KrStringView;

// Updates the _capacity by reallocating memory to the _data prop of the KrString object
bool kr_update_string_capacity(KrString* str);

// Sets string _capacity to the minimum possible value
void kr_minimize_string_capacity(KrString* str);

// Returns a new KrString object, user needs to free it with kr_free_string or declare it with autofree_krstring
KrString kr_new_string(const char src[]);

// Returns a new KrVector object, user needs to free it with kr_free_vector or declare it with autofree_krvector
KrVector kr_new_vector(size_t _type_size);

// Appends src at the end of dst
void kr_append_string(KrString* dst, const char src[]);

// Reverses the order of bytes of a given buffer, starting from 0 up to _size - 1
void kr_reverse_bytes(uint8_t* src, size_t _size);

// Appends src at the end of dst
void kr_append_to_vector(KrVector* dst, const void* src);

// Returns a void* user can assign with any type pointer since they defined the _size of each type at vector creation
void* kr_get_vector_element(const KrVector* src, size_t index);

// Inserts one char into string at specified index
void kr_insert_char(KrString* dst, char src, size_t index);

// Inserts src into string at specified index
void kr_insert_string(KrString* dst, const char src[], size_t index);

// Returns a new KrSting objects containing a slice of the inputted string, excludes last char and includes first char
KrString kr_slice_string(const KrString* src, size_t start, size_t end);

// Returns true if KrString->_data is an empty string
bool kr_is_empty_string(const KrString* str);

// If inputs are valid returns a KrSlicePos object containing start and end indices also returns a bool prop called success
// if false both start and end is set to 0 to save on memory management, might change it in the future but the user will
// have to free the result manually
// Usage: KrSlicePos.success needs to be checked if false both props will be 0 as mentioned above
KrSlicePos kr_find_substring(const KrString* str, const char src[]);

// Returns a KrVector object containing all occurrences of the substring in the form of KrSlicePos
KrVector kr_find_all_substrings(const KrString* str, const char src[]);

// Replaces src substring with 'with' substring
void kr_replace_first_substring(KrString* str, const char src[], const char with[]);

// Replaces all occurrences of src with 'with' substring
void kr_replace_all_substrings(KrString* str, const char src[], const char with[]);

// Replaces char at index with 'with'
void kr_replace_char(KrString* str, char with, size_t index);

// Returns the first char of the string
char kr_string_starts_with(const KrString* str);

// Returns the last char of the string
char kr_string_ends_with(const KrString* str);

// Changes strings _data prop
void kr_set_string_data(KrString* str, const char _data[]);

// Splits the str1 in half stores first half in str1 second half in str2, char at index is included in str1
void kr_split_string(KrString* str1, KrString* str2, size_t index);

// Note: Preallocate once with enough or more _size than you think you will need
// so KrString doesn't need to do it everytime it reaches its _capacity
void kr_preallocate_string(KrString* str, size_t capacity);

// Note: Preallocate once with enough or more _size than you think you will need
// so KrVector doesn't need to do it everytime it reaches its _capacity
void kr_preallocate_vector(KrVector* vector, size_t size_mb);

// Returns true if strings are equal
bool kr_is_equal_string(const KrString* str1, const KrString* str2);

// Returns a new KrString object with the same _data prop
KrString kr_copy_string(const KrString* str);

// Removes the last element of the string
void kr_string_pop_back(KrString* str);

// Removes the first element of the string
void kr_string_pop(KrString* str);

// Removes whitespaces at the beginning and end of the string
void kr_trim_string(KrString* src);

// Free KrVector object from memory
void kr_free_vector(KrVector* vector);

// Free KrString object from memory
void kr_free_string(KrString* str);

#endif //KREST_H

#ifdef KREST_IMPLEMENTATION

// Implementations
#include <string.h>
#include <stdio.h>

void kr_free_string_ptr(KrString* s) { if(s->_data) kr_free_string(s); }
void kr_free_vector_ptr(KrVector* v) { if (v->_data) kr_free_vector(v); }

#define autofree_krstring __attribute__((cleanup(kr_free_string_ptr)))
#define autofree_krvector __attribute__((cleanup(kr_free_vector_ptr)))

__attribute__((constructor))
void _kr_auto_init() {

    static_assert(sizeof(size_t) <= 8 && "Krest only supports 64-bit systems!");

}

bool kr_update_string_capacity(KrString* str) {

    // Error handling
    if (!str) {
        fprintf(stderr, "Krest Error (kr_update_string_capacity): Input KrString is a nullptr pointer.\n");
        return false;
    }

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
        if (new_mem == nullptr) {
            fprintf(stderr, "Krest Error (kr_update_string_capacity): Failed to reallocate memory for KrString object.\n");
            return false;
        }

        // Else
        str->_capacity = new_capacity;
        str->_data = new_mem;

    }

    return true;

}

void kr_minimize_string_capacity(KrString* str) {

    const size_t used_capacity = str->_size + NULL_TERMINATOR_LEN;
    if (str->_capacity > used_capacity) {

        char* new_mem = realloc(str->_data, used_capacity);
        if (!new_mem) {
            fprintf(stderr, "Krest Error (kr_minimize_string_capacity): Failed to shrink str.\n");
            return;
        }

        str->_capacity = used_capacity;
        str->_data = new_mem;

    }

}

KrString kr_new_string(const char src[]) {

    if (!src) {
        fprintf(stderr, "Krest Error (kr_new_string): src is a nullptr pointer.\n");
    }

    // Create new KrString object
    KrString s = {0};

    // Initialise
    s._size = strlen(src);

    if (s._size + NULL_TERMINATOR_LEN >= DEFAULT_STRING_CAPACITY) {
        s._capacity = s._size + DEFAULT_STRING_CAPACITY;
    }
    else {
        s._capacity = DEFAULT_STRING_CAPACITY;
    }

    s._data = malloc(s._capacity);

    // _data transfer
    memcpy(s._data, src, s._size + NULL_TERMINATOR_LEN);

    return s;

}

KrVector kr_new_vector(size_t _type_size) {

    KrVector vector;
    vector._capacity = DEFAULT_VECTOR_CAPACITY;
    vector._data = malloc(_type_size * vector._capacity);
    vector._type_size = _type_size;
    vector._size = 0;

    return vector;

}

void kr_append_string(KrString* dst, const char src[]) {

    if (!dst) {
        fprintf(stderr, "Krest Error (kr_append_string): dst is a nullptr pointer.\n");
        return;
    }

    if (!src) {
        fprintf(stderr, "Krest Error (kr_append_string): src is a nullptr pointer.\n");
        return;
    }

    // Calculate new _size and update dst's _capacity accordingly
    const size_t old_size = dst->_size;
    const size_t src_len = strlen(src);
    dst->_size = dst->_size + src_len;
    if (!kr_update_string_capacity(dst)) {

        // By doing this I avoid zombie state if realloc fails we reset the state before return since this function
        // does mutation
        dst->_size = old_size;
        fprintf(stderr, "Krest Error (kr_append_string): Failed to reallocate memory for dst.\n");
        return;

    }

    // Appending
    memcpy(&dst->_data[old_size], src, src_len + NULL_TERMINATOR_LEN);

}

void kr_reverse_bytes(uint8_t* src, size_t size) {
    // Nothing to reverse
    if (size < 2) return;

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

}

void kr_append_to_vector(KrVector* dst, const void* src) {

    if (!dst) {
        fprintf(stderr, "Krest Error (kr_append_to_vector): dst is a nullptr pointer.");
        return;
    }

    if (!src) {
        fprintf(stderr, "Krest Error (kr_append_to_vector): src is a nullptr pointer.");
        return;
    }

    if (dst->_size + 1 >= dst->_capacity) {

        dst->_capacity *= 2;
        void* new_data = realloc(dst->_data, dst->_capacity * dst->_type_size);
        if (!new_data) {
            fprintf(stderr, "Krest Error (kr_append_to_vector): Failed to reallocate dst->_size.\n");
            return;
        }
        // Else
        dst->_data = new_data;

    }

    // Actual append
    memcpy(&dst->_data[dst->_size * dst->_type_size], src, dst->_type_size);
    dst->_size += 1;

}

void* kr_get_vector_element(const KrVector* src, size_t index) {

    return &src->_data[index * src->_type_size];

}

void kr_insert_char(KrString* dst, char src, size_t index) {

    if (!dst) {
        fprintf(stderr, "Krest Error (kr_insert_char): dst is a nullptr pointer.\n");
        return;
    }

    if (index >= dst->_size) {
        fprintf(stderr, "Krest Error (kr_insert_char): Index cannot be bigger than or equal to input string _size.\n");
        return;
    }

    // Calculate new _size and update _capacity
    const size_t old_size = dst->_size;
    dst->_size = dst->_size + sizeof(src);
    if (!kr_update_string_capacity(dst)) {
        dst->_size = old_size;
        fprintf(stderr, "Krest Error (kr_insert_char): Failed to reallocate memory for dst.\n");
        return;
    }

    // 1 here means start moving into after the insert gap, do not move what will be inserted char
    memmove(&dst->_data[index] + 1, &dst->_data[index], old_size - index + NULL_TERMINATOR_LEN);

    // Insertion
    // +1 here is needed since we change the _size old index points to a different place than it used to
    // amount of increment to index is equal to how far away we moved the memory after index which is +1
    // as reader can see above in the first argument of the memmove
    index += 1;
    dst->_data[index] = src;
    dst->_data[dst->_size] = '\0';

}

void kr_insert_string(KrString* dst, const char src[], size_t index) {

    if (!dst) {
        fprintf(stderr, "Krest Error (kr_insert_char): dst is a nullptr pointer.\n");
        return;
    }

    if (index >= dst->_size) {
        fprintf(stderr, "Krest Error (kr_insert_char): Index cannot be bigger than or equal to input string _size.\n");
        return;
    }

    // Calculate new _size and update _capacity
    const size_t old_size = dst->_size;
    const size_t src_len = strlen(src);
    dst->_size = dst->_size + src_len;
    if (!kr_update_string_capacity(dst)) {
        dst->_size = old_size;
        fprintf(stderr, "Krest Error (kr_insert_char): Failed to reallocate memory for dst.\n");
        return;
    }

    // Notice we don't do +1 like we did with kr_insert_char because src_len is a cardinal value it starts from 1
    memmove(&dst->_data[index] + src_len, &dst->_data[index], old_size - index + NULL_TERMINATOR_LEN);

    // Insertion
    // +src_len here is needed since we change the _size old index points to a different place than it used to
    // amount of increment to index is equal to how far away we moved the memory after index which is +src_len
    // as reader can see above in the first argument of the memmove
    index += src_len;
    memcpy(&dst->_data[index], src, src_len + NULL_TERMINATOR_LEN);
    dst->_data[dst->_size] = '\0';

}

KrString kr_slice_string(const KrString* src, size_t start, size_t end) {

    if (!src) {
        fprintf(stderr, "Krest Error (kr_slice_string): src parameter is a nullptr pointer.\n");
    }

    if (end > src->_size) {
        fprintf(stderr, "Krest Error (kr_slice_string): end cannot be bigger than src _size.\n");
    }

    if (start > end) {
        fprintf(stderr, "Krest Error (kr_slice_string): start cannot be bigger than end.\n");
    }

    KrString s = kr_new_string("");
    s._size = end - start;
    if (!kr_update_string_capacity(&s)) {
        kr_free_string(&s);
        fprintf(stderr, "Krest Error (kr_slice_string): Failed to reallocate memory for KrString object.\n");
    }

    memcpy(s._data, &src->_data[start], s._size);
    s._data[s._size] = '\0';

    return s;

}

bool kr_is_empty_string(const KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_is_empty_string): str is a nullptr pointer.\n");
        return false;
    }

    if (!str->_data) {
        fprintf(stderr, "Krest Error (kr_is_empty_string): str->_data is a nullptr pointer.\n");
        return false;
    }

    if (str->_data[0] == '\0') { return true; }
    // Else if not empty
    return false;

}

KrSlicePos kr_find_substring(const KrString* str, const char src[]) {

    KrSlicePos pos = { 0, 0, 0 };

    if (!str) {
        fprintf(stderr, "Krest Error (kr_find_substring): str is a nullptr pointer.\n");
        return pos;
    }

    if (!src) {
        fprintf(stderr, "Krest Error (kr_find_substring): src is a nullptr pointer.\n");
        return pos;
    }

    if (strlen(src) > str->_size) {
        fprintf(stderr, "Krest Error (kr_find_substring): src cannot be bigger than str.\n");
        return pos;
    }

    // Number of iterations = main string length - substring length
    // Sliding window implementation
    const size_t src_size = strlen(src);
    for (size_t i = 0; i <= str->_size - src_size; i++) {

        if (memcmp(&str->_data[i], src, src_size) == 0) {

            // -1 is a convertion from string length to array index value
            pos._start = i;
            pos._end = src_size + i - 1;
            pos._success = true;
            break;

        }

    }

    return pos;

}

bool kr_check_replace_helper(const KrString* str, const char src[], const char with[]) {

    if (!src) {
        fprintf(stderr, "Krest Error (kr_replace_substring): src is a nullptr pointer.\n");
        return false;
    }

    if (!str) {
        fprintf(stderr, "Krest Error (kr_replace_substring): str is a nullptr pointer.\n");
        return false;
    }

    if (!with) {
        fprintf(stderr, "Krest Error (kr_replace_substring): with is a nullptr pointer.\n");
        return false;
    }

    if (strlen(src) > str->_size) {
        fprintf(stderr, "Krest Error (kr_replace_string): src cannot be longer than str->_data.\n");
        return false;
    }

    return true;

}

void kr_replace_first_substring(KrString* str, const char src[], const char with[]) {

    if (!kr_check_replace_helper(str, src, with)) return;

    const KrSlicePos pos = kr_find_substring(str, src);
    if (!pos._success) {
        fprintf(stderr, "Krest Error (kr_replace_substring): Failed to find src in str.\n");
        return;
    }

    const ssize_t diff = (ssize_t)strlen(with) - (ssize_t)strlen(src);
    if (diff >= 0) {

        str->_size += diff;
        if (!kr_update_string_capacity(str)) {
            fprintf(stderr, "Krest Error (kr_replace_substring): Failed to resize str.\n");
            return;
        }

    }

    // Calculating how much and what to move
    // +1 here is present because pos.end is exclusive
    char* tail = &str->_data[pos._end + 1];
    size_t bytes_to_move = strlen(tail) + NULL_TERMINATOR_LEN;

    // We don't need to use memmove if there is nothing to move meaning the gap is where it needs to be
    if (diff != 0) memmove(tail + diff, tail, bytes_to_move);
    memcpy(&str->_data[pos._start], with, strlen(with));

}

KrVector kr_find_all_substrings(const KrString* str, const char src[]) {

    // Positions array
    KrVector positions = kr_new_vector(sizeof(KrSlicePos));

    // Sliding window implementation and occurrence storing
    size_t src_len = strlen(src);
    size_t step_amount = str->_size - src_len;
    KrSlicePos pos = { 0, 0, 0 };
    for (size_t i = 0; i <= step_amount; i++) {

        // Start from ith offset go as long as length of src and compare with src, 0 means no difference has been found
        if (memcmp(&str->_data[i], src, src_len) == 0) {

            // -1 means pos.end includes the char after the end for some reason I don't know why
            pos._start = i;
            pos._end = src_len + i - 1;
            pos._success = true;

            kr_append_to_vector(&positions, &pos);

        }

    }

    return positions;

}

void kr_replace_all_substrings(KrString* str, const char src[], const char with[]) {

    if (!kr_check_replace_helper(str, src, with)) return;

    // Positions array
    autofree_krvector const KrVector positions = kr_find_all_substrings(str, src);

    // Length of src
    const size_t src_len = strlen(src);

    // Construction of the new string buffer
    const ssize_t diff_per_occurrence = (ssize_t)strlen(with) - (ssize_t)src_len;
    const ssize_t cum_diff_of_changes = diff_per_occurrence * (ssize_t)positions._size;
    const size_t final_size = str->_size + cum_diff_of_changes;
    char* new_buffer = malloc(final_size  + NULL_TERMINATOR_LEN * sizeof(char));
    new_buffer[final_size] = '\0';

    // Filling new_buffer
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

    const size_t tail_len = str->_size - str_cursor;
    if (tail_len > 0) {

        memcpy(&new_buffer[nb_cursor], &str->_data[str_cursor], tail_len);

    }

    // Binding new_buffer to str
    str->_size = final_size;
    str->_capacity = final_size + NULL_TERMINATOR_LEN;
    free(str->_data);
    str->_data = new_buffer;

}

void kr_replace_char(KrString* str, char with, size_t index) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_replace_char): str is a nullptr pointer.\n");
        return;
    }

    if (index >= str->_size) {
        fprintf(stderr, "Krest Error (kr_replace_char): index is out of str's range.\n");
        return;
    }

    str->_data[index] = with;

}

char kr_string_starts_with(const KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_string_starts_with): str is a nullptr pointer.\n");
    }

    return str->_data[0];

}

char kr_string_ends_with(const KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_string_ends_with): str is a nullptr pointer.\n");
    }

    return str->_data[str->_size - 1];

}

void kr_set_string_data(KrString* str, const char _data[]) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_set_string_data): str is a nullptr pointer.\n");
        return;
    }

    if (!_data) {
        fprintf(stderr, "Krest Error (kr_set_string_data): _data is a nullptr pointer.\n");
        return;
    }

    const size_t old_len = str->_size;
    str->_size = strlen(_data);
    if (!kr_update_string_capacity(str)) {
        str->_size = old_len;
        fprintf(stderr, "Krest Error (kr_set_string_data): Failed to resize str.\n");
        return;
    }

    memcpy(str->_data, _data, str->_size);

}

void kr_split_string(KrString* str1, KrString* str2, size_t index) {

    if (!str1) {
        fprintf(stderr, "Krest Error (kr_split_string): str1 is a nullptr pointer.\n");
        return;
    }

    if (!str2) {
        fprintf(stderr, "Krest Error (kr_split_string): str2 is a nullptr pointer.\n");
        return;
    }

    // Filling str2
    const size_t right_hand_size = str1->_size - index - 1;
    const size_t old_len = str2->_size;
    str2->_size = right_hand_size;
    if (!kr_update_string_capacity(str2)) {
        str2->_size = old_len;
        fprintf(stderr, "Krest Error (kr_split_string): Failed to resize str2.\n");
        return;
    }

    memcpy(str2->_data, &str1->_data[index], right_hand_size);
    str2->_data[right_hand_size] = '\0';

    // Clipping str1
    const size_t left_hand_size = str1->_size - right_hand_size;
    str1->_data[left_hand_size] = '\0';

}

void kr_preallocate_string(KrString* str, size_t capacity) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_preallocate_string): str is a nullptr pointer.\n");
        return;
    }

    // No need to reallocate
    if (capacity <= str->_capacity) return;

    // Resize
    const size_t old_capacity = capacity;
    char* new_mem = realloc(str->_data, capacity);
    if (new_mem == nullptr) {
        str->_capacity = old_capacity;
        fprintf(stderr, "Krest Error (kr_update_string_capacity): Failed to reallocate memory for KrString object.\n");
        return;
    }

    // Else
    str->_capacity = capacity;
    str->_data = new_mem;

}

void kr_preallocate_vector(KrVector* vector, size_t size_mb) {

    if (!vector) {
        fprintf(stderr, "Krest Error (kr_preallocate_vector): vector is a nullptr pointer.\n");
        return;
    }

    // No need to reallocate
    if (size_mb <= vector->_capacity) return;

    vector->_capacity = size_mb;
    void* new_data = realloc(vector->_data, vector->_capacity * vector->_type_size);
    if (!new_data) {
        fprintf(stderr, "Krest Error (kr_preallocate_vector): Failed to reallocate dst->_size.\n");
        return;
    }
    // Else
    vector->_data = new_data;

}

bool kr_is_equal_string(const KrString* str1, const KrString* str2) {

    if (!str1) {
        fprintf(stderr, "Krest Error (kr_is_equal_string): str1 is a nullptr pointer.\n");
        return false;
    }

    if (!str2) {
        fprintf(stderr, "Krest Error (kr_is_equal_string): str2 is a nullptr pointer.\n");
        return false;
    }

    if (str1->_size != str2->_size) { return false; }
    if (memcmp(str1->_data, str2->_data, str1->_size) != 0) { return false; }

    return true;

}

KrString kr_copy_string(const KrString* str) {

    return kr_new_string(str->_data);

}

void kr_string_pop_back(KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_string_pop_back): str is nullptr pointer.\n");
        return;
    }

    if (str->_size == 0) return;

    // -1 here is a convertion from size_t into an index value, last element to be precise
    str->_data[str->_size - 1] = '\0';
    str->_size -= 1;

}

void kr_string_pop(KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_string_pop): str is nullptr pointer.\n");
        return;
    }

    if (str->_size == 0) return;

    str->_size -= 1;
    memmove(str->_data, &str->_data[1], str->_size);
    str->_data[str->_size] = '\0';

}

void kr_trim_string(KrString* src) {

    if (!src) {
        fprintf(stderr, "Krest Error (kr_trim_string): src is a nullptr pointer.");
        return;
    }

    // If all whitespaces nothing to trim
    if (src->_size == 0) return;

    // Trim front
    for (size_t i = 0; i < src->_size; i++) {

        if (src->_data[i] != ' ') {

            src->_size -= i;
            memmove(src->_data, &src->_data[i], src->_size);
            src->_data[src->_size] = '\0';
            break;

        }

    }

    // If the string is all whitespaces it is trimmed
    if (src->_size == 0) return;

    // Trim back
    // +1 and -1 here are conversions from nth element to index array values and back
    // accessing _data requires starting from 0 where _size starts from 1
    // This is often called cardinal (how many items) and ordinal (which one, index number)
    if (src->_data[src->_size - 1] != ' ') return;
    for (size_t i = src->_size - 1; i > 0; i--) {

        if (src->_data[i] != ' ') {

            src->_data[i + NULL_TERMINATOR_LEN] = '\0';
            src->_size = i + 1;
            break;

        }

    }

}

void kr_free_vector(KrVector* vector) {

    if (!vector) {
        fprintf(stderr, "Krest Error (kr_free_vector): vector is a nullptr pointer.");
        return;
    }

    if (!vector->_data) {
        fprintf(stderr, "Krest Error (kr_free_vector): vector->_data is a nullptr pointer, you might've double freed this object.");
        return;
    }

    free(vector->_data);
    vector->_data = nullptr;

}

void kr_free_string(KrString* str) {

    if (!str) {
        fprintf(stderr, "Krest Error (kr_free_string): str is a nullptr pointer.\n");
        return;
    }

    if (!str->_data) {
        fprintf(stderr, "Krest Error (kr_free_string): str->_data is a nullptr pointer, you might've double freed this object.\n");
        return;
    }

    free(str->_data);
    str->_data = nullptr;

}

#endif // KREST_IMPLEMENTATION
