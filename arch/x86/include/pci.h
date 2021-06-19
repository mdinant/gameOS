#ifndef __PCI_H
#define __PCI_H

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

bool check_pci();


void pci_check_all_buses(void);

#endif