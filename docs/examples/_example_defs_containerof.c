/* Declare driver structure */
typedef struct {
    int x;
    int y;
} drv_t

/*
 * Declare custom structure for
 * use in custom software
 */
typedef struct {
    int a;
    int b;
    int c;
    drv_t driver;
} my_structure;

/* Create structure object */
my_structure my;

/* Create pointer to structure object */
drv_t* driver_ptr = &my->driver;

/*
 * Pass pointer to our working function somewhere
 *
 * Assume this function is callback called from our software
 */
void myFunc(drv_t* drv) {
    /* We know drv pointer is child of my_structure */
    /* Let's get that my_structure pointer from drv_t structure */
    my_structure* my_struct = gui_containerof(drv, my_structure, driver);
    
    /* Check if they match */
    if (my_struct == &my) {
        /* This should be true */
    }
}