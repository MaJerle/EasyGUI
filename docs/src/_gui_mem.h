/**
 * \addtogroup      GUI_MEM
 * \{
 *
 * Since dynamic allocation can be hard job for some microcontrollers, 
 * library implements memory management parts with simple allocation process in different regions.
 *
 * Memory management mode allows allocation of different sizes and allows free memory. 
 * It also takes cover to free memory and to create one big block if multiple regions are freed in sequence
 *
 * Memory management can be used in multiple regions which may optimize RAM usage, for example:
 *  - Use TCM/CCM RAM for widget structures (low amount of bytes, up to ~150, but depends on widget)
 *  - Use of large amount of memory (ex. temporary frame buffers) on external RAM (SDRAM) module if supported by embedded device
 *
 * \note            All functions are thread safe and can be used by application if required
 *
 * \}
 */