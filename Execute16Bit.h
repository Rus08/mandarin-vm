

enum Instructions16BitId{
	VM_HADD,
	VM_HSUB,
	VM_HAND,
	VM_HOR,
	VM_HFADD,
	VM_HFSUB,
	VM_HFMUL,
	VM_HNOP,
};



uint32_t Execute16Bit(struct VirtualMachine* pVM, uint16_t Instruction);