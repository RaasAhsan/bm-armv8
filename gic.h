#include <stdint.h>

#define GICD_CTRL_ENABLE 0x0001
#define GICD_CTRL_DISABLE 0x0000

#define GICC_CTRL_ENABLE 0x0001
#define GICC_CTRL_DISABLE 0x0000

typedef uint16_t irq_id;

// Definition of GIC Distributor register map
typedef struct {
    volatile uint32_t ctlr; // GICD_CTLR
    const volatile uint32_t typer; // GICD_TYPER
    const volatile uint32_t iidr; // GICD_IIDR
    volatile uint8_t _reserved0[116];
    volatile uint32_t igroup[32]; // GICD_IGROUPR(n)
    volatile uint32_t isenabler[32]; // GICD_ISENABLERn, 1 bit per interrupt ID
    volatile uint32_t icenabler[32]; // GICD_ICENABLERn, 1 bit per interrupt ID
    volatile uint32_t ispendr[32];
    volatile uint32_t icpendr[32]; // GICD_ICPENDRn, 1 bit per interrupt ID
    volatile uint32_t isactive[32];
    volatile uint32_t icactive[32];
    volatile uint32_t ipriorityr[255]; // GICD_IPRIORITYRn, 1 byte per interrupt ID
    const volatile uint8_t _reserved1;
    volatile uint32_t itargetsr[255]; // GICD_ITARGETSRn, 1 byte per interrupt ID
    const volatile uint8_t _reserved2;
    volatile uint32_t icfgr[64];
    const volatile uint8_t _reserved3[256];
    volatile uint8_t nsacr[256];
    volatile uint32_t sgir;
    const volatile uint8_t _reserved4[12];
    volatile uint32_t cpendsgir[4];
    volatile uint32_t spendsgir[4];
} gicd;

typedef struct {
    volatile uint32_t ctlr; // GICC_CTLR
    volatile uint32_t pmr; // GICC_PMR
    volatile uint32_t bpr; // GICC_BPR
    const volatile uint32_t iar; // GICC_IAR
} gicc;

void gicd_init(gicd *dist);
void gicd_enable_irq(gicd *dist, irq_id id);
void gicd_disable_irq(gicd *dist, irq_id id);
void gicc_init(gicc *cpu);
void gicd_sgi(gicd *dist, irq_id id);
