#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

#define STR_CAPACITY 20

/* Matching union and struct definitions */
union union_data {
    int i;
    float f;
    char str[STR_CAPACITY];
};

struct struct_data {
    int i;
    float f;
    char str[STR_CAPACITY];
};

/* Tagged union to track active member safely */
enum value_type {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
};

struct tagged_record {
    union union_data data;
    enum value_type type;
};

/* --- Setter Functions with Validation --- */

int set_integer(struct tagged_record *record, int value) {
    if (record == NULL) {
        return 0;
    }

    record->data.i = value;
    record->type = VALUE_INT;

    return 1;
}

int set_float(struct tagged_record *record, float value) {
    if (record == NULL) {
        return 0;
    }

    record->data.f = value;
    record->type = VALUE_FLOAT;

    return 1;
}

int set_string(struct tagged_record *record, const char *value) {
    if (record == NULL || value == NULL) {
        return 0;
    }

    /* String must fit in 20 bytes including null terminator */
    if (strlen(value) >= STR_CAPACITY) {
        printf("Error: String exceeds maximum capacity of %d characters.\n", STR_CAPACITY - 1);
        return 0;
    }

    strncpy(record->data.str, value, STR_CAPACITY - 1);
    record->data.str[STR_CAPACITY - 1] = '\0';
    record->type = VALUE_TEXT;

    return 1;
}

/* --- Display Function Reading Only the Active Member --- */

int display_tagged_record(const struct tagged_record *record) {
    if (record == NULL) {
        return 0;
    }

    switch (record->type) {
        case VALUE_INT: {
            printf("[Type: INTEGER] Value: %d\n", record->data.i);
            break;
        }
        case VALUE_FLOAT: {
            printf("[Type: FLOAT]   Value: %.2f\n", record->data.f);
            break;
        }
        case VALUE_TEXT: {
            printf("[Type: TEXT]    Value: %s\n", record->data.str);
            break;
        }
        default: {
            printf("Error: Invalid or unknown active type tag.\n");
            return 0;
        }
    }

    return 1;
}

/* --- Memory Representation and Layout Analysis --- */

void run_memory_analysis(void) {
    struct struct_data sample_struct;
    union union_data sample_union;

    printf("====================================================\n");
    printf("     Part 1: Memory Representation & Layout         \n");
    printf("====================================================\n");

    /* Size and alignment comparison */
    printf("StructData size     : %zu bytes\n", sizeof(struct struct_data));
    printf("StructData alignment: %zu bytes\n", alignof(struct struct_data));
    printf("UnionData size      : %zu bytes\n", sizeof(union union_data));
    printf("UnionData alignment : %zu bytes\n\n", alignof(union union_data));

    /* Member offsets for struct */
    printf("--- StructData Member Offsets ---\n");
    printf("Offset of member 'i'  : %zu bytes\n", offsetof(struct struct_data, i));
    printf("Offset of member 'f'  : %zu bytes\n", offsetof(struct struct_data, f));
    printf("Offset of member 'str': %zu bytes\n\n", offsetof(struct struct_data, str));

    /* Member addresses comparison */
    printf("--- Member Addresses in Memory ---\n");
    printf("StructData members:\n");
    printf("  &sdata.i   : %p\n", (void *)&sample_struct.i);
    printf("  &sdata.f   : %p\n", (void *)&sample_struct.f);
    printf("  &sdata.str : %p\n", (void *)&sample_struct.str);

    printf("UnionData members (overlapping starting address):\n");
    printf("  &udata.i   : %p\n", (void *)&sample_union.i);
    printf("  &udata.f   : %p\n", (void *)&sample_union.f);
    printf("  &udata.str : %p\n\n", (void *)&sample_union.str);

    /* Array size comparison for 100 records */
    size_t struct_100_bytes = 100 * sizeof(struct struct_data);
    size_t union_100_bytes = 100 * sizeof(union union_data);

    printf("--- Memory for 100 Records ---\n");
    printf("100 StructData records : %zu bytes\n", struct_100_bytes);
    printf("100 UnionData records  : %zu bytes\n", union_100_bytes);
    printf("Memory saved by Union  : %zu bytes\n\n", struct_100_bytes - union_100_bytes);
}

/* --- Demonstration of Tagged Records and Edge Cases --- */

int main(void) {
    /* Step 1: Run memory and layout analysis */
    run_memory_analysis();

    printf("====================================================\n");
    printf("     Part 2: Safe Tagged-Value Record System        \n");
    printf("====================================================\n");

    /* Store 5 records of mixed types as required */
    struct tagged_record records[5];

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5f);
    set_string(&records[2], "Union Programming");
    set_integer(&records[3], 2026);
    set_string(&records[4], "Faculty of Tech");

    printf("Displaying 5 mixed tagged records safely:\n");
    for (int i = 0; i < 5; i++) {
        printf("Record %d -> ", i + 1);
        display_tagged_record(&records[i]);
    }

    printf("\nSize comparison with tagged record:\n");
    printf("TaggedRecord size: %zu bytes\n", sizeof(struct tagged_record));
    printf("Plain Union size : %zu bytes\n", sizeof(union union_data));
    printf("Plain Struct size: %zu bytes\n\n", sizeof(struct struct_data));

    /* Step 3: Edge cases testing */
    printf("====================================================\n");
    printf("     Part 3: Edge Cases Demonstration               \n");
    printf("====================================================\n");

    struct tagged_record edge_case_record;

    /* Edge Case 1: Max-length string (19 chars + 1 null terminator) */
    printf("1. Testing maximum valid length string (19 chars):\n");
    if (set_string(&edge_case_record, "1234567890123456789")) {
        display_tagged_record(&edge_case_record);
    }

    /* Edge Case 2: Overlength string rejection (>= 20 chars) */
    printf("\n2. Testing overlength string rejection (20 chars):\n");
    if (!set_string(&edge_case_record, "12345678901234567890")) {
        printf("Correctly rejected overlength string. Previous value preserved.\n");
        display_tagged_record(&edge_case_record);
    }

    /* Edge Case 3: Switching a single record through different types */
    printf("\n3. Testing dynamic type transitions on a single record:\n");
    set_integer(&edge_case_record, 999);
    printf("Transition to INT   -> ");
    display_tagged_record(&edge_case_record);

    set_string(&edge_case_record, "Safe Transition");
    printf("Transition to TEXT  -> ");
    display_tagged_record(&edge_case_record);

    set_float(&edge_case_record, 45.67f);
    printf("Transition to FLOAT -> ");
    display_tagged_record(&edge_case_record);

    /* Edge Case 4: Invalid tag handling */
    printf("\n4. Testing invalid tag handling:\n");
    edge_case_record.type = (enum value_type)99;
    if (!display_tagged_record(&edge_case_record)) {
        printf("Safely detected corrupted/invalid tag without reading garbage memory.\n");
    }

    return 0;
}
